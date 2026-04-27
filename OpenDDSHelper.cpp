#include "OpenDDSHelper.h"

OpenDDSHelper::OpenDDSHelper(){
}

OpenDDSHelper::~OpenDDSHelper() {
    dissConnect();
}


bool OpenDDSHelper::configureTransport(const char* address, CTYPE TYPE) {

    registry = OpenDDS::DCPS::TransportRegistry::instance();
    
    // Clean up old config and instances for reconnect support
    registry->remove_config("config");
    registry->remove_inst("tcp1");
    registry->remove_inst("udp1");
    registry->remove_inst("rtps1");
    
    config = registry->create_config("config");

    std::string addr = std::string("corbaloc::") + address + "/DCPSInfoRepo";
    std::cout << "InfoRepo = " << addr<<std::endl;

    if (TYPE == CTYPE::TCP) {

        // InfoRepo = "corbaloc::localhost:12345/DCPSInfoRepo"
        TheServiceParticipant->set_repo_ior(addr.c_str());

        auto inst = registry->create_inst("tcp1", "tcp");
        config->instances_.push_back(inst);
    }
    else if (TYPE == CTYPE::UDP) {

        // address = "0.0.0.0:5000"
        TheServiceParticipant->set_repo_ior(addr.c_str());

        auto inst = registry->create_inst("udp1", "udp");

        if (auto* udp = dynamic_cast<OpenDDS::DCPS::UdpInst*>(inst.in())) {
            udp->local_address("0.0.0.0:0");
        }

        config->instances_.push_back(inst);
    }
    else if (TYPE == CTYPE::RTPS_UDP) {

        // 🔥 Disable InfoRepo
        TheServiceParticipant->set_repo_ior("");

        // 🔥 Force RTPS discovery
        TheServiceParticipant->set_default_discovery(
            OpenDDS::DCPS::Discovery::DEFAULT_RTPS);

        auto inst = registry->create_inst("rtps1", "rtps_udp");

        if (auto* rtps = dynamic_cast<OpenDDS::DCPS::RtpsUdpInst*>(inst.in())) {
            rtps->local_address(OpenDDS::DCPS::NetworkAddress("0.0.0.0:0"));
            rtps->use_multicast(true);  // important
        }

        config->instances_.push_back(inst);
    }

    // Apply config globally
    registry->global_config(config);
    return true;
}

bool OpenDDSHelper::createDomainParticipant(int domainId) {
    dpf = TheParticipantFactory;

    participant = dpf->create_participant(
        domainId,
        PARTICIPANT_QOS_DEFAULT,
        DDS::DomainParticipantListener::_nil(),
        OpenDDS::DCPS::DEFAULT_STATUS_MASK
    );

    if (CORBA::is_nil(participant.in())){
        std::cerr << "ERROR: DomainParticipant creation failed." << std::endl;
        return false;
    }
    return true;
}

