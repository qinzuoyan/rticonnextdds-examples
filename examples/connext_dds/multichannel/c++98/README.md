# Example Code: MultiChannel

## Building the Example :wrench:

To build this example, first run CMake to generate the corresponding build
files. We recommend you use a separate directory to store all the generated
files (e.g., ./build).

```sh
mkdir build
cd build
cmake ..
```

Once you have run CMake, you will find a number of new files in your build
directory (the list of generated files will depend on the specific CMake
Generator). To build the example, run CMake as follows:

```sh
cmake --build .
```

**Note**: if you are using a multi-configuration generator, such as Visual
Studio solutions, you can specify the configuration mode to build as follows:

```sh
cmake --build . --config Release|Debug
```

Alternatively, you can use directly the generated infrastructure (e.g.,
Makefiles or Visual Studio Solutions) to build the example. If you generated
Makefiles in the configuration process, run make to build the example. Likewise,
if you generated a Visual Studio solution, open the solution and follow the
regular build process.

## Running the Example

In two separate command prompt windows for the publisher and subscriber. Run the
following commands from the example directory (this is necessary to ensure the
application loads the QoS defined in *USER_QOS_PROFILES.xml*):

On *Windows* systems run:

```sh
market_data_publisher.exe  -d <domain_id> -s <samples_to_send>
market_data_subscriber.exe -d <domain_id> -s <samples_to_receive>
```

On *UNIX* systems run:

```sh
./market_data_publisher  -d <domain_id> -s <samples_to_send>
./market_data_subscriber -d <domain_id> -s <samples_to_receive>
```

The applications accept up to two arguments:

1.  The `<domain_id>`. Both applications must use the same domain ID in order to
    communicate. The default is 0.

2.  How long the examples should run, measured in samples. The default is
    infinite.

## Wireshark Output

-   **multichannel.pcap**: Observe that data samples are being sent on the wire
    in only the channels that are necessary for delivering samples that pass the
    reader's filter.

-   **singlechannel.pcap**: Shows the wire traffic when multichannel is not used
    and when the reader specifies a `multicast.receive_address` of
    `239.255.0.1`. Observe that all data samples are being sent on the wire,
    i.e. reader-side filtering, since there is only one channel.

## Publisher Output

Nothing

## Subscriber Output

```plaintext
filter is Symbol MATCH 'A'

   Symbol: "A"
   Price: 26.000000

   Symbol: "A"
   Price: 52.000000

   Symbol: "A"
   Price: 78.000000

   Symbol: "A"
   Price: 104.000000
changed filter to Symbol MATCH 'A,D'

   Symbol: "A"
   Price: 130.000000

   Symbol: "D"
   Price: 133.000000

   Symbol: "A"
   Price: 156.000000

   Symbol: "D"
   Price: 159.000000

   Symbol: "A"
   Price: 182.000000

   Symbol: "D"
   Price: 185.000000

   Symbol: "A"
   Price: 208.000000

   Symbol: "D"
   Price: 211.000000

   Symbol: "A"
   Price: 234.000000

   Symbol: "D"
   Price: 237.000000
changed filter to Symbol MATCH 'D'

   Symbol: "D"
   Price: 263.000000

   Symbol: "D"
   Price: 289.000000

   Symbol: "D"
   Price: 315.000000

   Symbol: "D"
   Price: 341.000000

   Symbol: "D"
   Price: 367.000000

   Symbol: "D"
   Price: 393.000000
```

## Customizing the Build

### Configuring Build Type and Generator

By default, CMake will generate build files using the most common generator for
your host platform (e.g., Makefiles on Unix-like systems and Visual Studio
solution on Windows), \. You can use the following CMake variables to modify the
default behavior:

-   `-DCMAKE_BUILD_TYPE` -- specifies the build mode. Valid values are Release
    and Debug. See the [CMake documentation for more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

-   `-DBUILD_SHARED_LIBS` -- specifies the link mode. Valid values are ON for
    dynamic linking and OFF for static linking. See [CMake documentation for
    more details.
    (Optional)](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

-   `-G` -- CMake generator. The generator is the native build system to use
    build the source code. All the valid values are described described in the
    CMake documentation [CMake Generators
    Section.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

For example, to build a example in Debug/Static mode run CMake as follows:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON .. -G "Visual Studio 15 2017" -A x64
```

### Configuring Connext DDS Installation Path and Architecture

The CMake build infrastructure will try to guess the location of your Connext
DDS installation and the Connext DDS architecture based on the default settings
for your host platform.If you installed Connext DDS in a custom location, you
can use the CONNEXTDDS_DIR variable to indicate the path to your RTI Connext DDS
installation folder. For example:

```sh
cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z ..
```

Also, If you installed libraries for multiple target architecture on your system
(i.e., you installed more than one target rtipkg), you can use the
CONNEXTDDS_ARCH variable to indicate the architecture of the specific libraries
you want to link against. For example:

```sh
cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0 ..
```

### CMake Build Infrastructure

The CMakeListst.txt script that builds this example uses a generic CMake
function called connextdds_add_example that defines all the necessary constructs
to:

1.  Run RTI Code Generator to generate the serialization/deserialization code
    for the types defined in the IDL file associated with the example.

2.  Build the corresponding Publisher and Subscriber applications.

3.  Copy the USER_QOS_PROFILES.xml file into the directory where the publisher
    and subscriber executables are generated.

You will find the definition of connextdds_add_example, along with detailed
documentation, in
[resources/cmake/ConnextDdsAddExample.cmake](../../../../resources/cmake/ConnextDdsAddExample.cmake).

For a more comprehensive example on how to build an RTI Connext DDS application
using CMake, please refer to the
[hello_world](../../../connext_dds/build_systems/cmake/) example, which includes
a comprehensive CMakeLists.txt script with all the steps and instructions
described in detail.
