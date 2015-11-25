/*
For each pixel
  Compute ray r
    If Somehow detect that ray hit surface, we know the t at which it we hit the surface
      So we know point at which the surface was hit. just calc r(t)
      get norm (the surface we hit computes this)
      for each light
        calc the light vector
        use all above info to pass to Shader to get rgb
        setPixel(rgb)
    else
      setPixel(bckgrnd)

*/
#include <fstream>
#include <sstream>
#include <vector>
#include <float.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include <stdio.h>
#include <string.h>

#include "TVector.h"
#include "TRay.h"
#include "TSurface.h"
#include "TLight.h"
#include "TMaterial.h"
#include "TShader.h"
#include "TCamera.h"
#include "THit.h"
#include "TTransform.h"
//#include "TObj.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

float epsilon = .1;
int maxDepth = 1;

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport  viewport;

TVector zero = TVector(0,0,0,0);
TVector one = TVector(1,0,1,0);
TVector lesss = TVector(0.1,0,0.1,0);

TCamera cam = TCamera(zero,zero,zero,zero,zero);
  TVector a = TVector(1, 0, 0, 0);
  TVector b = TVector(0, 1, 0, 0);
  TVector c = TVector(0, 0, 1, 0);
  TVector d = TVector(0, 0, 0, 1);
TTransform currTrans = TTransform(a, b, c, d);
TTransform idTrans = TTransform(a, b, c, d);

TMaterial currMat = TMaterial(zero, zero, zero, 0.0f, zero);

TVector bckgrndColor = TVector(0,0,0,0);


std::vector<TSurface> objects;
std::vector<TLight> lights;

//std::vector<TObj> verts;
//std::vector<TObj> norms;
std::vector<TVector> verts;
std::vector<TVector> norms;

int vertCount = 1;
int normCount = 1;

bool anitA = false;
int aaNum = 1; // so start off no anti-aliasing
int ssNum = 10; //used for soft-shadowing, how many point lights will be on the plane

struct RGBType {
    float r;
    float g;
    float b;
};

