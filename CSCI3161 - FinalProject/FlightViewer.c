/************************************************************************************

	File: 			FlightViewer.c

	Description:	A complete OpenGL program simulating a planes flight,
					including a textured sea, sky, and mountains, with
					togglable scene elements. CSCI 3161 - Assignment 3



	Author:			Mahdeen Abrar (B00860738)
	Date:			Nov. 28th, 2023

*************************************************************************************/

/* include the library header files */
#include <stdlib.h>
#include <stdio.h>
#include <freeglut.h>
#include <time.h>
#include <math.h>
#include <string.h>

/* making use of stb_image.h for reading jpg images to be used as textures */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* defining constants, point and face counts for cessna and propeller given by prof */
#define CESSNA_POINT_COUNT 6763
#define CESSNA_FACE_COUNT 3639
#define PROP_POINT_COUNT 6763
#define PROP_FACE_COUNT 132
#define CESSNA_OBJECT_COUNT 34
#define M_PI 3.141592
#define MESH_RES 64

/* function signature definitions */
void printKeyboardControls(void);
void positionCamera(void);
void drawOrigin(void);
void drawCessna(void);
void drawPropellers(void);
void drawSea(void);
void drawSky(void);
void drawMountains();
void drawHUD(void);

/* struct to define polygon shapes for easy storing of individual shapes*/
typedef struct {
	int indices[50];
	int numVertices;
} PolygonShape;

/* struct used to define an object comprising of polygons, useful for constructing plane and propeller from their parts*/
typedef struct {
	PolygonShape polygons[700];
	int numPolygons;

} Object;

// cesnna arrays to help build the plane
Object cessnaParts[CESSNA_OBJECT_COUNT];
GLfloat cessnaPoints[CESSNA_POINT_COUNT][3];
GLfloat cessnaNormals[CESSNA_POINT_COUNT][3];

// propeller arrays to help build the propellers
Object propParts[3];
GLfloat propPoints[PROP_POINT_COUNT][3];
GLfloat propNormals[PROP_POINT_COUNT][3];

// window variables
int originalWindowPosX = 100;
int originalWindowPosY = 150;
int originalWidth = 900;
int originalHeight = 600;
int currentWidth = 900;
int currentHeight = 600;

// toggle variables
int wireframeToggled = 1;
int fullScreenToggled = 0;
int seaSkyToggled = 0;
int mountainToggled = 0;
int mountainTexturedToggled = 0;
int fogToggled = 0; 
int hudToggled = 0;

// position and camera variables
GLfloat camPos[3] = { 0.0, 4, 10.0 };
GLfloat forwardVector[3] = {0.0, 3.25, 7.0};
GLfloat distOffset = 3.0;
GLfloat forwardAngle = M_PI / 2;

// cesnna movement variables
GLfloat turnAngle = 0;
GLfloat moveSpeed = 0.05;
GLfloat speedIncrement = 0.001;
GLfloat heightIncrement = 0.03;
GLfloat turnRate = 0.1;

// propeller movement variables
GLfloat theta = 0.0;
GLint propellerSpeed = 2;

// light variables
GLfloat lightPos[] = { 0, 20.0, 5, 0.0 };
GLfloat cessnaShininess = 200.0;
GLfloat cessnaAmbient[] = { 0.1, 0.1, 0.1, 1.0 };

