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
#define MESH_RES 32

/* function signature definitions */
void printKeyboardControls(void);
void positionCamera(void);
void drawOrigin(void);
void drawCessna(void);
void drawPropellers(void);
void drawSea(void);
void drawSky(void);
void drawMountains();

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

// position and camera variables
GLfloat camPos[3] = { 0.0, 3, 10.0 };
GLfloat forwardVector[3] = {0.0, 2.25, 7.0};
GLfloat distOffset = 3.0;
GLfloat forwardAngle = M_PI / 2;

// cesnna movement variables
GLfloat turnAngle = 0;
GLfloat moveSpeed = 0.05;
GLfloat speedIncrement = 0.001;
GLfloat heightIncrement = 0.01;
GLfloat turnRate = 0.1;

// propeller movement variables
GLfloat theta = 0.0;
GLint propellerSpeed = 2;

// light variables
GLfloat lightPos[] = { 0, 10.0, 0, 0.0 };
GLfloat cessnaShininess = 100.0;

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

// fog variables
GLfloat fogColor[] = { 1.0, 156.0 / 255.0, 219.0 / 255.0, 1.0 };
GLfloat fogDensity = 0.0175;


// texture variables 
GLuint seaTexture;
GLuint skyTexture;
GLuint mountainTexture;

// mountain arrays to help build mountains
GLint mountainVertexCount = (MESH_RES + 1) * (MESH_RES + 1);

GLfloat mountainVertices[(MESH_RES + 1)][(MESH_RES + 1)][3];
GLfloat mountainPolygonFaces[MESH_RES][MESH_RES][4][3];
GLfloat mountainNormals[(MESH_RES + 1)][(MESH_RES + 1)][3];

GLfloat mountainHeight = 10.0f;
GLfloat initialRandAmount = 1.0f;

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
	gluDisk(seaQuadricPtr, 0, 42, 50, 8);

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
	glTranslatef(0, 50, 0);

	// rotate our cylinder so it's upright
	glRotatef(90, 1, 0, 0);

	// create cylinder with altered quadric
	gluCylinder(skyQuadricPtr, 41, 41, 50, 20, 20);

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

		// set the rest of the material properties, give the cessna some shine
		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, cessnaShininess);

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
				glNormal3fv(propNormals[propParts[i].polygons[j].indices[k] - 1]);
				glVertex3fv(propPoints[propParts[i].polygons[j].indices[k] - 1]);
			}
			glEnd();
		}
	}
	glPopMatrix();

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


//void calculateNormal(GLfloat v1[], GLfloat v2[]) {
//
//}

/************************************************************************

	Function:		drawMountains

	Description:	function used in myDisplay() to draw the cessna

*************************************************************************/
void drawMountains(void) {
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, greenDiffuse);
	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {
			glBegin(GL_POLYGON);
			glVertex3fv(mountainVertices[i][j]);
			glVertex3fv(mountainVertices[i + 1][j]);
			glVertex3fv(mountainVertices[i + 1][j + 1]);
			glVertex3fv(mountainVertices[i][j + 1]);
			glEnd();
		}
	}
}

void myIdle(void) {
	theta += 1;
	moveCessna();

	glutPostRedisplay();
}

GLfloat getRandomNumber(GLfloat n) {
	GLfloat randNum = ((float)rand() / RAND_MAX) * 2.0f * n - n;
	return randNum;
}

void addPlaneObject(Object cessnaParts[], int* cessnaCount, Object* currentObj) {
	if (*cessnaCount < CESSNA_OBJECT_COUNT) {
		cessnaParts[*cessnaCount] = *currentObj;
		(*cessnaCount)++;
		currentObj->numPolygons = 0;
	}
}

void addPolygon(Object* planeObj, int indices[], int numVertices) {
	if (planeObj->numPolygons < 700) {
		PolygonShape* polygon = &planeObj->polygons[planeObj->numPolygons];
		memcpy(polygon->indices, indices, numVertices * sizeof(int));
		polygon->numVertices = numVertices;
		planeObj->numPolygons++;
	}
}

