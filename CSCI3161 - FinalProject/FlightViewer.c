
/************************************************************************************

	File: 			DisplayJPG.c

	Description:	Draw a JPG image.

	Author:			Edward Angel, Edited by Stephen Brooks

*************************************************************************************/


// include the library header files 
#include <stdio.h>
#include <stdlib.h>
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// image size
int imageWidth, imageHeight, numChannels;

// the image data
GLubyte* imageData;



/************************************************************************

	Function:		myDisplay

	Description:	Displays a the image.

*************************************************************************/
void myDisplay()
{
	// clear the display
	glClear(GL_COLOR_BUFFER_BIT);

	//set the position of the bottom left hand corner of the image
	glRasterPos2i(0, 0);

	// draw the image
	glDrawPixels(imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	// dump out the image to the screen
	glFlush();
}


/************************************************************************

	Function:		myReshape

	Description:	Displays a the image.

*************************************************************************/
void myReshape(int h, int w)
{
	// set the drawing area to be the same size as the image
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLfloat)imageWidth, 0.0, (GLfloat)imageHeight);

}



/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(int argc, char* argv[])
{

	// load in the image
	imageData = stbi_load("mount03.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// set the window size to match the image
	glutInitWindowSize(imageWidth, imageHeight);
	// set window position on screen
	glutInitWindowPosition(100, 150);
	// open the screen window
	glutCreateWindow("image");

	// register redraw function
	glutDisplayFunc(myDisplay);
	// register the reshape function
	glutReshapeFunc(myReshape);

	// go into a perpetual loop
	glutMainLoop();
}


