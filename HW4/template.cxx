/*
    CMPSC 457 Section 001
    Homework 4

    Alexander Petrov
    aop5448

    10/22/25
*/
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <iostream>
#include <cmath>


// typedefs
typedef glm::dvec3 Vector3;   // 3D vectors of double
typedef glm::dvec3 Point3;    // 3D points of double
typedef glm::dvec4 HPoint3;   // 3D points in Homogeneous coordinate system
typedef glm::dmat4 Matrix4;   // 4-by-4 matrix


// glut callbacks
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);


// helpers
void init();
void initObj();
void initCam();
void drawFaces();


// projection
void SetViewMatrix();
void SetOrthoMatrix();
void SetPerspMatrix();


// utilities for matrices and vectors
void    DeviceToWorld(double u, double v, double& x, double& y);
Matrix4 Mult4(Matrix4 a, Matrix4 b);         // (4x4 matrix) . (4x4 matrix)
HPoint3 Homogenize(HPoint3 a);               // returns homogenized HPoint3
HPoint3 TransHPoint3(Matrix4 m, HPoint3 p);  // (4x4 matrix) . (4x1 Vector)


// transformations
void Rotate(double dx, double dy);
void Translate_xy(double tx, double ty);
void Translate_xz(double tx, double ty);
void Scale(double s);


// transformation helpers
Matrix4 SetScaleMatrix(double sx, double sy, double sz); // 4x4 scale matrix
Matrix4 SetTransMatrix(double tx, double ty, double tz); // 4x4 translation matrix
Matrix4 SetRotMatrix(Vector3 n, double angle);           // 4x4 rotation matrix


// default device window size
int win_w = 512;
int win_h = 512;


const double EPSILON = 0.0000001;


// for your convenience while debugging
using std::cout;
using std::cerr;
using std::endl;

void PrintMat(Matrix4 m);     // print Matrix4
void PrintHPoint(HPoint3 p);  // print HPoint3
void PrintPoint(Point3 p);    // print Point3



// for tracking mouse events
struct MouseTracker
{
  int modifiers;
  int button;
  double initx, inity;
  double finalx, finaly;
};

MouseTracker mtracker;


// for camera parameters
struct Camera
{
  bool perspective;               /* projection method */
  double l, r, b, t, n, f;        /* view volume */
  Point3 eye;                     /* eye position */
  Vector3 u, v, w;                /* eye coordinate system */
  Matrix4 Mo;                     /* orthographic projection */
  Matrix4 Mv;                     /* view matrix for arbitrary view*/
  Matrix4 Mp;                     /* perspective matrix */
};

Camera cam;


// for objects
const int MAXNAMESTRING = 20;
const int MAXVERTICES = 1000;
const int MAXEDGES = 500;
const int MAXFACES = 50;


struct Object3D {
  char name[MAXNAMESTRING];       /* The name of object for printing */
  int Nvertices;                  /* number of vertices */
  int Nfaces ;                    /* number of faces */
  Matrix4 frame;                  /* the object to world coord transform */
  Point3 center;                  /* center of mass */
  HPoint3 vertices[MAXVERTICES];  /* coodrdinates of each vertex */
  int faces[MAXFACES][6];         /* If face has N vertices, give N + 1
			  	     numbers -> first the number of vertices
			  	     in the face, then the index numbers of
				     each vertex as it appears in the
				     "vertices"  array. */
};



// Note: We will keep the initial coordinates of the vertices
//       as originally given.  In other words, we will not change
//       the given coordinates of the vertices, even after any
//       transformation.  All the transformation will be recorded
//       in frame.  This way, you can reset the object to the
//       original position at any time, even after applying many
//       transformations.

Object3D obj = {
  "house", 10, 7,

  Matrix4(1.0), // identiy matrix (no transformations applied yet)

  // center of the object is at origin
  {0.0, 0.0, 0.0},

  // vertices of the object in no particular order
  {  HPoint3(0.0, 1.0, 2.0, 1.0),    HPoint3(-1.0, 0.5, 2.0, 1.0),
     HPoint3(-1.0, -1.0, 2.0, 1.0),  HPoint3(1.0, -1.0, 2.0, 1.0),
     HPoint3(1.0, 0.5, 2.0, 1.0),    HPoint3(0.0, 1.0, -2.0, 1.0),
     HPoint3(-1.0, 0.5, -2.0, 1.0),  HPoint3(-1.0, -1.0, -2.0, 1.0),
     HPoint3(1.0, -1.0, -2.0, 1.0),  HPoint3(1.0, 0.5, -2.0, 1.0)   },

  // faces
  {  {5,   0, 1, 2, 3, 4},
     {5,   9, 8, 7, 6, 5},
     {4,   4, 3, 8, 9},
     {4,   0, 4, 9, 5},
     {4,   1, 0, 5, 6},
     {4,   2, 1, 6, 7},
     {4,   3, 2, 7, 8}    }
};




