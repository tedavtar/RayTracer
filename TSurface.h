#ifndef TSURFACE
#define TSURFACE
#include <string.h>
#include "TVector.h"
#include "TRay.h"
#include "TTransform.h"
#include "TMaterial.h"

class TSurface{
	public:
		TSurface(bool TisTri, TVector Ta, TVector Tb, TVector Tc, TVector Tcenter, float Tr, TTransform Ttrans, TMaterial Tmat, bool isObj, TVector Tn1, TVector Tn2, TVector Tn3);
		bool isTri;
		bool isObj;
		TVector a, b, c, n1, n2, n3;
		TMaterial mat;
		TVector center;
		float r;
		TTransform trans;
		float hitMe(TRay ray);
		TVector getNorm(TVector Tpos);	
};

#endif