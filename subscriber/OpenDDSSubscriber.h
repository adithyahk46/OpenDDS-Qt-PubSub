#ifndef OPENDDSSUBSCRIBER_H
#define OPENDDSSUBSCRIBER_H

#include <string>

#include "../idl/MessagerTypeSupportImpl.h"
#include "MessageReaderListener.h"
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <ace/streams.h>
#include <ace/OS_NS_unistd.h>

class OpenDDSSubscriber
{
public:
    OpenDDSSubscriber();

    enum CTYPE{
        TCP,
        UDP,
        RTSP_UDP
    };

    bool createSubscriberWithArgs(std::string config, int DOMAINE_ID, const char *EXCHANGE_EVT_TOPIC_NAME);

    bool createSubscriber(const char* ADDR, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    bool recieveMessage(Messager::Message message);

private:
    //DDS::DomainId_t domain_id = 42;
    //const char* EXCHANGE_EVT_TOPIC_NAME = "ExchangeEventTopic";

    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    DDS::Subscriber_var subscriber;
    Messager::MessageTypeSupport_var mts;
    DDS::Topic_var exchange_evt_topic;
    DDS::DataReaderQos dr_qos;

    MessageReaderListener* MessageReader = nullptr;
    DDS::DataReaderListener_var listener;
    DDS::DataReader_var exchange_evt_reader;

};

#endif // OPENDDSSUBSCRIBER_H
