#include "Client.h"
#include <iostream>
#include <cstring>
#include <string>


static ssize_t readline (int fd, char* vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c; 
    char* ptr;
    ptr = (char*)vptr;
    for (n = 1 ; n < maxlen ; n++) {
        again:
        if ( (rc = read(fd,&c,1)) == 1) {
            *ptr++ = c ;
            if (c == '\n')
                break ; // newline ist stored, like fgets()
        } else if (rc == 0) {
            if (n == 1)
                return (0) ; // EOF, no data read
            else
                break ; // EOF, some data was read
        } else {
        if (errno == EINTR)
        goto again;
        return (-1); // error, errno set by read()
        } ;
    } ;
    *ptr = 0 ; // null terminate like fgets()
    return (n) ;
} 

int MailClient::conn(std::string IP, int port){
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, IP.c_str(), &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd
        = connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    //valread = readline(sock,)
    //std::cout << buffer << std::endl;
    std::string command;
    std::string msg;
    std::string line;

    std::getline(std::cin,command);
    if(command == "SEND"){
        std::getline(std::cin,msg,'.');

    }
    else if(command == "LIST"){
        std::getline(std::cin,msg);
    }
    else if(command == "READ"){
        std::getline(std::cin,line);
        msg += line + '\n';
        std::getline(std::cin,line);
        msg += line + '\n';
    }

    msg = command + '\n' + msg;

    //std::cout << msg;

    //std::cout << msg << std::endl;
    send(sock, msg.c_str(), msg.length(), 0);
    //printf("Hello message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);

    // closing the connected socket
    close(client_fd);
    return 0;
}
