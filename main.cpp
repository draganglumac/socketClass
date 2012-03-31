#include <iostream>
#include <boost/thread.hpp>
#include "src/socketcontrol.h"
using namespace std;

void workerOne(socketControl *sc)
{
  sc->transmission("127.0.0.1","8021","Wake up..."); //tested - OK
}
int main(int argc, char **argv) {
   
    socketControl *sc = new socketControl();
    boost::thread *worker = new boost::thread(workerOne,sc);
    
    
      sc->startListener("8021");
      worker->join();
    return 0;
}
