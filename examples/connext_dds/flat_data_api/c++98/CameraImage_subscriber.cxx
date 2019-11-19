
/* CameraImage_subscriber.cxx

A subscription example

This file is derived from code automatically generated by the rtiddsgen
command:

rtiddsgen -language C++ -example <arch> CameraImage.idl

Example subscription of type CameraImage automatically generated by
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example publication.

(2) Start the subscription with the command
objs/<arch>/CameraImage_subscriber <domain_id> <sample_count>

(3) Start the publication with the command
objs/<arch>/CameraImage_publisher <domain_id> <sample_count>

(4) [Optional] Specify the list of discovery initial peers and
multicast receive addresses via an environment variable or a file
(in the current working directory) called NDDS_DISCOVERY_PEERS.

You can run any number of publishers and subscribers programs, and can
add and remove them dynamically from the domain.

Example:

To run the example application on domain <domain_id>:

On Unix:

objs/<arch>/CameraImage_publisher <domain_id>
objs/<arch>/CameraImage_subscriber <domain_id>

On Windows:

objs\<arch>\CameraImage_publisher <domain_id>
objs\<arch>\CameraImage_subscriber <domain_id>

*/

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "CameraImage.h"
#include "CameraImageSupport.h"
#include "ndds/ndds_cpp.h"

class CameraImageListener : public DDSDataReaderListener {
public:
    virtual void on_requested_deadline_missed(
            DDSDataReader * /*reader*/,
            const DDS_RequestedDeadlineMissedStatus & /*status*/)
    {
    }

    virtual void on_requested_incompatible_qos(
            DDSDataReader * /*reader*/,
            const DDS_RequestedIncompatibleQosStatus & /*status*/)
    {
    }

    virtual void on_sample_rejected(
            DDSDataReader * /*reader*/,
            const DDS_SampleRejectedStatus & /*status*/)
    {
    }

    virtual void on_liveliness_changed(
            DDSDataReader * /*reader*/,
            const DDS_LivelinessChangedStatus & /*status*/)
    {
    }

    virtual void on_sample_lost(
            DDSDataReader * /*reader*/,
            const DDS_SampleLostStatus & /*status*/)
    {
    }

    virtual void on_subscription_matched(
            DDSDataReader * /*reader*/,
            const DDS_SubscriptionMatchedStatus & /*status*/)
    {
    }

    virtual void on_data_available(DDSDataReader *reader);
};