// OpenGL/glut programs will have the structure shown here
//    although with different args and callbacks.
//
// You should not modify main().
// If you really want to modify it, do it at your own risk.
//
// For complete description of each glut functions used, see
// glut manual page on class website.

int main(int argc, char *argv[])
{
  // initialize glut
  glutInit(&argc, argv);

  // use double buffering with RGB colors
  // double buffer removes most of the flickering
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // set window size and position
  glutInitWindowSize(win_w, win_h);
  glutInitWindowPosition(100, 100);

  // now, create window with title "Viewing"
  glutCreateWindow("Viewing");


  // other stuffs like background color, viewing, etc will be
  // set up in this function.
  init();

  // initialize (arrange) the object
  initObj();

  // initialize the camera
  initCam();


  // register callbacks for glut
  glutDisplayFunc(display);   // for display
  glutReshapeFunc(reshape);   // for window move/resize
  glutMouseFunc(mouse);       // for mouse buttons
  glutMotionFunc(motion);     // for mouse movement while mouse button pressed
  glutKeyboardFunc(keyboard); // for keyboard


  // start event processing, i.e., accept user inputs
  glutMainLoop();

  return 0;
}

/********************************************
 *     implementation for glut callbacks    *
 ********************************************/

// called when the window is resized/moved (and some other cases)
void reshape(int w, int h)
{
  // change window size
  win_w = w;
  win_h = h;

  // set the new viewport
  glViewport(0, 0, (GLint)win_w, (GLint)win_h);

  // we will use orthographic projection when drawing the object.
  //
  // NOTE: This has nothing to do with the projections you are
  //       to implement in this assignment.  We only need this
  //       when you draw 2D lines.  In other words, find the 2D
  //       projections of the end points of a given 3D line using
  //       the projection matrices you implemented and then draw
  //       a 2D line between the projected end-points.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 511.0, 0.0, 511.0, -1.0, 1.0);
}


// called when the window needs to be redrawn
void display()
{
  // clear the buffer with bg color set in init()
  // you can think of the buffer as a raster array provided by GL
  glClear(GL_COLOR_BUFFER_BIT);

  // draw the object on the buffer you just cleared
  drawFaces();

  // swap the buffers.
  // we are using 2 buffers provided by GL (see main) -- double buffer.
  // they are called front / back buffers.
  // what you see on the screen is the content of front buffer
  // what you clear/draw above is done on back buffer
  // once drawing is done on the back buffer,
  //       you need to display the content of the back buffer.
  // swapping buffers means swapping back buffer with front buffer
  //       so that front buffer becomes back buffer and
  //       back buffer becomes front buffer.
  // once back buffer becomes front buffer, the content of it will be
  //       drawn on the screen, so you can see it.
  glutSwapBuffers();
}


// called when a mouse event (button pressed/released) occurs in glut,
//     mouse buttons are represented as
//           GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON
//     status of mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
//
void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {  // mouse pressed.  retrieve the detail
    // which button is pressed?
    mtracker.button = button;
    // any modifiers (keys like shift/ctrl/alt) pressed?
    mtracker.modifiers = glutGetModifiers();
    // convert the mouse position (x,y) in device coord. system
    //   to the corresponding position in world coord. system
    DeviceToWorld(double(x), double(y), mtracker.initx, mtracker.inity);
  }
}


