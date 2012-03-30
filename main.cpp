#include <iostream>
#include <boost/thread.hpp>
#include "src/socketcontrol.h"
using namespace std;

void workerOne(void)
{
  
}
int main(int argc, char **argv) {
   
    boost::thread *worker = new boost::thread(workerOne);
    socketControl *sc = new socketControl();
    
  //  sc->transmission("127.0.0.1","8021","something"); //tested - OK
  //    sc->startListener("8021");
    return 0;
}
