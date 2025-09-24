/*
    CMPSC 457 Section 001
    Homework 2

    Alexander Petrov
    aop5448

    9/24/25

    Description:
    This program draws lines or circles.
    Press L for line drawing mode and C for circle drawing mode.
    Lines and circles are drawn using the midpoint algorithm with no floating point operations
    during the drawing loop.
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>


using std::cin;
using std::cerr;
using std::endl;

// Simple structure for a point
struct Point
{
    int x;
    int y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}
};

// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);


// helpers
void init();
void addPoint(int x, int y);
void keyboard_input();
void midpoint_line();
void draw_circle();
int implicit_line(Point* p0, Point* p1, int x, int y);
float implicit_circle(float x, float y, float r);



// Keeps track of what I am drawing currently.
enum DrawingMode { NONE, LINE, CIRCLE };
DrawingMode drawing_mode = NONE;

// Initial window size
int win_w = 512;
int win_h = 512;

// For lines, 2 points will do.
Point points[2];

// Used to keep track of how many points I have so far
int num_points;



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


    // start event processing, i.e., accept user inputs
    glutMainLoop();

    return 0;
}


void init()
{
    // set background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // create viewing volume
    // -- will use orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

}


// called when the window is resized/moved (plus some other cases)
void reshape(int w, int h)
{
    win_w = w;
    win_h = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

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
    switch (drawing_mode) {
        case LINE:
            midpoint_line();
            break;
        case CIRCLE:
            draw_circle();
            break;
        break;
            default:
        break;
    }

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
//     state of mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
//     (x, y) is the mouse position when the event occurred
void mouse(int button, int state, int x, int y)
{
    switch (button) {
    case GLUT_LEFT_BUTTON:
	if (state == GLUT_DOWN)
	    addPoint(x, y);
	break;
    default:
	break;
    }
}


// called when a keyboard event (key typed) occurs
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':  // quit the program
            exit(0);
        case 'l':  // draw a line
            drawing_mode = LINE;
            num_points = 0;
            break;
        case 'c':  // draw a circle; not implemented
            drawing_mode = CIRCLE;
            num_points = 0;
            break;
        case 'k':  // for grading purpose only--do not modify
            keyboard_input();
            num_points = 0;
            break;
        default:
            drawing_mode = NONE;
            break;
    }
}


// add the point just selected by mouse button
void addPoint(int x, int y)
{
    switch (drawing_mode) {
        case LINE:  // save the points until we have 2 points
            points[num_points++] = Point(x, y);
            if (num_points == 2) {
                // we have 2 points now, so we can draw a line

                // reset the num_points to 0 for next line
                num_points = 0;

                // tell glut that the current window needs to be redisplayed.
                // glut will then redisplay the current window.
                // this means display() callback will be called.
                // display() in turn will draw a midpoint line on back buffer
                //   and swap the back buffer with the front buffer
                // by swapping the buffers, the back buffer becomes visible,
                //   ie, displayed on the window
                glutPostRedisplay();
            }
            break;
        case CIRCLE:
            points[num_points] = Point(x,y);
            num_points = (num_points + 1) % 2;

            if(num_points == 0){
                glutPostRedisplay();
            }
            break;
        default:
            break;
    }
}


// for grading purpose only
// do not modify this function
// do not use this function
//   -- it's not tested fully and does not work correctly
void keyboard_input()
{
    if (drawing_mode == NONE) {
	cerr << "Select drawing mode first...\n";
	return;
    }

    int x, y;
    num_points = 0;
    for (int i=0; i<2; i++) {
	cerr << "Enter point " << i << " => ";
	cin >> x >> y;
	cerr << endl;
	addPoint(x, y);
    }
}



void midpoint_line()
{
    //Ensure we draw from left to right
    Point* startPoint;
    Point* endPoint;
    if(points[0].x <= points[1].x){
        startPoint = &points[0];
        endPoint = &points[1];
    } else {
        startPoint = &points[1];
        endPoint = &points[0];
    }
    // float m = ((float)endPoint->y - (float)startPoint->y)/((float)endPoint->x - (float)startPoint->x);
    int rise = endPoint->y - startPoint->y;
    int run = endPoint->x - startPoint->x;
    int temp;

    /*
    This next part moves points into c0, and then sets up a 2x2 matrix
    This matrix transforms coords back to where they should be
    The swap above handles c2-c5
    Note that the drawn line is flipped, so a line in c0 is rendered as if its in c7
    ```
            \c2 | c1/
             \  |  /
          c3  \ | /  c0
        _______\|/_____
               /|\
          c4  / | \  c7
             /  |  \
            /c5 | c6\
    ```
    */
    int lineCorrMat[4] = {1,0,0,1};
    if(rise > 0 && rise > run){ //m > 1, c1, flip about x=y
        lineCorrMat[0] =  0;  lineCorrMat[1] =  1;
        lineCorrMat[2] =  1;  lineCorrMat[3] =  0;

        temp = startPoint->x;
        startPoint->x = startPoint->y;
        startPoint->y = temp;

        temp = endPoint->x;
        endPoint->x = endPoint->y;
        endPoint->y = temp;

    } else if(rise >= 0 && rise <= run){ //0 < m < 1, c0, do nothing

    } else if(rise < 0 && rise*-1 <= run){ //-1 < m < 0, c7, flip about x
        lineCorrMat[0] =  1;  lineCorrMat[1] =  0;
        lineCorrMat[2] =  0;  lineCorrMat[3] = -1;

        startPoint->y = -startPoint->y;
        endPoint->y = -endPoint->y;

    } else { // m < -1, c6, flip about x, then about x=y
        lineCorrMat[0] =  0;  lineCorrMat[1] =  1;
        lineCorrMat[2] = -1;  lineCorrMat[3] =  0;

        temp = startPoint->x;
        startPoint->x = -startPoint->y;
        startPoint->y = temp;

        temp = endPoint->x;
        endPoint->x = -endPoint->y;
        endPoint->y = temp;
    }

    //Loop init values
    int x = startPoint->x;
    int y = startPoint->y;
    int xdiff = endPoint->x - startPoint->x; //(x_1 - x_0), used for `d_next` //y'know, the compiler would have probably made these constants for us inline......
    int ydiff = startPoint->y - endPoint->y; //(y_0 - y_1), used for `d_next`
    float d = 2*implicit_line(startPoint, endPoint, startPoint->x, startPoint->y) +2*ydiff + xdiff;

    //Point drawing loop
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);

    while(x <= endPoint->x){
        glVertex2d(
            lineCorrMat[0]*x + lineCorrMat[1]*y,
            win_h - lineCorrMat[2]*x - lineCorrMat[3]*y
        );

        if(d > 0){
            d = d + 2*ydiff;
        } else {
            y++;
            d = d + 2*ydiff + 2*xdiff;
        }
        x++;
    }
    glEnd();
}


