#include "TLight.h"
#include "TVector.h"

TLight::TLight(int Ttype,TVector Tpos, TVector Tcolor, int Tfall):
    	myType(Ttype), pos(Tpos), color(Tcolor), falloff(Tfall) {}


TLight::TLight(int Ttype,TVector Tpos, TVector Tcolor, int Tfall, TVector Ta, TVector Tb):
myType(Ttype), pos(Tpos), color(Tcolor), falloff(Tfall), a(Ta), b(Tb) {}