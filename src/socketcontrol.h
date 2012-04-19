/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  jonesax <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SOCKETCONTROL_H
#define SOCKETCONTROL_H
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strstream>
#include <netdb.h> 
#include <iostream>
#include <sstream>
#include <string>

class socketControl
{

public:
    socketControl();
    virtual ~socketControl();
    void startListener(std::string);
    void stopListener(void);
    int transmission(std::string,std::string,std::string);
    void subscribeBroadcast(void);
    void sendBroadcast(std::string);
    std::string getPrimaryIp(void);
private:
    int listenfd,sendfd,newsockfd, portno, n;
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;
    struct hostent *server;
    char transmissionBuffer[1024];  //buffer we'll push our protocal into 
    char buffer[1024];  //buffer we'll receive any messaegs with
    bool listening;
    void mainLoop(void);
};

#endif // SOCKETCONTROL_H
