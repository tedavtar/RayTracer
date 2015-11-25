#ifndef TCAMERA
#define TCAMERA
#include "TVector.h"
#include "TRay.h"

class TCamera {
  public:
    TCamera(TVector e,
        TVector Tll,
        TVector Tlr,
        TVector Tul,
        TVector Tur);
        

    TVector e;
    TVector ll;
    TVector lr;
    TVector ul;
    TVector ur;

    TRay getEyeRay(float i, float j);
};

#endif