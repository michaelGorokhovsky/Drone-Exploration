/*******************************************************************
		   Michael Gorokhovsky 
********************************************************************/
#define _USE_MATH_DEFINES
#define RAD (M_PI/180)

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include "CubeMesh.h"

int shootTorpedo();
int drone2Move();

const int meshSize = 100;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels
static CubeMesh cube[10];
double lowerX[10];
double upperX[10];
double lowerZ[10];
double upperZ[10];
double upperY[10];
int abc = 0;
bool firstP = false;
float cEyeX = 0;
float cEyeY = 0;
float cEyeZ = 0;
float camX = 0;
float camY = 0;
float camZ = 0;
float angleCamY = 0;
float zoom = 0.0;

bool rotors = false;
static GLfloat spin = 0.0;
static GLfloat horzSpeed = 0.0;
static GLfloat LRTurn = 0.0;
static GLfloat LRHorzSpeed = 0.0;
static GLfloat elevation = 20.0;

static GLfloat horzSpeed2 = -40.0;
static GLfloat LRTurn2 = 270.0;
static GLfloat LRHorzSpeed2 = 0.0;
static GLfloat elevation2 = 40.0;
static GLfloat lowerX2 = 0.0;
static GLfloat upperX2 = 0.0;
static GLfloat lowerZ2 = 0.0;
static GLfloat upperZ2 = 0.0;
static GLfloat lowerY2 = 0.0;
static GLfloat upperY2 = 0.0;



static GLfloat torpSpeed = 0.0;
static float torpLRSpeed = 0.0;
bool shootTorp = false;
bool drone2Dir = true;

static int currentButton;
static unsigned char currentKey;

static int numBuildings = 0;
static double camXPos = 0.0;
static double camYPos = 6.0;
int prevMouseXPos, prevMouseYPos;
//static Building building;
bool transl = false;
bool scaleY = false;
bool scaleXZ = false;
bool extrusion = false;
bool polygonScaleMode = false;
bool justChanged = false;
static double builPosX = 0;
static double builPosZ = 0.0;


static double builScaleX = 1.0;
static double builScaleY = 1.0;
static double builScaleZ = 1.0;




// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };


//Building after you are done editing
static GLfloat buil_mat_ambient[] = { 0.0F, 0.05F, 0.0F, 1.0F };
static GLfloat buil_mat_specular[] = { 0.0F, 0.0F, 0.004F, 1.0F };
static GLfloat buil_mat_diffuse[] = { 0.5F, 0.0F, 0.5F, 1.0F };
static GLfloat buil_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);


