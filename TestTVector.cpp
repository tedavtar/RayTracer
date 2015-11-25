#include <iostream>
#include "TVector.h"
#include "TRay.h"
#include "TSurface.h"
#include "TLight.h"
#include "TMaterial.h"
#include "TShader.h"

using namespace std;

float fatAlbert = 1000000;

int main() {
  //usefull vectors
	TVector zero = TVector(0.0f, 0.0f, 0.0f);
	TVector i = TVector(1,0,0);
	TVector j = TVector(0,1,0);
	TVector k = TVector(0,0,1);
  TVector r2 = TVector(7.0f,2.0f,3.0f);
  TVector d2 = TVector(4.0f,5.0f,6.0f);
	cout << "Testing TVector Class" << endl;
	cout << " " << endl;
  cout << "r2.y: " << r2.y << endl;
  cout << "r2 dot d2: " << r2.Dot(d2) << endl;
	TVector tm = r2.Sub(d2);
  cout << "r2 minus d2: " << tm.x << ", " << tm.y << ", " << tm.z  << endl;
  tm = r2.Minus();
	cout << "r2.Minus or the it's negative: " << tm.x << ", " << tm.y << ", " << tm.z  << endl;
  TRay t1 = TRay(r2,d2,1,fatAlbert);
  cout << "Origin at: " << r2.x << ", " << r2.y << ", " << r2.z << endl;
	
	
	
	
	TSurface s1 = TSurface(false, r2, r2, r2, zero, 5.0f);
  TRay r1 = TRay(zero,j,1,fatAlbert);
	cout << "Should see # > 0 : " << s1.hitMe(r1) << endl;
	
	TVector blah = TVector(0, 5, -5);
  TRay r3 = TRay(blah,k,1,fatAlbert);
	cout << "Should see # > 0 : " << s1.hitMe(r3) << endl;
	
  TLight one = TLight(3, r2, d2, 1);
  cout << "My position is " << one.pos.x << ", " << one.pos.y << ", " << one.pos.z << " and my color is " << one.color.x << ", " << one.color.y << ", " << one.color.z << endl;
  
  TMaterial rrh = TMaterial(i,j,k,4,r2);
  cout << "My K reflectance " << rrh.kr.x << ", " << rrh.kr.y << ", " << rrh.kr.z << endl;

  TShader ts = TShader(rrh,j,one,r2, d2);
  cout << "My normal is " << ts.norm.x << ", " << ts.norm.y << ", " << ts.norm.z << endl;

  cout << "rgbvalue: " << ts.computeColor().x << ", " << ts.computeColor().y << ", " << ts.computeColor().z << endl;

  
  TVector a1 = TVector(0, 0, 0);
  TVector b1 = TVector(0, 2, 0);
  TVector c1 = TVector(2, 0, 0);
  TVector e1 = TVector(1, 1, -1);
  TVector d1 = TVector(0, 0, 1);
  TRay triT = TRay(e1, d1, 1, fatAlbert);
  TSurface tritest = TSurface(true,a1,b1,c1,zero,0);
  cout << "Should Return a num > 0:" << tritest.hitMe(triT) << endl;


	cout << " " << endl;
	cout << "End of testing" << endl;	

  return 0;
}