/*
    CMPSC 457 Section 001
    Homework 5

    Alexander Petrov
    aop5448

    11/5/25
*/


#include <GL/glut.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "model.h"

#include <iterator>
#include <algorithm>
#include <vector>


struct TriangleBounds {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};


// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);



// helpers
void init();

void draw_point(int x, int y, Color c);
void draw_line(int x0, int y0, int x1, int y1, Color c);
void draw_model();


void draw_model_wireframe();
void draw_model_flat_shading();


vec3 world2screen(vec3 v);

bool is_visible(const vec3& p0, const vec3& p1, const vec3& p2);
bool is_inside(const int x, const int y,                        // current pixel
	       const vec3& p0, const vec3& p1, const vec3& p2,  // triangle vertices
	       float& alpha, float& beta, float& gamma);        // barycentric coords for current pixel

TriangleBounds getTriangleRange(const vec3& p0, const vec3& p1, const vec3& p2);

vec3 get_normal(const vec3& p0, const vec3& p1, const vec3& p2);

float getZBufferPoint(int x, int y);
void setZBufferPoint(int x, int y, float value);
void resetZBuffer();


// for debugging purpose, overload operator<< for vec3
std::ostream& operator<< (std::ostream& out, const vec3& v);



// Keeps track of current shading mode.
enum ShadingMode { WIREFRAME, FLAT };
ShadingMode shading_mode = WIREFRAME;


// Initial window size
int win_w = 512;
int win_h = 512;


// z-buffer
float *zbuffer { nullptr };
bool zbuffer_on { false };


// Model to render
Model *model{ nullptr };
int current_model { 0 };   // not selected yet


// Camera position
vec3 cam { 0.0, 0.0, 10.0 };
// light vector for a directional light source
vec3 light_dir { 0.0, 0.0, 1.0 };   // must be a unit vector


// colors
// vec3 c_ambient { 0.0, 0.05, 0.3 };   // ambient light (Ca)
vec3 c_ambient { 0.0, 0.0, 0.0 };   // ambient light (Ca)
vec3 c_diffuse { 1.0, 1.0, 1.0 };   // diffuse reflectance (Cr)
vec3 c_light   { 1.0, 1.0, 1.0 };   // directional light (Cl)



// OpenGL/glut programs typically have the structure shown here,
// although with different args and callbacks.
//
// You should not need to modify main().
// If you want to modify it, do it at your own risk.
//
// For complete description of each glut functions used, see
// glut manual page.
int main(int argc, char* argv[])
{
    // initialize glut
    glutInit(&argc, argv);

    // use double buffering with RGB colors
    // double buffer removes most of the flickering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // set window size and position
    glutInitWindowSize(win_w, win_h);
    glutInitWindowPosition(100, 100);

    // now, create window with title "Scan Conversion"
    glutCreateWindow("Scan Conversion");

    // other stuffs like background color, viewing, etc will be
    // set up in this function.
    init();

    // register callbacks for glut
    glutDisplayFunc(display);   // for display
    glutReshapeFunc(reshape);   // for window move/resize
    glutMouseFunc(mouse);       // for mouse buttons
    glutKeyboardFunc(keyboard); // for keyboard
    glutSpecialFunc(special);   // for arrow keys and function keys

    // start event processing, i.e., accept user inputs
    glutMainLoop();

    if (zbuffer) delete [] zbuffer;
    if (model)   delete [] model;

    return 0;
}




/*****************
 *   callbacks   *
 *****************/


// called when the window is resized/moved (plus some other cases)
void reshape(int w, int h)
{
    win_w = w;
    win_h = h;

    // recreate zbuufer
    if (zbuffer)
	delete [] zbuffer;
    zbuffer = new float[win_w * win_h];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w - 1, 0.0, win_h - 1, -1.0, 1.0);

    glViewport(0, 0, win_w, win_h);
}


// called when the window needs to be redrawn
void display()
{
    // buffer is a raster array provided by OpenGL
    // recall that we are using 2 buffers (double buffering)
    // they are called front buffer and back buffer
    // what you see on the screen is the content of front buffer
    // what you draw is drawn only on back buffer


    // clear back buffer with background color that is set in init()
    glClear(GL_COLOR_BUFFER_BIT);

    // now, draw on back buffer just cleared
    draw_model();


    // swap the buffers.
    // - all the drawing is done on the back buffer
    // - once drawing is done on the back buffer,
    //       you need to display the content of the back buffer.
    // - swapping buffers means swapping back buffer with front buffer
    //       so that front buffer becomes back buffer and
    //       back buffer becomes front buffer.
    // - once back buffer becomes front buffer, the content of it will be
    //       displayed on the screen, so you can see it.
    glutSwapBuffers();
}


// called when a mouse event (button pressed/released/moved/dragged) occurs
// in glut,
//     mouse buttons are represented as
//           GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON
//     status of mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
//     (x, y) is the mouse position when the event occurred
void mouse(int button, int state, int x, int y)
{
}


