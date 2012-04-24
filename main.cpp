#include <iostream>
#include <boost/thread.hpp>
#include "src/localcontrol.h"
using namespace std;

void workerOne(localcontrol *sc)
{
 cout << sc->getwlanIp() << endl;
};
int main(int argc, char **argv) {
   
    localcontrol *l = new localcontrol();
    
    boost::thread *worker = new boost::thread(workerOne,l);  
  
    worker->join();
    
    return 0;
}