int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 3");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutTimerFunc(1, shootTorpedo, 0);
	glutTimerFunc(1, drone2Move, 0);
	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);   // This light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	// Set up ground quad mesh
	Vector3D origin = NewVector3D(-50.0f, 0.0f, 50.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 100.0, 100.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	//Set(&BBox.min, -8.0f, 0.0, -8.0);
	//Set(&BBox.max, 8.0f, 6.0,  8.0);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
float leftSideX[100];
float rightSideX[100];
float topSideZ[100];
float bottomSideZ[100];
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//First Person View
	if (firstP) {
		gluLookAt(cEyeX, cEyeY, cEyeZ, camX, camY + angleCamY, camZ, 0.0, 1.0, 0.0);
	}
	else {
		gluLookAt(camXPos, camYPos + 50, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

	

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);


	// Draw Drone
	// Apply transformations to construct drone, modify this!
	GLdouble bodyXpos = -0.5;
	GLdouble bodyYpos = 4.0;
	GLdouble bodyZpos = 0.0;
	//draw the body of the drone
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(bodyXpos, bodyYpos, bodyZpos);
	//glRotatef(45.0, 0.0, 1.0, 0.0);
	glScalef(7.0, 1.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	//draw propeller beam 1

	GLdouble propeller1Xpos = 3.0;
	GLdouble propeller1Ypos = 4.0;
	GLdouble propeller1Zpos = 1.0;
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(propeller1Xpos, propeller1Ypos + 2, propeller1Zpos);
	glRotatef(45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();


	//Need an end point for our propellor cylinder 
	//Math done via special triangles
	GLdouble p1Final1x = propeller1Xpos + 1;
	GLdouble p1Final1y = propeller1Ypos + (2 / sqrt(2)) + 2;
	GLdouble p2Final1x = propeller1Xpos;
	GLdouble p2Final1y = propeller1Ypos + (2 / sqrt(2)) + 2;

	//Now we need rotors around the endpoint of the propellors
	//rotor 1
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, 2, 5, 1);
	glTranslatef(p1Final1x, p1Final1y, propeller1Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, 3, 5, 1);
	glTranslatef(p2Final1x, p2Final1y, propeller1Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();


	//draw propeller beam 2

	GLdouble propeller2Xpos = 0.0;
	GLdouble propeller2Ypos = 4.0;
	GLdouble propeller2Zpos = 2.0;
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(propeller2Xpos, propeller2Ypos + 2, propeller2Zpos - 3);
	glRotatef(-45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	GLdouble p3Final1x = propeller2Xpos + 1;
	GLdouble p3Final1y = propeller2Ypos + (2 / sqrt(2)) + 2;
	GLdouble p4Final1x = propeller2Xpos;
	GLdouble p4Final1y = propeller2Ypos + (2 / sqrt(2)) + 2;

	//rotor 1 on beam 2
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, p3Final1x - 0.5, p3Final1y + 1, propeller2Zpos - 2.5);
	glTranslatef(p3Final1x + 0.2, p3Final1y, propeller2Zpos - 1);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2 on beam 2
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, p4Final1x, p4Final1y - 1, propeller2Zpos - 3.5);
	glTranslatef(p4Final1x - 0.3, p4Final1y, propeller2Zpos - 5);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//draw propeller beam 3

	GLdouble propeller3Xpos = -3.0;
	GLdouble propeller3Ypos = 4.0;
	GLdouble propeller3Zpos = 1.0;
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(propeller3Xpos, propeller3Ypos + 2, propeller3Zpos);
	glRotatef(45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	GLdouble p5Final1x = propeller3Xpos + 1;
	GLdouble p5Final1y = propeller3Ypos + (2 / sqrt(2)) + 2;
	GLdouble p6Final1x = propeller3Xpos;
	GLdouble p6Final1y = propeller3Ypos + (2 / sqrt(2)) + 2;

	//rotor 1 on beam 3
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, p5Final1x - 1, p5Final1y, propeller3Zpos);
	glTranslatef(p5Final1x + 0.2, p5Final1y, propeller3Zpos + 1);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2 on beam 3
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glRotated(spin, p6Final1x - 0.3, p6Final1y, propeller3Zpos + 1);
	glTranslatef(p6Final1x - 0.3, p6Final1y, propeller3Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//Drawing a 'front' or 'head' for the drone
	glPushMatrix();
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(bodyXpos + 3, bodyYpos, bodyZpos);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//Drawing the torpedo
	glPushMatrix();
	glTranslatef(torpSpeed, 0, torpLRSpeed);
	glTranslated(0, elevation, 0);
	glTranslated(horzSpeed, 0, LRHorzSpeed);
	glRotated(LRTurn, 0, 1, 0);
	glTranslatef(bodyXpos + 3, bodyYpos, bodyZpos);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();


	drawBuil();
	DrawDrone2();


	//Check if any collisions

	//Collision with ground
	if (elevation == -3) {
		resetDrone();
	}

	//Collision with buildings
	for (int i = 0; i < 10; i++)
	if (bodyYpos + elevation < upperY[i]) {
		abc += 1;
		if ((bodyXpos + 3 + horzSpeed) < upperX[i] && (bodyXpos + 3 + horzSpeed) > lowerX[i]) {
			if ((bodyZpos + LRHorzSpeed) < upperZ[i] && (bodyZpos + LRHorzSpeed) > lowerZ[i]) {
				resetDrone();
			}
		}
	}
	// Draw ground mesh
	DrawMeshQM(&groundMesh, meshSize);

	cEyeX = horzSpeed + (2.5 * (cos(LRTurn * RAD)));
	cEyeY = elevation + 3;
	cEyeZ = LRHorzSpeed - (2.5 * (sin(LRTurn * RAD)));
	camX = cEyeX + ((cos(LRTurn * RAD)) * 5);
	camY = cEyeY - 1;
	camZ = cEyeZ - ((sin(LRTurn * RAD)) * 5);


	//printf("horzspeed is %f\n", horzSpeed);

	glutSwapBuffers();   // Double buffering, swap buffers
}


DrawDrone2() {

	glMaterialfv(GL_FRONT, GL_AMBIENT, buil_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, buil_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, buil_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, buil_shininess);
	// Draw Drone
	// Apply transformations to construct drone, modify this!
	GLdouble bodyXpos = -0.5;
	GLdouble bodyYpos = 4.0;
	GLdouble bodyZpos = 0.0;
	//draw the body of the drone
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glTranslatef(bodyXpos, bodyYpos, bodyZpos);
	//glRotatef(45.0, 0.0, 1.0, 0.0);
	glScalef(7.0, 1.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	//draw propeller beam 1

	GLdouble propeller1Xpos = 3.0;
	GLdouble propeller1Ypos = 4.0;
	GLdouble propeller1Zpos = 1.0;
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glTranslatef(propeller1Xpos, propeller1Ypos + 2, propeller1Zpos);
	glRotatef(45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();


	//Need an end point for our propellor cylinder 
	//Math done via special triangles
	GLdouble p1Final1x = propeller1Xpos + 1;
	GLdouble p1Final1y = propeller1Ypos + (2 / sqrt(2)) + 2;
	GLdouble p2Final1x = propeller1Xpos;
	GLdouble p2Final1y = propeller1Ypos + (2 / sqrt(2)) + 2;

	//Now we need rotors around the endpoint of the propellors
	//rotor 1
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, 2, 5, 1);
	glTranslatef(p1Final1x, p1Final1y, propeller1Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, 3, 5, 1);
	glTranslatef(p2Final1x, p2Final1y, propeller1Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();


	//draw propeller beam 2

	GLdouble propeller2Xpos = 0.0;
	GLdouble propeller2Ypos = 4.0;
	GLdouble propeller2Zpos = 2.0;
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glTranslatef(propeller2Xpos, propeller2Ypos + 2, propeller2Zpos - 3);
	glRotatef(-45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	GLdouble p3Final1x = propeller2Xpos + 1;
	GLdouble p3Final1y = propeller2Ypos + (2 / sqrt(2)) + 2;
	GLdouble p4Final1x = propeller2Xpos;
	GLdouble p4Final1y = propeller2Ypos + (2 / sqrt(2)) + 2;

	//rotor 1 on beam 2
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, p3Final1x - 0.5, p3Final1y + 1, propeller2Zpos - 2.5);
	glTranslatef(p3Final1x + 0.2, p3Final1y, propeller2Zpos - 1);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2 on beam 2
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, p4Final1x, p4Final1y - 1, propeller2Zpos - 3.5);
	glTranslatef(p4Final1x - 0.3, p4Final1y, propeller2Zpos - 5);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//draw propeller beam 3

	GLdouble propeller3Xpos = -3.0;
	GLdouble propeller3Ypos = 4.0;
	GLdouble propeller3Zpos = 1.0;
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glTranslatef(propeller3Xpos, propeller3Ypos + 2, propeller3Zpos);
	glRotatef(45.0, 0.5, 0.5, 0.0);
	glScalef(0.4, 2.0, 0.4);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	GLdouble p5Final1x = propeller3Xpos + 1;
	GLdouble p5Final1y = propeller3Ypos + (2 / sqrt(2)) + 2;
	GLdouble p6Final1x = propeller3Xpos;
	GLdouble p6Final1y = propeller3Ypos + (2 / sqrt(2)) + 2;

	//rotor 1 on beam 3
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, p5Final1x - 1, p5Final1y, propeller3Zpos);
	glTranslatef(p5Final1x + 0.2, p5Final1y, propeller3Zpos + 1);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();
	//rotor 2 on beam 3
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glRotated(spin, p6Final1x - 0.3, p6Final1y, propeller3Zpos + 1);
	glTranslatef(p6Final1x - 0.3, p6Final1y, propeller3Zpos);
	glScalef(0.5, 0.1, 0.5);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//Drawing a 'front' or 'head' for the drone
	glPushMatrix();
	glTranslated(0, elevation2, 0);
	glTranslated(horzSpeed2, 0, LRHorzSpeed2);
	glRotated(LRTurn2, 0, 1, 0);
	glTranslatef(bodyXpos + 3, bodyYpos, bodyZpos);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	//update bounding box

	upperX2 = bodyXpos + horzSpeed2 + 2;
	lowerX2 = bodyXpos + horzSpeed2 - 2;
	upperZ2 = bodyZpos + LRHorzSpeed2 + 5;
	lowerZ2 = bodyZpos + LRHorzSpeed2 - 5;
	upperY2 = bodyYpos + elevation2 + 6;
	lowerY2 = bodyYpos + elevation2 - 6;
}
drone2Move() {

	if (drone2Dir == false) {
		LRHorzSpeed2 += 0.1;
	}
	else {
		LRHorzSpeed2 -= 0.1;
	}
	
	if (LRHorzSpeed2 >= 30) {
		drone2Dir = true;
	} 
	if (LRHorzSpeed2 <= -30) {
		drone2Dir = false;
	}

	glutPostRedisplay();
	glutTimerFunc(1, drone2Move, 0);

}
//shoot torpedo
shootTorpedo() {
	if (shootTorp == true) {
			torpSpeed += cos(LRTurn * RAD) / 20;
			torpLRSpeed -= sin(LRTurn * RAD) / 20;
	}
	//reset if torpedo hits end of the world
	if (torpSpeed >75 || torpSpeed < -75 || torpLRSpeed > 75 || torpLRSpeed < -75) {
		torpSpeed = 0;
		torpLRSpeed = 0;
		shootTorp = false;
	}
	//reset torpedo and drone2 if torpedo hits drone2
	if (shootTorp && torpSpeed + horzSpeed >= lowerX2 && torpSpeed + horzSpeed <= upperX2 && torpLRSpeed + LRHorzSpeed >= lowerZ2 && torpLRSpeed + LRHorzSpeed <= upperZ2 && elevation <= upperY2 && elevation >= lowerY2) {
		if (horzSpeed2 < 0) {
			horzSpeed2 = 40;
		}
		else {
			horzSpeed2 = -40;
		}
	}

	//torpSpeed = 0;
	//torpLRSpeed = 0;
	//shootTorp = false;
	glutPostRedisplay();
	glutTimerFunc(1, shootTorpedo, 0);
	
}
//After a collision, reset the drone's position
resetDrone() {
	elevation = 20;
	horzSpeed = 0;
	LRHorzSpeed = 0;
	LRTurn = 0;
}

//handles the spinning of each propellor
void spinRotor(void)
{
	spin += 0.1;
	if (spin > 360.0)
		spin -= 360.0;
	
	glutPostRedisplay();
}
//handles forward ghorizontal movement
moveForward()
{
	if (spin != 0)
		if (LRTurn == 0)
		{
			horzSpeed += 1;
		}
		else {
			horzSpeed += cos(LRTurn * RAD);
			LRHorzSpeed -= sin(LRTurn * RAD);
		}

	else
		printf("rotor must be spinning before movement!\n");



	glutPostRedisplay();
}
//handles backwards ghorizontal movement
moveBackward()
{
	if (spin != 0)
		if (LRTurn == 0)
		{
			horzSpeed -= 1;
		}
		else {
			horzSpeed -= cos(LRTurn * RAD);
			LRHorzSpeed += sin(LRTurn * RAD);
		}

	else
		printf("rotor must be spinning before movement!\n");
	glutPostRedisplay();
}
//handles turning left
turnLeft()
{
	LRTurn += 1;

}
//handles turning right
turnRight()
{
	LRTurn -= 1;

}
//raising the drone
ascend()
{
	elevation += 1;
}
//lowering the drone
descend()
{
	elevation -= 1;
}

//draw 10 buildings
drawBuil() 
{
	//Draw 10 Buildings
	//building1
	glPushMatrix();
	glTranslatef(25, 0, 25);
	glScalef(5, 20, 5);
	glTranslatef(0, 1, 0);
	cube[0] = newCube();
	drawCube(&cube[0]);
	glPopMatrix();
	lowerX[0] = 20;
	upperX[0] = 35;
	lowerZ[0] = 17;
	upperZ[0] = 35;
	upperY[0] = 40;
	//building2
	glPushMatrix();
	glTranslatef(-25, 0, -25);
	glScalef(2, 25, 5);
	glTranslatef(0, 1, 0);
	cube[1] = newCube();
	drawCube(&cube[1]);
	glPopMatrix();
	lowerX[1] = -27;
	upperX[1] = -18;
	lowerZ[1] = -32;
	upperZ[1] = -15;
	upperY[1] = 50;
	//building3
	glPushMatrix();
	glTranslatef(25, 0, -25);
	glScalef(5, 27, 5);
	glTranslatef(0, 1, 0);
	cube[2] = newCube();
	cube[2] = newCube();
	drawCube(&cube[2]);
	glPopMatrix();
	lowerX[2] = 20;
	upperX[2] = 35;
	lowerZ[2] = -32;
	upperZ[2] = -15;
	upperY[2] = 54;
	//building4
	glPushMatrix();
	glTranslatef(-25, 0, 25);
	glScalef(5, 25, 7);
	glTranslatef(0, 1, 0);
	cube[3] = newCube();
	drawCube(&cube[3]);
	glPopMatrix();
	lowerX[3] = -30;
	upperX[3] = -15;
	lowerZ[3] = 16;
	upperZ[3] = 35;
	upperY[3] = 50;
	//building5
	glPushMatrix();
	glTranslatef(-25, 0, 0);
	glScalef(8, 30, 3);
	glTranslatef(0, 1, 0);
	cube[4] = newCube();
	drawCube(&cube[4]);
	glPopMatrix();
	lowerX[4] = -33;
	upperX[4] = -11;
	lowerZ[4] = -5;
	upperZ[4] = 6;
	upperY[4] = 60;
	//building6
	glPushMatrix();
	glTranslatef(25, 0, 0);
	glScalef(5, 25, 7);
	glTranslatef(0, 1, 0);
	cube[5] = newCube();
	drawCube(&cube[5]);
	glPopMatrix();
	lowerX[5] = 20;
	upperX[5] = 35;
	lowerZ[5] = -9;
	upperZ[5] = 10;
	upperY[5] = 50;
	//building7
	glPushMatrix();
	glTranslatef(0, 0, 25);
	glScalef(7, 15, 5);
	glTranslatef(0, 1, 0);
	cube[6] = newCube();
	drawCube(&cube[6]);
	glPopMatrix();
	lowerX[6] = -7;
	upperX[6] = 14;
	lowerZ[6] = 17;
	upperZ[6] = 33;
	upperY[6] = 30;
	//building8
	glPushMatrix();
	glTranslatef(0, 0, -25);
	glScalef(5, 26, 7);
	glTranslatef(0, 1, 0);
	cube[7] = newCube();
	drawCube(&cube[7]);
	glPopMatrix();
	lowerX[7] = -6;
	upperX[7] = 10;
	lowerZ[7] = -40;
	upperZ[7] = -12;
	upperY[7] = 54;
	//building9
	glPushMatrix();
	glTranslatef(10, 0, -10);
	glScalef(5, 25, 5);
	glTranslatef(0, 1, 0);
	cube[8] = newCube();
	drawCube(&cube[8]);
	glPopMatrix();
	lowerX[8] = 6;
	upperX[8] = 20;
	lowerZ[8] = -17;
	upperZ[8] = -3.5;
	upperY[8] = 50;
	//building10
	glPushMatrix();
	glTranslatef(-10, 0, 10);
	glScalef(6, 18, 3);
	glTranslatef(0, 1, 0);
	cube[9] = newCube();
	drawCube(&cube[9]);
	glPopMatrix();
	lowerX[9] = -16;
	upperX[9] = 2;
	lowerZ[9] = 5;
	upperZ[9] = 15;
	upperY[9] = 36;
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 400.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis

}


// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':
		
		shootTorp = true;
		break;

	case 's':
		glutIdleFunc(spinRotor);

		break;

	case 'f':
		moveForward();
		break;

	case 'b':
		moveBackward();
		break;

	case 'i':
		
		firstP = !firstP;
		break;
	case 'w':
		ascend();
		break;

	case 'x':
		descend();
		break;

	case 'z':

	case '0':
		
		break;
	case '1':

		
		break;

	case '2':
		
		break;

	case '3':
		
		break;
	case '4':
		
		break;
	case '5':
		
		break;
	case '6':
		
		break;
	case '7':
		
		break;
	case '8':
		
		break;
	case '9':
		
		break;

	}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("Usage Guide\n");
		printf("------------------------------------------------------\n");
		printf("press 's' to start your propeller\n");
		printf("press 'f' to move forward and 'b' to move back.\n");
		printf("NOTE: The propellers must be spinning in order to move\n");
		printf("press the left arrow key to turn towards the left\n");
		printf("press the right arrow key to turn towards the right\n");
		printf("press the up arrow key to tilt the camera up\n");
		printf("press the down arrow key to tilt the camera down\n");
		printf("press 'i' to switch camera modes\n");
		printf("press 'w' key to ascend\n");
		printf("press 'x' to descend\n");
		printf("press t' to shoot the torpedo \n");
	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	else if (key == GLUT_KEY_LEFT)
	{
		turnLeft();
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		turnRight();
	}
	else if (key == GLUT_KEY_UP)
	{
		angleCamY += 0.5;
		if (angleCamY >= 1) {
			angleCamY = 1;
		}
	}
	else if (key == GLUT_KEY_DOWN)
	{
		angleCamY -= 0.5;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	POINT mouse;
	GetCursorPos(&mouse);
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		if (prevMouseXPos < xMouse) {
			camXPos -= 0.5;
			if (camXPos < -56) {
				camXPos = -56;
			}
		}
		else if (prevMouseXPos > xMouse) {
			camXPos += 0.5;
			if (camXPos > 75) {
				camXPos = 75;
			}
		}
		else if (prevMouseYPos < yMouse) {
			camYPos -= 0.5;
			if (camYPos < -49.5) {
				camYPos = -49.5;
			}
		}
		else if (prevMouseYPos > yMouse) {
			camYPos += 0.5;
			if (camYPos > 33) {
				camYPos = 33;
			}
		}

	}
	prevMouseXPos = xMouse;
	prevMouseYPos = yMouse;
	glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
	// you will need to finish this if you use the mouse
	return NewVector3D(0, 0, 0);
}



