#include "OpenDDSPublisher.h"

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/transport/framework/TransportRegistry.h>

#include <dds/DCPS/transport/tcp/TcpInst.h>
#include <dds/DCPS/transport/udp/UdpInst.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdpInst.h>
#include <dds/DCPS/NetworkAddress.h>

#include "../idl/MessagerTypeSupportImpl.h"


OpenDDSPublisher::OpenDDSPublisher()
{

}

OpenDDSPublisher::~OpenDDSPublisher()
{
    if(dpf || participant)
        cleanup();
}

bool OpenDDSPublisher::CreatePublisherWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME )
{
    try {
            // Fake argc/argv
            int argc = 3;

            ACE_TCHAR* argv[] = {
                const_cast<ACE_TCHAR*>("app"),
                const_cast<ACE_TCHAR*>("-DCPSConfigFile"),
                const_cast<ACE_TCHAR*>(config.c_str())
            };

            dpf = TheParticipantFactoryWithArgs(argc, argv);

            participant = dpf->create_participant(DOMAINE_ID,
                                        PARTICIPANT_QOS_DEFAULT,
                                        DDS::DomainParticipantListener::_nil(),
                                        ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

            if (CORBA::is_nil(participant.in())){
                ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DomainParticipant creation failed.\n")), 1);
                cleanup();
                return false;
            }

            // Register type
            Messager::MessageTypeSupport_var mts = new Messager::MessageTypeSupportImpl();
            if (mts->register_type(participant.in(), "") != DDS::RETCODE_OK)
                ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Failed to register type.\n")), 1);
                cleanup();
                return false;

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
                cleanup();
                return false;

            // Create publisher and datawriter
            DDS::Publisher_var publisher =
                participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                              DDS::PublisherListener::_nil(),
                                              ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
            if (CORBA::is_nil(publisher.in()))
                ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Publisher creation failed.\n")), 1);
                cleanup();
                return false;

            DDS::DataWriterQos dw_qos;
            publisher->get_default_datawriter_qos(dw_qos);
            DDS::DataWriter_var exchange_evt_writer =
                publisher->create_datawriter(exchange_evt_topic.in(),
                                             dw_qos,
                                             DDS::DataWriterListener::_nil(),
                                             ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
            if (CORBA::is_nil(exchange_evt_writer.in()))
                ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DataWriter creation failed.\n")), 1);
                cleanup();
                return false;

            writer = Messager::MessageDataWriter::_narrow(exchange_evt_writer.in());
            if (CORBA::is_nil(writer.in()))
                ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Narrowing DataWriter failed.\n")), 1);
                cleanup();
                return false;

            return true;

        }
        catch (const CORBA::Exception& ex) {
            ex._tao_print_exception("ERROR: Exception caught:");
            return false;
        }
}

bool OpenDDSPublisher::CreatePublisher(
        const char* address,
        CTYPE type,
        int domainId,
        const char* EXCHANGE_EVT_TOPIC_NAME)
{
    try {

        auto* registry = OpenDDS::DCPS::TransportRegistry::instance();
        auto config = registry->create_config("config");

        // ============================================
        // 2. Transport Selection
        // ============================================


        std::string addr = std::string("corbaloc::") + address + "/DCPSInfoRepo";
        std::cout << addr<<std::endl;
        if (type == CTYPE::TCP) {

            // address = "corbaloc::localhost:12345/DCPSInfoRepo"
            TheServiceParticipant->set_repo_ior("corbaloc::0.0.0.0:5000/DCPSInfoRepo");

            auto inst = registry->create_inst("tcp1", "tcp");
            config->instances_.push_back(inst);
        }
        else if (type == CTYPE::UDP) {

            // address = "0.0.0.0:5000"
            TheServiceParticipant->set_repo_ior("corbaloc::0.0.0.0:5000/DCPSInfoRepo");

            auto inst = registry->create_inst("udp1", "udp");

            if (auto* udp = dynamic_cast<OpenDDS::DCPS::UdpInst*>(inst.in())) {
                udp->local_address(address);
            }

            config->instances_.push_back(inst);
        }
        else if (type == CTYPE::RTSP_UDP) {

            // address = "0.0.0.0:7400"
            auto inst = registry->create_inst("rtps1", "rtps_udp");

            if (auto* rtps = dynamic_cast<OpenDDS::DCPS::RtpsUdpInst*>(inst.in())) {
                OpenDDS::DCPS::NetworkAddress addr(address);
                rtps->local_address(addr);
            }
            config->instances_.push_back(inst);
        }

        // Apply config globally
        registry->global_config(config);

        // ============================================
        // 3. Participant
        // ============================================
        dpf = TheParticipantFactory;

        participant = dpf->create_participant(
            domainId,
            PARTICIPANT_QOS_DEFAULT,
            DDS::DomainParticipantListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );

        if (CORBA::is_nil(participant.in())){
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DomainParticipant creation failed.\n")), 1);
            return false;
        }

        // ============================================
        // 4. Register Type
        // ============================================
        Messager::MessageTypeSupport_var mts =
            new Messager::MessageTypeSupportImpl();

        if (mts->register_type(participant.in(), "") != DDS::RETCODE_OK)
        {
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Failed to register MessageTypeSupport_var.\n")), 1);
            return false;
        }

        // ============================================
        // 5. Topic
        // ============================================
        DDS::TopicQos topic_qos;
        participant->get_default_topic_qos(topic_qos);

        DDS::Topic_var exchange_evt_topic =
            participant->create_topic(
                EXCHANGE_EVT_TOPIC_NAME,
                mts->get_type_name(),
                topic_qos,
                DDS::TopicListener::_nil(),
                OpenDDS::DCPS::DEFAULT_STATUS_MASK
            );

        if (CORBA::is_nil(exchange_evt_topic.in())){
           ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Exchange Event Topic creation failed.\n")), 1);
            return false;
        }

        // 6. Publisher
        DDS::Publisher_var publisher =
            participant->create_publisher(
                PUBLISHER_QOS_DEFAULT,
                DDS::PublisherListener::_nil(),
                OpenDDS::DCPS::DEFAULT_STATUS_MASK
            );

        if (CORBA::is_nil(publisher.in())){
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Publisher creation failed.\n")), 1);
            return false;
        }

        // 7. DataWriter
        DDS::DataWriterQos dw_qos;
        publisher->get_default_datawriter_qos(dw_qos);

        DDS::DataWriter_var exchange_evt_data_writer =
            publisher->create_datawriter(
                exchange_evt_topic.in(),
                dw_qos,
                DDS::DataWriterListener::_nil(),
                OpenDDS::DCPS::DEFAULT_STATUS_MASK
            );

        if (CORBA::is_nil(exchange_evt_data_writer.in())){
          ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: DataWriter creation failed.\n")), 1);
          return false;
        }

        writer = Messager::MessageDataWriter::_narrow(exchange_evt_data_writer.in());

        if (CORBA::is_nil(writer.in())){
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Narrowing DataWriter failed.\n")), 1);
            return false;
        }

        return true;
    }
    catch (const CORBA::Exception& ex) {
        ex._tao_print_exception("CreatePublisher Exception:");
        return false;
    }
}


bool OpenDDSPublisher::publishMessage(const Messager::Message message)
{
    if (CORBA::is_nil(writer.in())){
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Narrowing DataWriter failed.\n")), 1);
        return false;
    }

    DDS::ReturnCode_t ret = writer->write(message, DDS::HANDLE_NIL);
    if (ret != DDS::RETCODE_OK)
    {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("ERROR: Failed to write message: %d\n"), ret), 1);
        return false;
    }
    return true;
}

void OpenDDSPublisher::cleanup(){
    // Cleanup
    participant->delete_contained_entities();
    dpf->delete_participant(participant.in());
    TheServiceParticipant->shutdown();
}
