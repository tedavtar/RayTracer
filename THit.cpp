#include "TVector.h"
#include "THit.h"
#include "TMaterial.h"
#include "TTransform.h"

THit::THit(bool ThasHit, float Tmin, TVector Tnorm,TMaterial Tmat, TVector Tpos, TVector Tview):
	hasHit(ThasHit), min(Tmin), norm(Tnorm), mat(Tmat), pos(Tpos), view(Tview) {}