// called when a keyboard event (key typed) occurs
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q':  // quit the program
        exit(0);
    case 'f':  // flat shading
        std::cerr << "shading_mode changed to FLAT\n";
        shading_mode = FLAT;
        glutPostRedisplay();
        break;
    case 'z':
	if (shading_mode == WIREFRAME) {
	    std::cerr << "WIREFRAME mode does not use zbuffer\n";
	}
	else {
	    std::cerr << "turning zbuffer " << (zbuffer_on ? "off\n" : "on\n");
	    zbuffer_on = !zbuffer_on;
	    glutPostRedisplay();
	}
	break;
    case 'w':
        std::cerr << "shading_mode changed to WIREFRAME\n";
        shading_mode = WIREFRAME;
        glutPostRedisplay();
        break;
    }
}



// called when a keyboard event (special key typed) occurs
// - you must place the models directory in the parent dirctory of this file
//   (i.e., the parent directory of template.cxx) to load them properly.
void special(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_F1:        // F1 key for test model
        if (current_model == 1) return;
        current_model = 1;
	if (!model)
	    delete [] model;
        model = new Model("../models/test.obj");
        break;
    case GLUT_KEY_F2:        // F1 key for sphere model
        if (current_model == 2) return;
        current_model = 2;
	if (!model)
	    delete [] model;
        model = new Model("../models/sphere-223.obj");
        break;
    case GLUT_KEY_F3:        // F2 key for teapot model
        if (current_model == 3) return;
        current_model = 3;
	if (!model)
	    delete [] model;
        model = new Model("../models/teapot.obj");
        break;
    case GLUT_KEY_F4:        // F3 key for african_head model
        if (current_model == 4) return;
        current_model = 4;
	if (!model)
	    delete [] model;
        model = new Model("../models/african_head.obj");
        break;
    case GLUT_KEY_F5:        // F4 key for Disablo model
        if (current_model == 5) return;
        current_model = 5;
	if (!model)
	    delete [] model;
        model = new Model("../models/Diablo.obj");
        break;
    case GLUT_KEY_F6:        // F4 key for beethoven model
        if (current_model == 6) return;
        current_model = 6;
	if (!model)
	    delete [] model;
        model = new Model("../models/beethoven.obj");
        break;
    }

    glutPostRedisplay();
}




/**************
 *   helpers  *
 **************/


void init()
{
    // set background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // create viewing volume
    // -- will use orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w - 1, 0.0, win_h - 1, -1.0, 1.0);

    // create zbuffer
    zbuffer = new float[win_w * win_h];
}


void draw_point(int x, int y, Color c)
{
    glBegin(GL_POINTS);
    {
        glColor3f(c.r, c.g, c.b);
        glVertex2d(x, y);
    }
    glEnd();
}


void draw_line(int x0, int y0, int x1, int y1, Color c)
{
    glBegin(GL_LINES);
    glColor3f(c.r, c.g, c.b);
    glVertex2d(x0, y0);
    glVertex2d(x1, y1);
    glEnd();
}


void draw_model()
{
    if (!model) return;

    switch (shading_mode) {
    case WIREFRAME:
        draw_model_wireframe();
        break;
    case FLAT:
        draw_model_flat_shading();
        break;
    }
}


void draw_model_wireframe()
{
    int cnt = 0;

    // Use red for wires
    Color color(1.0, 0.0, 0.0);

    for(int i=0; i < model->num_faces(); i++){
        std::vector<int> face = model->face(i);
        vec3 p0 = model->vertex(face[0]);
        vec3 p1 = model->vertex(face[1]);
        vec3 p2 = model->vertex(face[2]);

        if(!is_visible(p0, p1, p2)){
            continue;
        }

        p0 = world2screen(p0);
        p1 = world2screen(p1);
        p2 = world2screen(p2);


        draw_line(p0.x, p0.y, p1.x, p1.y, color); //p0 -> p1
        draw_line(p1.x, p1.y, p2.x, p2.y, color); //p1 -> p2
        draw_line(p2.x, p2.y, p0.x, p0.y, color); //p2 -> p0

        cnt++;
    }

    std::cerr << "draw_model_wireframe: drawn " << cnt << " / " << model->num_faces() << " triangles\n";
}


void draw_model_flat_shading()
{
    int cnt = 0;
    float alpha, beta, gamma;

    //Color constants
    vec3 CrCa = c_diffuse * c_ambient; //Cr*Ca
    vec3 CrCl = c_diffuse * c_light; //Cr*Cl

    resetZBuffer();

    for(int i=0; i < model->num_faces(); i++){
        std::vector<int> face = model->face(i);
        vec3 p0 = model->vertex(face[0]);
        vec3 p1 = model->vertex(face[1]);
        vec3 p2 = model->vertex(face[2]);

        vec3 n_hat = glm::normalize(get_normal(p0, p1, p2));
        vec3 l_hat = glm::normalize(light_dir);

        if(!is_visible(p0,p1,p2) || glm::dot(n_hat, l_hat) < 0.0f){ // If not visible
            continue;
        }


        p0 = world2screen(p0);
        p1 = world2screen(p1);
        p2 = world2screen(p2);

        TriangleBounds faceBounds = getTriangleRange(p0,p1,p2);
        Color color = CrCa + CrCl * std::max(0.0f, glm::dot(n_hat, l_hat));


        for(int x=faceBounds.xMin; x <= faceBounds.xMax; x++){
            for(int y=faceBounds.yMin; y <= faceBounds.yMax; y++){
                if(!is_inside(x, y, p0, p1, p2, alpha, beta, gamma)){
                    continue;
                }

                if(zbuffer_on){
                    float depth = p0.z * alpha + p1.z * beta + p2.z * gamma;
                    if(getZBufferPoint(x,y) > depth){
                        continue;
                    }
                    setZBufferPoint(x, y, depth);
                }

                draw_point(x,y,color);
            }
        }

        cnt++;
    }

    std::cerr << "draw_model_flat_shading: drawn " << cnt << " / " << model->num_faces() << " triangles\n";
}




