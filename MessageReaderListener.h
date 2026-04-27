#pragma once
#include "idl/MessagerTypeSupportC.h"
#include <functional>
#include <dds/DCPS/LocalObject.h>
#include <dds/DCPS/Marked_Default_Qos.h>

/**
 * @brief DataReader listener that forwards received messages to a Qt-safe callback.
 */
class MessageReaderListener
    : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener>
{
public:
    /** Callback type invoked for each valid received sample. */
    using MessageCallback = std::function<void(const Messager::Message&)>;

    /**
     * @brief Sets the callback invoked for each valid message sample.
     * @param callback Function called when data arrives.
     */
    void setMessageCallback(const MessageCallback& callback) { messageCallback_ = callback; }

    /**
     * @brief Called by DDS when data is available on the attached reader.
     * @param reader DataReader that contains message samples.
     */
    void on_data_available(DDS::DataReader_ptr reader) override;

    void on_requested_deadline_missed(
        DDS::DataReader_ptr,
        const DDS::RequestedDeadlineMissedStatus&) override {}

    void on_requested_incompatible_qos(
        DDS::DataReader_ptr,
        const DDS::RequestedIncompatibleQosStatus&) override {}

    void on_sample_rejected(
        DDS::DataReader_ptr,
        const DDS::SampleRejectedStatus&) override {}

    void on_liveliness_changed(
        DDS::DataReader_ptr,
        const DDS::LivelinessChangedStatus&) override {}

    void on_subscription_matched(
        DDS::DataReader_ptr,
        const DDS::SubscriptionMatchedStatus&) override {}

    void on_sample_lost(
        DDS::DataReader_ptr,
        const DDS::SampleLostStatus&) override {}

private:
    /** User-provided callback invoked for valid samples. */
    MessageCallback messageCallback_;
};
