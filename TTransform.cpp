#include "TVector.h"
#include "TTransform.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

TTransform::TTransform(TVector Trow1,TVector Trow2, TVector Trow3 ,TVector Trow4):
	row1(Trow1),row2(Trow2), row3(Trow3),row4(Trow4) {}


TTransform TTransform::mult(TTransform matr) {

	TVector a = TVector(0, 0, 0, 0);
	TVector b = TVector(0, 0, 0, 0);
	TVector c = TVector(0, 0, 0, 0);
	TVector d = TVector(0, 0, 0, 0);

	TVector col1 = TVector(matr.row1.x, matr.row2.x, matr.row3.x, matr.row4.x);
	TVector col2 = TVector(matr.row1.y, matr.row2.y, matr.row3.y, matr.row4.y);
	TVector col3 = TVector(matr.row1.z, matr.row2.z, matr.row3.z, matr.row4.z);
	TVector col4 = TVector(matr.row1.h, matr.row2.h, matr.row3.h, matr.row4.h);

	a.x = (row1).Dot(col1);
	b.x = (row2).Dot(col1);
	c.x = (row3).Dot(col1);
	d.x = (row4).Dot(col1);

	a.y = (row1).Dot(col2);
	b.y = (row2).Dot(col2);
	c.y = (row3).Dot(col2);
	d.y = (row4).Dot(col2);

	a.z = (row1).Dot(col3);
	b.z = (row2).Dot(col3);
	c.z = (row3).Dot(col3);
	d.z = (row4).Dot(col3);

	a.h = (row1).Dot(col4);
	b.h = (row2).Dot(col4);
	c.h = (row3).Dot(col4);
	d.h = (row4).Dot(col4);

	TTransform retn = TTransform(a, b, c, d);
	
	return retn;


}

