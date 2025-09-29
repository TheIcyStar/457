/*
    CMPSC 457 Section 001
    Homework 3

    Alexander Petrov
    aop5448

    10/1/25

    Description:
        Takes three points from a user and draws a triangle depending
        on the shading mode.
    Available Shading modes:
        Press w for WIREFRAME
        Press f for FLAT
        press g for GOURAUD
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>


using std::cin;
using std::cerr;
using std::endl;


// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);




// Simple structure for a point
struct Point {
    int x;
    int y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}
};

struct Color {
    float r;
    float g;
    float b;

    Color() : r(0), g(0), b(0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};


// helpers
void init();
void addPoint(int x, int y);
void keyboard_input();
void draw_point(int x, int y, Color c);
void draw_line(int x0, int y0, int x1, int y1, Color c);

void draw_triangle();
void triangle_wireframe(Color color);


// State
enum ShadingMode { WIREFRAME, FLAT, GOURAUD };
ShadingMode shading_mode = WIREFRAME;

int win_w = 512;
int win_h = 512;

Point points[3];
int num_points; // Used to keep track of how many points I have so far



int main(int argc, char* argv[]){
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

    // other stuffs like background color, viewing, etc will be set up in this function.
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

//
// Callbacks //
//

// called when the window is resized/moved (plus some other cases)
void reshape(int w, int h){
    win_w = w;
    win_h = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

    glViewport(0, 0, win_w, win_h);
}


// called when the window needs to be redrawn
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    draw_triangle();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y){
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
void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'q':  // quit the program
            exit(0);
        case 'f':  // flat shading
            shading_mode = FLAT;
            break;
        case 'g':  // gouraud shading
            shading_mode = GOURAUD;
            break;
        case 'k':  // for grading purpose only--do not modify
            keyboard_input();
            num_points = 0;
            break;
        default:
            shading_mode = WIREFRAME;
        break;
    }
}

void init(){
    // set background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // create viewing volume
    // -- will use orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

}

//
// Helpers //
//

// add the point just selected by mouse button
void addPoint(int x, int y){
    points[num_points++] = Point(x, y);
    if (num_points == 3) {
        // //Ensure counter-clockwise points as needed
        // //turns out its not needed at all
        // int crossProd = (points[1].x - points[0].x)*(points[2].y - points[0].y) - (points[1].y - points[0].y)*(points[2].x - points[0].x);
        // if(crossProd < 0){
        //     int temp = points[1].x;
        //     points[1].x = points[2].x;
        //     points[2].x = temp;

        //     temp = points[1].y;
        //     points[1].y = points[2].y;
        //     points[2].y = temp;
        //     std::cout << "CCwise'd it!\n";
        // }

        num_points = 0;
        glutPostRedisplay();
    }
}



// for grading purpose only
void keyboard_input(){
    int x, y;
    num_points = 0;
    for (int i=0; i<3; i++) {
        cerr << "Enter point " << i << " => ";
        cin >> x >> y;
        cerr << endl;
        addPoint(x, y);
    }
}


void draw_point(int x, int y, Color c)
{
    glBegin(GL_POINTS);
    glColor3f(c.r, c.g, c.b);
    glVertex2d(x, win_h-y);
    glEnd();
}


void draw_line(int x0, int y0, int x1, int y1, Color c)
{
    glBegin(GL_LINES);
    glColor3f(c.r, c.g, c.b);
    glVertex2d(x0, win_h - y0);
    glVertex2d(x1, win_h - y1);
    glEnd();
}


// a helper function to draw a triangle
void draw_triangle(){
    switch (shading_mode) {
        case WIREFRAME: {
            triangle_wireframe(Color(1.0, 0.0, 0.0));
            break;
        }

        case FLAT: {
            Color color(0.0, 1.0, 0.0);
            int xMin = points[0].x;
            int xMax = points[0].x;
            int yMin = points[0].y;
            int yMax = points[0].y;
            float alpha, beta, gamma;

            //precalculate bits of the beta/gamma equations
            //Compiler would have probably inlined it but... ehh
            int YaMinusYc = points[0].y - points[2].y;
            int XcMinusXa = points[2].x - points[0].x;
            int XaTimesYcMinusXcTimesYa = points[0].x*points[2].y - points[2].x*points[0].y;
            int betaDenominator = YaMinusYc*points[1].x + XcMinusXa*points[1].y + XaTimesYcMinusXcTimesYa;

            int YaMinusYb = points[0].y - points[1].y;
            int XbMinusXa = points[1].x - points[0].x;
            int XaTimesYbMinusXbTimesYa = points[0].x*points[1].y - points[1].x*points[0].y;
            int gammaDenominator = YaMinusYb*points[2].x + XbMinusXa*points[2].y + XaTimesYbMinusXbTimesYa;


            //find x/y min/max
            for(int i=1; i<3; i++){
                xMin = points[i].x < xMin ? points[i].x : xMin;
                xMax = points[i].x > xMax ? points[i].x : xMax;
                yMin = points[i].y < yMin ? points[i].y : yMin;
                yMax = points[i].y > yMax ? points[i].y : yMax;
            }

            for(int y=yMin; y <= yMax; y++){
                for(int x=xMin; x <= xMax; x++){
                    beta = (float)(YaMinusYc*x + XcMinusXa*y + XaTimesYcMinusXcTimesYa) / (float)betaDenominator;
                    gamma = (float)(YaMinusYb*x + XbMinusXa*y + XaTimesYbMinusXbTimesYa) / (float)gammaDenominator;
                    alpha = 1 - beta - gamma;

                    if(alpha > 0 && beta > 0 && gamma > 0){
                        draw_point(x,y, color);
                    }
                }
            }

            break;
        }

        case GOURAUD: {
            Color c0(1.0, 0.0, 0.0);
            Color c1(0.0, 1.0, 0.0);
            Color c2(0.0, 0.0, 1.0);

            //Braindead copypaste of the flat shading, plus some color stuff
            //Code comments said to implement it "HERE", so HERE it is

            int xMin = points[0].x;
            int xMax = points[0].x;
            int yMin = points[0].y;
            int yMax = points[0].y;
            float alpha, beta, gamma;
            Color color;

            //precalculate bits of the beta/gamma equations
            //Compiler would have probably inlined it but... ehh
            int YaMinusYc = points[0].y - points[2].y;
            int XcMinusXa = points[2].x - points[0].x;
            int XaTimesYcMinusXcTimesYa = points[0].x*points[2].y - points[2].x*points[0].y;
            int betaDenominator = YaMinusYc*points[1].x + XcMinusXa*points[1].y + XaTimesYcMinusXcTimesYa;

            int YaMinusYb = points[0].y - points[1].y;
            int XbMinusXa = points[1].x - points[0].x;
            int XaTimesYbMinusXbTimesYa = points[0].x*points[1].y - points[1].x*points[0].y;
            int gammaDenominator = YaMinusYb*points[2].x + XbMinusXa*points[2].y + XaTimesYbMinusXbTimesYa;


            //find x/y min/max
            for(int i=1; i<3; i++){
                xMin = points[i].x < xMin ? points[i].x : xMin;
                xMax = points[i].x > xMax ? points[i].x : xMax;
                yMin = points[i].y < yMin ? points[i].y : yMin;
                yMax = points[i].y > yMax ? points[i].y : yMax;
            }

            for(int y=yMin; y <= yMax; y++){
                for(int x=xMin; x <= xMax; x++){
                    beta = (float)(YaMinusYc*x + XcMinusXa*y + XaTimesYcMinusXcTimesYa) / (float)betaDenominator;
                    gamma = (float)(YaMinusYb*x + XbMinusXa*y + XaTimesYbMinusXbTimesYa) / (float)gammaDenominator;
                    alpha = 1 - beta - gamma;

                    if(alpha > 0 && beta > 0 && gamma > 0){
                        color = Color(
                            c0.r * (float)alpha + c1.r * (float)beta + c2.r * (float)gamma,
                            c0.g * (float)alpha + c1.g * (float)beta + c2.g * (float)gamma,
                            c0.b * (float)alpha + c1.b * (float)beta + c2.b * (float)gamma
                        );
                        draw_point(x,y, color);
                    }
                }
            }

            break;
        }
    }
}


// just draw 3 lines using the 3 points
void triangle_wireframe(Color color){
    for (int i=0; i<3; i++) {
        int x0 = points[i].x, y0 = points[i].y;
        int x1 = points[(i+1)%3].x, y1 = points[(i+1)%3].y;
        draw_line(x0, y0, x1, y1, color);
    }
}