#include "Server.h"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

enum Command{ SEND, LIST, READ, DEL, QUIT, ERR};

static std::vector<std::string> parseMessage(std::string buffer){
    std::string msg = buffer;        
    std::string delimiter = "\n";

    size_t pos = 0;
    std::string token;
    std::vector<std::string> parsedMsg;
    while ((pos = msg.find(delimiter)) != std::string::npos) {
        token = msg.substr(0, pos);
        parsedMsg.push_back(token);
        msg.erase(0, pos + delimiter.length());
    }
    if(msg != ".\n"){
        parsedMsg.push_back(msg);
    }

    return parsedMsg;
}

static Command determineCommand(std::string command){
    
    if(command == "SEND")
        return SEND;
    else if(command == "LIST")
        return LIST;
    else if(command == "READ")
        return READ;
    else if(command == "DEL")
        return DEL;
    else if(command == "QUIT")
        return QUIT;
    else{
        return ERR;
    }
}

static int checkSendValidity(std::vector<std::string>& parsedMsg){
    if(parsedMsg.size() < 5){
        return 1;
    }
    if(parsedMsg[1].length() > 8){
        return 1;
    } 
    if(parsedMsg[2].length() > 8){
        return 1;
    }
    if(parsedMsg[3].length() > 80){
        return 1;
    }

    return 0;
    
}


static int checkListValidity(std::vector<std::string>& parsedMsg){
    if(parsedMsg.size() != 2){
        return 1;
    }
    if(parsedMsg[1].length() > 8){
        return 1;
    }

    return 0;
}

static std::string getAllMessages(std::string dirName, std::string username){
    fs::path fp(dirName + '/' + username);

    //std::cout << fp.string() << std::endl;

    //std::cout << "Here 1" << std::endl;

    std::string messages;
    int messageCnt = 0;
    if(fs::is_directory(fp)){
        std::ifstream myfile;
        
        //std::cout << "Here 2" << std::endl;
        fs::directory_iterator dir(fp);

        for(auto dirEntry : dir){
            std::string message;
            //std::cout << "Here 3" << std::endl;
            //std::cout << dirEntry.path() << std::endl;
            myfile.open (dirEntry.path());
            for(int i = 0; i < 2; i++){
                myfile >> message; //ignorieren die ersten zwei einträge in der gespeicherten Mail
                /*
                if(myfile.eof()){
                    break;
                }
                message += '\n';
                std::cout << message << std::endl;
                messages.push_back(message);
                */

            }
            message.clear();
            messageCnt++;


            myfile >> message; //speicher den titel der nachricht
            message += " " + std::to_string(messageCnt) + '\n';
            myfile.close();
            messages += message;
        }
        
    }

    messages = std::to_string(messageCnt) + '\n' + messages;

    std::cout << "Printing all messages" << std::endl;
    for(auto message : messages){
        std::cout << message << std::endl;
    }
    return messages;
}

static int checkReadValidity(std::vector<std::string> parsedMsg){
    
    if(parsedMsg.size() != 4){
        return 1;
    }
    if(parsedMsg[1].length() > 8){
        return 1;
    }
    return 0;
}

static int saveMsgToSpool(std::string msg, std::string dirName, std::string username){
    fs::path asd(fs::current_path());
    std::cout << asd.string() << std::endl;

    std::string path = asd.string().substr(0, asd.string().find_last_of("/"));

    auto currentTime = std::chrono::system_clock::now();

    fs::create_directory(dirName);
    
    //printf("here 1");
    fs::create_directory(dirName + '/' + username);

    fs::path metaPath(dirName + "/meta.txt");

    if(!fs::exists(metaPath)){
        std::ofstream metaFile(metaPath.string());
        metaFile << 0;
        metaFile.close();
    }

    std::ifstream metaFileInput(metaPath.string());
    int messageCnt = 0;
    metaFileInput >> messageCnt;
    metaFileInput.close();
    std::ofstream metaFileOutput(metaPath.string());
    messageCnt++;
    metaFileOutput << std::to_string(messageCnt).c_str();
    metaFileOutput.close();

    std::ofstream myfile;
    
    msg.erase(0, msg.find('\n') + 1); // remove SEND from message string
    myfile.open (dirName + '/' + username + '/' + std::to_string(messageCnt));
    myfile << msg;
    myfile.close();

    return 0;
}