// material variables
GLfloat yellowDiffuse[] = {224.0 / 255.0, 185.0 / 255.0, 76.0 / 255.0, 1.0};
GLfloat blackDiffuse[] = { 0.0, 0.0, 0.0, 1.0};
GLfloat purpleDiffuse[] = { 154.0 / 255.0, 94.0 / 255.0, 191.0 / 255.0, 1.0};
GLfloat blueDiffuse[] = { 23.0 / 255.0, 37.0 / 255.0, 128.0 / 255.0 , 1.0};
GLfloat greenDiffuse[] = { 0, 1, 0 , 1.0 };
GLfloat lightblueDiffuse[] = { 129.0 / 255.0, 167.0 / 255.0, 230.0 / 255.0 , 1.0 };
GLfloat greyDiffuse[] = { 0.7, 0.7, 0.7 , 1.0 };
GLfloat redDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat whiteDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat whiteSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat highEmission[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat halfAmbient[] = { 0.5, 0.5, 0.5, 1.0 };


// fog variables
GLfloat fogColor[] = { 1.0, 156.0 / 255.0, 219.0 / 255.0, 1.0 };
GLfloat fogDensity = 0.0175;


// texture variables 
GLuint seaTexture;
GLuint skyTexture;
GLuint mountainTexture;

// mountain arrays to help build mountains
GLfloat coordinatePlaneSize = 4;
GLfloat mountainVertices[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountainNormals[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountain2Vertices[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountain2Normals[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountain3Vertices[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountain3Normals[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountainPolygonFaceNormals[MESH_RES][MESH_RES][3];

// mountain transformation arrays
GLfloat mountainScale[3] = { 10, 1, 10};
GLfloat mountainTranslation[3] = {0, -3, 0};

GLfloat mountain2Scale[3] = { 10, 0.25, 10 };
GLfloat mountain2Translation[3] = { -30, -0.25, -30 };

GLfloat mountain3Scale[3] = { 10, 1, 10 };
GLfloat mountain3Translation[3] = { 10, -1, 20 };

// mountain variables
GLfloat mountainHeight1 = 8;
GLfloat mountainHeight2 = 8;
GLfloat mountainHeight3 = 4;
GLfloat initialRandMount1 = 2;
GLfloat initialRandMount2 = 4;
GLfloat initialRandMount3 = 2;
GLint maxDepth = 6;

// size of sky and sea
GLint sceneSize = 70;

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

	// position the camera
	positionCamera();

	// set the directional light's position
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// if wire frame toggled, draw polygons as lines rather than filling them
	if (wireframeToggled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	// if fog toggled, draw it only on the sea
	if (fogToggled) {
		glEnable(GL_FOG);
	}
	
	// if sea and sky toggled, draw them, otherwise draw origin and base grid
	if (seaSkyToggled) {
		drawSea();
		glDisable(GL_FOG);
		drawSky();
	}
	else {
		drawOrigin();
	}

	// if mountains toggled, draw them
	if (mountainToggled) {
		drawMountains();
	}

	// draw cessna (includes propellers)
	drawCessna();

	if (hudToggled) {
		drawHUD();
	}


	// swap the drawing buffers
	glutSwapBuffers();
}


/************************************************************************

	Function:		positionCamera

	Description:	function used to position the camera to follow the
					cessna

*************************************************************************/
void positionCamera(void) {
	
	// use separate vector for cameras position and direction of travel to maintain an offset from the cam and plane
	gluLookAt(camPos[0], camPos[1], camPos[2], forwardVector[0], forwardVector[1], forwardVector[2], 0.0, 1.0, 0.0);
}


/************************************************************************

	Function:		drawHUD

	Description:	function used to draw the HUD overlay when toggled

*************************************************************************/
void drawHUD(void) {

	// disable lighting to use normal coloring
	glDisable(GL_LIGHTING);

	// disable depth testing to create a flat overlay
	glDisable(GL_DEPTH_TEST);

	// regardless if wire frame is toggled, use fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// change to projection to change camera view
	glMatrixMode(GL_PROJECTION);

	// save current projection matrix
	glPushMatrix();

	// reset projection matrix
	glLoadIdentity();

	// change camera to a flat 2D ortho camera to make overlay creation easy
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	// switch to model view to change object positions
	glMatrixMode(GL_MODELVIEW);

	// save current model view matrix
	glPushMatrix();

	// reset model view matrix
	glLoadIdentity();

	// create green overlay
	glBegin(GL_QUADS);
	glColor4f(57.0/255.0, 110.0/255.0, 52.0/255.0, 0.25);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, -1);
	glEnd();

	// variable to keep distances symmetrical
	GLfloat sidesDist = 0.5;

	// change color and line width for actual HUD elements
	glColor4f(0, 1, 0, 1);
	glLineWidth(2);

	// use line mode for our polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// middle green triangle indicator
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(-0.01, 0.02);
	glVertex2f(0.01, 0.02);
	glEnd();

	// outer box
	glBegin(GL_LINES);

	// right vertical line
	glVertex2f(sidesDist, -sidesDist);
	glVertex2f(sidesDist, sidesDist);

	// right vertical line accents
	glVertex2f(sidesDist, -sidesDist);
	glVertex2f(sidesDist + 0.075, -sidesDist);
	glVertex2f(sidesDist, sidesDist);
	glVertex2f(sidesDist + 0.075, sidesDist);

	// left vertical line
	glVertex2f(-sidesDist, -sidesDist);
	glVertex2f(-sidesDist, sidesDist);

	// left vertical line accents
	glVertex2f(-sidesDist, -sidesDist);
	glVertex2f(-sidesDist - 0.075, -sidesDist);
	glVertex2f(-sidesDist, sidesDist);
	glVertex2f(-sidesDist - 0.075, sidesDist);

	// increments
	for (int i = 0; i < 8; i++) {
		glVertex2f(sidesDist, i * -sidesDist / 4 + sidesDist);
		glVertex2f(sidesDist + 0.02, i * -sidesDist / 4 + sidesDist);

		glVertex2f(-sidesDist, i * -sidesDist / 4 + sidesDist);
		glVertex2f(-sidesDist - 0.02, i * -sidesDist / 4 + sidesDist);
	}

	glEnd();

	// based on the planes height, we will adjust our middle indicator's height as well
	GLfloat hudLineHeight = forwardVector[1] / 10 - 0.5;
	
	// set a min and max for our UI indicator
	if (hudLineHeight <= -0.5) {
		hudLineHeight = -0.5;
	}
	else if (hudLineHeight >= 0.5) {
		hudLineHeight = 0.5;
	}

	// scope transformations for our middle indicators
	glPushMatrix();

	// rotate about the origin regardless of height
	glTranslatef(0, hudLineHeight, 0);
	glRotatef(-45 * turnAngle, 0, 0, 1);
	glTranslatef(0, -hudLineHeight, 0);

	// middle line indicator
	glBegin(GL_LINES);
	glVertex2f(-sidesDist + 0.1, hudLineHeight);
	glVertex2f(sidesDist - 0.1, hudLineHeight);

	// square lines top left
	glVertex2f(-sidesDist + 0.3, hudLineHeight + 0.2);
	glVertex2f(-sidesDist + 0.4, hudLineHeight + 0.2);
	glVertex2f(-sidesDist + 0.3, hudLineHeight + 0.2);
	glVertex2f(-sidesDist + 0.3, hudLineHeight + 0.15);

	// square lines top right
	glVertex2f(sidesDist - 0.3, hudLineHeight + 0.2);
	glVertex2f(sidesDist - 0.4, hudLineHeight + 0.2);
	glVertex2f(sidesDist - 0.3, hudLineHeight + 0.2);
	glVertex2f(sidesDist - 0.3, hudLineHeight + 0.15);

	// square lines bottom left
	glVertex2f(-sidesDist + 0.3, hudLineHeight - 0.2);
	glVertex2f(-sidesDist + 0.4, hudLineHeight - 0.2);
	glVertex2f(-sidesDist + 0.3, hudLineHeight - 0.2);
	glVertex2f(-sidesDist + 0.3, hudLineHeight - 0.15);

	// square lines bottom right
	glVertex2f(sidesDist - 0.3, hudLineHeight - 0.2);
	glVertex2f(sidesDist - 0.4, hudLineHeight - 0.2);
	glVertex2f(sidesDist - 0.3, hudLineHeight - 0.2);
	glVertex2f(sidesDist - 0.3, hudLineHeight - 0.15);

	glEnd();

	// grab our current height and create a string value with it to display
	int len = snprintf(NULL, 0, "%.2f m", forwardVector[1]);
	char* heightString = malloc(len + 1);
	snprintf(heightString, len + 1, "%.2f m", forwardVector[1]);

	// set our raster pos just below the middle line indicator
	glRasterPos2f(-0.02, hudLineHeight - 0.05);

	// display the text 
	for (int i = 0; i < strlen(heightString); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, heightString[i]);
	}

	// return to previous matrix
	glPopMatrix();

	// scope transformations for the bottom circular indicator
	glPushMatrix();

	// scale down the indicator and shift it to the bottom
	glTranslatef(0, -0.75, 0);
	glScalef(0.175, 0.25, 0.25);


	// create a semi circle of lines to look similar to a radar
	glBegin(GL_LINES);
	for (int i = 0; i < 17; i++) {
		GLfloat angleIncrement = (M_PI + M_PI / 6) - i * M_PI / 12;
		glVertex2f(cos(angleIncrement), sin(angleIncrement));
		glVertex2f(1.1 * cos(angleIncrement), 1.1 * sin(angleIncrement));
	}

	// create two small line accents in the middle of the circle indicator
	glVertex2f(0, 0.05);
	glVertex2f(0, 0.9);
	glVertex2f(-0.1, 0.2);
	glVertex2f(0.1, 0.2);

	glEnd();

	// return to previous matrix
	glPopMatrix();


	// reset back to our original camera and model view matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// re-enable lighting and depth testing, as well as setting the polygon mode to the correct mode
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	if (wireframeToggled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}


/************************************************************************

	Function:		moveCessna

	Description:	function used to move the cessna forward towards the
					direction of travel, which is changed by mouse movement

*************************************************************************/
void moveCessna(void) {
	
	// turn angle ranges from -1 to 1, so we will change our direction of movement based on that value scaled down
	forwardAngle += turnRate * turnAngle;
	
	// grab cos and sin components of direction to increment our x and z positions
	GLfloat moveVector[2] = { cos(forwardAngle), sin(forwardAngle) };
	forwardVector[0] += moveSpeed * -moveVector[0];
	forwardVector[2] += moveSpeed * -moveVector[1];

	// main an offset from the camera, add the opposite of the direction vector to the camera * the offset to scale it accordingly, y should remain the same
	camPos[0] = forwardVector[0] + moveVector[0] * distOffset;
	camPos[1] = forwardVector[1];
	camPos[2] = forwardVector[2] + moveVector[1] * distOffset;
}


/************************************************************************

	Function:		drawSea

	Description:	function used in myDisplay() to draw the sea with 
					its corresponding texture

*************************************************************************/
void drawSea(void) {
	// set color to white as texture is multiplied by current colour
	glColor3f(1.0, 1.0, 1.0);

	// enable texturing 
	glEnable(GL_TEXTURE_2D);

	// retrieve the corresponding sea texture and bind it so it's currently active
	glBindTexture(GL_TEXTURE_2D, seaTexture);

	// since we will be enclosed within the sea and sky, need to ensure both sides are shaded
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// set our emission value to high so the sea and sky shine through despite lighting effects
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, highEmission);
	
	// create a new quadric to be used for our sea
	GLUquadric* seaQuadricPtr = gluNewQuadric();

	// ensure shading is set to smooth
	glShadeModel(GLU_SMOOTH);

	// set our quadric to smooth
	gluQuadricNormals(seaQuadricPtr, GLU_SMOOTH);

	// allow our quadric to automatically generate texture coordinates
	gluQuadricTexture(seaQuadricPtr, GL_TRUE);

	// scope transformations for the disk
	glPushMatrix();

	// move disk slightly up to ensure no gap between sky and sea
	glTranslatef(0, 0.5, 0);

	// orient disc to be flat 
	glRotatef(90, 1, 0, 0);

	// create disk with altered quadric
	gluDisk(seaQuadricPtr, 0, sceneSize, 50, 10);

	// return to original matrix
	glPopMatrix();
}


/************************************************************************

	Function:		drawSky

	Description:	function used in myDisplay() to draw the sky with
					its corresponding texture

*************************************************************************/
void drawSky(void) {

	// retrieve the corresponding sky texture and bind it so it's currently active
	glBindTexture(GL_TEXTURE_2D, skyTexture);

	// create a new quadric to be used for our sky
	GLUquadric* skyQuadricPtr = gluNewQuadric();

	// ensure shading is set to smooth
	glShadeModel(GLU_SMOOTH);

	// set our quadric to smooth
	gluQuadricNormals(skyQuadricPtr, GLU_SMOOTH);

	// allow our quadric to automatically generate texture coordinates
	gluQuadricTexture(skyQuadricPtr, GL_TRUE);

	// scope transformations for the cylinder
	glPushMatrix();
	
	// rotating our cylinder pushes it down, so we have to translate it up
	glTranslatef(0, sceneSize, 0);

	// rotate our cylinder so it's upright
	glRotatef(90, 1, 0, 0);

	// create cylinder with altered quadric
	gluCylinder(skyQuadricPtr, sceneSize-1, sceneSize-1, sceneSize, 20, 20);

	// return to original matrix
	glPopMatrix();

	// disable texturing as we don't want the other objects to be textured
	glDisable(GL_TEXTURE_2D);

	// disable two side shading as we won't be inside any other objects
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	// reset our emission value so no other objects are self emitting
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroMaterial);
}


/************************************************************************

	Function:		drawCessna

	Description:	function used in myDisplay() to draw the cessna

*************************************************************************/
void drawCessna() {
	
	// scope transformations for cessna
	glPushMatrix();

	// move our cessna to be infront of our camera always
	glTranslatef(forwardVector[0], forwardVector[1] - 0.75, forwardVector[2]);
	
	// convert our direction of travel into degrees
	GLfloat angleConversion = forwardAngle * 180 / M_PI;

	// rotate cessna by the our turn angle to orient towards direction of travel
	glRotatef(-angleConversion, 0, 1, 0);

	// rotate cessna on x axis to tilt towards while turning
	glRotatef(-45 * turnAngle, 1, 0, 0);
	
	// set the rest of the material properties, give the cessna some shine
	glMaterialfv(GL_FRONT, GL_AMBIENT, cessnaAmbient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, cessnaShininess);

	// one indexed, iterate through object loop to draw each object with all the polygons associated, and colour accordingly
	for (int i = 1; i < CESSNA_OBJECT_COUNT; i++) {
		
		// color the object's polygons a specific color, according to the document specifications
		if (i <= 4 || (i >= 9 && i <= 14) || i >= 27) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, yellowDiffuse);
		}
		else if (i == 8 || (i >= 15 && i <= 26)) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
		}
		else if (i == 7) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, purpleDiffuse);
		}
		else {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blackDiffuse);
		}

		// iterate through the list of polygons, and create each one
		for (int j = 0; j < cessnaParts[i].numPolygons; j++) {

			// create polygon shape
			glBegin(GL_POLYGON);

			// for each polygon, iterate through the list of normals and vertices and set them accordingly
			for (int k = 0; k < cessnaParts[i].polygons[j].numVertices; k++) {
				glNormal3fv(cessnaNormals[cessnaParts[i].polygons[j].indices[k] - 1]);
				glVertex3fv(cessnaPoints[cessnaParts[i].polygons[j].indices[k]-1]);
			}
			glEnd();
		}
	}

	// after our cessna has been drawn, draw the propellers before popping to original matrix, this keeps propellers moving with cessna
	drawPropellers();

	// return to original matrix
	glPopMatrix();
}


