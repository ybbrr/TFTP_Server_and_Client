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

    bool status = client->create_socket();

    if (!status)
    {
        client.reset(client.get());
        return 1;
    }

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

    bool status = server->create_socket();

    if (!status)
    {
        server.reset(server.get());
        return 1;
    }

    status = server->bind_socket();

    if (!status)
    {
        server.reset(server.get());
        return 1;
    }

    // Server always awake with its file transfer directory
    server->wait_for_a_request(R"(D:\ABC)");

    return 0;
}
```
