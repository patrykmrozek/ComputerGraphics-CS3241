// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <time.h>
#include <iostream>
#include <thread> // Required for std::this_thread::sleep_for
#include <chrono> // Required for std::chrono::milliseconds
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1.0;
float tx = 0.0, ty=0.0;


void drawSphere(double r) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //x = (r*sin(Phi)*cos(Theta))
    //y = (r*sin(Phi)*sin(Theta))
    //z = (r*cos(Phi))
    int n = 20;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 2*n; j++) {
            //i, j, i, j TL
            float phi1 = i*M_PI/n;
            //i, j+1, i, j+1 BL
            float theta1 = j*M_PI/n;
            //i+1, j+1, i+1, j+1 BR
            float theta2 = (j+1)*M_PI/n;
            //i+1, j, i+1, j TR
            float phi2 = (i+1)*M_PI/n;
            glBegin(GL_QUADS);

            glVertex3f(r*sin(phi1)*cos(theta1),
                       r*sin(phi1)*sin(theta1),
                       r*cos(phi1));

            glVertex3f(r*sin(phi1)*cos(theta2),
                       r*sin(phi1)*sin(theta2),
                       r*cos(phi1));

            glVertex3f(r*sin(phi2)*cos(theta2),
                       r*sin(phi2)*sin(theta2),
                       r*cos(phi2));

            glVertex3f(r*sin(phi2)*cos(theta1),
                       r*sin(phi2)*sin(theta1),
                       r*cos(phi2));

            glEnd();
        }
    }
}




void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w/h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(tx, ty, 5.0,  // eye position
              0.0, 0.0, 0.0,  // look at center
              0.0, 1.0, 0.0); // up vector
}

void init(void)
{
    glClearColor (0.0, 0.0, 0.1, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
    glRotatef(alpha, 0, 0, 1);

    drawSphere(1.0);


	glPopMatrix();
	glFlush ();
}

void idle() {
    alpha += 0.2;
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

        case 27: // press ESC to exit
		case 'q':
		case 'Q':
            exit(0);

    /*
		case 't':
			clockMode = !clockMode;
			if (clockMode)
				cout << "Current Mode: Clock mode." << endl;
			else
				cout << "Current Mode: Solar mode." << endl;
			break;
*/

		default:
			break;
	}
}

int main(int argc, char **argv)
{
    std::cout<<"CS3241 Lab 2\n\n";
    std::cout<<"+++++CONTROL BUTTONS+++++++\n\n";
    std::cout<<"Toggle Time Mode: T\n";
    std::cout<<"Exit: ESC or q/Q\n";
    std::cout << "Current Mode: Clock mode." << "\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);	
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
