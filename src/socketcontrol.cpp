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
using namespace std;
socketControl::socketControl()
{

}

socketControl::~socketControl()
{

}
int socketControl::transmission(std::string host, std::string port,std::string payload)
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
void socketControl::mainLoop(void )
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
void socketControl::startListener(std::string port )
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
void socketControl::stopListener(void )
{
  listening = false;
}
void socketControl::subscribeBroadcast(string port)
{
 int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[1024];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
		//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return ;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}

	if (p == NULL) {
	//	fprintf(stderr, "listener: failed to bind socket\n");
	cout << "Failed to bind socket for broadcast" << endl;
		exit(0);
	}

	freeaddrinfo(servinfo);

	printf("listener: waiting to recvfrom...\n");

	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, buf, 1024-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}


	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);
	close(sockfd);
  	 
}
void socketcontrol::sendBroadcast(string host, string port, string message)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(host.c_str(),port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return;
    }

    if ((numbytes = sendto(sockfd, message.c_str(), strlen(message.c_str()), 0,
                           p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, host.c_str());
    close(sockfd);
}


