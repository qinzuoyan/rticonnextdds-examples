/* asyncPublisher.java

 A publication of data of type async

 This file is derived from code automatically generated by the rtiddsgen 
 command:

 rtiddsgen -language java -example <arch> .idl

 Example publication of type async automatically generated by 
 'rtiddsgen' To test them follow these steps:

 (1) Compile this file and the example subscription.

 (2) Start the subscription with the command
 java asyncSubscriber <domain_id> <sample_count>

 (3) Start the publication with the command
 java asyncPublisher <domain_id> <sample_count>

 (4) [Optional] Specify the list of discovery initial peers and 
 multicast receive addresses via an environment variable or a file 
 (in the current working directory) called NDDS_DISCOVERY_PEERS.  

 You can run any number of publishers and subscribers programs, and can 
 add and remove them dynamically from the domain.

 Example:

 To run the example application on domain <domain_id>:

 Ensure that $(NDDSHOME)/lib/<arch> is on the dynamic library path for
 Java.                       

 On Unix: 
 add $(NDDSHOME)/lib/<arch> to the 'LD_LIBRARY_PATH' environment
 variable

 On Windows:
 add %NDDSHOME%\lib\<arch> to the 'Path' environment variable


 Run the Java applications:

 java -Djava.ext.dirs=$NDDSHOME/class asyncPublisher <domain_id>

 java -Djava.ext.dirs=$NDDSHOME/class asyncSubscriber <domain_id>        



 modification history
 ------------ -------         
 */

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;

import com.rti.dds.domain.*;
import com.rti.dds.infrastructure.*;
import com.rti.dds.publication.*;
import com.rti.dds.topic.*;
import com.rti.ndds.config.*;

// ===========================================================================

public class asyncPublisher {
    // -----------------------------------------------------------------------
    // Public Methods
    // -----------------------------------------------------------------------

    public static void main(String[] args) {
        // --- Get domain ID --- //
        int domainId = 0;
        if (args.length >= 1) {
            domainId = Integer.valueOf(args[0]).intValue();
        }

        // -- Get max loop count; 0 means infinite loop --- //
        int sampleCount = 0;
        if (args.length >= 2) {
            sampleCount = Integer.valueOf(args[1]).intValue();
        }

        /*
         * Uncomment this to turn on additional logging
         * Logger.get_instance().set_verbosity_by_category(
         * LogCategory.NDDS_CONFIG_LOG_CATEGORY_API,
         * LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
         */

        // --- Run --- //
        publisherMain(domainId, sampleCount);
    }

    // -----------------------------------------------------------------------
    // Private Methods
    // -----------------------------------------------------------------------

    // --- Constructors: -----------------------------------------------------

    private asyncPublisher() {
        super();
    }

    // -----------------------------------------------------------------------