void savebmp(const char *filename, int w, int h, int dpi, RGBType *data){
    FILE *f;
    int k = w*h;
    int s = 4*k;
    int filesize = 54 + s;
    double factor = 39.375;
    int m = static_cast<int>(factor);
    
    int ppm = dpi*m;
    unsigned char bmpfileheader[14] = {'B', 'M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);
    
    bmpinfoheader[4] = (unsigned char) (w);
    bmpinfoheader[5] = (unsigned char) (w>>8);
    bmpinfoheader[6] = (unsigned char) (w>>16);
    bmpinfoheader[7] = (unsigned char) (w>>24);
    
    bmpinfoheader[8] = (unsigned char) (h);
    bmpinfoheader[9] = (unsigned char) (h>>8);
    bmpinfoheader[10] = (unsigned char) (h>>16);
    bmpinfoheader[11] = (unsigned char) (h>>24);
    
    bmpinfoheader[21] = (unsigned char) (s);
    bmpinfoheader[22] = (unsigned char) (s>>8);
    bmpinfoheader[23] = (unsigned char) (s>>16);
    bmpinfoheader[24] = (unsigned char) (s>>24);
    
    bmpinfoheader[25] = (unsigned char) (ppm);
    bmpinfoheader[26] = (unsigned char) (ppm>>8);
    bmpinfoheader[27] = (unsigned char) (ppm>>16);
    bmpinfoheader[28] = (unsigned char) (ppm>>24);
    
    bmpinfoheader[29] = (unsigned char) (ppm);
    bmpinfoheader[30] = (unsigned char) (ppm>>8);
    bmpinfoheader[31] = (unsigned char) (ppm>>16);
    bmpinfoheader[32] = (unsigned char) (ppm>>24);
    
    f = fopen(filename, "wb");
    
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    
    for (int i = 0; i < k; i++){
        RGBType rgb = data[i];
        float red = rgb.r*255;
        float green = rgb.g*255;
        float blue = rgb.b*255;
        
        unsigned char color[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};
        
        fwrite(color, 1, 3, f);
    }
    
    fclose(f);
}

//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}




  void rayCast(TRay ray, THit &th){
    //cout << "ime" <<endl;
    //bool rtn = false;
    for (std::vector<TSurface>::iterator iter=objects.begin(); iter!=objects.end(); ++iter){
        TTransform t = (*iter).trans;
        TTransform tI = t.invert();
        
        //ray.dir.print();
        //tI.print();
        
        TVector ro = ray.ori;
        TVector rd = ray.dir;
        
        //rd.print(); so the problem is that the shadow ray has a 1 on the end of it's rd
        
        TVector oI = (tI).multVec(ro);
        TVector dI = (tI).multVec(rd);
        
        //ray.ori.print();
        //oI.print();
        
        //ray.dir.print();
        //dI.print();
        
        TRay rI = TRay(oI, dI, ray.tMin, ray.tMax);
        
        
        
        float tTry = (*iter).hitMe(rI);
        
        //cout << tTry << endl;

        //tI.print();

        if (tTry > ray.tMin){
          //rtn = ((*iter).isTri);
            //TVector oldnorm = (*iter).getNorm(rI.TDist(tTry));
            //oldnorm.print();
            
          TVector n = (tI.transpose()).multVec((*iter).getNorm(rI.TDist(tTry)));   //AHAHAHAHAH  HA had to replace ray.TDist(...) with RI since we only want to deal w/ inverse ray
            
            //n.print();
            
          if((th).hasHit) {
            if (tTry < (th).min) {
              (th).min = tTry;
              (th).norm = n;
              (th).mat = (*iter).mat;
              (th).pos = (ray.TDist(tTry));
              (th).view = ray.dir.Minus();
            }
          } else {
            //cout << " indeed" << endl;
            (th).hasHit = true;
            (th).min = tTry;
            (th).norm = n;
            (th).mat = (*iter).mat;
            (th).pos =(ray.TDist(tTry));
            (th).view = ray.dir.Minus();
          } 

    }


  }
  //return rtn;
}

  void shade(THit th, TVector &color){  //GOT RID OF THIS FUNCTION -- INCORPORATED IT INTO RAYTRACE -- basically to solve the problem of 
      //cout << "called" << endl;
      for (std::vector<TLight>::iterator iter=lights.begin(); iter!=lights.end(); ++iter){
        
          //cout << th.view.x << th.view.y << th.view.z << endl;
          //cout << th.pos.x << th.pos.y << th.pos.z << endl;
          //cout << th.norm.x << th.norm.y << th.norm.z << endl;  //HA HA Here lies the problem.  Wrong normal after transformation!
          


        TShader phong = TShader(th.mat, th.norm, (*iter), th.pos, th.view);
        if ((*iter).myType == 3){ //ambient
           TVector phongShade = phong.computeColor(); //used phong shading (ka,kd,ks) just for current light in lights
           color = color.Add(phongShade);
        } else {
          TVector sdir = TVector(0,0,0,0);
          TVector zero = TVector(0,0,0,0);
          if((*iter).myType == 1){ //point light

            sdir = ((*iter).pos).Sub(th.pos);

          } else{ //directional (or now also planar)
              if ((*iter).myType == 4){
                  int k;
                  //so now I'll create ssNum point lights that lie randomly on the plane-ok it's more of a cirle actually still on user provided plane
                  TVector blendedColor = TVector(0,0,0,0);
                  
                  for (k = 0; k < ssNum; k++){
                      
                    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //rand from 0 to 1
                    r = r - 0.5f; //this centers it to be b/w -.5 and .5
                      r = r / 8;
                    float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //rand from 0 to 1
                    r1 = r1 - 0.5f; //this centers it to be b/w -.5 and .5
                      r1 = r1 / 8;
                      TVector aoffset = (*iter).a.Scale(r);
                      TVector boffset = (*iter).b.Scale(r1);
                      TVector offset = aoffset.Add(boffset);
                      TVector posOfNewLightOnPlane = ((*iter).pos).Add(   offset       );
                      
                      sdir = (posOfNewLightOnPlane).Sub(th.pos);
                      
                      
                      TRay sray = TRay(th.pos,sdir,epsilon,FLT_MAX);
                      
                      THit th11 = THit(false,-1,zero,currMat, zero, zero);
                      THit &thref11 = th11;
                      rayCast(sray, thref11);
                      
                      //cout << th1.hasHit << endl;
                      
                      if (!th11.hasHit){
                          TLight lightOnPlane = TLight(1,posOfNewLightOnPlane,(*iter).color,0); //so this is a sample point light on the plane
                          
                          
                          TShader phong1 = TShader(th.mat, th.norm, lightOnPlane, th.pos, th.view);
                          TVector phongShade1 = phong1.computeColor();
                          blendedColor = blendedColor.Add(phongShade1);
                          
                      }
                      
                      
                      
                      
                  }
                  
                  
                  color = blendedColor;
                  color = color.Scale(   1.0f/( ssNum )     );
                  
                  return;
              }
            
              
            
            sdir = ((*iter).pos).Minus();

             
              

          }

             //sdir.print();  AH, so here is the shadow problem.  SDIR is given as a position when it should really be a direction.  Just zero out the homogenous coordinate
              sdir.h = 0;
              
              //th.pos.print();
              
            TRay sray = TRay(th.pos,sdir,epsilon,FLT_MAX);
              
            THit th1 = THit(false,-1,zero,currMat, zero, zero);
            THit &thref1 = th1;
            rayCast(sray, thref1);
              
              //cout << th1.hasHit << endl;
              
            if (!th1.hasHit){
                
             //cout << "reached" << endl;
                
              //cout << color.x << color.y << color.z << endl;
                
                TVector phongShade = phong.computeColor(); //used phong shading (ka,kd,ks) just for current light in lights
                //cout << phongShade.x << phongShade.y << phongShade.z << endl;
                //phongShade.print();
              color = color.Add(phongShade);
                
              //cout << color.x << color.y << color.z << endl;

            }  


        }


      }
            

            /*if: ambient, update Tshader accordingly. (pass in the ambient light to Tshader + pass in the TMat from THit th)

            else:

                empty THit th1 
                if point light compute light ray (lightPos - THit.pos) l (adjust with episilon=.1)\
                and just negative of direction light position to get the light ray
                
                if point light, PL(&l) Effect of this? (UPDATES the tmax of l)
                call F(l,th1)
                we can use th1 to determine if l hit any object (due to episolon won't self hit) 
                
                update Tshader accordingly (pass in light and other necessary fields from th)

            Call Tshader computeColor() to get something



          output: no output - color += something*/

  }


  TVector rayTrace(TRay ray, int depth){
      //cout << "depth: " << depth << endl;
    TVector zero = TVector(0,0,0,0);
    TMaterial zeroMat = TMaterial(zero, zero, zero, 0.0f, zero);
    THit th = THit(false,-1,zero,zeroMat, zero, zero);
    THit &thref = th;
      //ray.ori.print();
    rayCast(ray, thref); // this casts the ray against all objects and populates THit
    
    //cout << isTri << endl;
    //cout << thref.mat.ks.x << thref.mat.ks.y << thref.mat.ks.z <<endl;
      //cout << th.hasHit <<endl;
      //cout << th.min << endl;

    if (!th.hasHit){ // this means that the ray has not hit any object
      if (depth == maxDepth){ //this means that the ray is the eye ray so return background color
        return bckgrndColor;
      }
        //cout << "reached here" << endl;
      return zero; // so if we reached here, this means the ray missed and also, it was a reflected ray so we don't want to return background color (because that would mean the background is actively shading the object from which our input ray bounced out of (because depth is < maxDepth)
        
    } else { // this means that our input ray did hit some object
        
        //cout << "reached here" << endl;
        
        TVector emptyColor = TVector(0,0,0,0); //start empty, gonna fill this color up by looping over lights and shading
        
        TVector &ecref = emptyColor;

        /*
        TVector zero = TVector(0,0,0,0);
        TMaterial zeroMat = TMaterial(zero, zero, zero, 0.0f, zero);
        THit th1 = THit(false,-1,zero,zeroMat, zero, zero);
        THit &thref1 = th1;
        */
        
        //cout << emptyColor.x << " " << emptyColor.y << " " << emptyColor.z << endl;
        
        shade(th , ecref);  //loops over all the lights and sums their contributions
        
      
        //emptyColor.print();
        
        
        
        
        
        
        
        //cout << emptyColor.x << " " << emptyColor.y << " " << emptyColor.z << endl;

        if(depth == 0) {
            
          return emptyColor;
        } else {
          if (!((th.mat.kr.x < .001)&&(th.mat.kr.y < .001)&&(th.mat.kr.z < .001))){

            //compute the refl (based on the view and normal)
            TVector view = ray.dir.Minus().Norm(); //the reverse of the ray being traced
            TVector norm = th.norm.Norm(); //norm from position we hit

            TVector refl = view.Minus().Add(norm.Scale(2*view.DotNonH(norm)));
              
            TRay refRay = TRay(th.pos,refl,epsilon,FLT_MAX); 

            TVector recursedColor = rayTrace(refRay, depth-1);
            //cout << recursedColor.x << " " << recursedColor.y << " " << recursedColor.z << endl;

            TVector krrC = TVector(th.mat.kr.x * recursedColor.x, th.mat.kr.y * recursedColor.y, th.mat.kr.z * recursedColor.z,0);
              
            TVector rtn = emptyColor.Add(krrC);
              
            //cout << rtn.x << " " << rtn.y << " " << rtn.z << endl;
            return rtn;
          }
            return emptyColor;  //so kr is too little so don't recurse
            //cout << "reached here" << endl;
        }

      }

  }
//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  int i,j,k,l;  // Pixel indices
    
    int thisone;
    int dpi = 72;
    int width1 = 500;
    int height1 = 500;
    int n = width1 * height1;
    RGBType *pixels = new RGBType[n];

  //cout << objects.size() << endl;


  for (std::vector<TSurface>::iterator iter=objects.begin(); iter!=objects.end(); ++iter){
    
    //cout<< iter->r << endl;
  }
  
  /*
  TVector ori = TVector(0,0,10,1);
  TVector dir = TVector(0,0,-1,0);
  TRay ray = TRay(ori,dir,1,FLT_MAX);
  THit th = THit(false,-1,ori,currMat, ori, ori);
  THit &thref = th;
  rayCast(ray, thref);
  //cout<< th.min << endl;
  //cout << th.norm.x << th.norm.y << endl; */

  float xpix, ypix;
  xpix = 250.0f;
  ypix = 250.0f;
  float xt, yt;
  xt = 1.0f - (float)xpix/viewport.w;
  yt = 1.0f - (float)ypix/viewport.h;
    //cout << xpix << " " << ypix << endl;
    //cout << viewport.w << " " << viewport.h << endl;
  TRay er = cam.getEyeRay(xt,yt);
    //er.dir.print();
    
  TVector test = rayTrace(er, maxDepth);
  //test.print();
    
  

  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {
      float u,v;
      //so need to convert (i,j) to the u,v and have to call eye ray on those!
      //u = 1.0f - (float)i/viewport.w;
      //v = 1.0f - (float)j/viewport.h;

        thisone = j*width1 + i;
        
      //eyeRay.ori.print();

      TVector color = TVector(0,0,0,0);
        
        for (k = 0; k < aaNum; k++){
            for (l = 0; l < aaNum; l++){
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //rand from 0 to 1
                r = r - 0.5f; //this centers it to be b/w -.5 and .5
                u = 1.0f - (float)(i + (l + r)/aaNum)/viewport.w;
                v = 1.0f - (float)(j + (k + r)/aaNum)/viewport.h;
                TRay eyeRay = cam.getEyeRay(u,v);
                TVector sampleColor = rayTrace(eyeRay, maxDepth);
                
                color = color.Add(sampleColor);
                
            }
        }
        color = color.Scale(   1.0f/(   pow((float)aaNum,2)    )     );
      //color = rayTrace(eyeRay, maxDepth);
        //cout << i << " " << j << endl;
        //color.print();
        
        
      /*START FUNCTION T
        input: Ray r, Depth d (int)
        Output: color (need to setPixel on this color)

        How (coarse/overview):
        1) compute c'. This is the normal Phong shading.
        2) If d == 0 (return c')
        3) compute refl again and return c' + kr*(T(refl, d-1)) just if on kr here so if kr is 0 then return c'.

        How: fine details of computing c' itself
        1) Create THit th empty (it's tmin = -1, other fields zeroed out)
        2) Call F(r,th)
        3) If th.tmin == -1 , c'= background color 
        4) create a zero color c' and new empty THit th1 . G(&color,th1)

      */

      /*function F (raycast)
        input: ray, reference to empty THit
        
        how: loop over objects (take into account their transtorms)

        output: THit with fields set to best hit and also normal takes into account the surface's transformation peculiarly
      */
        //emmpty THit th

      //call F(eyeray,&THit th)

      //if th doesn't hit anything (it's t is -1) shade background return;

        /*function G (shade)
          input: THit th, &color

          how: loop over all lights.
            Create Tshader object empty

            if: ambient, update Tshader accordingly. (pass in the ambient light to Tshader + pass in the TMat from THit th)

            else:

                empty THit th1 
                if point light compute light ray (lightPos - THit.pos) l (adjust with episilon=.1)\
                and just negative of direction light position to get the light ray
                
                if point light, PL(&l) Effect of this? (UPDATES the tmax of l)
                call F(l,th1)
                we can use th1 to determine if l hit any object (due to episolon won't self hit) 
                
                update Tshader accordingly (pass in light and other necessary fields from th)

            Call Tshader computeColor() to get something



          output: no output - color += something
        */

        //call G(th)

        pixels[thisone].r = color.x;
        pixels[thisone].g = color.y;
        pixels[thisone].b = color.z;

        
      //setPixel(i,j, color.x, color.y, color.z);

    }
  }

    savebmp("scene.bmp",width1,height1,dpi,pixels);

  glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);             // indicate we are specifying camera transformations
  glLoadIdentity();               // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();          // swap buffers (we earlier set double buffer)
}