bool OpenDDSHelper::createTopic(const char* EXCHANGE_EVT_TOPIC_NAME) {
    // 4. Register Type
    mts =
        new Messager::MessageTypeSupportImpl();

    if (mts->register_type(participant.in(), "") != DDS::RETCODE_OK)
    {
        std::cerr << "ERROR: Failed to register MessageTypeSupport_var." << std::endl;
        return false;
    }

    // 5. Topic
    DDS::TopicQos topic_qos;
    participant->get_default_topic_qos(topic_qos);

    exchange_evt_topic =
        participant->create_topic(
            EXCHANGE_EVT_TOPIC_NAME,
            mts->get_type_name(),
            topic_qos,
            DDS::TopicListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

    if (CORBA::is_nil(exchange_evt_topic.in())){
       std::cerr << "ERROR: Exchange Event Topic creation failed." << std::endl;
        return false;
    }
    return true;
}

bool OpenDDSHelper::createPublisherAndDataWriter(const char* EXCHANGE_EVT_TOPIC_NAME) {
    // 6. Publisher
    publisher =
        participant->create_publisher(
            PUBLISHER_QOS_DEFAULT,
            DDS::PublisherListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

    if (CORBA::is_nil(publisher.in())){
        std::cerr << "ERROR: Publisher creation failed." << std::endl;
        return false;
    }

    // 7. DataWriter
    DDS::DataWriterQos dw_qos;
    publisher->get_default_datawriter_qos(dw_qos);

    exchange_evt_data_writer =
        publisher->create_datawriter(
            exchange_evt_topic.in(),
            dw_qos,
            DDS::DataWriterListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

    if (CORBA::is_nil(exchange_evt_data_writer.in())){
      std::cerr << "ERROR: DataWriter creation failed." << std::endl;
      return false;
    }

    writer = Messager::MessageDataWriter::_narrow(exchange_evt_data_writer.in());

    if (CORBA::is_nil(writer.in())){
        std::cerr << "ERROR: Narrowing DataWriter failed." << std::endl;
        return false;
    }
    return true;
}

bool OpenDDSHelper::initPubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME) {
    
    try {
        // Fake argc/argv
        int argc = 3;

        ACE_TCHAR* argv[] = {
            const_cast<ACE_TCHAR*>("app"),
            const_cast<ACE_TCHAR*>("-DCPSConfigFile"),
            const_cast<ACE_TCHAR*>(config.c_str())
        };
       // Create DomainParticipant
       dpf = TheParticipantFactoryWithArgs(argc, argv);
       participant =
           dpf->create_participant(DOMAINE_ID,
                                   PARTICIPANT_QOS_DEFAULT,
                                   DDS::DomainParticipantListener::_nil(),
                                   ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

       if (CORBA::is_nil(participant.in())){
              std::cerr << "ERROR: DomainParticipant creation failed." << std::endl;
              return false;
       }

        // create topic
        if (!createTopic(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }

        // create publisher and datawriter
        if (!createPublisherAndDataWriter(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }
        return true;
    }
    catch (const CORBA::Exception& ex) {
        ex._tao_print_exception("CreatePublisher Exception:");
        return false;
    }

}

bool OpenDDSHelper::initPublisher(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME) {
    // check if the publisher is already initialized if yes return true
    if (!CORBA::is_nil(writer.in())) {
        std::cerr << "Publisher is already initialized." << std::endl;
        return true;
    }
    
    try {

        if (!configureTransport(address, TYPE)) {
            std::cerr << "ERROR: Failed to configure transport." << std::endl;
            return false;
        }

        // 3. Create DomainParticipant
            if (!createDomainParticipant(DOMAINE_ID)) {
                return false;
            }

        // create topic
        if (!createTopic(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }

        // create publisher and datawriter
        if (!createPublisherAndDataWriter(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }
    }
    catch (const CORBA::Exception& ex) {
        ex._tao_print_exception("CreatePublisher Exception:");
        return false;
    }
}

bool OpenDDSHelper::publishMessage(const Messager::Message message)
{
    if (CORBA::is_nil(writer.in())) {
        std::cerr << "ERROR: DataWriter is not initialized." << std::endl;
        return false;
    }
    DDS::ReturnCode_t ret = writer->write(message, DDS::HANDLE_NIL);
    if (ret != DDS::RETCODE_OK)
    {
        std::cerr << "ERROR: Failed to write message: " << ret << std::endl;
        return false;
    }
    return true;
}


bool OpenDDSHelper::createSubscriberAndDataReader(const char* EXCHANGE_EVT_TOPIC_NAME) {
    // 6. Subscriber
    subscriber =
        participant->create_subscriber(
            SUBSCRIBER_QOS_DEFAULT,
            DDS::SubscriberListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

    if (CORBA::is_nil(subscriber.in())){
        std::cerr << "ERROR: Subscriber creation failed." << std::endl;
        return false;
    }

    // 7. DataReader with listener
    DDS::DataReaderQos dr_qos;
    subscriber->get_default_datareader_qos(dr_qos);

    MessageReader = new MessageReaderListener();
    data_reader_listener = DDS::DataReaderListener_var(MessageReader);

    exchange_evt_data_reader =
        subscriber->create_datareader(
            exchange_evt_topic.in(),
            dr_qos,
            data_reader_listener.in(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

    if (CORBA::is_nil(exchange_evt_data_reader.in())) {
        std::cerr << "ERROR: DataReader creation failed." << std::endl;
        return false;
    }
    return true;
}


bool OpenDDSHelper::initSubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME) {

    try
    {
        // Fake argc/argv
        int argc = 3;

        ACE_TCHAR* argv[] = {
            const_cast<ACE_TCHAR*>("app"),
            const_cast<ACE_TCHAR*>("-DCPSConfigFile"),
            const_cast<ACE_TCHAR*>(config.c_str())
        };
       // Create DomainParticipant
       dpf = TheParticipantFactoryWithArgs(argc, argv);
       participant =
           dpf->create_participant(DOMAINE_ID,
                                   PARTICIPANT_QOS_DEFAULT,
                                   DDS::DomainParticipantListener::_nil(),
                                   ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

       if (CORBA::is_nil(participant.in())){
              std::cerr << "ERROR: DomainParticipant creation failed." << std::endl;
              return false;
       }

        // create topic
        if (!createTopic(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }

        // create subscriber and datareader with listener
        if (!createSubscriberAndDataReader(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}


bool OpenDDSHelper::initSubscriber(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME) {
    try {

        if (!configureTransport(address, TYPE)) {
            std::cerr << "ERROR: Failed to configure transport." << std::endl;
            return false;
        }

        if (!createDomainParticipant(DOMAINE_ID)) {
            return false;
        }

        // create topic
         if (!createTopic(EXCHANGE_EVT_TOPIC_NAME)) {
              return false;
            }

        // create subscriber and datareader with listener
        if (!createSubscriberAndDataReader(EXCHANGE_EVT_TOPIC_NAME)) {
            return false;
        }

          std::cout << "INFO: Subscriber running. Waiting for data..." << std::endl;
            return true;
      }
      catch (const CORBA::Exception& ex) {
          ex._tao_print_exception("ERROR: Exception caught:");
          return false;
    }
}

void OpenDDSHelper::dissConnect() {
    try {
        // Delete datareader (which owns MessageReader via reference counting)
        if (!CORBA::is_nil(subscriber.in()) && !CORBA::is_nil(exchange_evt_data_reader.in())) {
            subscriber->delete_datareader(exchange_evt_data_reader);
            exchange_evt_data_reader = DDS::DataReader::_nil();
        }

        // Delete subscriber
        if (!CORBA::is_nil(participant.in()) && !CORBA::is_nil(subscriber.in())) {
            participant->delete_subscriber(subscriber);
            subscriber = DDS::Subscriber::_nil();
        }

        // Delete datawriter
        if (!CORBA::is_nil(publisher.in()) && !CORBA::is_nil(exchange_evt_data_writer.in())) {
            publisher->delete_datawriter(exchange_evt_data_writer);
            exchange_evt_data_writer = DDS::DataWriter::_nil();
        }

        // Delete publisher
        if (!CORBA::is_nil(participant.in()) && !CORBA::is_nil(publisher.in())) {
            participant->delete_publisher(publisher);
            publisher = DDS::Publisher::_nil();
        }

        // Delete topic
        if (!CORBA::is_nil(participant.in()) && !CORBA::is_nil(exchange_evt_topic.in())) {
            participant->delete_topic(exchange_evt_topic);
            exchange_evt_topic = DDS::Topic::_nil();
        }

        // Delete participant
        if (!CORBA::is_nil(participant.in())) {
            dpf->delete_participant(participant.in());
            participant = DDS::DomainParticipant::_nil();
        }

        // Clean up transport registry to enable reconnect
        if (registry) {
            registry->remove_config("config");
            registry->remove_inst("tcp1");
            registry->remove_inst("udp1");
            registry->remove_inst("rtps1");
        }

        // Reset all handles
        writer = Messager::MessageDataWriter::_nil();
        mts = Messager::MessageTypeSupport::_nil();
        data_reader_listener = DDS::DataReaderListener::_nil();
        MessageReader = nullptr;
        registry = nullptr;
        config = OpenDDS::DCPS::TransportConfig_rch();

        std::cout << "INFO: Disconnected successfully." << std::endl;
        // DO NOT call shutdown() - keeps service alive for reconnect
    }
    catch (const CORBA::Exception& ex) {
        ex._tao_print_exception("ERROR: Exception during dissConnect:");
    }
}
