#include <math.h>
#include <GLUT/glut.h>

float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0f;


void drawSphere(double r) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //x = (r*sin(Phi)*cos(Theta))
    //y = (r*sin(Phi)*sin(Theta))
    //z = (r*cos(Phi))
    int n = 20;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 2*n; j++) {
            //AntiClockwise
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
    gluPerspective(45.0, (double)w/h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(tx, ty, 5.0,  // eye position
              0.0, 0.0, 0.0,  // look at center
              0.0, 1.0, 0.0); // up vector
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //draw_square();

    glPushMatrix();

    //glTranslatef(tx, ty, 0);
    glRotatef(alpha, 0, 0, 1);
    glScalef(k, k, k);

    glColor3f(1.0, 0.0, 0.0);
    drawSphere(1.0);

    glPopMatrix();

    glutSwapBuffers();
}

void keyboard (unsigned char key, int x, int y)
{
    //keys to control scaling - k
    //keys to control rotation - alpha
    //keys to control translation - tx, ty
    switch (key) {

    case 'a':
        alpha+=10;
        glutPostRedisplay();
        break;

    case 'd':
        alpha-=10;
        glutPostRedisplay();
        break;

    case 'q':
        k+=0.1;
        glutPostRedisplay();
        break;

    case 'e':
        if(k>0.1)
            k-=0.1;
        glutPostRedisplay();
        break;

    case 'z':
        tx -= 0.1;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glutPostRedisplay();
        break;

    case 'c':
        tx += 0.1;
        reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glutPostRedisplay();
        break;

    case 's':
        ty-=0.1;
        glutPostRedisplay();
        break;

    case 'w':
        ty+=0.1;
        glutPostRedisplay();
        break;

    case 27:
        exit(0);

    default:
        break;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (800, 800);
    glutInitWindowPosition (50, 50);
    glutCreateWindow("Sphere Wireframe");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glutMainLoop();

    return 0;
}



//Question 1
/*
 * glutMainLoop runs the main event loop - keeps running until explicitly terminated.
 * it handles all events - window resize, inputs, etc..
 *
 */

//Question 2
/*
 * Initialize glut
 * Create a window
 * Call your predefined display function w/ a function pointer
 * Set a clear color
 * Call main loop
 *
*/

//Question 3
/*
 * functions created by users that are called automatically when some event occurs
 *
*/

//Question 4
/*
 * Rasterization: fast rendering, efficient for complex scenes, loses quality when transformed
 * Vector Graphics: Independent of resoultion, easy to edit, slow rendering for complex scenes
 */

//Question 5
/*
 * Double buffering ensures that only the complete, finished frames are seen.
 */

//Question 6
/*
 * If you forget to call swapBuffers(), the drawing would execute in the back buffer and
 * never be swapped, so it would appear frozen
 */

//Question 7
/*
 * We clip before scan conversion so that we can eliminate vertices not currenly in the frame
 */

//Question 8
/*
 * a) {(13, 16), (1, 10), (25, 10), (15, 6), (11, 2), (15, 2), (22, 1), (4, 1)}
 * b) (13, 15.5)
 * c) LHS: (12, 15.5), RHS: (14 15.5)
 * d) Color from x=12 to x=13
 * e) LHS, m = 0.5 so deltaX = -2 : RHS, , = -0.5 so deltaX = 2
 * f) LHS: {(11, 14.5), (9, 13.5), (7, 12.5), (5, 11.5), (3, 10.5), (1, 9.5)}
 *    RHS: {(15, 14.5), (17, 13.5), (19, 12.5), (21, 11.5), (23, 10.5), (24, 9.5)}
 * g) fill from x=approx(3) to x=13, then from x=15 to x=22
 * 	  keep a parity bit, that will keekp track of whether it is inside or outside the polygon
 *    each time ther is an intersection, flip the bit - initially false
 *
 * h) Every point P on the sphere can be specified by two angles Theta and Phi, radius r
 * 		x = (r*sin(Phi)*cos(Theta))
 * 		y = (r*sin(Phi)*sin(Theta))
 * 		z = (r*cos(Phi))
 *
 */
