# CMPSC 475 - Computer Graphics
From-scratch implementations of various drawing, viewing, and rendering techniques.
Uses OpenGL/GLUT as a base, but most library functions were not used.

## Plagarism note
If you found this repository while doing an assignment, don't look at the code until after you are done with yours and **do NOT use my code**. Open up that textbook and *get good*.

### HW1 - Render to .ppm
Renders a checkerboard pattern to a .ppm file

<img width="100" height="100" alt="hw1" src="https://github.com/user-attachments/assets/74ebf364-5fd0-407d-8e69-4113397cb00d" />

### HW2 - Midpoint line, Midpoint circle
Draws lines/circles using the midpoint line/circle algorithm.
Use `L` or `C` to enter a mode, and click to create two points.

<img width="80%" alt="hw2" src="https://github.com/user-attachments/assets/5caffdb5-c14e-43c9-aefe-ea6ce5398c17" />

### HW3 - Triangle drawing: Wireframe, Flat, Gouraud
Draws triangles with a given shading mode.
Press `W`, `F`, or `G` to enter a mode, and click to create three points.

<img width="80%" alt="hw3" src="https://github.com/user-attachments/assets/5055178c-ce02-466d-be09-f7c13220278b" />

### HW4 - Viewing
Renders a simple house-like object from a set of vertices and a transformation matrix.\
`Left click` with or without `shift` to pan the camera, `Middle click` to rotate, and `Right click` to zoom.\
Use `P` to toggle perspective.

https://github.com/user-attachments/assets/53a143b7-0348-45c3-9d34-de9c4d7d00e4


### HW5 - Wireframe, Flat shading, Z-Buffer
Renders .obj files in wireframe or with flat shading.\
Press `W`, `F` to change modes, and `Z` to toggle Z-buffering\
Load the model with function keys (`F1`, `F2`, `F3`, etc)

<img width="1579" height="710" alt="hw5" src="https://github.com/user-attachments/assets/3d32ee14-5d0a-4231-bda7-db87c67dbfa2" />


### HW6 - Gouraud, Phong shading
Renders .obj files with Gouraud or Phong shading.\
Press `G`, `P` to change modes, `H` to toggle highlights\
Load the model with function keys (`F1`, `F2`, `F3`, etc)

<img width="80%" alt="hw6" src="https://github.com/user-attachments/assets/2c1cb642-58c2-452d-8ef9-ee58fb028a49" />

### HW7 - Raytracing
Renders three spheres to a .ppm file.

<img width="250" height="250" alt="hw7" src="https://github.com/user-attachments/assets/79d7e724-b3f4-4299-a212-ccaeeaa11495" />

## .obj loading improvements
The homework template code's .obj loader uses several RegExs to parse the model file. Removing these speeds up model loading for HW5 and HW6 by several orders of magnitude. Check the `fastload` branch for the faster loader.\
Note that both the original and new loader do not fully implement the .obj format.

## Build & Run
This code should compile with whatever version of g++ in a Linux environment. If you have VS Code, you can select which HW number to run in the "run and debug" tab, and press F5.
To build and run without VS Code, see `.vscode/launch.json` and `.vscode/tasks.json` for build and running commands.

**IGNORE the devcontainer** unless you need to use an environment similar to the one used for grading.