/************************************************************************

	Function:		drawPropellers

	Description:	function used in drawCessna() to draw the propellers
					attached to the cessna

*************************************************************************/
void drawPropellers() {

	// scope transformations for the propellers
	glPushMatrix();
	
	// set this propeller to the left side of the cessna
	glTranslatef(-0.25, -0.15, 0.35);

	// rotate the propeller by theta which is changing, negative to spin opposite of the other propeller
	glRotatef(-theta * propellerSpeed, 1, 0, 0);
	
	// scale propeller down to an appropriate size
	glScalef(0.75, 0.75, 0.75);

	// iterate through the propellers objects and draw the polygons in the same colour
	for (int i = 1; i < 3; i++) {
		// depending on which object it is in the propeller, we colour accordingly
		if (i == 1) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, greyDiffuse);
		}
		else {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
		}

		// set additional material properties, including shininess
		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, cessnaShininess);

		// for each propeller object, iterate through each polygon to create it as part of the same object
		for (int j = 0; j < propParts[i].numPolygons; j++) {
			glBegin(GL_POLYGON);

			for (int k = 0; k < propParts[i].polygons[j].numVertices; k++) {
				// use corresponding normal and vertex
				glNormal3fv(propNormals[propParts[i].polygons[j].indices[k] - 1]);
				glVertex3fv(propPoints[propParts[i].polygons[j].indices[k] - 1]);
			}
			glEnd();
		}
	}
	glPopMatrix();

	// repeat above process for second propeller, except translating to the right of plane
	glPushMatrix();
	glTranslatef(-0.25, -0.15, -0.35);
	glRotatef(theta * propellerSpeed, 1, 0, 0);
	glScalef(0.75, 0.75, 0.75);

	for (int i = 1; i < 3; i++) {
		if (i == 1) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, greyDiffuse);
		}
		else {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
		}

		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, cessnaShininess);
		for (int j = 0; j < propParts[i].numPolygons; j++) {
			glBegin(GL_POLYGON);

			for (int k = 0; k < propParts[i].polygons[j].numVertices; k++) {
				glNormal3fv(propNormals[propParts[i].polygons[j].indices[k] - 1]);
				glVertex3fv(propPoints[propParts[i].polygons[j].indices[k] - 1]);
			}
			glEnd();
		}
	}
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}


