#include "Server.h"
#include <sstream>

int main(int argc, char const* argv[]){
    if(argc != 3){
        printf("Wrong CLI input\n");
        return 1;
    }

    std::string directoryName;
    directoryName = argv[2];
    
    std::stringstream intValue(argv[1]);
    int port = 0;
    intValue >> port;


    if(!port){
        printf("Port number error\n");
        return 1;
    }
    
    Server* serv = new MailServer();
    serv->start(port, directoryName);

    return 0;
}