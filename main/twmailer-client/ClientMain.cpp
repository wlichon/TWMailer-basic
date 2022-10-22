#include "Client.h"
#include <string>
#include <sstream>
#include <iostream>

int main(int argc, char const* argv[])
{ 
    if(argc != 3){
        printf("Wrong CLI input\n");
        return 1;
    }
    
    std::string IP = argv[1];
    std::stringstream intValue(argv[2]);
    int port = 0;
    intValue >> port;
    if(!port){
        printf("Port number error\n");
        return 1;
    }

    //std::cout << port << IP;

    Client* client = new MailClient();
    client->conn(IP, port);
    
    return 0;
}