    private static void publisherMain(int domainId, int sampleCount) {

        DomainParticipant participant = null;
        Publisher publisher = null;
        Topic topic = null;
        asyncDataWriter writer = null;

        try {
            // --- Create participant --- //

            /*
             * To customize participant QoS, use the configuration file
             * USER_QOS_PROFILES.xml
             */

            participant = DomainParticipantFactory.TheParticipantFactory
                    .create_participant(domainId,
                            DomainParticipantFactory.PARTICIPANT_QOS_DEFAULT,
                            null /* listener */, StatusKind.STATUS_MASK_NONE);
            if (participant == null) {
                System.err.println("create_participant error\n");
                return;
            }

            // --- Create publisher --- //

            /*
             * To customize publisher QoS, use the configuration file
             * USER_QOS_PROFILES.xml
             */

            publisher = participant.create_publisher(
                    DomainParticipant.PUBLISHER_QOS_DEFAULT,
                    null /* listener */, StatusKind.STATUS_MASK_NONE);
            if (publisher == null) {
                System.err.println("create_publisher error\n");
                return;
            }

            // --- Create topic --- //

            /* Register type before creating topic */
            String typeName = asyncTypeSupport.get_type_name();
            asyncTypeSupport.register_type(participant, typeName);

            /*
             * To customize topic QoS, use the configuration file
             * USER_QOS_PROFILES.xml
             */

            topic = participant.create_topic("Example async", typeName,
                    DomainParticipant.TOPIC_QOS_DEFAULT, null /* listener */,
                    StatusKind.STATUS_MASK_NONE);
            if (topic == null) {
                System.err.println("create_topic error\n");
                return;
            }

            // --- Create writer --- //

            /*
             * To customize data writer QoS, use the configuration file
             * USER_QOS_PROFILES.xml
             */

            writer = (asyncDataWriter) publisher.create_datawriter(topic,
                    Publisher.DATAWRITER_QOS_DEFAULT, null, // listener
                    StatusKind.STATUS_MASK_NONE);
            if (writer == null) {
                System.err.println("create_datawriter error\n");
                return;
            }

            /*
             * If you want to change the DataWriter's QoS programmatically
             * rather than using the XML file, you will need to add the
             * following lines to your code and comment out the
             * create_datawriter call above.
             * 
             * In this case, we set the publish mode qos to asynchronous publish
             * mode, which enables asynchronous publishing. We also set the flow
             * controller to be the fixed rate flow controller, and we increase
             * the history depth.
             */

            // // Start changes for Asynchronous_Publication

            // --- Create writer --- //

            /* Get default datawriter QoS to customize */
            // DataWriterQos datawriter_qos = new DataWriterQos();
            // publisher.get_default_datawriter_qos(datawriter_qos);
            //
            // // Since samples are only being sent once per second, datawriter
            // // will need to keep them on queue. History defaults to only
            // // keeping the last sample enqueued, so we increase that here.
            // datawriter_qos.history.depth = 12;
            //
            // // Set flowcontroller for datawriter
            // datawriter_qos.publish_mode.kind =
            // PublishModeQosPolicyKind.ASYNCHRONOUS_PUBLISH_MODE_QOS;
            // datawriter_qos.publish_mode.flow_controller_name =
            // FlowController.FIXED_RATE_FLOW_CONTROLLER_NAME;
            //
            // /* To create datawriter with default QoS, use
            // Publisher.DATAWRITER_QOS_DEFAULT instead of datawriter_qos */
            // writer = (asyncDataWriter)
            // publisher.create_datawriter(
            // topic, datawriter_qos,
            // null /* listener */, StatusKind.STATUS_MASK_NONE);
            // // End changes for Asynchronous_Publication
            // --- Write --- //

            /* Create data sample for writing */
            async instance = new async();

            InstanceHandle_t instance_handle = InstanceHandle_t.HANDLE_NIL;
            /*
             * For a data type that has a key, if the same instance is going to
             * be written multiple times, initialize the key here and register
             * the keyed instance prior to writing
             */
            // instance_handle = writer.register_instance(instance);

            final long sendPeriodMillis = 500; // 1 sample per 0.5 second

            for (int count = 0; (sampleCount == 0) || (count < sampleCount);
                    ++count) {
                System.out.println("Writing async, count " + count);

                /* Modify the instance to be written here */
                instance.x = count;

                /* Write data */
                writer.write(instance, instance_handle);
                try {
                    Thread.sleep(sendPeriodMillis);
                } catch (InterruptedException ix) {
                    System.err.println("INTERRUPTED");
                    break;
                }
            }

            /*
             * The publisher have to sleep while the publisher get the last
             * sample
             */
            try {
                Thread.sleep(1000); // sleep 1 sec
            } catch (InterruptedException ix) {
                System.err.println("INTERRUPTED");
            }

            // writer.unregister_instance(instance, instance_handle);

        } finally {

            // --- Shutdown --- //

            if (participant != null) {
                participant.delete_contained_entities();

                DomainParticipantFactory.TheParticipantFactory
                        .delete_participant(participant);
            }
            /*
             * RTI Connext provides finalize_instance() method for people who
             * want to release memory used by the participant factory singleton.
             * Uncomment the following block of code for clean destruction of
             * the participant factory singleton.
             */
            // DomainParticipantFactory.finalize_instance();
        }
    }
}