vec3 world2screen(vec3 v)
{
    float width = win_w / 1.0;
    float height = win_h / 1.0;

    // note that z-coordinate is not changed
    // that is,
    //   x and y coords are in screen coord sys
    //   z coord is in world coord sys
    // so that you can use z coord for z-buffering
    return vec3(static_cast<int>((v.x + 1.0) * width / 2.0 + 0.5),
		static_cast<int>((v.y + 1.0) * height / 2.0 + 0.5),
		v.z);
}


/**
 * Check if the triangle is visible to the camera
 * can be used by draw_model_wire_frame and draw_model_flat_shading
 * @returns true if visible
 * @note Uses `cam`
 */
bool is_visible(const vec3& p0, const vec3& p1, const vec3& p2)
{
    vec3 normal = glm::cross(p1-p0, p2-p0);
    vec3 toCam = cam - p0;
    return glm::dot(toCam, normal) >= 0.0f;
}


/**
 * Checks if a pixel is in the triangle p0-p1-p2
 * can be used by draw_model_flat_shading
 * @param x screen-space coordinate
 * @param y screen-space coordinate
 * @param p0 screen-space coordinate
 * @param p1 screen-space coordinate
 * @param p2 screen-space coordinate
 * @param alpha reference to int to set
 * @param beta  reference to int to set
 * @param gamma reference to int to set
 * @returns true if inside
 * @note Also sets `alpha`, `beta`, `gamma`
 */
bool is_inside(const int x, const int y,                         // current point
	       const vec3& p0, const vec3& p1, const vec3& p2,   // triangle vertices
	       float& alpha, float& beta, float& gamma)          // barycentric coords for current pixel
{

    float fullArea = 0.5 * (p0.x*p1.y + p1.x*p2.y + p2.x*p0.y - p0.x*p2.y - p1.x*p0.y - p2.x*p1.y);
    float area12P = 0.5 * (p1.x*p2.y + p2.x*y + x*p1.y - p1.x*y - p2.x*p1.y - x*p2.y);
    float area01P = 0.5 * (p0.x*p1.y + p1.x*y + x*p0.y - p0.x*y - p1.x*p0.y - x*p1.y);

    beta = area12P / fullArea;
    gamma = area01P / fullArea;
    alpha = 1 - beta - gamma;

    return alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f;
}

/**
 * Calculates the min and max screen coordinates for a face
 * @param p0 screen-space coordinate
 * @param p1 screen-space coordinate
 * @param p2 screen-space coordinate
 */
TriangleBounds getTriangleRange(const vec3& p0, const vec3& p1, const vec3& p2){
    TriangleBounds bounds;

    bounds.xMin = p0.x;
    bounds.xMax = p0.x;
    bounds.yMin = p0.y;
    bounds.yMax = p0.y;

    // x
    if(p1.x < bounds.xMin)
        bounds.xMin = p1.x;
    if(p2.x < bounds.xMin)
        bounds.xMin = p2.x;
    if(p1.x > bounds.xMax)
        bounds.xMax = p1.x;
    if(p2.x > bounds.xMax)
        bounds.xMax = p2.x;

    // y
    if(p1.y < bounds.yMin)
        bounds.yMin = p1.y;
    if(p2.y < bounds.yMin)
        bounds.yMin = p2.y;
    if(p1.y > bounds.yMax)
        bounds.yMax = p1.y;
    if(p2.y > bounds.yMax)
        bounds.yMax = p2.y;

    return bounds;
}

//(p1 - p0) x (p2 - p0)
vec3 get_normal(const vec3& p0, const vec3& p1, const vec3& p2){
    return glm::cross(p1-p0, p2-p0);
}

float getZBufferPoint(int x, int y){
    return zbuffer[x + y*win_w];
}

void setZBufferPoint(int x, int y, float value){
    zbuffer[x + y*win_w] = value;
}

/**
 * Sets all values in the zbuffer to negative infinity
 */
void resetZBuffer(){
    for(int i=0; i < win_h * win_w; i++){
        zbuffer[i] = -INFINITY;
    }
}



// for debugging purpose,
// overload operator<< for vec3
std::ostream& operator<< (std::ostream& out, const vec3& v)
{
    out << "(" << v.r << ", " << v.g << ", " << v.b << ")";
    return out;
}
