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

#include <error.h>
#include "socketcontrol.h"
#include <cstdio>
#include <ifaddrs.h>
using namespace std;
socketcontrol::socketcontrol()
{

}

socketcontrol::~socketcontrol()
{

}
int socketcontrol::transmission(std::string host, std::string port,std::string payload)
{
  
  portno = atoi(port.c_str());
  sendfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sendfd < 0) 
  {
        cout << "ERROR opening socket" << endl;
    return 1;
  }
  server = gethostbyname(host.c_str());
  if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 1;
    }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
 
  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  
  serv_addr.sin_port = htons(portno);
  if (connect(sendfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
  {
    cout << "error whilst connecting" << endl;
    return 1;
  }
  bzero(transmissionBuffer,1024);
  //copy the payload into the buffer
  strcpy(transmissionBuffer,payload.c_str());
  n = write(sendfd,transmissionBuffer,strlen(payload.c_str()));
  // lets listen for a callback 
  //zero out the buffer
  if (n < 0) 
  {
    cout << "ERROR reading from socket" << endl;
  }
  bzero(transmissionBuffer,1024); 
  n = read(sendfd,transmissionBuffer,255);
  cout << "Received reply :" << transmissionBuffer << endl;
  close(sendfd);
  return 0;
}
void socketcontrol::mainLoop(void )
{
  while(listening)
  {
    cout << "Listening..." << endl;
        listen(listenfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(listenfd,(struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0)
            cout << "ERROR on accept" << endl;
    bzero(buffer,1024);
    n = read(newsockfd,buffer,255); // <- Reading into the buffer
    cout <<"Message recieved via socket:" << buffer << endl; //squawk our message receieved
    if (n < 0)
    {
      cout << "ERROR reading from socket" << endl;   
    };
    // we received a message.. Lets tell them we got it
    bzero(buffer,1024); //zero out the buffer
    string reply = "The sleeper has awakened..";
    strcpy(buffer,reply.c_str());
    n = write(newsockfd,buffer,strlen(reply.c_str()));  
  }
}
void socketcontrol::startListener(std::string port )
{
   listening = true;
   listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0)
    {
        cout << "ERROR opening socket" << endl;
    }
    cout << "opened socket..." << endl;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(port.c_str());
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(listenfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        cout << "ERROR on binding" << endl;

    this->mainLoop();
}
void socketcontrol::stopListener(void )
{
  listening = false;
}
#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 256
void socketcontrol::subscribeBroadcast()
{
  struct sockaddr_in addr;
     int fd, nbytes,addrlen;
     struct ip_mreq mreq;
     char msgbuf[MSGBUFSIZE];

     u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }


/**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }
/*** END OF MODIFICATION TO ORIGINAL */

     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
     addr.sin_port=htons(HELLO_PORT);
     
     /* bind to receive address */
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  perror("bind");
	  exit(1);
     }
     
     /* use setsockopt() to request that the kernel join a multicast group */
     mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
     mreq.imr_interface.s_addr=htonl(INADDR_ANY);
     if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
	  perror("setsockopt");
	  exit(1);
     }

     /* now just enter a read-print loop */
     while (1) {
	  addrlen=sizeof(addr);
	  if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
			       (struct sockaddr *) &addr,(socklen_t*)&addrlen)) < 0) {
	       perror("recvfrom");
	       exit(1);
	  }
	  cout <<msgbuf << endl;
     }
  	 
}
void socketcontrol::sendBroadcast(string message)
{
   struct sockaddr_in addr;
     int fd, cnt;
     struct ip_mreq mreq;

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }

     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
     addr.sin_port=htons(HELLO_PORT);
     
     /* now just sendto() our destination! */
     while (1) {
	  if (sendto(fd,message.c_str(),strlen(message.c_str()),0,(struct sockaddr *) &addr,
		     sizeof(addr)) < 0) {
	       perror("sendto");
	       exit(1);
	  }
	  sleep(1);
     }
}
string socketcontrol::getPrimaryIp(string interface) 
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa ->ifa_addr->sa_family==AF_INET)
	{ // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
	    
	    
	    if(strcmp(ifa->ifa_name,interface.c_str()) == 0)
	    {
	      cout << "FOUND PREFERED NETWORK" << endl;
	     	      
	      return addressBuffer;
	    }
	   
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    
    return "NOT FOUND";
}

