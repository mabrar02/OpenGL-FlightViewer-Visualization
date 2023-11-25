


/* include the library header files */
#include <stdlib.h>
#include <stdio.h>
#include <freeglut.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define textureWidth  64
#define textureHeight 64


void printKeyboardControls(void);
void positionCamera(void);
void drawOrigin(void);

int originalWindowPosX = 100;
int originalWindowPosY = 150;
int originalWidth = 900;
int originalHeight = 600;

int wireframeToggled = 0;
int fullScreenToggled = 0;
int seaSkyToggled = 0;
int mountainToggled = 0;
int fogToggled = 0; 

GLfloat moveSpeed = 0.01;
GLfloat speedIncrement = 0.001;

/************************************************************************

	Function:		myDisplay

	Description:	Display callback, clears frame buffer and depth buffer,
					Draws the texture mapped polygon.

*************************************************************************/
void myDisplay(void)
{

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the identity matrix into the model view matrix
	glLoadIdentity();

	positionCamera();

	if (wireframeToggled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	if (seaSkyToggled) {

	}
	else {
		drawOrigin();
	}




	// swap the drawing buffers
	glutSwapBuffers();
}

void drawOrigin(void) {


	//origin lines
	glBegin(GL_LINES);
	glLineWidth(5.0f);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();

	glLineWidth(1.0f);
	glColor3f(1.0, 1.0, 1.0);
	
	// origin ball
	GLUquadric* quad = gluNewQuadric();
	glPushMatrix();
	glScalef(0.01, 0.01, 0.01);
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();

	glColor3f(129.0 / 255.0, 167.0 / 255.0, 230.0 / 255.0);
	glPushMatrix();

	glScalef(5.0, 0.0, 5.0);

	glBegin(GL_POLYGON);
	glVertex3f(-0.5, 0, 0.5);
	glVertex3f(-0.5, 0, -0.5);
	glVertex3f(0.5, 0, -0.5);
	glVertex3f(0.5, 0, 0.5);
	glEnd();

	glBegin(GL_LINES);
	for (GLfloat z = -0.4; z <= 0.4; z += 0.05) {
		glVertex3f(-0.5, 0, z);
		glVertex3f(0.5, 0, z);
	}
	glEnd();
	
	glBegin(GL_LINES);
	for (GLfloat x = -0.4; x <= 0.4; x += 0.05) {
		glVertex3f(x, 0, -0.5);
		glVertex3f(x, 0, 0.5);
	}
	glEnd();

	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}

void positionCamera(void) {
	gluLookAt(1, 1, 1, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}




/************************************************************************

	Function:		initializeGL

	Description:	Initializes the OpenGL rendering context for display

*************************************************************************/
void initializeGL(void)
{
	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// enable blending for fading opacity
	glEnable(GL_BLEND);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set window mode to perspective
	gluPerspective(45, (GLfloat)originalWidth / (GLfloat)originalHeight, 0.1, 20);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}

void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, (GLfloat) newWidth / (GLfloat) newHeight, 0.1, 20);

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}


void myKeyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		exit(0);
	}
	else if (key == 'w') {
		if (wireframeToggled == 0) {
			wireframeToggled = 1;
		}
		else {
			wireframeToggled = 0;
		}
	}
	else if (key == 'f') {
		if (fullScreenToggled == 0) {
			fullScreenToggled = 1;
			glutFullScreen();
		}
		else {
			fullScreenToggled = 0;
			glutPositionWindow(originalWindowPosX, originalWindowPosY);
			glutReshapeWindow(originalWidth, originalHeight);
		}
	}
	else if (key == 's') {
		if (seaSkyToggled == 0) {
			seaSkyToggled = 1;
		}
		else {
			seaSkyToggled = 0;
		}
	}
	else if (key == 'g') {
		if (fogToggled == 0) {
			fogToggled = 1;
		}
		else {
			fogToggled = 0;
		}
	}
	else if (key == 'm') {
		if (mountainToggled == 0) {
			mountainToggled = 1;
		}
		else {
			mountainToggled = 0;
		}
	}
	glutPostRedisplay();
}


void mySpecialKeyboard(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			break;
		case GLUT_KEY_DOWN:
			break;
		case GLUT_KEY_PAGE_UP:
			moveSpeed += speedIncrement;
			break;
		case GLUT_KEY_PAGE_DOWN:
			if (moveSpeed - speedIncrement > 0) {
				moveSpeed -= speedIncrement;
			}
			break;
	}
	glutPostRedisplay();
}

/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// seed random number generator
	srand(time(NULL));


	// initialize the toolkit
	glutInit(&argc, argv);

	// set display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set window size
	glutInitWindowSize(originalWidth, originalHeight);

	// set window position on screen
	glutInitWindowPosition(originalWindowPosX, originalWindowPosY);

	// open the screen window
	glutCreateWindow("Flight Viewer");

	// register redraw function
	glutDisplayFunc(myDisplay);

	glutReshapeFunc(myReshape);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(mySpecialKeyboard);

	// register the blend function to enable opacity fading
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize the rendering context
	initializeGL();

	// print the controls on the console for the user
	printKeyboardControls();

	// go into a perpetual loop
	glutMainLoop();
}

/************************************************************************

	Function:		printKeyboardControls

	Description:	prints the necessary user key controls to the console
					for ease of understanding the program

*************************************************************************/
void printKeyboardControls(void)
{
	printf("Scene Controls\n-------------------\n\n");
	printf("r \t: toggle rings\n");
	printf("s \t: toggle stars\n");
	printf("c \t: toggle sun's corona\n");
	printf("LClick \t: shoot left lasers\n");
	printf("RClick \t: shoot right lasers\n\n");

	printf("Camera Controls\n------------------- \n\n");
	printf("Up \tArrow\t: \tmove up\n");
	printf("Down \tArrow\t: \tmove down\n");
	printf("Right \tArrow\t: \tmove right\n");
	printf("Left \tArrow\t: \tmove left\n");
	printf("PAGE \tUP\t: \tforward\n");
	printf("PAGE \tDOWN\t: \tbackward\n");


	printf("\nNote: May need to use FN key to use Page Up and Page Down on Laptops.\n");
}