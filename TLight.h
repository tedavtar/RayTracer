#ifndef TLIGHT
#define TLIGHT
#include <string.h>
#include "TVector.h"

class TLight{
    public: 
    int myType; //1, 2, 3, 4 = point, directional, ambient, plane (soft shadows)
    int falloff;
    TVector pos;
    TVector color;
    TLight(int Ttype, TVector Tpos, TVector Tcolor, int Tfall);
    TLight(int Ttype, TVector Tpos, TVector Tcolor, int Tfall,TVector Ta, TVector Tb);
    TVector a = TVector(0,0,0,0);
    TVector b = TVector(0,0,0,0);
};

#endif
/*class PointL {
  public: 
    float arr[6];
    PointL(float,float,float,float,float,float);
};

  PointL::PointL(float x1, float y1, float z1, float r1, float g1, float b1) {
        arr[0] = x1;
        arr[1] = y1;
        arr[2] = z1;
        arr[3] = r1;
        arr[4] = g1;
        arr[5] = b1;
    }

    
  
class DirectionalL {
    public: 
      float arr[6];
      DirectionalL(float,float,float,float,float,float);
};

 DirectionalL::DirectionalL(float x1, float y1, float z1, float r1, float g1, float b1) {
        arr[0] = x1;
        arr[1] = y1;
        arr[2] = z1;
        arr[3] = r1;
        arr[4] = g1;
        arr[5] = b1;
    }*/

