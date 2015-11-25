#ifndef TOBJ
#define TOBJ
#include <string.h>
#include "TVector.h"
#include "TObj.h"

class TObj { 
  public:
    TObj(TVector Tvec,int Tid);
    TVector vec;
    int id;
};



#endif