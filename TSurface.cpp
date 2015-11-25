#include <string.h>
#include "TVector.h"
#include "TRay.h"
#include "TSurface.h"
#include <iostream>
#include <math.h>
#include "TTransform.h"

using namespace std;

TSurface::TSurface(bool TisTri, TVector Ta, TVector Tb, TVector Tc, TVector Tcenter, float Tr, TTransform Ttrans, TMaterial Tmat, bool TisObj, TVector Tn1, TVector Tn2, TVector Tn3):
	isTri(TisTri), a(Ta), b(Tb), c(Tc), center(Tcenter), r(Tr), trans(Ttrans), mat(Tmat), isObj(TisObj), n1(Tn1), n2(Tn2), n3(Tn3) {
    
        if(!isObj){
            //cout << "reached" << endl;
            TVector rtn = TVector(0,0,0,0);
            TVector ba = b.Sub(a);
            TVector ca = c.Sub(a);
            
            rtn.x = ba.y* ca.z - ba.z*ca.y;
            rtn.y = ba.z* ca.x - ba.x*ca.z;
            rtn.z = ba.x* ca.y - ba.y*ca.x;
            
            rtn.h = 0;
            
            //rtn.print();
            
            n1 = rtn.Norm();
        }
    
    }

float TSurface::hitMe(TRay ray){
	float result = -1.0f; //no intersection (false)

	//compute time t of intersection for triangle using Cramer's rule

	if (isTri) {

		float a1 = a.x - b.x;
		float b1 = a.y - b.y; 
		float c1 = a.z - b.z;
		float d = a.x - c.x;
		float e = a.y - c.y;
		float f = a.z - c.z;
		float g = ray.dir.x;
		float h = ray.dir.y;
		float i = ray.dir.z;
		float j = a.x - ray.ori.x;
		float k = a.y - ray.ori.y;
		float l = a.z - ray.ori.z;

		float M = a1*(e*i - h*f) + b1*(g*f - d*i) + c1*(d*h - e*g);

		float t = - ((f*(a1*k - j*b1) + e*(j*c1 - a1*l) + d*(b1*l - k*c1))/M);

		if ((t < ray.tMin) || (t > ray.tMax)) {
			return result;
		} 

		float gamma = (i*(a1*k - j*b1) + h*(j*c1 - a1*l) + g*(b1*l - k*c1))/M;

		if ((gamma <0) || (gamma > 1)) {
			return result;
		}

	
		float beta = (j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g))/M;

		if ((beta < 0) || (beta > 1 - gamma)) {
			return result;
		}

		result = t;


	//compute time t of intersection for sphere using the quadratic formula 
	} else {
		TVector d = ray.dir;
		TVector e = ray.ori;
		TVector c = center;
		//disriminant computation 
		TVector eminusc = e.Sub(c);
		float disc =  (d.DotNonH(eminusc)*d.DotNonH(eminusc)) - d.DotNonH(d)*((eminusc).DotNonH(eminusc) - r*r);
		//cout << "discriminant: " << disc << endl;
		if (disc == 0) {
			result = ((d.Minus()).DotNonH(eminusc))/(d.DotNonH(d));
		} else if (disc > 0) {
			float t1 = (((d.Minus()).DotNonH(eminusc)) + sqrt(disc))/ (d.DotNonH(d));
			float t2 = (((d.Minus()).DotNonH(eminusc)) - sqrt(disc))/ (d.DotNonH(d));
			/*cout << "t1: " << t1 << endl;
			cout << "t2: " << t2 << endl;*/
			//return the first time t that the ray hits
			if (t1 < t2){
				if (t1 > 0){
					result = t1;
				} else if (t2 > 0) {
					result = t2;
				}	
			} else {
				if (t2 > 0) {
					result = t2;
				} else if (t1 > 0) {
					result = t1;
				}
			}	
		}
	}
	
	return result;
}

TVector TSurface::getNorm(TVector pos){
	TVector rtn = TVector(0,0,0,0);
	if (isObj) {

		float yaminusyb = a.y - b.y;
		float xbminusxa = b.x - a.x;
		float xaybminusxbya = (a.x*b.y) - (b.x*a.y);

		float gamma = ((yaminusyb * pos.x) + (xbminusxa * pos.y) + xaybminusxbya)/((yaminusyb * c.x) + (xbminusxa * c.y) + xaybminusxbya);

		float yaminusyc = a.y - c.y;
		float xcminusxa = c.x -a.x;
		float xaycminusxcya = (a.x*c.y) - (c.x*a.y);

		float beta = ((yaminusyc * pos.x) + (xcminusxa * pos.y) + xaycminusxcya)/((yaminusyc * b.x) + (xcminusxa * b.y) + xaycminusxcya);

		float alpha = 1 - (gamma + beta);

		rtn = n1.Scale(alpha).Add(n2.Scale(beta)).Add(n3.Scale(gamma));




	} else {


	if(!isTri){
		rtn = pos.Sub(center);
		rtn.h = 0;
		rtn = rtn.Norm();
	} else {
        /*
		//cout << "you called?" <<endl;
		TVector ba = b.Sub(a);
		TVector ca = c.Sub(a);

		rtn.x = ba.y* ca.z - ba.z*ca.y;
		rtn.y = ba.z* ca.x - ba.x*ca.z;
		rtn.z = ba.x* ca.y - ba.y*ca.x;

		rtn.h = 0;

		//rtn.print();

		rtn = rtn.Norm();

		//rtn.print();*/
        rtn = n1;
	}



	}

	return rtn;
} 
