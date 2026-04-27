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


/**
 * @brief Helper that manages OpenDDS transport, entities, and pub/sub lifecycle.
 */
class OpenDDSHelper {
public:
	/**
	 * @brief Constructs a helper with no active DDS entities.
	 */
	OpenDDSHelper();
	/**
	 * @brief Destroys the helper and disconnects active DDS entities.
	 */
	~OpenDDSHelper();

    /**
     * @brief Supported transport types.
     */
    enum CTYPE {
        /** TCP transport via InfoRepo discovery. */
        TCP,
        /** UDP transport via InfoRepo discovery. */
        UDP,
        /** RTPS-UDP transport using RTPS discovery. */
        RTPS_UDP
    };

    /**
     * @brief Initializes publisher entities using a configuration argument set.
     * @param config Configuration source or file identifier.
     * @param DOMAINE_ID DDS domain id.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name used for message exchange.
     * @return true on success, false otherwise.
     */
    bool initPubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Initializes publisher entities for the selected transport.
     * @param address Transport endpoint (for example host:port).
     * @param TYPE Transport type.
     * @param DOMAINE_ID DDS domain id.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name used for message exchange.
     * @return true on success, false otherwise.
     */
    bool initPublisher(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Publishes a single message sample.
     * @param message Message to write.
     * @return true if the write succeeds, false otherwise.
     */
    bool publishMessage(const Messager::Message message);

    /**
     * @brief Initializes subscriber entities using a configuration argument set.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name used for message exchange.
     * @return true on success, false otherwise.
     */
    bool createSubscriberAndDataReader(const char* EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Initializes subscriber entities using a configuration argument set.
     * @param config Configuration source or file identifier.
     * @param DOMAINE_ID DDS domain id.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name used for message exchange.
     * @return true on success, false otherwise.
     */
    bool initSubWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);


    /**
     * @brief Initializes subscriber entities for the selected transport.
     * @param address Transport endpoint (for example host:port).
     * @param TYPE Transport type.
     * @param DOMAINE_ID DDS domain id.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name used for message exchange.
     * @return true on success, false otherwise.
     */
    bool initSubscriber(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Gets the active message reader listener.
     * @return Pointer to listener or nullptr when subscriber is not initialized.
     */
    MessageReaderListener* getMessageReaderListener() const {
        return MessageReader;
    }
    
    /**
     * @brief Disconnects and releases all active DDS entities.
     */
    void dissConnect();

protected:
    /**
     * @brief Configures transport and discovery settings.
     * @param address Transport endpoint (for example host:port).
     * @param TYPE Transport type.
     * @return true on success, false otherwise.
     */
    bool configureTransport(const char* address, CTYPE TYPE);

    /**
     * @brief Creates the domain participant.
     * @param domainId DDS domain id.
     * @return true on success, false otherwise.
     */
    bool createDomainParticipant(int domainId);

    /**
     * @brief Registers type support and creates the topic.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name.
     * @return true on success, false otherwise.
     */
    bool createTopic(const char* EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Creates publisher and data writer entities.
     * @param EXCHANGE_EVT_TOPIC_NAME Topic name.
     * @return true on success, false otherwise.
     */
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
    MessageReaderListener* MessageReader = nullptr;
    DDS::DataReaderListener_var data_reader_listener;
    DDS::DataReader_var exchange_evt_data_reader;

};
