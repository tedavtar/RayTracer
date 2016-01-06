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
Also, in honor of Stanford, let's make this red.
<br>
In input.txt, replace "mat &nbsp;&nbsp;&nbsp;1 0.1 0.1&nbsp;&nbsp;&nbsp;.1 .1 .1&nbsp;&nbsp;&nbsp;1 1 1&nbsp;&nbsp;&nbsp;64&nbsp;&nbsp;&nbsp;0.0 0.0 0.0"
<br>
with "mat &nbsp;&nbsp;&nbsp;  1 0.1 0.1 &nbsp;&nbsp;&nbsp;     .8 .1 .1   &nbsp;&nbsp;&nbsp;   1 1 1  &nbsp;&nbsp;&nbsp;   64   &nbsp;&nbsp;&nbsp;  0.0 0.0 0.0."
<br>
So you've toggled with the RGB values...made R channel 8x louder + antialiased. Result:
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/scene.jpg" alt="orignal"> 
<br>
The jagged edges are removed, much cleaner now!
<br>
If you are feeling particularly sadistic and want to heat your computer, try rendering "campanile.obj"
For ex. doing: <br>
xfr 0 70 0<br>
xfs 2.3 2.5 2.3<br>
xft 0 -1.5 0<br>
cam   0 0 30 -3 -5 1 3 -5 1 -3 5 1 3 5 1<br>
mat   0.1 0.1 0.1   .8 .8 .8   1 1 1   64   0.8 0.8 0.8<br>
ltd   0 0 -1  1 1 1    0<br>
obj campanile.obj<br>
And letting that run for a couple hours resulted in: <br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/Campanile.png" alt="orignal"> 
<br>
Note to self: maybe do like an AABB or some optimizations...
<br>
Ok, so what do all the directives even mean?<br>
Just play around to learn what they do...the list of them can be found at the bottom (parsing code) of "TRayTracer.cpp"! <br>
Note, these transformations accumulate (namely, they matrix multiply) so to transform one object, not the rest, got to apply inverse <br>
Transformations: xf? where ?="r" means rotate, "s" scale, "t" means translate. <br>
Ex. xft 2 1 1 will stretch out all objects below in the X direction by a factor of 2.
mat kar kag kab kdr kdg kdb ksr ksg ksb ksp krr krg krb specifies an object's material: <br>
Let ? be an R,G,B. ka? is ambient coeffs, ks for specular, kd for diffuse, ksp for the specular coefficient, and my favorite, kr? which handles reflections <br>
Of course, due to the recursive nature of reflection, enter "depth X" to specify how many levels of reflection to truncate at for base case.
<br>
Let's do a demo of this reflection using tris for octhedron reflected across a mirror.  Credit to this is to a Mr. Baruch in my graphics class:
<br>
Paste in these lines for input.txt: "depth 1<br>
aa 3<br>


cam   0 0 2   -1 -1 1   1 -1 1   -1 1 1   1 1 1<br>
mat   0.1 0.1 0.1   1 0 0   0 1 0   64   0.8 0.8 0.8<br>

ltp   -4 1 -1.9   1 1 1    0<br>
lta   1 1 1<br>

xft 1 0 0<br>
xfr 20 0 0<br>
obj octahedron_1.obj<br>

xfz<br>

mat 0.1 0.1 0.1   0.2 0.2 0.2    0.2 0.2 0.2    64   0.8 0.8 0.8<br>
tri 4 4 -5   -4 4 -2  -4 -4 -2<br>
tri 4 4 -5   -4 -4 -2   4 -4 -5"<br>
(remove the quotes of course) and here's the result:
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/scene(1).jpg" alt="orignal"> 
<br>
Note that this is depth 1 reflection. Now replace the first line to depth 2 and here's what you'll see:
<br>
<img src="https://dl.dropboxusercontent.com/u/105935968/raytracerImages/scene2.jpg" alt="orignal"> 
<br>
Play close attention to the octahedron reflection in the mirror compared to the previous image above! It has an additional reflection of the plane itself!
