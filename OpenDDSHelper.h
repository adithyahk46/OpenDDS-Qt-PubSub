#pragma once

#include <functional>
#include <string>

#include "idl/MessagerTypeSupportImpl.h"

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>

#include <dds/DCPS/transport/framework/TransportRegistry.h>
#include <dds/DCPS/transport/tcp/TcpInst.h>
#include <dds/DCPS/transport/udp/UdpInst.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdpInst.h>
#include <dds/DCPS/NetworkAddress.h>

#include "MessageReaderListener.h"


class OpenDDSHelper {
public:
	OpenDDSHelper();
	~OpenDDSHelper();

    enum CTYPE {
        TCP,
        UDP,
        RTPS_UDP
    };

    bool initPubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);
    bool initPublisher(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    bool publishMessage(const Messager::Message message);


    bool initSubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);
    bool initSubscriber(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    MessageReaderListener* getMessageReaderListener() const {
        return MessageReader;
    }
    
    bool dissConnect();

    bool cleanup();


protected:
    bool configureTransport(const char* address, CTYPE TYPE);

    bool createDomainParticipant(int domainId);

    bool createTopic(const char* EXCHANGE_EVT_TOPIC_NAME);

    bool createPublisherAndDataWriter(const char* EXCHANGE_EVT_TOPIC_NAME);

private:

    OpenDDS::DCPS::TransportRegistry* registry;
    OpenDDS::DCPS::TransportConfig_rch config;

    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    DDS::Topic_var exchange_evt_topic;

    Messager::MessageTypeSupport_var mts;


    DDS::Publisher_var publisher;
    DDS::DataWriter_var exchange_evt_data_writer;
    Messager::MessageDataWriter_var writer;

    DDS::Subscriber_var subscriber;
    DDS::DataReader_var exchange_evt_data_reader;
    DDS::DataReaderListener_var data_reader_listener;
    MessageReaderListener* MessageReader = nullptr;




};