void processObj(string objFileName){
    //cout << objFileName << endl;
    //istringstream iss;
    
    ifstream iss(objFileName.c_str());
    
    //iss << is.rdbuf();
    //is.close();
   
    string test;
   
    
    while(getline(iss, test)){
        //cout << test << endl;
        istringstream issl(test);
        while(issl){
            string sub;
            issl >> sub;
            //cout << sub << endl;
            if (strcmp(sub.c_str(),"v") == 0){
                //cout << "reached" << endl;
                
                issl >> sub;
                float vx = atof(sub.c_str());
                issl >> sub;
                float vy = atof(sub.c_str());
                issl >> sub;
                float vz = atof(sub.c_str());
                
                TVector newVert = TVector(vx,vy, vz,1);
                
                //TObj newVertObj = TObj(newVert, vertCount);
                //newVert.print();
                verts.push_back(newVert);
                
                
            }
            else if (strcmp(sub.c_str(),"vn") == 0){
                issl >> sub;
                float vnx = atof(sub.c_str());
                issl >> sub;
                float vny = atof(sub.c_str());
                issl >> sub;
                float vnz = atof(sub.c_str());
                
                TVector newNorm = TVector(vnx,vny, vnz,0);
                
                //TObj newNormObj = TObj(newNorm, normCount);
                
                norms.push_back(newNorm);
                
            }
            else if (strcmp(sub.c_str(),"f") == 0) {
                
                bool isObj = false;
                
                TVector a = TVector(0,0,0,1);
                TVector b = TVector(0,0,0,1);
                TVector c = TVector(0,0,0,1);
                
                TVector na = TVector(0,0,0,0);
                TVector nb = TVector(0,0,0,0);
                TVector nc = TVector(0,0,0,0);
                
                
                
                issl >> sub;
                string fx = sub;
                //cout << fx << endl;
                issl >> sub;
                string fy = sub;
                issl >> sub;
                string fz = sub;
                
                string str (fx);
                string::iterator it;
                
                bool seenSlash = false;
                //int index = 0;
                string vertexID = "";
                string normalID = "";
                int vertID = 0;
                int normID = 0;
                
                
                for ( it = str.begin() ; it < str.end(); it++ ) //,index++
                {
                    
                    
                    
                    if (*it == '/') {
                        seenSlash = true;
                    } else {
                        
                        if (!seenSlash){ //so haven't seen slash, so are processing vertex
                            vertexID += *it;
                        } else { //so have already encountered slash, so are processing norm
                            normalID += *it;
                        }
                        
                        
                    }
                    
                    
                }
                
                
                //cout << "vertexID: " << vertexID << endl;
                vertID = atoi(vertexID.c_str());
                a = verts[vertID - 1];
                //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
                
                if (normalID.length() > 0){
                    isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                    normID = atoi(normalID.c_str());
                    //cout << normID << endl;
                    na = norms[normID - 1];
                }
                
                //a.print();
                //na.print();
                
                
                
                string str1 (fy);
                string::iterator it1;
                
                seenSlash = false;
                //int index = 0;
                vertexID = "";
                normalID = "";
                vertID = 0;
                normID = 0;
                
                for ( it1 = str1.begin() ; it1 < str1.end(); it1++ ) //,index++
                {
                    
                    
                    
                    if (*it1 == '/') {
                        seenSlash = true;
                    } else {
                        
                        if (!seenSlash){ //so haven't seen slash, so are processing vertex
                            vertexID += *it1;
                        } else { //so have already encountered slash, so are processing norm
                            normalID += *it1;
                        }
                        
                        
                    }
                    
                    
                }
                
                
                //cout << "vertexID: " << vertexID << endl;
                vertID = atoi(vertexID.c_str());
                b = verts[vertID - 1];
                //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
                
                if (normalID.length() > 0){
                    isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                    normID = atoi(normalID.c_str());
                    //cout << normID << endl;
                    nb = norms[normID - 1];
                }
                
                
                //b.print();
                //nb.print();
                
                
                
                
                
                
                
                
                
                string str2 (fz);
                string::iterator it2;
                
                seenSlash = false;
                //int index = 0;
                vertexID = "";
                normalID = "";
                vertID = 0;
                normID = 0;
                
                for ( it2 = str2.begin() ; it2 < str2.end(); it2++ ) //,index++
                {
                    
                    
                    
                    if (*it2 == '/') {
                        seenSlash = true;
                    } else {
                        
                        if (!seenSlash){ //so haven't seen slash, so are processing vertex
                            vertexID += *it2;
                        } else { //so have already encountered slash, so are processing norm
                            normalID += *it2;
                        }
                        
                        
                    }
                    
                    
                }
                
                
                //cout << "vertexID: " << vertexID << endl;
                vertID = atoi(vertexID.c_str());
                c = verts[vertID - 1];
                //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
                
                if (normalID.length() > 0){
                    isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                    normID = atoi(normalID.c_str());
                    //cout << normID << endl;
                    nc = norms[normID - 1];
                }
                
                
                //c.print();
                //nc.print();
                TVector z = TVector(0,0,0,0);
                
                
                TSurface newTri = TSurface(true, a, b, c, z, 0, currTrans, currMat,isObj,na,nb,nc);
                //a.print();
                //b.print();
                //c.print();
                objects.push_back(newTri);
                
                
                
                
                
                
            }

            
        }
        /*
        do
        {
            string sub;
            issl >> sub;
            cout << sub << endl;
            //cout << "Substring: " << sub << endl;
            if (strcmp(sub.c_str(),"v") == 0){
                //cout << "reached" << endl;
                iss >> sub;
                float vx = atof(sub.c_str());
                iss >> sub;
                float vy = atof(sub.c_str());
                iss >> sub;
                float vz = atof(sub.c_str());
                
                TVector newVert = TVector(vx,vy, vz,1);
                
                //TObj newVertObj = TObj(newVert, vertCount);
                //newVert.print();
                verts.push_back(newVert);
                
            }
            
            
        } while (issl); */
    }
    /*
    do
    {
        string sub;
       
        iss >> sub;
        //s
        
        if (strcmp(sub.c_str(),"v") == 0){
            cout << "reached" << endl;
            iss >> sub;
            float vx = atof(sub.c_str());
            iss >> sub;
            float vy = atof(sub.c_str());
            iss >> sub;
            float vz = atof(sub.c_str());
            
            TVector newVert = TVector(vx,vy, vz,1);
            
            //TObj newVertObj = TObj(newVert, vertCount);
            newVert.print();
            verts.push_back(newVert);
            
        }
        else if (strcmp(sub.c_str(),"vn") == 0){
            iss >> sub;
            float vnx = atof(sub.c_str());
            iss >> sub;
            float vny = atof(sub.c_str());
            iss >> sub;
            float vnz = atof(sub.c_str());
            
            TVector newNorm = TVector(vnx,vny, vnz,0);
            
            //TObj newNormObj = TObj(newNorm, normCount);
            
            norms.push_back(newNorm);
            
        }
        else if (strcmp(sub.c_str(),"f") == 0) {
            
            bool isObj = false;
            
            TVector a = TVector(0,0,0,1);
            TVector b = TVector(0,0,0,1);
            TVector c = TVector(0,0,0,1);
            
            TVector na = TVector(0,0,0,0);
            TVector nb = TVector(0,0,0,0);
            TVector nc = TVector(0,0,0,0);
            
            
            
            iss >> sub;
            string fx = sub;
            //cout << fx << endl;
            iss >> sub;
            string fy = sub;
            iss >> sub;
            string fz = sub;
            
            string str (fx);
            string::iterator it;
            
            bool seenSlash = false;
            //int index = 0;
            string vertexID = "";
            string normalID = "";
            int vertID = 0;
            int normID = 0;
            
            
            for ( it = str.begin() ; it < str.end(); it++ ) //,index++
            {
                
                
                
                if (*it == '/') {
                    seenSlash = true;
                } else {
                    
                    if (!seenSlash){ //so haven't seen slash, so are processing vertex
                        vertexID += *it;
                    } else { //so have already encountered slash, so are processing norm
                        normalID += *it;
                    }
                    
                    
                }
                
                
            }
            
            
            //cout << "vertexID: " << vertexID << endl;
            vertID = atoi(vertexID.c_str());
            a = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
            if (normalID.length() > 0){
                isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                normID = atoi(normalID.c_str());
                //cout << normID << endl;
                na = norms[normID - 1];
            }
            
            //a.print();
            //na.print();
            
            
            
            string str1 (fy);
            string::iterator it1;
            
            seenSlash = false;
            //int index = 0;
            vertexID = "";
            normalID = "";
            vertID = 0;
            normID = 0;
            
            for ( it1 = str1.begin() ; it1 < str1.end(); it1++ ) //,index++
            {
                
                
                
                if (*it1 == '/') {
                    seenSlash = true;
                } else {
                    
                    if (!seenSlash){ //so haven't seen slash, so are processing vertex
                        vertexID += *it1;
                    } else { //so have already encountered slash, so are processing norm
                        normalID += *it1;
                    }
                    
                    
                }
                
                
            }
            
            
            //cout << "vertexID: " << vertexID << endl;
            vertID = atoi(vertexID.c_str());
            b = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
            if (normalID.length() > 0){
                isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                normID = atoi(normalID.c_str());
                //cout << normID << endl;
                nb = norms[normID - 1];
            }
            
            
            //b.print();
            //nb.print();
            
            
            
            
            
            
            
            
            
            string str2 (fz);
            string::iterator it2;
            
            seenSlash = false;
            //int index = 0;
            vertexID = "";
            normalID = "";
            vertID = 0;
            normID = 0;
            
            for ( it2 = str2.begin() ; it2 < str2.end(); it2++ ) //,index++
            {
                
                
                
                if (*it2 == '/') {
                    seenSlash = true;
                } else {
                    
                    if (!seenSlash){ //so haven't seen slash, so are processing vertex
                        vertexID += *it2;
                    } else { //so have already encountered slash, so are processing norm
                        normalID += *it2;
                    }
                    
                    
                }
                
                
            }
            
            
            //cout << "vertexID: " << vertexID << endl;
            vertID = atoi(vertexID.c_str());
            c = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
            if (normalID.length() > 0){
                isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                normID = atoi(normalID.c_str());
                //cout << normID << endl;
                nc = norms[normID - 1];
            }
            
            
            //c.print();
            //nc.print();
            TVector z = TVector(0,0,0,0);
            
            
            TSurface newTri = TSurface(true, a, b, c, z, 0, currTrans, currMat,isObj,na,nb,nc);
            a.print();
            b.print();
            c.print();
            objects.push_back(newTri);
            
            
            
            
            
            
        }
    }while (iss);

        */
        
        
        //e
        
}