/************************************************************************

	Function:		calculateNormal

	Description:	function used to compute the cross product between
					two vectors and store result in result vector (calculate
					normal for polygon)

*************************************************************************/
void calculateNormal(GLfloat v1[], GLfloat v2[], GLfloat resultVector[]) {
	// calculate cross product
	resultVector[0] = v1[1] * v2[2] - v1[2] * v2[1];
	resultVector[1] = v1[2] * v2[0] - v1[0] * v2[2];
	resultVector[2] = v1[0] * v2[1] - v1[1] * v2[0];
}


/************************************************************************

	Function:		addVector

	Description:	simple utility function used to add two vectors together 
					and store in the resultant vector

*************************************************************************/
void addVector(GLfloat v1[], GLfloat result[]) {
	// simple vector addition calculation
	result[0] += v1[0];
	result[1] += v1[1];
	result[2] += v1[2];
}


/************************************************************************

	Function:		findVectorMagnitude

	Description:	simple utility function used to find the magnitude of 
					a vector

*************************************************************************/
GLfloat findVectorMagnitude(GLfloat v1[]) {
	// magnitude calculated by square root of sum of squares of components in vector
	GLfloat result = sqrt(pow(v1[0], 2) + pow(v1[1], 2) + pow(v1[2], 2));
	return result;
}


/************************************************************************

	Function:		findMountainVerticeNormals

	Description:	function used to calculate the normal for each vertice
					based on the average normals of the polygons surrounding
					the vertex

*************************************************************************/
void findMountainVerticeNormals(GLfloat mountN[MESH_RES+1][MESH_RES+1][3]) {

	// iterate through every vertex in the mountain and calculate the normal by getting normal of surrounding polygons
	for (int i = 0; i < MESH_RES + 1; i++) {
		for (int j = 0; j < MESH_RES + 1; j++) {
			// resVector represents the numerator and resMagnitude the denominator
			GLfloat resVector[3] = { 0, 0, 0 };
			GLfloat resMagnitude = 0;
			int count = 0;

			/* There is 1 more vertex for each face on every side, meaning we need to check every edge case
			* before we take the polygon into account for the average normal calculation.
			* 
			* E.g/ a 4 mesh by 4 mesh pyramid will have 5 x 5 vertices. The relationship between the [4][4] faces and the
			* [5][5] matrices are that the potentially connected polygons for a vertex are [j][i], [i-1][j], [j-1][i], and [j-1][i-1]
			* Check to see which of these 4 potential faces are within the index range, than add to the average if valid.
			*/
			if (i < MESH_RES && j < MESH_RES) {
				addVector(mountainPolygonFaceNormals[j][i], resVector);
				resMagnitude += findVectorMagnitude(mountainPolygonFaceNormals[j][i]);
				count++;
			}
			if (j < MESH_RES && i - 1 >= 0) {
				addVector(mountainPolygonFaceNormals[i-1][j], resVector);
				resMagnitude += findVectorMagnitude(mountainPolygonFaceNormals[i-1][j]);
				count++;
			}
			if (i < MESH_RES && j - 1 >= 0) {
				addVector(mountainPolygonFaceNormals[j - 1][i], resVector);
				resMagnitude += findVectorMagnitude(mountainPolygonFaceNormals[j - 1][i]);
				count++;
			}
			if (i - 1 >= 0 && j - 1 >= 0) {
				addVector(mountainPolygonFaceNormals[j - 1][i - 1], resVector);
				resMagnitude += findVectorMagnitude(mountainPolygonFaceNormals[j - 1][i - 1]);
				count++;
			}

			// calculate and set average on a component basis
			mountN[i][j][0] = (GLfloat) resVector[0] / count;
			mountN[i][j][1] = (GLfloat) resVector[1] / count;
			mountN[i][j][2] = (GLfloat) resVector[2] / count;
		}
	}
}


