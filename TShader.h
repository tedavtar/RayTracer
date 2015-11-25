#ifndef TSHADER
#define TSHADER
#include <string.h>
#include "TVector.h"
#include "TMaterial.h"
#include "TLight.h"

class TShader { 
  public:
    TShader(TMaterial Tmat, TVector Tnorm, TLight Tlight, TVector Tpos, TVector TView);
    TMaterial mat;
    TVector norm;
    TLight light;
    TVector pos;
    TVector view;
    TVector computeColor();
};



#endif