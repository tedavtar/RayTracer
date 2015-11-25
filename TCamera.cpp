#include "TCamera.h"
#include "TRay.h"
#include <math.h>
#include <float.h>
#include "TVector.h"
#include <stdio.h>
#include <string.h>
#include <iostream>


using namespace std;

TCamera::TCamera(
        TVector Te,
        TVector Tll,
        TVector Tlr,
        TVector Tul,
        TVector Tur):
        e(Te),ll(Tll), lr(Tlr), ul(Tul), ur(Tur){}

TRay TCamera::getEyeRay(float i, float j){
    TVector point = TVector(0,0,0,0);

    //cout << ll.x << ll.y << ll.z << endl;
    //cout << lr.x << lr.y << lr.z << endl;
    //out << ul.x << ul.y << ul.z << endl;
    //cout << ur.x << ur.y << ur.z << endl; all these correct...
    //cout << i << " " << j << endl; as are these...

    TVector left = (ll.Scale(j)).Add((ul).Scale(1-j));

    TVector right = (lr.Scale(j)).Add((ur).Scale(1-j));

    point = (left.Scale(i)).Add(right.Scale(1-i));

    TVector dir = point.Sub(e);

    TRay rtn = TRay(e,dir,1,FLT_MAX);

    return rtn;
};
