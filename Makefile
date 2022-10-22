SRC=./main
TARGET=./target
GCC = g++
CPPNAME = main.cpp Client.cpp Server.cpp 
ONAME = twmailer-server
CFLAGS = -std=c++17 -o $(TARGET)/$(ONAME)
RM = rm -rf
ARGS = 8080 testdir

all: clean default run

default: ClientMain.cpp ServerMain.cpp

ClientMain.cpp: 
	$(GCC) -std=c++17 -o $(TARGET)/twmailer-client $(SRC)/twmailer-client/ClientMain.cpp $(SRC)/twmailer-client/Client.cpp

ServerMain.cpp: 
	$(GCC) -std=c++17 -o $(TARGET)/twmailer-server $(SRC)/twmailer-server/ServerMain.cpp $(SRC)/twmailer-server/Server.cpp


run: 
	$(TARGET)/$(ONAME) $(ARGS)


clean:
	$(RM) $(TARGET)/*.o

