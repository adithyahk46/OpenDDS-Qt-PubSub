#ifndef OPENDDSPUBLISHER_H
#define OPENDDSPUBLISHER_H

#include <string>
#include "../idl/MessagerTypeSupportImpl.h"
#include <dds/DCPS/Service_Participant.h>


class OpenDDSPublisher
{
public:
    OpenDDSPublisher();

    ~OpenDDSPublisher();

    enum CTYPE{
        TCP,
        UDP,
        RTSP_UDP
    };

    bool CreatePublisherWithArgs(std::string config, int DOMAINE_ID, const char *EXCHANGE_EVT_TOPIC_NAME);

    /**
     * @brief Creates an OpenDDS Publisher with the specified transport.
     *
     * Configures transport (TCP, UDP, or RTPS_UDP) in code, creates a DomainParticipant,
     * registers the type, and initializes a Publisher with a DataWriter for the given topic.
     *
     * @param address   Transport-specific address:
     *                  - TCP      : InfoRepo IOR (e.g., "corbaloc::localhost:12345/DCPSInfoRepo")
     *                  - UDP      : Local address in "ip:port" format (e.g., "0.0.0.0:5000")
     *                  - RTPS_UDP : Local address in "ip:port" format (e.g., "0.0.0.0:7400")
     * @param type      Transport type (TCP, UDP, RTPS_UDP)
     * @param domainId  DDS domain ID (must match subscriber)
     * @param topicName Topic name for publishing data(must match subscriber)
     *
     * @return true if successful, false otherwise
     */
    bool CreatePublisher(const char* address, CTYPE type, int domainId, const char* EXCHANGE_EVT_TOPIC_NAME);

    bool publishMessage(const Messager::Message message);

    void cleanup();
private:
    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    Messager::MessageDataWriter_var writer;
};

#endif // OPENDDSPUBLISHER_H