void initializeCessna(void) {
	FILE* file;
	if (fopen_s(&file, "cessna.txt", "r") != 0) {
		printf("Error opening file\n");
		return;
	}

	char line[128];

	for (int i = 0; i < CESSNA_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'v') {
			sscanf_s(line, "v %f %f %f", &cessnaPoints[i][0], &cessnaPoints[i][1], &cessnaPoints[i][2]);
		}
	}

	for (int i = 0; i < CESSNA_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'n') {
			sscanf_s(line, "n %f %f %f", &cessnaNormals[i][0], &cessnaNormals[i][1], &cessnaNormals[i][2]);
		}
	}
	//skip gap
	fgets(line, sizeof(line), file);

	int cessnaCount = 0;
	Object currentObj;
	currentObj.numPolygons = 0;

	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'g') {
			addPlaneObject(cessnaParts, &cessnaCount, &currentObj);
		}
		else if (line[0] == 'f') {
			int indices[50];
			int numVertices = 0;
			char* token = NULL;
			char* nextToken = NULL;
			token = strtok_s(line + 1, " \t\n", &nextToken);
			while (token != NULL) {
				indices[numVertices] = atoi(token);
				numVertices++;
				token = strtok_s(NULL, " \t\n", &nextToken); 
			}
			addPolygon(&currentObj, indices, numVertices);
		}
	}


	addPlaneObject(cessnaParts, &cessnaCount, &currentObj);
	fclose(file);
}

void initializePropellers(void) {
	FILE* file;
	if (fopen_s(&file, "propeller.txt", "r") != 0) {
		printf("Error opening file\n");
		return;
	}

	char line[128];

	for (int i = 0; i < PROP_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'v') {
			sscanf_s(line, "v %f %f %f", &propPoints[i][0], &propPoints[i][1], &propPoints[i][2]);
		}
		propPoints[i][0] += 0.16;
		propPoints[i][1] += 0.13;
		propPoints[i][2] += -0.36;
	}

	for (int i = 0; i < PROP_POINT_COUNT; i++) {
		fgets(line, sizeof(line), file);
		if (line[0] == 'n') {
			sscanf_s(line, "n %f %f %f", &propNormals[i][0], &propNormals[i][1], &propNormals[i][2]);
		}
	}
	//skip gap
	fgets(line, sizeof(line), file);

	int propCount = 0;
	Object currentObj;
	currentObj.numPolygons = 0;

	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'g') {
			addPlaneObject(propParts, &propCount, &currentObj);
		}
		else if (line[0] == 'f') {
			int indices[50];
			int numVertices = 0;
			char* token = NULL;
			char* nextToken = NULL;
			token = strtok_s(line + 1, " \t\n", &nextToken);
			while (token != NULL) {
				indices[numVertices] = atoi(token);
				numVertices++;
				token = strtok_s(NULL, " \t\n", &nextToken);
			}
			addPolygon(&currentObj, indices, numVertices);
		}
	}


	addPlaneObject(propParts, &propCount, &currentObj);
	fclose(file);
}

void initializeMountains(void) {
	GLfloat meshSideLength = (GLfloat) 4/MESH_RES;
	for (int i = 0; i < MESH_RES + 1; i++) {
		for (int j = 0; j < MESH_RES + 1; j++) {
			mountainVertices[i][j][0] = (GLfloat) i * meshSideLength;
			mountainVertices[i][j][2] = (GLfloat) j * meshSideLength;
		}
	}

	for (int i = 2; i < log(MESH_RES) / log(2); i *= 2) {
		for (int j = 0; j < MESH_RES; j += MESH_RES / i) {
			if (mountainVertices[i][j][1] == 0) {
				mountainVertices[i][j][1] = getRandomNumber(initialRandAmount) / i + mountainVertices[i][j][0] * mountainHeight / (MESH_RES + 1);
			}
		}
	}




	for (int i = 0; i < MESH_RES; i++) {
		for (int j = 0; j < MESH_RES; j++) {

			for (int k = 0; k < 3; k++) {
				mountainPolygonFaces[i][j][0][k] = mountainVertices[i][j][k];
				mountainPolygonFaces[i][j][1][k] = mountainVertices[i + 1][j][k];
				mountainPolygonFaces[i][j][2][k] = mountainVertices[i + 1][j + 1][k];
				mountainPolygonFaces[i][j][3][k] = mountainVertices[i][j + 1][k];
			}
		}
	}
}