static bool fileExists (const std::string path) {
    std::ifstream f(path.c_str());
    return f.good();
}

static std::string readMessage(std::string dirName, std::string username, std::string msgNumber){
    fs::path dirpath(dirName + '/' + username);
    fs::path filepath(dirpath.string() + '/' + msgNumber);

    //std::cout << fp.string() << std::endl;

    //std::cout << "Here 1" << std::endl;

    std::string message;
    std::string line;
    
    if(fileExists(filepath.string())){
        std::ifstream myfile;
        myfile.open(filepath);
        myfile >> line; //wird sind nur am Inhalt der Nachricht interessiert deshalb löschen wir mit >> Operator die ersten 3 Zeilen
        myfile >> line;
       
        myfile >> line;
        line.clear();
        while(true){
            std::getline(myfile,line);
            //std::cout << line << std::endl;
            if(myfile.eof()){
                break;
            }

            message += line + ' ';

        }


        }
    return message;
        
}



int MailServer::start(int port, std::string dirName){
    
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::string receiveBuffer;
    std::string sendBuffer;

    // Creating socket file descriptor
    // server_fd speichert information über das Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //AF_INET beschreibt eine IPv4 Verbindung, SOCK_STREAM beschreibt das Protokoll in diesem Fall TCP, 0 steht für den Protokollwert des IP (Internet Protocol)
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    /*
    if (setsockopt(server_fd, SOL_SOCKET,
                SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
            sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //socket descriptor, 3 ist die Anzahl der maximalen ausstehenden Clients die sich verbinden wollen
    while(true){
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket
            = accept(server_fd, (struct sockaddr*)&address,
                    (socklen_t*)&addrlen))
            < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    
        char tempBuffer[1024] = {0};
        valread = read(new_socket, tempBuffer, 1024);
        receiveBuffer = tempBuffer;
       
        std::vector<std::string> parsedMsg = parseMessage(receiveBuffer);

   
     
        Command command = determineCommand(parsedMsg[0]);

     
        int lines = parsedMsg.size();
        
        std::string username;
        std::string msgNumber;


  
        

        //std::cout << "Command is " << parsedMsg[0] << std::endl;
        std::string messages;
        bool err = false;

        switch(command){
            case SEND:
            if(checkSendValidity(parsedMsg) == 0){
                username = parsedMsg[1];
                saveMsgToSpool(receiveBuffer,dirName,username);
            }
            else{
                err = true;
            }
            break;
            case LIST:
       
            if(checkListValidity(parsedMsg) == 0){
                username = parsedMsg[1];
                messages = getAllMessages(dirName, username);
                
                sendBuffer = messages;
                  
            }
            else{
                err = true;
            }
       
            break;


            case READ:
            if(checkReadValidity(parsedMsg) == 0){
            username = parsedMsg[1];
            msgNumber = parsedMsg[2];
            sendBuffer = readMessage(dirName,username,msgNumber);
            if(!sendBuffer.length()){
                err = true;
            }
            }
            else{
                err = true;
            }
        
            break;
            case DEL:
            //todo
            break;
            case QUIT:
            close(new_socket);

            break;
            default:
            err = true;
            break;

        }
        
        
        //fs::create_directory();
        
        std::string errMsg = "ERR\n";
        std::string okMsg = "OK\n";

        if(err){
            send(new_socket, errMsg.c_str(), errMsg.length(), 0);
            printf("ERROR MESSAGE SENT\n");
        }
        else if(command == SEND){
            send(new_socket, okMsg.c_str(),okMsg.length() , 0);
            printf("OK MESSAGE SENT\n");
        }

        //std::cout << msg << std::endl;
        else if(command == LIST){
            
            send(new_socket, sendBuffer.c_str(), sendBuffer.length(), 0);
            printf("LIST MESSAGE SENT\n");

        }

        else if(command == READ){
            sendBuffer = okMsg + sendBuffer;
            send(new_socket, sendBuffer.c_str(), sendBuffer.length(), 0);
            printf("READ MESSAGE SENT\n");
        }

        receiveBuffer.clear();
        sendBuffer.clear();

        // closing the connected socket
        close(new_socket);

    }
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
};    
