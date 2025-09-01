// CS3241 Assignment 1: Doodle
#include <cmath>
#include <vector>
#include <iostream>
#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

#define DEG_TO_RAD(x) ((x) * (M_PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / PI))


GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;

typedef struct {
    float x, y, z;
} vec3;



std::vector<vec3> createCircle(float radius, int vector_count) {
    std::vector<vec3> vertices;
    float angle = 360.0f / vector_count;
    int triangle_count = vector_count - 2; //n vertices = n-2 triangles
    std::vector<vec3> temp;

    for (int i = 0; i < vector_count; i++) {
        float current_angle = angle * i;
        float x = radius * cos(DEG_TO_RAD(current_angle));
        float y = radius * sin(DEG_TO_RAD(current_angle));
        temp.push_back({x, y, 0.0f});
    }

    //all triangles are generated from a common vertex - in this case temp[0]
    for (int i = 0; i < triangle_count; i++) {
        vertices.push_back(temp[0]);	    //the origin vertex
        vertices.push_back(temp[i + 1]);	//current vertex
        vertices.push_back(temp[i + 2]);	//next vertex
    }

    return vertices;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    //controls transformation
    glScalef(k, k, k);
    glTranslatef(tx, ty, 0);
    glRotatef(alpha, 0, 0, 1);

    //draw your stuff here (Erase the triangle code)
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0, 0);
    glVertex2f(-3,-3);
    glVertex2f(3,-3);
    glVertex2f(0,3);
    glEnd();

    glPopMatrix();
    glFlush ();
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init(void)
{
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glShadeModel (GL_SMOOTH);
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
        tx-=0.1;
        glutPostRedisplay();
        break;

    case 'c':
        tx+=0.1;
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

int main(int argc, char **argv)
{
    cout<<"CS3241 Lab 1\n\n";
    cout<<"+++++CONTROL BUTTONS+++++++\n\n";
    cout<<"Scale Up/Down: Q/E\n";
    cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
    cout<<"Move Up/Down: W/S\n";
    cout<<"Move Left/Right: Z/C\n";

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (600, 600);
    glutInitWindowPosition (50, 50);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