/************************************************************************

	Function:		drawMountains

	Description:	function used in myDisplay() to draw the mountains

*************************************************************************/
void drawMountains(void) {
	
	// set color to white to avoid dark textures
	glColor3f(1.0, 1.0, 1.0);

	// shade both sides of the model
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// set our material properties, slight shininess as instructed
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, halfAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroMaterial);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);

	// if our mountain texture should be toggled, we enable it as well as give the material a white diffuse as to not be affected, otherwise set to green
	if (mountainTexturedToggled) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuse);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mountainTexture);

	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, greenDiffuse);
	}


	// scope transformations for mountain 1
	glPushMatrix();

	// scale and move our mountain accordingly 
	glTranslatef(mountainTranslation[0], mountainTranslation[1], mountainTranslation[2]);
	glScalef(mountainScale[0], mountainScale[1], mountainScale[2]);


	// iterate through every vertex, combining square sets of 4 into polygons
	// Ensure we are going in counter clockwise fashion and following right hand rule
	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {

			glBegin(GL_POLYGON);
			
			// for each vertex, we set the normal, the texture coord, and the vertex itself. To keep texture coord within (0,1), we divide by size
			glNormal3fv(mountainNormals[i][j]);
			glTexCoord2f(mountainVertices[i][j][0] / coordinatePlaneSize, mountainVertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountainVertices[i][j]);

			glNormal3fv(mountainNormals[i+1][j]);
			glTexCoord2f(mountainVertices[i+1][j][0] / coordinatePlaneSize, mountainVertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountainVertices[i + 1][j]);

			glNormal3fv(mountainNormals[i+1][j+1]);
			glTexCoord2f(mountainVertices[i+1][j+1][0] / coordinatePlaneSize, mountainVertices[i+1][j+1][2] / coordinatePlaneSize);
			glVertex3fv(mountainVertices[i + 1][j + 1]);

			glNormal3fv(mountainNormals[i][j+1]);
			glTexCoord2f(mountainVertices[i][j+1][0] / coordinatePlaneSize, mountainVertices[i][j+1][2] / coordinatePlaneSize);
			glVertex3fv(mountainVertices[i][j + 1]);
			glEnd();
		}
	}

	// return to original matrix
	glPopMatrix();


	// scope transformations for mountain 2
	glPushMatrix();

	// scale and move our mountain accordingly 
	glTranslatef(mountain2Translation[0], mountain2Translation[1], mountain2Translation[2]);
	glScalef(mountain2Scale[0], mountain2Scale[1], mountain2Scale[2]);


	// iterate through every vertex, combining square sets of 4 into polygons3
	// Ensure we are going in counter clockwise fashion and following right hand rule
	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {

			glBegin(GL_POLYGON);

			// for each vertex, we set the normal, the texture coord, and the vertex itself. To keep texture coord within (0,1), we divide by size
			glNormal3fv(mountain2Normals[i][j]);
			glTexCoord2f(mountain2Vertices[i][j][0] / coordinatePlaneSize, mountain2Vertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountain2Vertices[i][j]);

			glNormal3fv(mountain2Normals[i + 1][j]);
			glTexCoord2f(mountain2Vertices[i + 1][j][0] / coordinatePlaneSize, mountain2Vertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountain2Vertices[i + 1][j]);

			glNormal3fv(mountain2Normals[i + 1][j + 1]);
			glTexCoord2f(mountain2Vertices[i + 1][j + 1][0] / coordinatePlaneSize, mountain2Vertices[i + 1][j + 1][2] / coordinatePlaneSize);
			glVertex3fv(mountain2Vertices[i + 1][j + 1]);

			glNormal3fv(mountain2Normals[i][j + 1]);
			glTexCoord2f(mountain2Vertices[i][j + 1][0] / coordinatePlaneSize, mountain2Vertices[i][j + 1][2] / coordinatePlaneSize);
			glVertex3fv(mountain2Vertices[i][j + 1]);
			glEnd();
		}
	}

	// return to original matrix
	glPopMatrix();


	// scope transformations for mountain 3
	glPushMatrix();

	// scale and move our mountain accordingly 
	glTranslatef(mountain3Translation[0], mountain3Translation[1], mountain3Translation[2]);
	glScalef(mountain3Scale[0], mountain3Scale[1], mountain3Scale[2]);


	// iterate through every vertex, combining square sets of 4 into polygons
	// Ensure we are going in counter clockwise fashion and following right hand rule
	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {

			glBegin(GL_POLYGON);

			// for each vertex, we set the normal, the texture coord, and the vertex itself. To keep texture coord within (0,1), we divide by size
			glNormal3fv(mountain3Normals[i][j]);
			glTexCoord2f(mountain3Vertices[i][j][0] / coordinatePlaneSize, mountain3Vertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountain3Vertices[i][j]);

			glNormal3fv(mountain3Normals[i + 1][j]);
			glTexCoord2f(mountain3Vertices[i + 1][j][0] / coordinatePlaneSize, mountain3Vertices[i][j][2] / coordinatePlaneSize);
			glVertex3fv(mountain3Vertices[i + 1][j]);

			glNormal3fv(mountain3Normals[i + 1][j + 1]);
			glTexCoord2f(mountain3Vertices[i + 1][j + 1][0] / coordinatePlaneSize, mountain3Vertices[i + 1][j + 1][2] / coordinatePlaneSize);
			glVertex3fv(mountain3Vertices[i + 1][j + 1]);

			glNormal3fv(mountain3Normals[i][j + 1]);
			glTexCoord2f(mountain3Vertices[i][j + 1][0] / coordinatePlaneSize, mountain3Vertices[i][j + 1][2] / coordinatePlaneSize);
			glVertex3fv(mountain3Vertices[i][j + 1]);
			glEnd();
		}
	}

	// return to original matrix
	glPopMatrix();


	// reset material/texture related properties to not affect outside state
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroMaterial);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glDisable(GL_TEXTURE_2D);
}


/************************************************************************

	Function:		getRandomNumber

	Description:	simple utility function to get a random float number
					between -n and n

*************************************************************************/
GLfloat getRandomNumber(GLfloat n) {

	// first get a random float number between 0 and 1, then set the range by doubling, multiplying by n, then subtracting n
	GLfloat randNum = ((GLfloat)rand() / RAND_MAX) * 2.0f * n - n;
	return randNum;
}


/************************************************************************

	Function:		addMidpointHeightNoise

	Description:	recursive function used to increase/decrease height of midpoints 
					by  random amount whose range halves with each recursive call.

*************************************************************************/
void addMidpointHeightNoise(int v1[], int v2[], int v3[], int v4[], GLfloat n, int depth, GLfloat vertices[MESH_RES+1][MESH_RES+1][3]) {

	// end recursion once we've reached the desired depth
	if (depth == 0) {
		return;
	}

	// find all mid points of the 4 corners provided in the recursive call, including the centre
	int mid12[] = {(v1[0] + v2[0])/2, (v1[1] + v2[1]) / 2 };
	int mid23[] = {(v2[0] + v3[0]) / 2, (v2[1] + v3[1]) / 2 };
	int mid34[] = { (v3[0] + v4[0]) / 2, (v3[1] + v4[1]) / 2 };
	int mid41[] = { (v4[0] + v1[0]) / 2, (v4[1] + v1[1]) / 2 };
	int center[] = { (v1[0] + v2[0] + v3[0] + v4[0]) / 4, (v1[1] + v2[1] + v3[1] + v4[1]) / 4 };

	// if this is the first iteration, don't alter heights
	if (depth != maxDepth) {
		
		// get a different random value from the random range for each height to be altered
		GLfloat r = getRandomNumber(n);
		vertices[mid12[0]][mid12[1]][1] += r;

		r = getRandomNumber(n);
		vertices[mid23[0]][mid23[1]][1] += r;

		r = getRandomNumber(n);
		vertices[mid34[0]][mid34[1]][1] += r;

		r = getRandomNumber(n);
		vertices[mid41[0]][mid41[1]][1] += r;

		r = getRandomNumber(n);
		vertices[center[0]][center[1]][1] += r;
	}

	// the 4 sub squares we have created will be passed recursively to alter their mid point heights, with range of numbers halved
	addMidpointHeightNoise(v1, mid12, center, mid41, n / 2, depth - 1, vertices);
	addMidpointHeightNoise(mid12, v2, mid23, center, n / 2, depth - 1, vertices);
	addMidpointHeightNoise(center, mid23, v3, mid34, n / 2, depth - 1, vertices);
	addMidpointHeightNoise(mid41, center, mid34, v4, n / 2, depth - 1, vertices);
}