TTransform TTransform::invert(){
	TVector a = TVector(0, 0, 0, 0);
	TVector b = TVector(0, 0, 0, 0);
	TVector c = TVector(0, 0, 0, 0);
	TVector d = TVector(0, 0, 0, 0);

	float a11 = row1.x;
	float a12 = row1.y;
	float a13 = row1.z;
	float a14 = row1.h;
	float a21 = row2.x;
	float a22 = row2.y;
	float a23 = row2.z;
	float a24 = row2.h;
	float a31 = row3.x;
	float a32 = row3.y;
	float a33 = row3.z;
	float a34 = row3.h;
	float a41 = row4.x;
	float a42 = row4.y;
	float a43 = row4.z;
	float a44 = row4.h;

	float det = a11*a22*a33*a44 + a11*a23*a34*a42 + a11*a24*a32*a43 + a12*a21*a34*a43 + a12*a23*a31*a44 + a12*a24*a33*a41+ a13*a21*a32*a44 + a13*a22*a34*a41 + a13*a24*a31* a42 + a14*a21*a33*a42 + a14*a22*a31*a43 + a14*a23*a32*a41;
	det = det - (a11*a22*a34*a43 + a11*a23*a32*a44 + a11*a24*a33*a42 + a12*a21*a33*a44 + a12*a23*a34*a41 + a12*a24*a31*a43 + a13*a21*a34*a42 + a13*a22*a31*a44 + a13*a24*a32*a41 + a14*a21*a32*a43 + a14*a22*a33*a41 + a14*a23*a31*a42);
	det = (1/det);


	a.x = det*(a22*a33*a44 + a23*a34*a42 + a24*a32*a43 -a22*a34*a43 - a23*a32*a44 - a24*a33*a42);
	a.y = det*(a12*a34*a43 + a13*a32*a44 + a14*a33*a42 - a12*a33*a44 - a13*a34*a42 - a14*a32*a43);
	a.z = det*(a12*a23*a44+a13*a24*a42+a14*a22*a43 - a12*a24*a43 - a13*a22*a44 - a14*a23*a42);
	a.h = det*(a12*a24*a33 + a13*a22*a34 + a14*a23*a32 - a12*a23*a34 - a13*a24*a32 - a14*a22*a33);

	b.x = det*(a21*a34*a43+a23*a31*a44+ a24*a33*a41 - a21*a33*a44 - a23*a34*a41 - a24*a31*a43);
	b.y = det*(a11*a33*a44 + a13*a34*a41 + a14*a31*a43 - a11*a34*a43 - a13*a31*a44 - a14*a33*a41);
	b.z = det*(a11*a24*a43 + a13*a21*a44+ a14*a23*a41 - a11*a23*a44 - a13*a24*a41 - a14*a21*a43);
	b.h = det*(a11*a23*a34+ a13*a24*a31 + a14*a21*a33 - a11*a24*a33 - a13*a21*a34 - a14*a23*a31);

	c.x = det*(a21*a32*a44 + a22*a34*a41+ a24*a31*a42 - a21*a34*a42 - a22*a31*a44 - a24*a32*a41);
	c.y = det*(a11*a34*a42 + a12*a31*a44 + a14*a32*a41 - a11*a32*a44 - a12*a34*a41 - a14*a31*a42);
	c.z = det*(a11*a22*a44 + a12*a24*a41 + a14*a21*a42 - a11*a24*a42 - a12*a21*a44 - a14*a22*a41);
	c.h = det*(a11*a24*a32 + a12*a21*a34 + a14*a22*a31 - a11*a22*a34 - a12*a24*a31 - a14*a21*a32);

	d.x = det*(a21*a33*a42 + a22*a31*a43+ a23*a32*a41 - a21*a32*a43 - a22*a33*a41 - a23*a31*a42);
	d.y = det*(a11*a32*a43 + a12*a33*a41 + a13*a31*a42 - a11*a33*a42 - a12*a31*a43 - a13*a32*a41);
	d.z = det*(a11*a23*a42 + a12*a21*a43 + a13*a22*a41 - a11*a22*a43 - a12*a23*a41 - a13*a21*a42);
	d.h = det*(a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a11*a23*a32 - a12*a21*a33 - a13*a22*a31);


	TTransform retn = TTransform(a, b, c, d);
	
	return retn;

}

TTransform TTransform::transpose(){
	TVector a = TVector(0, 0, 0, 0);
	TVector b = TVector(0, 0, 0, 0);
	TVector c = TVector(0, 0, 0, 0);
	TVector d = TVector(0, 0, 0, 0);

	a.x = row1.x;
	b.x = row1.y;
	c.x = row1.z;
	d.x = row1.h;

	a.y = row2.x;
	b.y = row2.y;
	c.y = row2.z;
	d.y = row2.h;

	a.z = row3.x;
	b.z = row3.y;
	c.z = row3.z;
	d.z = row3.h;

	a.h = row4.x;
	b.h = row4.y;
	c.h = row4.z;
	d.h = row4.h;

	TTransform retn = TTransform(a, b, c, d);
	
	return retn;


}

TVector TTransform::multVec(TVector Tvec){
	TVector rtn = TVector(0, 0, 0, 0);
	rtn.x = row1.Dot(Tvec);
	rtn.y = row2.Dot(Tvec);
	rtn.z = row3.Dot(Tvec);
	rtn.h = row4.Dot(Tvec);
    
    //Tvec.print();
    //row4.print();
    //cout << rtn.h << endl;
	return rtn;

}

void TTransform::print(){
	cout << "( " << row1.x << ", " << row1.y << ", " << row1.z << ", " << row1.h << " )" << endl;
	cout << "( " << row2.x << ", " << row2.y << ", " << row2.z << ", " << row2.h << " )" << endl;
	cout << "( " << row3.x << ", " << row3.y << ", " << row3.z << ", " << row3.h << " )" << endl;
	cout << "( " << row4.x << ", " << row4.y << ", " << row4.z << ", " << row4.h << " )" << endl;
}
