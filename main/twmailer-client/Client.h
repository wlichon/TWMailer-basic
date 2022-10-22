#include <arpa/inet.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class Client{
    public:
        virtual int conn(std::string IP, int port) = 0;
};


class MailClient : public Client{
    public:
        int conn(std::string IP,int port);
};