/************************************************************************

	Function:		initializeMountains

	Description:	Function used to help initialize mountains by setting
					initial pyramid shape and starting height distortion
					process

*************************************************************************/
void initializeMountains(GLfloat mountV[MESH_RES+1][MESH_RES+1][3], GLfloat mountN[MESH_RES + 1][MESH_RES + 1][3], GLfloat initialRandAmount, GLfloat mountainHeight) {
	
	// divide our flat plane size by the mesh resolution to get each side length for the squares
	GLfloat meshSideLength = (GLfloat)coordinatePlaneSize / MESH_RES;

	// set the x and z values for each vertice based off the mesh res side lengths
	for (int i = 0; i < MESH_RES + 1; i++) {
		for (int j = 0; j < MESH_RES + 1; j++) {
			mountV[i][j][0] = (GLfloat)i * meshSideLength;
			mountV[i][j][2] = (GLfloat)j * meshSideLength;
		}
	}

	// iterate through every vertice and set a y value to create a pyramid
	for (int i = 0; i <= (MESH_RES + 1)/2; i++) {

		// calculate the y value for a given x coordinate on the pyramid
		GLfloat height = mountV[i][i][0] * 2 * mountainHeight / coordinatePlaneSize;

		// 4 values in a square fashion around the pyramid will have the same height (setting each level with same height)
		for (int j = i; j < MESH_RES + 1 - i; j++) {
			mountV[i][j][1] = height;
			mountV[j][i][1] = height;
			mountV[MESH_RES-i][j][1] = height;
			mountV[j][MESH_RES-i][1] = height;
		}
	}
	
	// get the 4 corners of the pyramid
	int v1[] = { 0, 0 };
	int v2[] = { MESH_RES, 0 };
	int v3[] = { MESH_RES, MESH_RES };
	int v4[] = { 0, MESH_RES };

	// using the 4 corners, call the recursive function to alter the pyramid's heights
	addMidpointHeightNoise(v1, v2, v3, v4, initialRandAmount, maxDepth, mountV);

	// iterate through every polygon on the pyramid and determine the corresponding normal
	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {
			calculateNormal(mountV[i + 1][j], mountV[i][j], mountainPolygonFaceNormals[i][j]);
		}
	}

	// calculate each vertice's normal based on the polygons surrounding the vertex
	findMountainVerticeNormals(mountN);
}


/************************************************************************

	Function:		myIdle

	Description:	idle function used for nonevents for GLUT, handles
					continuous movement of cessna as well as propeller angle 
					rotation

*************************************************************************/
void myIdle(void) {

	// increase theta to rotate propellers
	theta += 1;
	
	// continuously move cesnna in forward direction
	moveCessna();

	// force OpenGL to redraw the changes
	glutPostRedisplay();
}


/************************************************************************

	Function:		addPlaneObject

	Description:	Utility function used to create a new object/reset the 
					existing one that contains a number of polygons


	Used this reference as guidance for handling pointers in this manner
	https://stackoverflow.com/questions/5844972/using-arrow-and-dot-operators-together-in-c

*************************************************************************/
void addPlaneObject(Object cessnaParts[], int* cessnaCount, Object* currentObj) {

	// if we have our max number of objects, we can't create anymore
	if (*cessnaCount < CESSNA_OBJECT_COUNT) {

		// set the corresponding object index into the array of plane objects
		cessnaParts[*cessnaCount] = *currentObj;

		// increase our object count
		(*cessnaCount)++;

		// reset the current object
		currentObj->numPolygons = 0;
	}
}


/************************************************************************

	Function:		addPolygon

	Description:	Utility function used add polygons with its vertices
					to a particular object 

	Used this reference as guidance for handling pointers in this manner
	https://stackoverflow.com/questions/5844972/using-arrow-and-dot-operators-together-in-c

*************************************************************************/
void addPolygon(Object* planeObj, int indices[], int numVertices) {
	
	// arbitrary max number of polygons per object specified
	if (planeObj->numPolygons < 700) {

		// create a new polygon in the current object
		PolygonShape* polygon = &planeObj->polygons[planeObj->numPolygons];
		
		// set the polygons indices with the indices we have accumulated so far
		memcpy(polygon->indices, indices, numVertices * sizeof(int));
		
		// set the count of vertices accordingly
		polygon->numVertices = numVertices;

		// increment the number of polygons in our object
		planeObj->numPolygons++;
	}
}


/************************************************************************

	Function:		initializeCessna

	Description:	Function used to read in cessna.txt file and create arrays
					for the corresponding objects, polygons, and vertices.


	Used this reference as guidance for reading values in this manner:
	https://stackoverflow.com/questions/56602474/store-string-with-numbers-as-an-integer-array

*************************************************************************/
void initializeCessna(void) {

	// open cessna.txt file
	FILE* file;
	if (fopen_s(&file, "cessna.txt", "r") != 0) {
		printf("Error opening file\n");
		return;
	}

	// create line buffer to read values
	char line[128];

	// iterate first time to grab every vertex value and store in an array
	for (int i = 0; i < CESSNA_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'v') {
			sscanf_s(line, "v %f %f %f", &cessnaPoints[i][0], &cessnaPoints[i][1], &cessnaPoints[i][2]);
		}
	}

	// directly after the vertices is the normals so we grab the normals in a similar fashion as above
	for (int i = 0; i < CESSNA_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'n') {
			sscanf_s(line, "n %f %f %f", &cessnaNormals[i][0], &cessnaNormals[i][1], &cessnaNormals[i][2]);
		}
	}

	// skip the gap line before reading objects
	fgets(line, sizeof(line), file);

	// set variables for num of objects, the currentObject, and what the currentObject's polygon count is
	int cessnaCount = 0;
	Object currentObj;
	currentObj.numPolygons = 0;

	/*
	Iterate through the entire list of objects until you reach the end of the file. Every time you reach g, create
	a new Object. For every f you read, create a new polygon based off the vertices specified by the corresponding file line
	*/
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'g') {
			// add the new object to be created
			addPlaneObject(cessnaParts, &cessnaCount, &currentObj);
		}
		else if (line[0] == 'f') {
			// arbitrary max number of indices
			int indices[50];
			int numVertices = 0;

			// use strtok_s to grab every integer in the line, until the line is finished
			char* token = NULL;
			char* nextToken = NULL;
			token = strtok_s(line + 1, " \t\n", &nextToken);

			// add each vertice to be included in the corresponding polygon per line
			while (token != NULL) {
				indices[numVertices] = atoi(token);
				numVertices++;
				token = strtok_s(NULL, " \t\n", &nextToken); 
			}

			// create a polygon with the indices you've just added to the list
			addPolygon(&currentObj, indices, numVertices);
		}
	}

	// make sure to add the final object once you've finished grabbing all the polygons/vertices
	addPlaneObject(cessnaParts, &cessnaCount, &currentObj);

	// close the file
	fclose(file);
}


