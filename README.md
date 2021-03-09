Usage:
module add qt/5.3.1
qmake
make
./RaytraceRenderWindow /path/to/obj /path/to/texture
(Note the texture is for Opengl preview... but only works for a single one)





# Modifications to the textured object: 

I used a Triangle struct containing three small arrays for the vertices data. 
This means using a little bit more memory since we need to keep track of what 
face the triangles belong to but its simpler to iterate over when rendering


# Modifications to the .obj file:

1) creating and setting materials
all material entries start with an m. The subsequent character determines what
aspect of the material we want to set or which material to use.
- mc: tell the reader to create a new material. If this is not called, then 
the current material will be updated, overwriting its characteristic.
- me n n n: "me" followed by three space separated numbers, sets the emissive RGB
of the material.
- ml n n n: "ml" followed by three space separated numbers, sets the lambertian albedo RGB
of the material.
- mg n n n: "mg" followed by three space separated numbers, sets the glossy albedo RGB
of the material.
- mi n n n: "mi" followed by three space separated numbers, sets the impulse albedo RGB
of the material.
- mI n: "mI" followed by a single number, sets the impulse coefficient of the material.
- mx n: "mx" followed by a single number, sets the extinction coefficient of the material.
- mu n: "mu" followde by a single number, the number is the id of the current material for 
all following faces. If n is an invalid number then the default material is used.

2) creating and setting lights
- point light
A point light entry starts with a "lp" and is followed by five numbers following 
the format "lp p p p i b" where "p p p" refer to a cartesian position in space, 
"i" is a number representing the intensity of the light and "b" is a boolean, where 
0 is false and any other integer is true, determines if the light is atInfinity (true), 
or not (false)
- area light
An area light entry starts by first declaring "la i i i" where i is the intensity of the area light.
To set the triangles that are part of the light, declare a face as you normally would (f v v v)
but as "lf v v v" where v refers to an existing vertex

3) using textures
To use textures, first make a texture with "tm /path/to/ppm", with the texture a ppm 3 file.
Whenever a face should be textured, place the entry "tu t" where t is an integer identifying
a texture (starts at 1!). To stop a texture being used, place the entry "ts". Any face declared
in between the "tu" and "ts" entries will use the texture.
example :

\> tm ../textures/earth.ppm

\> tu 1

\> f 1 2 3

\> f 1 3 4

\> f 3 4 2

\> ts 

4) OpenGL color 
Keeping this for debugging, affects the OpenGL viewport on the left. The entry starts
with a "c" and is followed by three numbers (0-255) for the RGB values. Alpha is set to 255.


