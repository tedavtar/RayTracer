#ifndef TVECTOR
#include <string.h>
#include <stdio.h>
#include <iostream>
#define TVECTOR
using namespace std;

class TVector {
  public:
    TVector(float Tx, float Ty, float Tz, int Th);
    float x;
    float y;
    float z; 
    int h;
    float Dot(TVector Tv);
    float DotNonH(TVector Tv);
	TVector Sub(TVector Tv);
	TVector Minus();
	TVector Add(TVector Tv);
	TVector Norm();
	TVector Scale(float f);
    void print();
    
};

#endif