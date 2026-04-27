## Project Overview
**OpenDDS-Qt-PubSub** this project demonstrates a complete publish-subscribe messaging system using **OpenDDS and Qt**. It consists of two main applications: a Publisher that sends messages through a Qt GUI interface, and a Subscriber that receives and displays messages in real-time through another Qt GUI. The system supports multiple transport protocols **(TCP, UDP, RTPS-UDP)** and uses an **IDL**-defined message type for structured communication.

### Core Components

#### **OpenDDSHelper** (`OpenDDSHelper.h/cpp`)
Centralized helper class for managing OpenDDS entities and operations:

**Key Features:** Transport types
```cpp
enum CTYPE {
    TCP,        // TCP transport via InfoRepo discovery
    UDP,        // UDP transport via InfoRepo discovery
    RTPS_UDP    // RTPS-UDP transport using RTPS discovery
};
```
**Initialization Example:** with programmatic parameters:
```cpp
OpenDDSHelper::initPublisher(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);
OpenDDSHelper::initSubscriber(const char* address, CTYPE TYPE, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);

```
**Initialization Example:** with configuration file:
```cpp
   OpenDDSHelper::initPublisherWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);
   OpenDDSHelper::initSubscriberWithArgs(std::string config, int DOMAINE_ID, const char* EXCHANGE_EVT_TOPIC_NAME);
```
**Publish Message:**
```cpp
   OpenDDSHelper::publishMessage(const Messager::Message message);
```
**Receive Messages:**
- Subscriber uses a callback pattern via `MessageReaderListener` to handle incoming messages asynchronously.
```cpp
   // In MessageReaderListener.cpp
   void on_data_available(DDS::DataReader_ptr reader) {
       // Read message and update GUI
   }
```
or set a callback function 
```cpp
MessageReaderListener* listener = subscriber->getMessageReaderListener();
        if (listener) {
            listener->setMessageCallback([this](const Messager::Message& msg)
            {
                // Handle the received message (e.g., update GUI)
            });
        }
```

## Architecture
Publisher Application:
main.cpp → MainWindow (Qt GUI) → OpenDDSHelper (DDS management) → DataWriter (publishing messages)

Subscriber Application:
main.cpp → MainWindow (Qt GUI) → OpenDDSHelper (DDS management) → DataReader (receiving messages) → MessageReaderListener (callback for new messages)

## How To Build this project
------------------------------

### Core Dependencies
- **OpenDDS 3.33.0**: Open-source DDS( [Link to OpenDDS](https://github.com/OpenDDS/OpenDDS))
- **Qt 6.x / 5.15.x**: GUI framework
- **Windows Visual Studio**: MSVC compiler (tested on vs2022 Windows)
- **CMake 3.16 or later**: Build system

### How to build OpenDDS from source
1. **Clone OpenDDS Repository**
   ```bash
   git clone https://github.com/OpenDDS/OpenDDS.git
   cd OpenDDS
   configure
   setenv
   msbuild -p:Configuration=Debug,Platform=x64 -m DDS_TAOv2.sln
   ```
   if you faced any error during build, please refer to OpenDDS documentation for troubleshooting. or exicute setenv.cmd and try to build again.

2. **How to generate IDL files**
   - Navigate to the `idl/` directory
   - Run the `generate_idl.bat` script with the path to your OpenDDS installation and the IDL file you want to compile:
     ```bash
     cd idl
     generate_idl.bat "C:\path\to\OpenDDS-3.33.0"  "C:\path\to\idl\Messager.idl"
     ```
   - This will generate the necessary type support files for the `Messager::Message` type defined in `Messager.idl`.

3. **Configure Qt Projects**
   - Open the `publisher.pro` and `subscriber.pro` files in Qt Creator
   - Ensure the `OPENDDS_ROOT` variable points to your OpenDDS installation directory
   ```cpp
   OPENDDS_ROOT = C:/path/to/OpenDDS-3.33.0
   IDLNAME = Messager  # Name of the your IDL file without extension
   IDLFILESOURCE = ../idl # Path to the directory containing generated IDL files
   ```

4. ** How to connect Publisher and Subscriber**
   - Start the DCPSInfoRepo for InfoRepo discovery (for TCP/UDP transport):
     ```bash
     cd C:\path\to\OpenDDS-3.33.0
     setenv.cmd
     ```
     To test the TCP transport, run the following command to start the DCPSInfoRepo service:
     ```bash
     $DDS_ROOT/dds/InfoRepo/DCPSInfoRepo -ORBEndpoint iiop://localhost:12345 -d domain_ids
     ```
     This will start the InfoRepo service on localhost at port 12345, which both the Publisher and Subscriber will use for discovery when configured for TCP transport.

     For UDP transport,
      ```bash
      $DDS_ROOT/dds/InfoRepo/DCPSInfoRepo -ORBEndpoint iiop://0.0.0.0:12345 -d domain_ids
      ```
      This will start the InfoRepo service on all interfaces at port 12345, which both the Publisher and Subscriber will use for discovery when configured for UDP transport.

      for RTPS-UDP transport, no central InfoRepo service is required, so you can skip this step. Do not start the DCPSInfoRepo service when using RTPS-UDP transport, as it relies on decentralized peer-to-peer discovery.

## Implemented Features

### ✅ Completed Features

1. **OpenDDS Helper Library**
   - Transport abstraction layer
   - Publisher/Subscriber initialization
   - Message publishing interface
   - Graceful disconnection and cleanup
   - Support for both config file and programmatic initialization

2. **Publisher Application**
   - Qt6 GUI for message composition
   - Transport selection (TCP/UDP/RTPS-UDP)
   - Domain ID and topic configuration
   - Message sending with sender identification
   - Automatic timestamp generation

3. **Subscriber Application**
   - Qt6 GUI for real-time message display
   - Transport selection (TCP/UDP/RTPS-UDP)
   - Domain ID and topic configuration
   - Live message reception via callback pattern
   - Message history display

4. **Transport Support**
   - **TCP Transport**: Reliable point-to-point communication via InfoRepo discovery
   - **UDP Transport**: Lower-latency datagram-based messaging via InfoRepo discovery
   - **RTPS-UDP Transport**: Decentralized RTPS discovery without central InfoRepo service

5. **Type Safety**
   - Strongly-typed message structure via IDL
   - Automatic type code generation
   - Type consistency across pub/sub boundaries

6. **Discovery Mechanisms**
   - **InfoRepo Discovery**: Central repository-based discovery for TCP/UDP
   - **RTPS Discovery**: Peer-to-peer discovery for RTPS-UDP (no central service required)


## License

See [LICENSE](LICENSE) file for licensing information.

## Documentation

For more details, refer to:
- [OpenDDS Documentation](https://opendds.org/)
- [Qt 6 Documentation](https://doc.qt.io/qt-6/)
- [Example Code](https://medium.com/@aryrk/getting-started-with-opendds-everything-you-need-to-know-92ddd3d5bad4)

