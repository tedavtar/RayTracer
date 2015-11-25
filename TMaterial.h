#ifndef TMATERIAL
#define TMATERIAL
#include <string.h>
#include "TVector.h"

class TMaterial { 
  public:
  	TMaterial(TVector Tka, TVector Tkd, TVector Tks, float Tsp, TVector Tkr);
  	TVector ka;
  	TVector kd;
  	TVector ks;
  	float sp;
  	TVector kr;
 };





#endif