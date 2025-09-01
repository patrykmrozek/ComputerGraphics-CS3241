#include <GLUT/glut.h>

//g++ -o t1 t1.cpp -framework GLUT -framework OpenGL -DGL_SILENCE_DEPRECATION -framework Cocoa && ./t1

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