void process(string cmdLine){
  //cout << "line detected" << endl;
   istringstream iss(cmdLine);
    do
    {
        string sub;
        iss >> sub;

        if (strcmp(sub.c_str(),"cam") == 0){
          iss >> sub;
          cam.e.x = atof(sub.c_str());
          iss >> sub;
          cam.e.y = atof(sub.c_str());
           iss >> sub;
          cam.e.z = atof(sub.c_str());
            cam.e.h = 1;
           iss >> sub;  
          cam.ll.x = atof(sub.c_str());
           iss >> sub;
          cam.ll.y = atof(sub.c_str());
           iss >> sub;
          cam.ll.z = atof(sub.c_str());
           iss >> sub;
          cam.lr.x = atof(sub.c_str());
           iss >> sub;
          cam.lr.y = atof(sub.c_str());
           iss >> sub;
          cam.lr.z = atof(sub.c_str());
           iss >> sub;
          cam.ul.x = atof(sub.c_str());
           iss >> sub;
          cam.ul.y = atof(sub.c_str());
           iss >> sub;
          cam.ul.z = atof(sub.c_str());
             iss >> sub;
          cam.ur.x = atof(sub.c_str());
           iss >> sub;
          cam.ur.y = atof(sub.c_str());
           iss >> sub;
          cam.ur.z = atof(sub.c_str());
          //iss >> sub;

          //cout << "Our camera is centered at: " << cam.e.x << " " << cam.e.y << " " << cam.e.z << " "  << endl;
        
        }
        else if (strcmp(sub.c_str(),"sph") == 0){
           //example usage "-sph cx cy cz radius"
          TVector z = TVector(0,0,0,1);
          TVector cen = TVector(0,0,0,1);
          iss >> sub;
          cen.x = atof(sub.c_str());
          iss >> sub;
          cen.y = atof(sub.c_str());
          iss >> sub;
          cen.z = atof(sub.c_str());
          iss >> sub;
          float rad = atof(sub.c_str());
          //cout << "Center of sphere: " << cen.x << " " << cen.y << " " << cen.z << " "  << endl;
          //cout << "Radius of sphere: " << rad  << endl;
            
            //currTrans.invert().print();
          TSurface newSphere = TSurface(false, z, z, z, cen, rad, currTrans, currMat,false,z,z,z);
          //cout << "radius of sphere: " << newSphere.r << endl;
          objects.push_back(newSphere); 
        }
        else if (strcmp(sub.c_str(),"tri") == 0){
          TVector z = TVector(0,0,0,0);
          TVector a = TVector(0,0,0,1);
          TVector b = TVector(0,0,0,1);
          TVector c = TVector(0,0,0,1);
          iss >> sub;
          a.x = atof(sub.c_str());
          iss >> sub;
          a.y = atof(sub.c_str());
          iss >> sub;
          a.z = atof(sub.c_str());
          iss >> sub;
          b.x = atof(sub.c_str());
          iss >> sub;
          b.y = atof(sub.c_str());
          iss >> sub;
          b.z = atof(sub.c_str());
          iss >> sub;
          c.x = atof(sub.c_str());
          iss >> sub;
          c.y = atof(sub.c_str());
          iss >> sub;
          c.z = atof(sub.c_str());

          //a.print();

            //currTrans.invert().print();
          TSurface newTri = TSurface(true, a, b, c, z, 0, currTrans, currMat,false,z,z,z);
          //cout << "radius of sphere: " << newSphere.r << endl;
          objects.push_back(newTri);
        }
        else if (strcmp(sub.c_str(),"ltd") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
          iss >> sub;
          Tpos.x = atof(sub.c_str());
          iss >> sub;
          Tpos.y = atof(sub.c_str());
          iss >> sub;
          Tpos.z = atof(sub.c_str());
          iss >> sub;
          Tcolor.x = atof(sub.c_str());
          iss >> sub;
          Tcolor.y = atof(sub.c_str());
          iss >> sub;
          Tcolor.z = atof(sub.c_str());
          //Tpos.print();
          TLight newDLight = TLight(2,Tpos, Tcolor,0);

          lights.push_back(newDLight);
        }
        else if (strcmp(sub.c_str(),"lta") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
          iss >> sub;
          Tcolor.x = atof(sub.c_str());
          iss >> sub;
          Tcolor.y = atof(sub.c_str());
          iss >> sub;
          Tcolor.z = atof(sub.c_str());
          
          //Tcolor.print();
          TLight newDLight = TLight(3,Tpos, Tcolor,0);

          lights.push_back(newDLight);
        }

        //flag1
          else if (strcmp(sub.c_str(),"ltp") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
         
          iss >> sub;
          Tpos.x = atof(sub.c_str());
          iss >> sub;
          Tpos.y = atof(sub.c_str());
          iss >> sub;
          Tpos.z = atof(sub.c_str());
          iss >> sub;
          Tcolor.x = atof(sub.c_str());
          iss >> sub;
          Tcolor.y = atof(sub.c_str());
          iss >> sub;
          Tcolor.z = atof(sub.c_str());
          iss >> sub;
          int fallOff = atof(sub.c_str());

          TLight newPLight = TLight(1,Tpos, Tcolor,fallOff);

          lights.push_back(newPLight);
        }
          else if (strcmp(sub.c_str(),"ltpl") == 0){
              //example usage "-ltpl dx dy dz r g b ax bx cx bx by bz" (where a,b specify direction of plane)
              
              TVector Tpos = TVector(0,0,0,1);
              TVector Tcolor = TVector(0,0,0,0);
              TVector Ta = TVector(0,0,0,0);
              TVector Tb = TVector(0,0,0,0);
              
              iss >> sub;
              Tpos.x = atof(sub.c_str());
              iss >> sub;
              Tpos.y = atof(sub.c_str());
              iss >> sub;
              Tpos.z = atof(sub.c_str());
              iss >> sub;
              Tcolor.x = atof(sub.c_str());
              iss >> sub;
              Tcolor.y = atof(sub.c_str());
              iss >> sub;
              Tcolor.z = atof(sub.c_str());
              iss >> sub;
              Ta.x = atof(sub.c_str());
              iss >> sub;
              Ta.y = atof(sub.c_str());
              iss >> sub;
              Ta.z = atof(sub.c_str());
              iss >> sub;
              Tb.x = atof(sub.c_str());
              iss >> sub;
              Tb.y = atof(sub.c_str());
              iss >> sub;
              Tb.z = atof(sub.c_str());
              
              TLight newPlaneLight = TLight(4,Tpos, Tcolor,0,Ta,Tb);
              
              lights.push_back(newPlaneLight);
          }
else  if (strcmp(sub.c_str(),"mat") == 0){
          iss >> sub;
          currMat.ka.x = atof(sub.c_str());
          iss >> sub;
          currMat.ka.y = atof(sub.c_str());
          iss >> sub;
          currMat.ka.z = atof(sub.c_str());
          iss >> sub;
          currMat.kd.x = atof(sub.c_str());
          iss >> sub;
          currMat.kd.y = atof(sub.c_str());
          iss >> sub;
          currMat.kd.z = atof(sub.c_str());
          iss >> sub;
          currMat.ks.x = atof(sub.c_str());
          iss >> sub;
          currMat.ks.y = atof(sub.c_str());
          iss >> sub;
          currMat.ks.z = atof(sub.c_str());
          iss >> sub;
          currMat.sp = atof(sub.c_str());
          iss >> sub;
          currMat.kr.x = atof(sub.c_str());
          iss >> sub;
          currMat.kr.y = atof(sub.c_str());
          iss >> sub;
          currMat.kr.z = atof(sub.c_str());
        }
 else if (strcmp(sub.c_str(),"xfs") == 0){
           iss >> sub;
           float tx = atof(sub.c_str());
           iss >> sub;
           float ty = atof(sub.c_str());
           iss >> sub;
           float tz = atof(sub.c_str());
       
           TVector sx = TVector(tx,0,0,0);
           TVector sy = TVector(0,ty,0,0);
           TVector sz = TVector(0,0,tz,0);
           TVector sh = TVector(0,0,0,1);

           TTransform scale = TTransform(sx,sy,sz,sh);
           currTrans = currTrans.mult(scale);
       }
else  if (strcmp(sub.c_str(),"xft") == 0){
           iss >> sub;
           float mx = atof(sub.c_str());
           iss >> sub;
           float my = atof(sub.c_str());
           iss >> sub;
           float mz = atof(sub.c_str());
          
           TVector tx = TVector(1,0,0,mx);
           TVector ty = TVector(0,1,0,my);
           TVector tz = TVector(0,0,1,mz);
           TVector th = TVector(0,0,0,1);
          
           TTransform trans = TTransform(tx,ty,tz,th);
          
           currTrans = currTrans.mult(trans);
          
           //currTrans.print();
       }
else if (strcmp(sub.c_str(),"xfr") == 0){
          //input in degrees
           iss >> sub;
           float rx = atof(sub.c_str());
           iss >> sub;
           float ry = atof(sub.c_str());
           iss >> sub;
           float rz = atof(sub.c_str());
           //convert to rad
           rx = rx*PI/180;
           ry = ry*PI/180;
           rz = rz*PI/180;
          
           //cosines
           float crx = cosf(rx);
           float cry = cosf(ry);
           float crz = cosf(rz);
          
           //sines
           float srx = sinf(rx);
           float sry = sinf(ry);
           float srz = sinf(rz);
          
           TVector mir4 = TVector(0,0,0,1); //homogenous row
          
           TVector mxr1 = TVector(1,0,0,0);
           TVector mxr2 = TVector(0,crx,-1*srx,0);
           TVector mxr3 = TVector(0,srx,crx,0);
          
           TVector myr1 = TVector(cry,0,sry,0);
           TVector myr2 = TVector(0,1,0,0);
           TVector myr3 = TVector(-1*sry,0,cry,0);
          
           TVector mzr1 = TVector(crz,-1*srz,0,0);
           TVector mzr2 = TVector(srz,crz,0,0);
           TVector mzr3 = TVector(0,0,1,0);
          
           TTransform tx = TTransform(mxr1,mxr2,mxr3,mir4);
           TTransform ty = TTransform(myr1,myr2,myr3,mir4);
           TTransform tz = TTransform(mzr1,mzr2,mzr3,mir4);
          
           TTransform trans = tz;
           trans = trans.mult(ty);
           trans = trans.mult(tx);
          
           currTrans = currTrans.mult(trans);
        }
        else if (strcmp(sub.c_str(),"xfz") == 0){ 
          currTrans = idTrans;
        }
        else if (strcmp(sub.c_str(),"obj") == 0) {
            iss >> sub;
            processObj(sub);
        } else if (strcmp(sub.c_str(),"aa") == 0) {
            iss >> sub;
            aaNum = atoi(sub.c_str());
        }
        else if (strcmp(sub.c_str(),"depth") == 0) {
            iss >> sub;
            maxDepth = atoi(sub.c_str());
        }
        
        /*
        else if (strcmp(sub.c_str(),"v") == 0){ 
          iss >> sub;
          float vx = atof(sub.c_str());
          iss >> sub;
          float vy = atof(sub.c_str());
          iss >> sub;
          float vz = atof(sub.c_str());

          TVector newVert = TVector(vx,vy, vz,1);

          //TObj newVertObj = TObj(newVert, vertCount);

          verts.push_back(newVert);

        }
        else if (strcmp(sub.c_str(),"vn") == 0){ 
          iss >> sub;
          float vnx = atof(sub.c_str());
          iss >> sub;
          float vny = atof(sub.c_str());
          iss >> sub;
          float vnz = atof(sub.c_str());

          TVector newNorm = TVector(vnx,vny, vnz,0);

          //TObj newNormObj = TObj(newNorm, normCount);

          norms.push_back(newNorm);

        } 
        else if (strcmp(sub.c_str(),"f") == 0) {
            
            bool isObj = false;
            
        TVector a = TVector(0,0,0,1);
        TVector b = TVector(0,0,0,1);
        TVector c = TVector(0,0,0,1);
            
        TVector na = TVector(0,0,0,0);
        TVector nb = TVector(0,0,0,0);
        TVector nc = TVector(0,0,0,0);
            
            
            
          iss >> sub;
          string fx = sub;
          //cout << fx << endl;
          iss >> sub;
          string fy = sub;
          iss >> sub;
          string fz = sub;

          string str (fx);
          string::iterator it;
         
            bool seenSlash = false;
            //int index = 0;
            string vertexID = "";
            string normalID = "";
            int vertID = 0;
            int normID = 0;
            
            
        for ( it = str.begin() ; it < str.end(); it++ ) //,index++
        {
            
            
            
            if (*it == '/') {
                seenSlash = true;
            } else {
            
                if (!seenSlash){ //so haven't seen slash, so are processing vertex
                    vertexID += *it;
                } else { //so have already encountered slash, so are processing norm
                    normalID += *it;
                }
            
            
            }
            
    
        }
            
            
            //cout << "vertexID: " << vertexID << endl;
        vertID = atoi(vertexID.c_str());
        a = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
        if (normalID.length() > 0){
            isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
            normID = atoi(normalID.c_str());
            //cout << normID << endl;
            na = norms[normID - 1];
        }
            
            //a.print();
            //na.print();
             
            
            
            string str1 (fy);
            string::iterator it1;
            
            seenSlash = false;
            //int index = 0;
            vertexID = "";
            normalID = "";
            vertID = 0;
            normID = 0;
         
            for ( it1 = str1.begin() ; it1 < str1.end(); it1++ ) //,index++
            {
                
                
                
                if (*it1 == '/') {
                    seenSlash = true;
                } else {
                    
                    if (!seenSlash){ //so haven't seen slash, so are processing vertex
                        vertexID += *it1;
                    } else { //so have already encountered slash, so are processing norm
                        normalID += *it1;
                    }
                    
                    
                }
                
                
            }
            
            
            //cout << "vertexID: " << vertexID << endl;
            vertID = atoi(vertexID.c_str());
            b = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
            if (normalID.length() > 0){
                isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                normID = atoi(normalID.c_str());
                //cout << normID << endl;
                nb = norms[normID - 1];
            }
            
            
            //b.print();
            //nb.print();
            
            
            
            
            
            
            
            
            
            string str2 (fz);
            string::iterator it2;
            
            seenSlash = false;
            //int index = 0;
            vertexID = "";
            normalID = "";
            vertID = 0;
            normID = 0;
            
            for ( it2 = str2.begin() ; it2 < str2.end(); it2++ ) //,index++
            {
                
                
                
                if (*it2 == '/') {
                    seenSlash = true;
                } else {
                    
                    if (!seenSlash){ //so haven't seen slash, so are processing vertex
                        vertexID += *it2;
                    } else { //so have already encountered slash, so are processing norm
                        normalID += *it2;
                    }
                    
                    
                }
                
                
            }
            
            
            //cout << "vertexID: " << vertexID << endl;
            vertID = atoi(vertexID.c_str());
            c = verts[vertID - 1];
            //cout << "digit size of normal: " << normalID.length() << "normalID: " << normalID << endl;
            
            if (normalID.length() > 0){
                isObj = true; //so we are providing a normal-need to specify that surface isObj--just doing this once for 1st vertex is enough
                normID = atoi(normalID.c_str());
                //cout << normID << endl;
                nc = norms[normID - 1];
            }
            
            
            //c.print();
            //nc.print();
            TVector z = TVector(0,0,0,0);
            
            
            TSurface newTri = TSurface(true, a, b, c, z, 0, currTrans, currMat,isObj,na,nb,nc);
            a.print();
             b.print();
            c.print();
            objects.push_back(newTri);
            
            
            
            
            

       }*/

        //flag2









        //iss >> sub;

        //cout << sub << endl;
    } while (iss);
}