// called when a mouse moves with a button pressed
void motion(int x, int y)
{
  // get the mouse position in world
  DeviceToWorld(double(x), double(y), mtracker.finalx, mtracker.finaly);

  // I'm printing the following information just for you to see that mtracker is working.
  // This will work after you complete the function DeviceToWorld() correctly; otherwise,
  //     mtracker's initx, inity, finalx, finaly will not show correct values.
  // You may want to get rid of this output when you are sure that mtracker works fine.

  // cout << '(' << x << ',' << y << ',' << mtracker.initx << ',' << mtracker.inity << ")  ("
  //      << x << ',' << y << ',' << mtracker.finalx << ',' << mtracker.finaly << ")" << endl;

  // now, process the user input, i.e., mouse movement
  switch (mtracker.button) {
  case GLUT_LEFT_BUTTON:
    if (mtracker.modifiers & GLUT_ACTIVE_SHIFT) {
      // shift + left button ==> translate in xz plane
      Translate_xz(mtracker.finalx - mtracker.initx,
		   mtracker.finaly - mtracker.inity);
    }
    else {
      // left button ==> translate in xy plane
      Translate_xy(mtracker.finalx - mtracker.initx,
		   mtracker.finaly - mtracker.inity);
    }
    break;
  case GLUT_RIGHT_BUTTON:
      // right button ==> scale
    Scale(mtracker.finalx - mtracker.initx);
    break;
  case GLUT_MIDDLE_BUTTON:
      // middle button ==> rotate
    Rotate(mtracker.finalx - mtracker.initx,
	   mtracker.finaly - mtracker.inity);
    break;
  }

  // redisplay after transformation
  glutPostRedisplay();

  // reset the mouse position
  mtracker.initx = mtracker.finalx;
  mtracker.inity = mtracker.finaly;
}


// called when a keyboard event (key typed) occurs
// you need to add cases for 'r' and 'b'
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'Q':  // quit the program
  case 'q':
    exit(0);
  case 'P':  // toggle the projection method
  case 'p':  // between orthographic and perspective projections
    cam.perspective = !cam.perspective;
    SetPerspMatrix();
    glutPostRedisplay();
    break;
  case 'R': // reset
  case 'r':
      initObj();
      initCam();
      glutPostRedisplay();
      break;
  }
}

/********************************************
 *        implementation for helpers        *
 ********************************************/


void init()
{
  // set background color to black
  glClearColor(0.0, 0.0, 0.0, 0.0);
}


// arrange the object to its initial position
// NOTE: DO NOT CHANGE THIS SETUP
void initObj()
{
  Vector3 n;

  // rotate around y-axis
  n.x = 0.0;  n.y = 1.0;  n.z = 0.0;
  double angle = M_PI / 6.0;
  Matrix4 m1 = SetRotMatrix(n, angle);

  // rotate around x-axis
  n.x = 1.0;  n.y = 0.0;  n.z = 0.0;
  angle = M_PI / 6.0;
  Matrix4 m2 = SetRotMatrix(n, angle);

  // translate so that the object is inside view volume
  // (see initCam() for the view volume)
  Matrix4 m3 = SetTransMatrix(0.0, 0.0, -5.0);

  // notice the order of the transformations applied
  //  i.e., Ry -> Rx -> T  becomes  T.Rx.Ry in matrix multiplication
  obj.frame = Mult4(m3, Mult4(m2, m1));
}


// initialize camera parameters
// NOTE: DO NOT CHANGE THIS SETUP
void initCam()
{
  // use orthographic projection as default
  cam.perspective = false;

  // camera position
  cam.eye.x = 0.0;
  cam.eye.y = 0.0;
  cam.eye.z = 0.0;

  // view volume
  cam.l = -5.0;  cam.r = 5.0;
  cam.b = -5.0;  cam.t = 5.0;
  cam.n = -1.0;  cam.f = -6.0;

  // camera coordinate system
  cam.u.x = 1.0;  cam.u.y = 0.0;  cam.u.z = 0.0;
  cam.v.x = 0.0;  cam.v.y = 1.0;  cam.v.z = 0.0;
  cam.w.x = 0.0;  cam.w.y = 0.0;  cam.w.z = 1.0;

  // set Mcam, Mp, Mo
  SetViewMatrix();
  SetPerspMatrix();
  SetOrthoMatrix();
}


