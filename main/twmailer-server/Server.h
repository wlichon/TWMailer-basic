#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>

class Server{
    public:
        virtual int start(int port, std::string directoryName) = 0;
};

class MailServer : public Server{
    public:
        int start(int port, std::string directoryName);
};