# TFTP_Server_and_Client

Fully functional, high performance TFTP Server and Client applications.
It is Windows and Linux compatible cross-platform project.

## How to Compile

### Windows

Run `build.bat`

### Linux

Run `sh build.sh`

## Examples

### Client Side Usage

```c++
int main()
{
    const std::unique_ptr<YB::TFTPClient> client{new YB::TFTPClient()};

    client->create_socket("127.0.0.1", 1234);

    const std::string file_name = "name_of_the_file_you_want_to_send_or_pull.txt";;
    const std::string directory = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)";

    // If you want to send a file to Server use below.
    client->send_file(directory + file_name);

    // If you want to pull a file from Server use below.
    //client->receive_file(directory + file_name);

    return 0;
}
```

### Server Side Usage

```c++
int main()
{
    const std::unique_ptr<YB::TFTPServer> server{new YB::TFTPServer()};

    server->create_socket();
    server->bind_socket("127.0.0.1", 1234);

    // Server always awake with its file transfer directory
    const std::string root_dir = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)"; 
    server->wait_for_a_request(root_dir);

    return 0;
}
```