//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {




  // Initalize theviewport size
  viewport.w = 500;
  viewport.h = 500;



   ifstream input("input.txt");
    string line;

    while( std::getline( input, line ) ) {
        //cout<<line<<endl;
        process(line);
    }
/*
   for(int i = 0; i < argc; i++) {
        if (strcmp(argv[i],"-cam") == 0){
          cam.e.x = atof(argv[i+1]);
          cam.e.y = atof(argv[i+2]);
          cam.e.z = atof(argv[i+3]);
            cam.e.h = 1;

            
          cam.ll.x = atof(argv[i+4]);
          cam.ll.y = atof(argv[i+5]);
          cam.ll.z = atof(argv[i+6]);

          cam.lr.x = atof(argv[i+7]);
          cam.lr.y = atof(argv[i+8]);
          cam.lr.z = atof(argv[i+9]);

          cam.ul.x = atof(argv[i+10]);
          cam.ul.y = atof(argv[i+11]);
          cam.ul.z = atof(argv[i+12]);

          cam.ur.x = atof(argv[i+13]);
          cam.ur.y = atof(argv[i+14]);
          cam.ur.z = atof(argv[i+15]);


          //cout << "Our camera is centered at: " << cam.e.x << " " << cam.e.y << " " << cam.e.z << " "  << endl;
        
        }
        if (strcmp(argv[i],"-dimen") == 0){
          //example usage "-dimen width height"

          viewport.w = atof(argv[i+1]);
          viewport.h = atof(argv[i+2]);

          
        }
        if (strcmp(argv[i],"-sph") == 0){
          //example usage "-sph cx cy cz radius"
          TVector z = TVector(0,0,0,1);
          TVector cen = TVector(0,0,0,1);
          cen.x = atof(argv[i+1]);
          cen.y = atof(argv[i+2]);
          cen.z = atof(argv[i+3]);

          float rad = atof(argv[i+4]);
          //cout << "Center of sphere: " << cen.x << " " << cen.y << " " << cen.z << " "  << endl;
          //cout << "Radius of sphere: " << rad  << endl;
            
            //currTrans.invert().print();
          TSurface newSphere = TSurface(false, z, z, z, cen, rad, currTrans, currMat);
          //cout << "radius of sphere: " << newSphere.r << endl;
          objects.push_back(newSphere);
        }
        if (strcmp(argv[i],"-tri") == 0){
          TVector z = TVector(0,0,0,1);

          TVector a = TVector(0,0,0,1);
          TVector b = TVector(0,0,0,1);
          TVector c = TVector(0,0,0,1);

          a.x = atof(argv[i+1]);
          a.y = atof(argv[i+2]);
          a.z = atof(argv[i+3]);
          b.x = atof(argv[i+4]);
          b.y = atof(argv[i+5]);
          b.z = atof(argv[i+6]);
          c.x = atof(argv[i+7]);
          c.y = atof(argv[i+8]);
          c.z = atof(argv[i+9]);

            //currTrans.invert().print();
          TSurface newTri = TSurface(true, a, b, c, z, 0, currTrans, currMat);
          //cout << "radius of sphere: " << newSphere.r << endl;
          objects.push_back(newTri);
        }
      
        if (strcmp(argv[i],"-ltd") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
          
          Tpos.x = atof(argv[i+1]);
          Tpos.y = atof(argv[i+2]);
          Tpos.z = atof(argv[i+3]);
          Tcolor.x = atof(argv[i+4]);
          Tcolor.y = atof(argv[i+5]);
          Tcolor.z = atof(argv[i+6]);

          TLight newDLight = TLight(2,Tpos, Tcolor,0);

          lights.push_back(newDLight);
        }
        if (strcmp(argv[i],"-lta") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
          
          Tcolor.x = atof(argv[i+1]);
          Tcolor.y = atof(argv[i+2]);
          Tcolor.z = atof(argv[i+3]);
          

          TLight newDLight = TLight(3,Tpos, Tcolor,0);

          lights.push_back(newDLight);
        }
        if (strcmp(argv[i],"-ltp") == 0){
          //example usage "-ltd dx dy dz r g b"

          TVector Tpos = TVector(0,0,0,1);
          TVector Tcolor = TVector(0,0,0,0);
          
          Tpos.x = atof(argv[i+1]);
          Tpos.y = atof(argv[i+2]);
          Tpos.z = atof(argv[i+3]);
          Tcolor.x = atof(argv[i+4]);
          Tcolor.y = atof(argv[i+5]);
          Tcolor.z = atof(argv[i+6]);

          int fallOff = atoi(argv[i+7]);

          TLight newPLight = TLight(1,Tpos, Tcolor,fallOff);

          lights.push_back(newPLight);
        }




        if (strcmp(argv[i],"-mat") == 0){
            
          currMat.ka.x = atof(argv[i+1]);
          currMat.ka.y = atof(argv[i+2]);
          currMat.ka.z = atof(argv[i+3]);
          currMat.kd.x = atof(argv[i+4]);
          currMat.kd.y = atof(argv[i+5]);
          currMat.kd.z = atof(argv[i+6]);
          currMat.ks.x = atof(argv[i+7]);
          currMat.ks.y = atof(argv[i+8]);
          currMat.ks.z = atof(argv[i+9]);
          currMat.sp = atof(argv[i+10]);
          currMat.kr.x = atof(argv[i+11]);
          currMat.kr.y = atof(argv[i+12]);
          currMat.kr.z = atof(argv[i+13]);
        }
       if (strcmp(argv[i],"-xfs") == 0){
           float tx = atof(argv[i+1]);
           float ty = atof(argv[i+2]);
           float tz = atof(argv[i+3]);
        
           TVector sx = TVector(tx,0,0,0);
           TVector sy = TVector(0,ty,0,0);
           TVector sz = TVector(0,0,tz,0);
           TVector sh = TVector(0,0,0,1);

           TTransform scale = TTransform(sx,sy,sz,sh);
           currTrans = currTrans.mult(scale);
       }
       if (strcmp(argv[i],"-xft") == 0){
           float mx = atof(argv[i+1]);
           float my = atof(argv[i+2]);
           float mz = atof(argv[i+3]);
           
           TVector tx = TVector(1,0,0,mx);
           TVector ty = TVector(0,1,0,my);
           TVector tz = TVector(0,0,1,mz);
           TVector th = TVector(0,0,0,1);
           
           TTransform trans = TTransform(tx,ty,tz,th);
           
           currTrans = currTrans.mult(trans);
           
           //currTrans.print();
       }
       if (strcmp(argv[i],"-xfr") == 0){
           //input in degrees
           float rx = atof(argv[i+1]);
           float ry = atof(argv[i+2]);
           float rz = atof(argv[i+3]);
           //convert to rad
           rx = rx*PI/180;
           ry = ry*PI/180;
           rz = rz*PI/180;
           
           //cosines
           float crx = cosf(rx);
           float cry = cosf(ry);
           float crz = cosf(rz);
           
           //sines
           float srx = sinf(rx);
           float sry = sinf(ry);
           float srz = sinf(rz);
           
           TVector mir4 = TVector(0,0,0,1); //homogenous row
           
           TVector mxr1 = TVector(1,0,0,0);
           TVector mxr2 = TVector(0,crx,-1*srx,0);
           TVector mxr3 = TVector(0,srx,crx,0);
           
           TVector myr1 = TVector(cry,0,sry,0);
           TVector myr2 = TVector(0,1,0,0);
           TVector myr3 = TVector(-1*sry,0,cry,0);
           
           TVector mzr1 = TVector(crz,-1*srz,0,0);
           TVector mzr2 = TVector(srz,crz,0,0);
           TVector mzr3 = TVector(0,0,1,0);
           
           TTransform tx = TTransform(mxr1,mxr2,mxr3,mir4);
           TTransform ty = TTransform(myr1,myr2,myr3,mir4);
           TTransform tz = TTransform(mzr1,mzr2,mzr3,mir4);
           
           TTransform trans = tz;
           trans = trans.mult(ty);
           trans = trans.mult(tx);
           
           currTrans = currTrans.mult(trans);
           
           //trans.print();
          
       }
       
       
  } */

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();              // quick function to set up scene

  glutDisplayFunc(myDisplay);       // function to run when its time to draw something
  glutReshapeFunc(myReshape);       // function to run when the window gets resized

  glutMainLoop();             // infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}









    