// Shows how to access a flat data sample (this is the simplest code, for
// a more efficient implementation, see print_average_pixel_fast).
bool print_average_pixel_simple(const CameraImage &sample)
{
    CameraImageConstOffset camera = sample.root();
    if (camera.is_null()) {
        return false;
    }

    rti::flat::SequenceOffset<PixelConstOffset> pixels = camera.pixels();
    if (pixels.is_null()) {
        return false;
    }

    unsigned int pixel_count = pixels.element_count();
    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;

    for (rti::flat::SequenceOffset<PixelConstOffset>::iterator pixels_it =
                 pixels.begin();
         pixels_it != pixels.end();
         ++pixels_it) {
        PixelConstOffset pixel = *pixels_it;
        if (pixel.is_null()) {
            return false;
        }

        red_sum += pixel.red();
        green_sum += pixel.green();
        blue_sum += pixel.blue();
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";

    return true;
}


// Shows how to access a flat data sample in a more efficient way
bool print_average_pixel_fast(const CameraImage &sample)
{
    CameraImageConstOffset camera = sample.root();
    if (camera.is_null()) {
        return false;
    }

    rti::flat::SequenceOffset<PixelConstOffset> pixels = camera.pixels();
    if (pixels.is_null()) {
        return false;
    }

    unsigned int pixel_count = pixels.element_count();

    PixelPlainHelper *pixel_array = rti::flat::plain_cast(pixels);
    if (pixel_array == NULL) {
        return false;
    }

    unsigned int red_sum = 0, green_sum = 0, blue_sum = 0;
    for (unsigned int i = 0; i < pixel_count; i++) {
        const PixelPlainHelper &pixel = pixel_array[i];
        red_sum += pixel.red;
        green_sum += pixel.green;
        blue_sum += pixel.blue;
    }

    std::cout << "Avg. pixel: (" << red_sum / pixel_count << ", "
              << green_sum / pixel_count << ", " << blue_sum / pixel_count
              << ")";

    return true;
}

void CameraImageListener::on_data_available(DDSDataReader *reader)
{
    CameraImageDataReader *CameraImage_reader = NULL;
    CameraImageSeq data_seq;
    DDS_SampleInfoSeq info_seq;
    DDS_ReturnCode_t retcode;
    int i;

    CameraImage_reader = CameraImageDataReader::narrow(reader);
    if (CameraImage_reader == NULL) {
        fprintf(stderr, "DataReader narrow error\n");
        return;
    }

    retcode = CameraImage_reader->take(
            data_seq,
            info_seq,
            DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE,
            DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "take error %d\n", retcode);
        return;
    }

    for (i = 0; i < data_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            CameraImageOffset root = data_seq[i].root();
            if (root.is_null()) {
                fprintf(stderr, "error taking sample root\n");
                return;
            }

            // Print the source name
            rti::flat::StringOffset source = root.source();
            if (!source.is_null()) {
                std::cout << root.source().get_string() << ": ";
            } else {
                std::cout << "(Unknown source): ";
            }

            // Print the field resolution (if it was published)
            ResolutionOffset resolution = root.resolution();
            if (!resolution.is_null()) {
                std::cout << "(Resolution: " << resolution.height() << " x "
                          << resolution.width() << ") ";
            }

            // print_average_pixel_simple(data_seq[i]); // Method 1
            print_average_pixel_fast(data_seq[i]);  // Method 2
            std::cout << std::endl;
        }
    }

    retcode = CameraImage_reader->return_loan(data_seq, info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "return loan error %d\n", retcode);
    }
}

/* Delete all entities */
static int subscriber_shutdown(DDSDomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = DDSTheParticipantFactory->delete_participant(participant);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides the finalize_instance() method on
    domain participant factory for people who want to release memory used
    by the participant factory. Uncomment the following block of code for
    clean destruction of the singleton. */
    /*

    retcode = DDSDomainParticipantFactory::finalize_instance();
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "finalize_instance error %d\n", retcode);
        status = -1;
    }
    */
    return status;
}

extern "C" int subscriber_main(int domainId, int sample_count)
{
    DDSDomainParticipant *participant = NULL;
    DDSSubscriber *subscriber = NULL;
    DDSTopic *topic = NULL;
    CameraImageListener *reader_listener = NULL;
    DDSDataReader *reader = NULL;
    DDS_ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;
    DDS_Duration_t receive_period = { 4, 0 };
    int status = 0;

    /* To customize the participant QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    participant = DDSTheParticipantFactory->create_participant(
            domainId,
            DDS_PARTICIPANT_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        fprintf(stderr, "create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the subscriber QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    subscriber = participant->create_subscriber(
            DDS_SUBSCRIBER_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        fprintf(stderr, "create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic */
    type_name = CameraImageTypeSupport::get_type_name();
    retcode = CameraImageTypeSupport::register_type(participant, type_name);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the topic QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
            "Example CameraImage",
            type_name,
            DDS_TOPIC_QOS_DEFAULT,
            NULL /* listener */,
            DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        fprintf(stderr, "create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Create a data reader listener */
    reader_listener = new CameraImageListener();

    /* To customize the data reader QoS, use
    the configuration file USER_QOS_PROFILES.xml */
    reader = subscriber->create_datareader(
            topic,
            DDS_DATAREADER_QOS_DEFAULT,
            reader_listener,
            DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        fprintf(stderr, "create_datareader error\n");
        subscriber_shutdown(participant);
        delete reader_listener;
        return -1;
    }

    /* Main loop */
    for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {
        printf("CameraImage subscriber sleeping for %d sec...\n",
               receive_period.sec);

        NDDSUtility::sleep(receive_period);
    }

    /* Delete all entities */
    status = subscriber_shutdown(participant);
    delete reader_listener;

    return status;
}

int main(int argc, char *argv[])
{
    int domain_id = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
    set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
    NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return subscriber_main(domain_id, sample_count);
}
