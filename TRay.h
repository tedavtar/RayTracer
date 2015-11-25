#ifndef TRAY
#define TRAY
#include <string.h>
#include "TVector.h"


class TRay { 
  public:
    TRay(TVector Tori,TVector Tdir,float TtMin,float TtMax);
    TVector ori;
    TVector dir;
    float tMin;
    float tMax;
    TVector TDist(float t);
};

#endif