/************************************************************************

	Function:		initializePropellers

	Description:	Function used to read in propeller.txt file and create arrays
					for the corresponding objects, polygons, and vertices.


	Used this reference as guidance for reading values in this manner:
	https://stackoverflow.com/questions/56602474/store-string-with-numbers-as-an-integer-array

*************************************************************************/
void initializePropellers(void) {

	// open propeller.txt file
	FILE* file;
	if (fopen_s(&file, "propeller.txt", "r") != 0) {
		printf("Error opening file\n");
		return;
	}

	// create line buffer to read values
	char line[128];

	// iterate first time to grab every vertex value and store in an array
	for (int i = 0; i < PROP_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'v') {
			sscanf_s(line, "v %f %f %f", &propPoints[i][0], &propPoints[i][1], &propPoints[i][2]);
		}

		// offset every propeller vertex so that our propeller will end up in the origin (helps with rotation later)
		propPoints[i][0] += 0.16;
		propPoints[i][1] += 0.13;
		propPoints[i][2] += -0.36;
	}

	// directly after the vertices is the normals so we grab the normals in a similar fashion as above
	for (int i = 0; i < PROP_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'n') {
			sscanf_s(line, "n %f %f %f", &propNormals[i][0], &propNormals[i][1], &propNormals[i][2]);
		}
	}

	// skip the gap line before reading objects
	fgets(line, sizeof(line), file);

	// set variables for num of objects, the currentObject, and what the currentObject's polygon count is
	int propCount = 0;
	Object currentObj;
	currentObj.numPolygons = 0;


	/*
	Iterate through the entire list of objects until you reach the end of the file. Every time you reach g, create
	a new Object. For every f you read, create a new polygon based off the vertices specified by the corresponding file line
	*/
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'g') {
			// add the new object to be created
			addPlaneObject(propParts, &propCount, &currentObj);
		}
		else if (line[0] == 'f') {
			// arbitrary max number of indices
			int indices[50];
			int numVertices = 0;

			// use strtok_s to grab every integer in the line, until the line is finished
			char* token = NULL;
			char* nextToken = NULL;
			token = strtok_s(line + 1, " \t\n", &nextToken);

			// add each vertice to be included in the corresponding polygon per line
			while (token != NULL) {
				indices[numVertices] = atoi(token);
				numVertices++;
				token = strtok_s(NULL, " \t\n", &nextToken);
			}

			// create a polygon with the indices you've just added to the list
			addPolygon(&currentObj, indices, numVertices);
		}
	}

	// make sure to add the final object once you've finished grabbing all the polygons/vertices
	addPlaneObject(propParts, &propCount, &currentObj);

	// close the file
	fclose(file);
}


/************************************************************************

	Function:		drawOrigin

	Description:	function used to draw the origin lines, point, and starting
					grid. Used by myDisplay() 

*************************************************************************/
void drawOrigin(void) {

	// set basic materials as well as normal for our origin lines
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
	glNormal3f(0, 1, 0);

	//Draw our origin lines
	glBegin(GL_LINES);

	// change line width to assignment required width
	glLineWidth(5.0f);

	// create red line representing x axis
	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);

	// create green line representing y axis
	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.0);

	// create blue line representing  z axis
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();

	// reset our line width and material colors
	glLineWidth(1.0f);
	glColor3f(1.0, 1.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuse);
	
	// draw origin ball by creating a white sphere
	GLUquadric* quad = gluNewQuadric();

	// scope transformations for the origin ball
	glPushMatrix();

	// scale the sphere down
	glScalef(0.01, 0.01, 0.01);

	// create sphere
	gluSphere(quad, 1, 20, 20);

	// return to orignal matrix
	glPopMatrix();


	// scope transformations for starting grid
	glPushMatrix();

	// scale starting grid up
	glScalef(40.0, 0.0, 40.0);

	// set material color and normal
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightblueDiffuse);
	glNormal3f(0, 1, 0);

	// create rectangular plane
	glBegin(GL_POLYGON);
	glVertex3f(-0.5, 0, 0.5);
	glVertex3f(-0.5, 0, -0.5);
	glVertex3f(0.5, 0, -0.5);
	glVertex3f(0.5, 0, 0.5);
	glEnd();

	// draw lines along the plane to display a grid pattern when wireframed
	// draw lines along z axis
	glBegin(GL_LINES);
	for (GLfloat z = -0.4; z <= 0.4; z += 0.05) {
		glVertex3f(-0.5, 0, z);
		glVertex3f(0.5, 0, z);
	}
	glEnd();
	
	// draw lines along x axis
	glBegin(GL_LINES);
	for (GLfloat x = -0.4; x <= 0.4; x += 0.05) {
		glVertex3f(x, 0, -0.5);
		glVertex3f(x, 0, 0.5);
	}
	glEnd();

	// return to orignal matrix
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}


/************************************************************************

	Function:		myReshape

	Description:	Handles the functionality of reshaping the window,
					including fullscreening it

*************************************************************************/
void myReshape(int newWidth, int newHeight) {
	
	// adjust our viewport to the new dimensions
	glViewport(0, 0, newWidth, newHeight);

    // keep track of what our current dimensions are
	currentWidth = newWidth;
	currentHeight = newHeight;

	// changem matrix modes to alter the camera
	glMatrixMode(GL_PROJECTION);

	// need to load identity to clear existing transformations
	glLoadIdentity();

	// set our perspective camera with new dimensions for aspect ratio
	gluPerspective(45, (GLfloat) newWidth / (GLfloat) newHeight, 0.1, sceneSize * 2);

	// return to model view mode
	glMatrixMode(GL_MODELVIEW);

	// force OpenGL to redraw the changes
	glutPostRedisplay();
}


