CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lglut -lGLU -lGL
endif
	
RM = /bin/rm -f 

all: main

main: TRayTracer.o TVector.o TRay.o TSurface.o TLight.o TMaterial.o TShader.o TCamera.o THit.o TTransform.o TObj.o
	$(CC) $(CFLAGS) -o as2 TRayTracer.o TVector.o TRay.o TSurface.o TLight.o TMaterial.o TShader.o TCamera.o THit.o TTransform.o TObj.o $(LDFLAGS) 

TRayTracer.o: TRayTracer.cpp
	$(CC) $(CFLAGS) -c TRayTracer.cpp -o TRayTracer.o
TVector.o: TVector.cpp
	$(CC) $(CFLAGS) -c TVector.cpp
TRay.o: TRay.cpp TVector.o 
	$(CC) $(CFLAGS) -c TRay.cpp
TSurface.o: TSurface.cpp
	$(CC) $(CFLAGS) -c TSurface.cpp
TLight.o: TLight.cpp
	$(CC) $(CFLAGS) -c TLight.cpp
TMaterial.o: TMaterial.cpp
	$(CC) $(CFLAGS) -c TMaterial.cpp
TShader.o: TShader.cpp
	$(CC) $(CFLAGS) -c TShader.cpp  
TCamera.o: TCamera.cpp
	$(CC) $(CFLAGS) -c TCamera.cpp  
THit.o: THit.cpp
	$(CC) $(CFLAGS) -c THit.cpp  
TTransform.o: TTransform.cpp
	$(CC) $(CFLAGS) -c TTransform.cpp
TObj.o: TObj.cpp
	$(CC) $(CFLAGS) -c TObj.cpp

clean: 
	$(RM) *.o as2
 



