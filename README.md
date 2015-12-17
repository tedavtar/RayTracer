# RayTracer

make clean. gets rid of the .o files. 
then run make.
./as2

Place directives in input.txt
./as2
Result will be saved onto image.bmp, reproduced below using a model I created ("easy.obj") using Blender...
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/first.jpg" alt="orignal"> 
<br>
Ok, now observe that the quality isn't too good. All those jagged lines... Let's antialias this.
<br>
Go to input.txt and add the line "aa 3" at the very top. That is anti aliasing using a 3x3 grid jitter per pixel. Result:
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/first.jpg" alt="orignal"> 
<br>
Ok, now in honor of Stanford, let's make this red.
<br>
In input.txt, replace mat__1 0.1 0.1     .1 .1 .1      1 1 1     64     0.0 0.0 0.0
<br>
with "mat   1 0.1 0.1      .8 .1 .1      1 1 1     64     0.0 0.0 0.0."
<br>
So you've toggled with the RGB values...made R channel 8x louder. Result:
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/scene.jpg" alt="orignal"> 

