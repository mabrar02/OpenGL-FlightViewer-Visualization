


/* include the library header files */
#include <stdlib.h>
#include <stdio.h>
#include <freeglut.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define textureWidth  64
#define textureHeight 64
#define CESSNA_POINT_COUNT 6763
#define CESSNA_FACE_COUNT 3639
#define PROP_POINT_COUNT 6763
#define PROP_FACE_COUNT 132
#define CESSNA_OBJECT_COUNT 34
#define M_PI 3.141592


void printKeyboardControls(void);
void positionCamera(void);
void drawOrigin(void);
void drawCessna(void);
void drawPropellers(void);
void drawSeaSky(void);


typedef struct {
	int indices[50];
	int numVertices;
} PolygonShape;

typedef struct {
	PolygonShape polygons[700];
	int numPolygons;

} PlaneObject;

PlaneObject cessnaParts[CESSNA_OBJECT_COUNT];
GLfloat cessnaPoints[CESSNA_POINT_COUNT][3];
GLfloat cessnaNormals[CESSNA_POINT_COUNT][3];

PlaneObject propParts[3];
GLfloat propPoints[PROP_POINT_COUNT][3];
GLfloat propNormals[PROP_POINT_COUNT][3];


int originalWindowPosX = 100;
int originalWindowPosY = 150;
int originalWidth = 900;
int originalHeight = 600;
int currentWidth = 900;
int currentHeight = 600;

int wireframeToggled = 0;
int fullScreenToggled = 0;
int seaSkyToggled = 0;
int mountainToggled = 0;
int fogToggled = 0; 

GLfloat camPos[3] = { 0.0, 2.5, 10.0 };
GLfloat forwardVector[3] = {0.0, 1.75, 7.0};
GLfloat distOffset = 3.0;

GLfloat turnAngle = 0;
GLfloat moveSpeed = 0.05;
GLfloat speedIncrement = 0.001;
GLfloat heightIncrement = 0.01;
GLfloat turnRate = 0.1;
GLfloat forwardAngle = M_PI / 2;

GLfloat theta = 0.0;
GLint propellerSpeed = 2;


GLfloat lightPos[] = { 0, 50.0, 0, 1.0 };
GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };

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

GLfloat cessnaShininess = 100.0;

GLuint seaTexture;
GLuint skyTexture;
GLuint mountainTexture;


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

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if (wireframeToggled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	if (seaSkyToggled) {
		drawSeaSky();
	}
	else {
		drawOrigin();
	}


	drawCessna();

	// swap the drawing buffers
	glutSwapBuffers();
}

void positionCamera(void) {
	gluLookAt(camPos[0], camPos[1], camPos[2], forwardVector[0], forwardVector[1], forwardVector[2], 0.0, 1.0, 0.0);
}

void moveCessna(void) {
	forwardAngle += turnRate * turnAngle;
	GLfloat moveVector[2] = { cos(forwardAngle), sin(forwardAngle) };
	forwardVector[0] += moveSpeed * -moveVector[0];
	forwardVector[2] += moveSpeed * -moveVector[1];

	camPos[0] = forwardVector[0] + moveVector[0] * distOffset;
	camPos[1] = forwardVector[1];
	camPos[2] = forwardVector[2] + moveVector[1] * distOffset;
}

void drawSeaSky(void) {
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, seaTexture);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, highEmission);

	GLUquadric* seaQuadricPtr = gluNewQuadric();
	glShadeModel(GLU_SMOOTH);
	gluQuadricNormals(seaQuadricPtr, GLU_SMOOTH);
	gluQuadricTexture(seaQuadricPtr, GL_TRUE);

	glPushMatrix();

	glTranslatef(0, 0.5, 0);
	glRotatef(90, 1, 0, 0);
	gluDisk(seaQuadricPtr, 0, 42, 50, 8);


	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, skyTexture);
	GLUquadric* skyQuadricPtr = gluNewQuadric();
	glShadeModel(GLU_SMOOTH);
	gluQuadricNormals(skyQuadricPtr, GLU_SMOOTH);
	gluQuadricTexture(skyQuadricPtr, GL_TRUE);

	glPushMatrix();

	glTranslatef(0, 50, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(skyQuadricPtr, 41, 41, 50, 20, 20);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroMaterial);
}

void drawCessna() {
	glPushMatrix();

	glTranslatef(forwardVector[0], forwardVector[1] - 0.75, forwardVector[2]);
	GLfloat angleConversion = forwardAngle * 180 / M_PI;
	glRotatef(-angleConversion, 0, 1, 0);
	glRotatef(-45 * turnAngle, 1, 0, 0);

	for (int i = 1; i < CESSNA_OBJECT_COUNT; i++) {
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

		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, cessnaShininess);

		for (int j = 0; j < cessnaParts[i].numPolygons; j++) {
			glBegin(GL_POLYGON);

			for (int k = 0; k < cessnaParts[i].polygons[j].numVertices; k++) {
				glNormal3fv(cessnaNormals[cessnaParts[i].polygons[j].indices[k] - 1]);
				glVertex3fv(cessnaPoints[cessnaParts[i].polygons[j].indices[k]-1]);
			}
			glEnd();
		}
	}
	drawPropellers();

	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
}

void drawPropellers() {


	glPushMatrix();
	glTranslatef(-0.25, -0.15, 0.35);
	glRotatef(-theta * propellerSpeed, 1, 0, 0);
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



void myIdle(void) {
	theta += 1;
	moveCessna();

	glutPostRedisplay();
}

void addPlaneObject(PlaneObject cessnaParts[], int* cessnaCount, PlaneObject* currentObj) {
	if (*cessnaCount < CESSNA_OBJECT_COUNT) {
		cessnaParts[*cessnaCount] = *currentObj;
		(*cessnaCount)++;
		currentObj->numPolygons = 0;
	}
}


void addPolygon(PlaneObject* planeObj, int indices[], int numVertices) {
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
	PlaneObject currentObj;
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
	PlaneObject currentObj;
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
	// initialize the rendering context
	initializeGL();

	initializeCessna();

	initializePropellers();

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