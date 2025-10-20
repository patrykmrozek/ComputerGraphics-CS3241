// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <OpenGL/OpenGL.h>
#include <iostream>
#include <fstream>

#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8

using namespace std;

// Global variables that you can use
typedef struct Point {
	float x,y;
} Point;

// Storage of control points
int nPt = 0;
Point ptList[MAXPTNO];
bool modList[MAXPTNO] = {false}; //modified points (due to c1)


// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false;
bool C1Continuity = false;

void drawRightArrow()
{
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0,0);
		glVertex2f(100,0);
		glVertex2f(95,5);
		glVertex2f(100,0);
		glVertex2f(95,-5);
	glEnd();
}

void drawControlPoints()
{
    glPointSize(5);
	glBegin(GL_POINTS);
	for(int i=0;i<nPt; i++) {
	if (modList[i]) {
            glColor3f(1.0, 0.0, 0.0); //if modified (red)
        }
        else {
            glColor3f(0.0, 0.0, 0.0); //else (black)
        }
	    glVertex2d(ptList[i].x,ptList[i].y);
	}
	glEnd();
	glPointSize(1);
}

void drawControlLines()
{
    if (nPt < 2) return;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < nPt; i++) {

            glVertex2i(ptList[i].x, ptList[i].y);
        }
    glEnd();
}

Point getBezierPoint(float t, const Point p0, const Point p1, const Point p2, const Point p3)
{
    //cubic bezier:
    //p(t) = (1-t)^3p00 + 3t(1-t)p01 + 3t^2(1-t) + t^3p02
    float omt = 1.0f - t;
    //(1-t)^3
    float b0 = omt * omt * omt;
    //3t(1-t)
    float b1 = 3 * t * omt * omt;
    //3t^2(1-t)
    float b2 = 3 * t * t * omt;
    //t^3
    float b3 = t * t * t;

    //p(t).x
    Point p;
    p.x = (int)((b0 * p0.x) + (b1 * p1.x) + (b2 * p2.x) + (b3 * p3.x));
    p.y = (int)((b0 * p0.y) + (b1 * p1.y) + (b2 * p2.y) + (b3 * p3.y));

    return p;
}

/*
void applyC1Continuity()
{
    if (nPt < 7) return; //need at least the first 2 segments (skip first one)
    for (int i = 3; i+3 < nPt; i += 3) {
        ptList[i+1].x = ptList[i].x * 2 - ptList[i-1].x;
        ptList[i+1].y = ptList[i].y * 2 - ptList[i-1].y;

        modList[i+1] = true; //mark as modified
    }
}
*/

void drawBezierCurves()
{
    if (nPt < 4) return;

    glColor3f(0.0, 0.0, 0.0);
    //sectors of 4, sharing the last/first point
    for (int i = 0; i+3 < nPt; i+=3) {
        Point p0 = ptList[i+0];
        Point p1 = ptList[i+1];
        Point p2 = ptList[i+2];
        Point p3 = ptList[i+3];

        if (C1Continuity && i >= 3) {
            Point prevP2 = ptList[i-1]; //prev segments last handle
            //p1 = 2p0 - prevP2
            p1.x = 2.0f * p0.x - prevP2.x;
            p1.y = 2.0f * p0.x - prevP2.y;
        }

        glBegin(GL_LINE_STRIP);
            for (float t = 0.0f; t < 1.0f; t += 0.01f) {
                Point p = getBezierPoint(t, p0, p1, p2, p3);
                glVertex2f((float)p.x, (float)p.y);
            }
        glEnd();
    }
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(displayControlLines)
	{
	    drawControlLines();
	}

	glPushMatrix();
	if(displayControlPoints)
	{
	    drawControlPoints();
	}

	glPopMatrix();

	drawBezierCurves();

	glutSwapBuffers ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void init(void)
{
	glClearColor (1.0,1.0,1.0, 1.0);
}

void readFile()
{

	std::ifstream file;
    file.open("savefile.txt");
	file >> nPt;

	if(nPt>MAXPTNO)
	{
		cout << "Error: File contains more than the maximum number of points." << endl;
		nPt = MAXPTNO;
	}

	for(int i=0;i<nPt;i++)
	{
		file >> ptList[i].x;
		file >> ptList[i].y;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile()
{
	std::ofstream file;
    file.open("savefile.txt");
    file << nPt << endl;

	for(int i=0;i<nPt;i++)
	{
		file << ptList[i].x << " ";
		file << ptList[i].y << endl;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case 'r':
		case 'R':
			readFile();
		break;

		case 'w':
		case 'W':
			writeFile();
		break;

		case 'T':
		case 't':
			displayTangentVectors = !displayTangentVectors;
		break;

		case 'o':
		case 'O':
			displayObjects = !displayObjects;
		break;

		case 'p':
		case 'P':
			displayControlPoints = !displayControlPoints;
		break;

		case 'L':
		case 'l':
			displayControlLines = !displayControlLines;
			//printf("Control lines: %d\n", (int)displayControlLines);
		break;

		case 'C':
		case 'c':
			C1Continuity = !C1Continuity;
			if (!C1Continuity) {
			    memset(modList, false, sizeof(modList));
			}
			glutPostRedisplay();
		break;

		case 'e':
		case 'E':
		    nPt = 0;
			memset(modList, false, sizeof(modList)); //set all to false
            glutPostRedisplay();
		break;

		case 'Q':
		case 'q':
			exit(0);
		break;

		default:
		break;
	}

	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y)
{
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */
	enum
	{
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};
	if((button == MOUSE_LEFT_BUTTON)&&(state == GLUT_UP))
	{
		if(nPt==MAXPTNO)
		{
			cout << "Error: Exceeded the maximum number of points." << endl;
			return;
		}
		ptList[nPt].x=x;
		ptList[nPt].y=y;
		modList[nPt] = false;
		nPt++;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 4"<< endl<< endl;
	cout << "Left mouse click: Add a control point"<<endl;
	cout << "Q: Quit" <<endl;
	cout << "P: Toggle displaying control points" <<endl;
	cout << "L: Toggle displaying control lines" <<endl;
	cout << "E: Erase all points (Clear)" << endl;
	cout << "C: Toggle C1 continuity" <<endl;
	cout << "T: Toggle displaying tangent vectors" <<endl;
	cout << "O: Toggle displaying objects" <<endl;
	cout << "R: Read in control points from \"savefile.txt\"" <<endl;
	cout << "W: Write control points to \"savefile.txt\"" <<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS3241 Assignment 4");
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
