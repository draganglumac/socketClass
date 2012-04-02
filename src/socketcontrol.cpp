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


#include "socketcontrol.h"
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


