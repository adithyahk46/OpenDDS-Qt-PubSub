#include "OpenDDSSubscriber.h"


#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/transport/framework/TransportRegistry.h>

#include <dds/DCPS/transport/tcp/TcpInst.h>
#include <dds/DCPS/transport/udp/UdpInst.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdpInst.h>
#include <dds/DCPS/NetworkAddress.h>

OpenDDSSubscriber::OpenDDSSubscriber()
{
}

bool OpenDDSSubscriber::createSubscriberWithArgs(std::string config, int DOMAINE_ID, const char *EXCHANGE_EVT_TOPIC_NAME)
{
   try {
        // Fake argc/argv
        int argc = 3;

        ACE_TCHAR* argv[] = {
            const_cast<ACE_TCHAR*>("app"),
            const_cast<ACE_TCHAR*>("-DCPSConfigFile"),
            const_cast<ACE_TCHAR*>(config.c_str())
        };
       // Create DomainParticipant
       DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
       DDS::DomainParticipant_var participant =
           dpf->create_participant(DOMAINE_ID,
                                   PARTICIPANT_QOS_DEFAULT,
                                   DDS::DomainParticipantListener::_nil(),
                                   ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

       if (CORBA::is_nil(participant.in()))
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DomainParticipant creation failed.\n")), 1);

       // Create Subscriber
       DDS::Subscriber_var subscriber =
           participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                          DDS::SubscriberListener::_nil(),
                                          ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
       if (CORBA::is_nil(subscriber.in()))
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Subscriber creation failed.\n")), 1);

       // Register type
       Messager::MessageTypeSupport_var mts = new Messager::MessageTypeSupportImpl();
       if (mts->register_type(participant.in(), "") != DDS::RETCODE_OK)
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Failed to register type.\n")), 1);

       // Create topic
       DDS::TopicQos topic_qos;
       participant->get_default_topic_qos(topic_qos);
       DDS::Topic_var exchange_evt_topic =
           participant->create_topic(EXCHANGE_EVT_TOPIC_NAME,
                                     mts->get_type_name(),
                                     topic_qos,
                                     DDS::TopicListener::_nil(),
                                     ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

       if (CORBA::is_nil(exchange_evt_topic.in()))
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Exchange Event Topic creation failed.\n")), 1);

       // Create DataReader with listener
       DDS::DataReaderQos dr_qos;
       subscriber->get_default_datareader_qos(dr_qos);
       DDS::DataReaderListener_var listener(new MessageReaderListener);

       DDS::DataReader_var exchange_evt_reader =
           subscriber->create_datareader(exchange_evt_topic.in(),
                                         dr_qos,
                                         listener.in(),
                                         ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

       if (CORBA::is_nil(exchange_evt_reader.in()))
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DataReader creation failed.\n")), 1);

       ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: Subscriber running. Waiting for data...\n")));
       while (true) {
           ACE_OS::sleep(1);
       }
   }
   catch (const CORBA::Exception& ex) {
       ex._tao_print_exception("ERROR: Exception caught:");
       return 1;
   }

}

bool OpenDDSSubscriber::createSubscriber(const char *ADDR, CTYPE TYPE, int DOMAINE_ID, const char *EXCHANGE_EVT_TOPIC_NAME)
{

    try {

        auto* registry = OpenDDS::DCPS::TransportRegistry::instance();
        auto config = registry->create_config("config");


        std::string addr = std::string("corbaloc::") + ADDR + "/DCPSInfoRepo";
        std::cout << addr<<std::endl;

        if (TYPE == CTYPE::TCP) {

            // address = "corbaloc::localhost:12345/DCPSInfoRepo"
            TheServiceParticipant->set_repo_ior("corbaloc::0.0.0.0:5000/DCPSInfoRepo");

            auto inst = registry->create_inst("tcp1", "tcp");
            config->instances_.push_back(inst);
        }
        else if (TYPE == CTYPE::UDP) {

            // address = "0.0.0.0:5000"
            TheServiceParticipant->set_repo_ior("corbaloc::0.0.0.0:5000/DCPSInfoRepo");

            auto inst = registry->create_inst("udp1", "udp");

            if (auto* udp = dynamic_cast<OpenDDS::DCPS::UdpInst*>(inst.in())) {
                udp->local_address(ADDR);
            }

            config->instances_.push_back(inst);
        }
        else if (TYPE == CTYPE::RTSP_UDP) {

            // address = "0.0.0.0:7400"
            auto inst = registry->create_inst("rtps1", "rtps_udp");

            if (auto* rtps = dynamic_cast<OpenDDS::DCPS::RtpsUdpInst*>(inst.in())) {
                OpenDDS::DCPS::NetworkAddress addr(ADDR);
                rtps->local_address(addr);
            }
            config->instances_.push_back(inst);
        }

        registry->global_config(config);

        dpf = TheParticipantFactory;

        participant = dpf->create_participant(DOMAINE_ID,
                                           PARTICIPANT_QOS_DEFAULT,
                                           DDS::DomainParticipantListener::_nil(),
                                           ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(participant.in())){
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DomainParticipant creation failed.\n")), 1);
            return false;
        }

        subscriber =participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                          DDS::SubscriberListener::_nil(),
                                          ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
       if (CORBA::is_nil(subscriber.in())){
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Subscriber creation failed.\n")), 1);
           return false;
       }

       mts = new Messager::MessageTypeSupportImpl();
          if (mts->register_type(participant.in(), "") != DDS::RETCODE_OK){
              ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Failed to register type.\n")), 1);

          }

          // Create topic
          DDS::TopicQos topic_qos;
          participant->get_default_topic_qos(topic_qos);
          exchange_evt_topic =
              participant->create_topic(EXCHANGE_EVT_TOPIC_NAME,
                                        mts->get_type_name(),
                                        topic_qos,
                                        DDS::TopicListener::_nil(),
                                        ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

          if (CORBA::is_nil(exchange_evt_topic.in()))
              ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Exchange Event Topic creation failed.\n")), 1);

          // Create DataReader with listener
          DDS::DataReaderQos dr_qos;
          subscriber->get_default_datareader_qos(dr_qos);
          MessageReader = new MessageReaderListener;
          DDS::DataReaderListener_var listener(MessageReader) ;

          exchange_evt_reader =
              subscriber->create_datareader(exchange_evt_topic.in(),
                                            dr_qos,
                                            listener.in(),
                                            ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

          if (CORBA::is_nil(exchange_evt_reader.in()))
              ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DataReader creation failed.\n")), 1);

          ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: Subscriber running. Waiting for data...\n")));
          // while (true) {
          //     ACE_OS::sleep(1);
          // }
      }
      catch (const CORBA::Exception& ex) {
          ex._tao_print_exception("ERROR: Exception caught:");
          return 1;
    }
}

bool OpenDDSSubscriber::recieveMessage(Messager::Message message)
{
    return false;

}
