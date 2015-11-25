#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <cmath>
#include <iostream>


#include "TVector.h"
#include "TShader.h"
#include "TLight.h"

TShader::TShader(TMaterial Tmat, TVector Tnorm, TLight Tlight, TVector Tpos, TVector TView):
    mat(Tmat),norm(Tnorm), light(Tlight), pos(Tpos), view(TView){}

TVector TShader::computeColor(){
    
  //cout << mat.ka.x << mat.ka.y << mat.ka.z << endl;
    
    
  float r,g,b;
  r = g = b = 0.0f;

  TVector amb = mat.ka;
  TVector dif = mat.kd;
  TVector spec = mat.ks;
  norm = norm.Norm();
  view = view.Norm();

  TVector lint = light.color;
  TVector ldir = TVector(0,0,0, 0);

  //ambient color
  if (light.myType == 3){ //ambient light
    //cout << "raeced here"  << endl;
    r += (amb.x)*lint.x;
    g += (amb.y)*lint.y;
    b += (amb.z)*lint.z;

  } else {

    if (light.myType == 1){ //point light
      ldir = light.pos.Sub(pos);
      ldir = ldir.Norm();


    } 
    if (light.myType == 2){ //directional light
      ldir = light.pos.Minus();
        ldir = ldir.Norm();
         //cout << ldir.x << ldir.y << ldir.z << endl;
        //norm.print();
    } 



    //diffuse color
    float ndotl = norm.DotNonH(ldir);
      
      //cout << ndotl << endl;
      
    float df = max(0.0f,ndotl);
    r += (dif.x)*lint.x*df;
    g += (dif.y)*lint.y*df;
    b += (dif.z)*lint.z*df;

    TVector refl = ldir.Minus().Add(norm.Scale(2*ldir.DotNonH(norm)));


    //specular color
    float rdotv = refl.DotNonH(view);
    float sf = pow(max(0.0f, rdotv),mat.sp);
    r += (spec.x)*lint.x*sf;
    g += (spec.y)*lint.y*sf;
    b += (spec.z)*lint.z*sf;
  }

  


  TVector rtn = TVector(r,g,b, 0);
    
  //cout << rtn.x << rtn.y << rtn.z << endl;
    
    
  return rtn;
}