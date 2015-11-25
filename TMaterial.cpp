#include "TVector.h"
#include "TMaterial.h"

TMaterial::TMaterial(TVector Tka, TVector Tkd, TVector Tks, float Tsp, TVector Tkr):
	ka(Tka),kd(Tkd),ks(Tks),sp(Tsp),kr(Tkr){}