#include "TVector.h"
#include <math.h>

TVector::TVector(float Tx, float Ty, float Tz, int Th) {
  x = Tx;
  y = Ty;
  z = Tz;
  h = Th;

}

float TVector::Dot(TVector Tvec) {
  return x*Tvec.x + y*Tvec.y + z*Tvec.z + h*Tvec.h;
}

float TVector::DotNonH(TVector Tvec) {
  return x*Tvec.x + y*Tvec.y + z*Tvec.z;
}

TVector TVector::Sub(TVector Tvec) {
	TVector rtn = TVector(0,0,0, h);
	rtn.x = x - Tvec.x;
	rtn.y = y - Tvec.y;
	rtn.z = z - Tvec.z;
  return rtn;
}

TVector TVector::Minus(){
	TVector rtn = TVector(0,0,0, h);
	rtn.x = 0 - x;
	rtn.y = 0 - y;
	rtn.z = 0 - z;
	return rtn;
}

TVector TVector::Add(TVector Tvec){
	TVector rtn = TVector(0,0,0, h);
	rtn.x = x + Tvec.x;
	rtn.y = y + Tvec.y;
	rtn.z = z + Tvec.z;
	return rtn;

}

TVector TVector::Scale(float f){
	TVector rtn = TVector(0,0,0, h);
	rtn.x = f*x;
	rtn.y = f*y;
	rtn.z = f*z;
	return rtn;

}

TVector TVector::Norm() {
	float denom = sqrt(x*x + y*y + z*z);
	TVector rtn = TVector(0,0,0, h);
	rtn.x = x/denom;
	rtn.y = y/denom;
	rtn.z = z/denom;
	return rtn;

}

void TVector::print(){
    cout << x << " " << y << " " << z << " " << h << endl;
}