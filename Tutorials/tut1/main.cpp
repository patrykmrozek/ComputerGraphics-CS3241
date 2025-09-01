#include <GLUT/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glVertex2f(-0.5, -0.5);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glEnd();
    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("My  Window");

    glutDisplayFunc(display);
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
 * d) Color from x=12 to x=14
 * e) LHS, m = 0.5 so deltaX = -2 : RHS, , = -0.5 so deltaX = 2
 * f) LHS: {(11, 14.5), (9, 13.5), (7, 12.5), (5, 11.5), (3, 10.5), (1, 9.5)}
 *    RHS: {(15, 14.5), (17, 13.5), (19, 12.5), (21, 11.5), (23, 10.5), (24, 9.5)}
 * g) fill from x=approx(3) to x=13, then from x=15 to x=22
 *
 */