int implicit_line(Point* p0, Point* p1, int x, int y){
    return (p0->y - p1->y)*x + (p1->x - p0->x)*y +p0->x*p1->y - p1->x*p0->y;
}

void draw_circle(){
    //Draw center dot
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2d(points[0].x, win_h - points[0].y);
    glEnd();

    //Init values
    float xComponents = (float)points[0].x - (float)points[1].x;
    float yComponents = (float)points[0].y - (float)points[1].y;
    int radius = ( std::sqrt(xComponents*xComponents + yComponents*yComponents) );

    int y = radius;
    // float d = 5/4 - (float)radius;
    int d = 5 - 4*radius;

    //Drawing loop
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);

    for(int x=0; x <= y; x++){

        glVertex2d(points[0].x + x, win_h - points[0].y - y);
        glVertex2d(points[0].x + x, win_h - points[0].y + y);
        glVertex2d(points[0].x - x, win_h - points[0].y - y);
        glVertex2d(points[0].x - x, win_h - points[0].y + y);

        glVertex2d(points[0].x + y, win_h - points[0].y - x);
        glVertex2d(points[0].x + y, win_h - points[0].y + x);
        glVertex2d(points[0].x - y, win_h - points[0].y - x);
        glVertex2d(points[0].x - y, win_h - points[0].y + x);

        if(d < 0){
            // d += 2*x + 3;
            d += 8*x + 12;
        } else {
            // d += 2*(x-y) + 5;
            d += 8*(x-y) + 20;
            y--;
        }
    }
    glEnd();
}

float implicit_circle(float x, float y, float r){
    return x*x + y*y - r*r;
}