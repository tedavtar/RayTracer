#include "TVector.h"
#include "TRay.h"

TRay::TRay(TVector Tori,TVector Tdir,float TtMin,float TtMax):
	ori(Tori),dir(Tdir),tMin(TtMin),tMax(TtMax) {}

TVector TRay::TDist(float t){
	return ori.Add(dir.Scale(t));
}