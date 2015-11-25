#ifndef THIT
#define THIT
#include <string.h>
#include "TVector.h"
#include "TMaterial.h"
#include "TTransform.h"


class THit { 
  public:
    THit(bool ThasHit, float Tmin, TVector Tnorm, TMaterial Tmat, TVector Tpos, TVector Tview);
    bool hasHit;
    float min; 
    TVector norm;
    TMaterial mat; 
    TVector pos;
    TVector view;
   // TTransform trans;
    
};

#endif