// draw object faces
void drawFaces()
{
  // M = M_vp * M_orth * M_P * M_cam * M_world  //right to left
  // M = SetOrthoMatrix * SetPerspMatrix * SetViewMatrix * obj.frame //right to left
  Matrix4 M_final = Mult4(cam.Mo, Mult4(cam.Mp, Mult4(cam.Mv, obj.frame)));

  for(int iFace=0; iFace < obj.Nfaces; iFace++){
    glBegin(GL_LINE_LOOP);

    for(int iVert=0; iVert < obj.faces[iFace][0]; iVert++){
      int objVertIndex = obj.faces[iFace][iVert+1];

      HPoint3 CannPoint = TransHPoint3(M_final, obj.vertices[objVertIndex]);
      CannPoint = Homogenize(CannPoint);

      glVertex2d(CannPoint.x, CannPoint.y);
    }

    glEnd();
  }
}




/********************************************
 *      implementation for projection       *
 ********************************************/



// Mcam = (Rotate xyz (world coords) to uvw (cam coords)) * (translate world coords to cam coords)
void SetViewMatrix()
{
  Matrix4 McamRotate;
  Matrix4 McamTranslate;

  McamRotate = Matrix4(
    cam.u.x, cam.u.y, cam.u.z, 0,
    cam.v.x, cam.v.y, cam.v.z, 0,
    cam.w.x, cam.w.y, cam.w.z, 0,
    0, 0, 0, 1
  );


  McamTranslate = Matrix4(
    1, 0, 0, -cam.eye.x,
    0, 1, 0, -cam.eye.y,
    0, 0, 1, -cam.eye.z,
    0, 0, 0, 1
  );

  cam.Mv = Mult4(McamRotate, McamTranslate);

}

// Mo = Mvp . Morth
// -> Projection Transformation + Viewport transformation
// <- M_vp * M_ortho
void SetOrthoMatrix()
{
  Matrix4 Mvp;
  Matrix4 Mortho;
  double nx = (double)win_w;
  double ny = (double)win_h;

  Mvp = Matrix4(
    nx/2,   0,      0,    (nx-1)/2,
    0,      ny/2,   0,    (ny-1)/2,
    0,      0,      1,    0,
    0,      0,      0,    1
  );

  Mortho = Matrix4(
    2/(cam.r - cam.l),  0,                  0,                  -(cam.r+cam.l)/(cam.r-cam.l),
    0,                  2/(cam.t - cam.b),  0,                  -(cam.t+cam.b)/(cam.t-cam.b),
    0,                  0,                  2/(cam.n - cam.f),  -(cam.n+cam.f)/(cam.n-cam.f),
    0,                  0,                  0,                  1
  );

  cam.Mo = Mult4(Mvp, Mortho);
}


// Mp
void SetPerspMatrix()
{
  if(!cam.perspective){
    cam.Mp = Matrix4(1.0);  // identity matrix
  } else {
    cam.Mp = Matrix4(
      cam.n,  0,      0,            0,
      0,      cam.n,  0,            0,
      0,      0,      cam.n+cam.f,  -cam.f*cam.n,
      0,      0,      1,            0
    );
  }
}

/*
M = M_vp * M_orth * M_P * M_cam   //right to left
M = SetOrthoMatrix * SetPerspMatrix * SetViewMatrix  //right to left

    object space
vvv modeling transformation (obj.frame/M_world)
    world space
vvv Camera transformation (M_cam)
    ortho camera space
vvv perspective transformation (M_P)
    perspective camera space
vvv Projection transformation (M_orth, uses clip bounds)
    cannonical view volume
vvv viewport transformation (M_vp)
    viewport
*/

/********************************************************
 * implementation of utilities for matrices and vectors *
 ********************************************************/

// convert device coordinate to world coordinate
// "World" = 2d clip space after 2x2x2 cannonical cube projection, not the actual 3d world coords
void DeviceToWorld(double u, double v, double& x, double& y)
{
  x = (u - win_w)/(win_w)*2 + 1;
  y = -((v - win_h)/(win_h)*2 + 1);
}

// returns the product of two 4x4 matrices
Matrix4 Mult4(Matrix4 a, Matrix4 b)
{
  Matrix4 m;
  double sum;

  for (int j=0; j<4;  j++)
    for (int i=0; i<4; i++) {
      sum = 0.0;
      for (int k=0; k<4; k++)
	sum +=  a[j][k] * b[k][i];
      m[j][i] = sum;
    }

  return m;
}