/************************************************************************

	Function:		myPassiveMouse

	Description:	Handles the functionality of the movement of the mouse


*************************************************************************/
void myPassiveMouse(int x, int y) {
	
	//based on how far or right your mouse is, a value will  be associated between -1 and 1
	GLfloat halfScreen = (GLfloat)currentWidth / 2;
	GLfloat cursorRatio = (GLfloat)(x - halfScreen) / halfScreen;
	GLfloat tilt = cursorRatio;

	// clamp the value between -1 and 1 to be used with changing our turn angle later
	turnAngle = fmaxf(-1.0, fminf(1.0, tilt));

	// force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		myKeyboard

	Description:	Handles the functionality of normal key keyboard
					presses by the user

*************************************************************************/
void myKeyboard(unsigned char key, int x, int y) {

	/* If the user presses q, they should quit, w -> toggle the wireframe, f -> toggle the full screen mode
	* s -> toggle the sea and sky, g -> toggle the fog, m -> toggle the mountains, t -> toggle the mountain texture
	*/
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
			
			// to unfull screen, we want to reset back to the original position and dimensions of the window
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
	else if (key == 'b') {
		if (fogToggled == 0) {
			fogToggled = 1;
		}
		else {
			fogToggled = 0;
			glDisable(GL_FOG);
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
	else if (key == 't') {
		if (mountainTexturedToggled == 0) {
			mountainTexturedToggled = 1;
		}
		else {
			mountainTexturedToggled = 0;
		}
	}
	else if (key == 'h') {
		if (hudToggled == 0) {
			hudToggled = 1;
		}
		else {
			hudToggled = 0;
		}
	}
	
	// force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		mySpecialKeyboard

	Description:	Handles the functionality of special key keyboard
					presses by the user

*************************************************************************/
void mySpecialKeyboard(int key, int x, int y) {

	// if up or down is pressed, change height of plane, if page up or down is pressed, change speed (can't go past 0)
	switch (key) {
		case GLUT_KEY_UP:
			forwardVector[1] += heightIncrement;
			camPos[1] += heightIncrement;
			break;
		case GLUT_KEY_DOWN:
			forwardVector[1] -= heightIncrement;
			camPos[1] -= heightIncrement;
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

	// force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		initializeFog

	Description:	simple function used to set the fog parameters

*************************************************************************/
void intializeFog(void) {
	
	// set our pink fog color
	glFogfv(GL_FOG_COLOR, fogColor);

	// ensure we are using exponential fog mode
	glFogf(GL_FOG_MODE, GL_EXP);

	// set a low fog density
	glFogf(GL_FOG_DENSITY, fogDensity);
}


/************************************************************************

	Function:		generateTextures

	Description:	function used to set textureData with the corresponding
					images 

*************************************************************************/
void generateTextures(void) {
	
	// define 3 different texture datas
	GLubyte* seaData;
	GLubyte* skyData;
	GLubyte* mountainData;

	// generate a texture ID for each texture
	glGenTextures(1, &seaTexture);
	glGenTextures(1, &skyTexture);
	glGenTextures(1, &mountainTexture);

	// define variables to be used w/ stbi_load
	int imageWidth, imageHeight, numChannels;
	
	//use stbi_load to set corresponding texture data with image data (followed jpg example given by prof)
	seaData = stbi_load("sea02.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	// bind the corresponding texture to apply properties to it
	glBindTexture(GL_TEXTURE_2D, seaTexture);

	// set s and t to repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// use mipmapping for our textures and enable the respective filters
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, seaData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	

	// no longer need the data so free it
	stbi_image_free(seaData);

	//repeat process for next two textures
	skyData = stbi_load("sky08.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, skyData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_image_free(skyData);


	mountainData = stbi_load("mount03.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	glBindTexture(GL_TEXTURE_2D, mountainTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, mountainData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_image_free(mountainData);

	// unbind any existing texture
	glBindTexture(GL_TEXTURE_2D, 0);

}


/************************************************************************

	Function:		initializeGL

	Description:	Initializes the OpenGL rendering context for display

*************************************************************************/
void initializeGL(void)
{
	// set our light color properties
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	// give scene global ambient light
	GLfloat globalAmbientLight[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// set the light properties
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);

	// enable lighting
	glEnable(GL_LIGHTING);

	// enable the light we set properties for
	glEnable(GL_LIGHT0);

	// ensure our normals get converted to unit vectors to avoid messing with lighting calculations
	glEnable(GL_NORMALIZE);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	// set our shading model to use smooth (per vertex) shading
	glShadeModel(GL_SMOOTH);

	// enable blending for fading opacity
	glEnable(GL_BLEND);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set window mode to perspective
	gluPerspective(45, (GLfloat)originalWidth / (GLfloat)originalHeight, 0.1, sceneSize * 2);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
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

	// register reshape function
	glutReshapeFunc(myReshape);

	// register keyboard function
	glutKeyboardFunc(myKeyboard);

	// register special keyboard function
	glutSpecialFunc(mySpecialKeyboard);

	// register passive mouse movement function
	glutPassiveMotionFunc(myPassiveMouse);

	// register idle function
	glutIdleFunc(myIdle);

	// register the blend function to enable opacity fading
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// read the images in as texture data
	generateTextures();

	// initialize fog related properties
	intializeFog();

	// initialize the rendering context
	initializeGL();

	// read in the file pertaining to the cessna model's parts
	initializeCessna();

	// read in the file pertaining to the propeller model's parts
	initializePropellers();

	// generate mountains and store in an array to be drawn
	initializeMountains(mountainVertices, mountainNormals, initialRandMount1, mountainHeight1);
	initializeMountains(mountain2Vertices, mountain2Normals, initialRandMount2, mountainHeight2);
	initializeMountains(mountain3Vertices, mountain3Normals, initialRandMount3, mountainHeight3);

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
	printf("f \t: toggle fullscreen\n");
	printf("b \t: toggle fog\n");
	printf("m \t: toggle mountains\n");
	printf("t \t: toggle mountain texture\n");
	printf("s \t: toggle sea & sky\n");
	printf("w \t: toggle wire frame\n");
	printf("h \t: toggle HUD\n");
	printf("q \t: quit\n\n");

	printf("Camera Controls\n------------------- \n\n");
	printf("PAGE \tUP\t: \tfaster\n");
	printf("PAGE \tDOWN\t: \tslower\n");
	printf("Up \tArrow\t: \tmove up\n");
	printf("Down \tArrow\t: \tmove down\n");
	printf("Mouse \tRight\t: \tmove right\n");
	printf("Mouse \tLeft\t: \tmove left\n");



	printf("\nNote: May need to use FN key to use Page Up and Page Down on Laptops.\n");
}