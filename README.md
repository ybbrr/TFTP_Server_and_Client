# TFTP_Sever_and_Client

Fully functional, high performance TFTP Server and Client applications.

## How to Compile

Run `build.bat`

## Examples

### Client Side Usage

```
int main()
{
    std::unique_ptr<YB::TFTPClient> client{new YB::TFTPClient()};

    client->create_socket();

    std::string file_name = "name_of_the_file_you_want_to_send_or_pull.txt";
    std::string directory = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)";

    // If you want to send a file to Server use below.
    client->send_data(directory + file_name);

    // If you want to pull a file from Server use below.
    // client->receive_data(directory + file_name);

    return 0;
}
```

### Server Side Usage

```
int main()
{
    std::unique_ptr<YB::TFTPServer> server{new YB::TFTPServer()};

    server->create_socket();
    server->bind_socket();

    // Server always awake with its file transfer directory
    const std::string root_dir = R"(D:\root_directory)"; 
    server->wait_for_a_request(root_dir);

    return 0;
}
```