void movePropeller(int x, int y, int z) {
	for (int i = 0; i < PROP_POINT_COUNT; i++) {
		propPoints[i][0] += (x * 0.01);
		propPoints[i][1] += (y * 0.01);
		propPoints[i][2] += (z * 0.01);
	}
}

void drawOrigin(void) {

	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
	glNormal3f(0, 1, 0);

	//origin lines
	glBegin(GL_LINES);
	glLineWidth(5.0f);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.5, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();

	glLineWidth(1.0f);
	glColor3f(1.0, 1.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuse);
	
	// origin ball
	GLUquadric* quad = gluNewQuadric();
	glPushMatrix();
	glScalef(0.01, 0.01, 0.01);
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();



	glPushMatrix();

	glScalef(30.0, 0.0, 30.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightblueDiffuse);
	glNormal3f(0, 1, 0);
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



void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);

	currentWidth = newWidth;
	currentHeight = newHeight;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, (GLfloat) newWidth / (GLfloat) newHeight, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void myPassiveMouse(int x, int y) {
	GLfloat halfScreen = (GLfloat)currentWidth / 2;

	GLfloat cursorRatio = (GLfloat)(x - halfScreen) / halfScreen;


	GLfloat tilt = cursorRatio;


	turnAngle = fmaxf(-1.0, fminf(1.0, tilt));


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
	glutPostRedisplay();
}


void mySpecialKeyboard(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			forwardVector[1] += heightIncrement;
			break;
		case GLUT_KEY_DOWN:
			forwardVector[1] -= heightIncrement;
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

void intializeFog(void) {
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, fogDensity);
}

void generateTextures(void) {
	GLubyte* seaData;
	GLubyte* skyData;
	GLubyte* mountainData;

	glGenTextures(1, &seaTexture);
	glGenTextures(1, &skyTexture);
	glGenTextures(1, &mountainTexture);

	int imageWidth, imageHeight, numChannels;
	seaData = stbi_load("sea02.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	glBindTexture(GL_TEXTURE_2D, seaTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, seaData);

	stbi_image_free(seaData);

	skyData = stbi_load("sky08.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, skyData);

	stbi_image_free(skyData);


	mountainData = stbi_load("mount03.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	glBindTexture(GL_TEXTURE_2D, mountainTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, mountainData);

	stbi_image_free(mountainData);

	glBindTexture(GL_TEXTURE_2D, 0);

}

/************************************************************************

	Function:		initializeGL

	Description:	Initializes the OpenGL rendering context for display

*************************************************************************/
void initializeGL(void)
{
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat globalAmbientLight[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

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
	gluPerspective(45, (GLfloat)originalWidth / (GLfloat)originalHeight, 0.1, 20);

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

	glutReshapeFunc(myReshape);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(mySpecialKeyboard);

	glutPassiveMotionFunc(myPassiveMouse);

	glutIdleFunc(myIdle);

	// register the blend function to enable opacity fading
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	generateTextures();

	intializeFog();
	// initialize the rendering context
	initializeGL();

	initializeCessna();

	initializePropellers();

	initializeMountains();

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
	printf("g \t: toggle fog\n");
	printf("m \t: toggle mountains\n");
	printf("t \t: toggle mountain texture\n");
	printf("s \t: toggle sea & sky\n");
	printf("w \t: toggle wire frame\n");
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