#ifndef TTRANSFORM
#define TTRANSFORM
#include <string.h>
#include "TVector.h"


class TTransform { 
  public:
    TTransform(TVector Trow1,TVector Trow2, TVector Trow3 ,TVector Trow4);
    TVector row1;
    TVector row2;
    TVector row3;
    TVector row4;
    TTransform mult(TTransform matr);

    //don't change the original matrix, only create a new one
    TTransform invert();
    TTransform transpose(); 
    TVector multVec(TVector Tvec);
    void print();
};

#endif