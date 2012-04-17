#include <iostream>
#include <boost/thread.hpp>
#include "src/socketcontrol.h"
using namespace std;

void workerOne(socketControl *sc)
{
  sc->subscribeBroadcast();
}
int main(int argc, char **argv) {
   
    socketControl *sc = new socketControl();
    boost::thread *worker = new boost::thread(workerOne,sc);
   
    
    worker->join();
    
    return 0;
}