// returns the result of homogenization of the input point
// homogenization is to make w = 1
HPoint3 Homogenize(HPoint3 a)
{
  HPoint3 p;
  if ((a.w) != 0.0) {
    p.x = a.x /(a.w);  p.y = a.y /(a.w);
    p.z = a.z /(a.w);  p.w = 1.0;
  }
  else {
    cerr << "Cannot Homogenize, returning original point\n";
    p.x = a.x;  p.y = a.y;  p.z = a.z;  p.w = a.w;
  }
  return p;
}


// returns the homogeneous 3d point as a result of
// multiplying a 4x4 matrix with a homogeneous point
HPoint3 TransHPoint3(Matrix4 m, HPoint3 p)
{
  HPoint3 temp;
  temp.x = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3]*p.w;
  temp.y = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3]*p.w;
  temp.z = m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3]*p.w;
  temp.w = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3]*p.w;
  return temp;
}

/***********************************************
 *     implementation for transformations      *
 ***********************************************/

// translation in xy-plane
void Translate_xy(double tx, double ty)
{
  obj.frame[0][3] += tx;
  obj.frame[1][3] += ty;
}

// translation in xz-plane
void Translate_xz(double tx, double ty)
{
  obj.frame[0][3] += tx;
  obj.frame[2][3] += ty;
}

// uniform scale
void Scale(double sx)
{
  const double alpha = 0.05;

  obj.frame[0][0] *= 1 + alpha*sx;
  obj.frame[1][1] *= 1 + alpha*sx;
  obj.frame[2][2] *= 1 + alpha*sx;
}

// rotation using the Rolling Ball transformation
void Rotate(double dx, double dy)
{
  const double sensitivity = 5; //R
  const double dr = sqrt(dx*dx + dy*dy);

  if (dr < EPSILON){ //avoid div by zero
    return;
  }

  Vector3 n(-dy/dr, dx/dr, 0);
  double theta = atan(dr/sensitivity);

  obj.frame = Mult4(SetRotMatrix(n, theta), obj.frame);
}

/*********************************************
 * Implementation for transformation helpers *
 *********************************************/

// returns a 4x4 scale matrix, given sx, sy, sz as inputs
Matrix4 SetScaleMatrix(double sx, double sy, double sz)
{
  Matrix4 m;
  m = Matrix4(1.0);
  m[0][0] = sx;
  m[1][1] = sy;
  m[2][2] = sz;
  return m;
}

// returns a 4x4 translation matrix, given tx, ty, tz as inputs
Matrix4 SetTransMatrix(double tx, double ty, double tz)
{
  Matrix4 m;
  m = Matrix4(1.0);
  m[0][3] = tx;
  m[1][3] = ty;
  m[2][3] = tz;
  return m;
}

// returns a 4x4 rotation matrix, given an axis and an angle
//these functions need to be called *get*foo, not *set*foo >:C
Matrix4 SetRotMatrix(Vector3 n, double angle)
{
  double cosTheta = cos(angle);
  double sinTheta = sin(angle);

  return Matrix4(
    cosTheta+n.x*n.x*(1-cosTheta),      n.x*n.y*(1-cosTheta)-n.z*sinTheta,    n.x*n.z*(1-cosTheta)+n.y*sinTheta,  0,
    n.y*n.x*(1-cosTheta)+n.z*sinTheta,  cosTheta+n.y*n.y*(1-cosTheta),        n.y*n.z*(1-cosTheta)-n.x*sinTheta,  0,
    n.z*n.x*(1-cosTheta)-n.y*sinTheta,  n.z*n.y*(1-cosTheta)+n.x*sinTheta,    cosTheta+n.z*n.z*(1-cosTheta),      0,
    0,                                  0,                                    0,                                  1
  );
}

/*********************************************
 *    for your convenience when debugging    *
 *********************************************/

// prints a 4x4 matrix
void PrintMat(Matrix4 m)
{
   for (int i=0;i<4;i++) {
     for (int j=0;j<4;j++) {
       std::cerr << m[i][j] << " ";
    }
     std::cerr << std::endl;
   }
}

// prints a homogeneous 3d point / vector
void PrintHPoint(HPoint3 p)
{
  std::cerr << "("
            << p.x << " "
            << p.y << " "
            << p.z << " "
            << p.w << ")" << std::endl;
}

// prints a 3d point / vector
void PrintPoint(Point3 p) {
  std::cerr << "("
            << p.x << " "
            << p.y << " "
            << p.z << " " << std::endl;
}