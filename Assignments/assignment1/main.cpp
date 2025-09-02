////////////////////////////////////
/// Name: Patryk Mrozek
/// Functions:
///////////////////////////////////

// CS3241 Assignment 1: Doodle
#include <cmath>
#include <vector>
#include <iostream>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

#define DEG_TO_RAD(x) ((x) * (M_PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / PI))

GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1;
float tx = 0.0, ty=1;

typedef struct {
    float x, y, z;
} vec3;



std::vector<vec3> createCircle(float radius, int vertex_count) {
    std::vector<vec3> vertices;
    float angle = 360.0f / vertex_count;
    int triangle_count = vertex_count - 2; //n vertices = n-2 triangles - eg pentagon needs only 3 triangles
    std::vector<vec3> temp;

    for (int i = 0; i < vertex_count; i++) {
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

void drawCircle(float radius, int vertex_count) {
    std::vector<vec3> circle_vertices = createCircle(radius, vertex_count);
    glBegin(GL_POLYGON);
    for (vec3 vertex : circle_vertices) {
        glVertex2f(vertex.x, vertex.y);
        //std::cout << vertex.x << " - " << vertex.y << "\n";
    }
    glEnd();
}

void drawHead() {
    drawCircle(5, 50);
}

void drawMouth() {
    glPushMatrix();
    glTranslatef(0.0, -5, 0.0);
    glScalef(0.7, 0.7, 0.0);
    drawCircle(5, 50);
    glPopMatrix();
}

void drawEye() {
    glColor3f(0.0, 0.0, 0.0);
    drawCircle(1.5, 30);
}

void drawEyes() {
    glPushMatrix();
    glScalef(1, 1, 0.0);
    glTranslatef(-2, -1.5, 0);
    drawEye();
    glTranslatef(4, 0, 0);
    drawEye();
    glPopMatrix();
}

void drawBones() {
    /*
     * drawCricle,
     * translate it to the edge of the screen,
     * rotate it about the center - ends up in the corner
     *
     */
    float offset = 12.0;
    glTranslatef(offset/2, -offset/2, 0);
    glPushMatrix();
    for (int i = 0; i < 4; i++) {
        float angle = 360/4;
        glRotatef(angle, 0, 0, 10);
        glTranslatef(offset, 0.0, 0.0f);

        glPushMatrix();
        glRotatef(angle/2, 0, 0, 1);
        glTranslatef(1, 0, 0);
        drawCircle(1.5, 50);
        glTranslatef(-3, 0, 0);
        drawCircle(1.5, 50);
        glPopMatrix();

        std::cout << "ANGLE: " << angle*i << "\n";
    }
    glPopMatrix();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    //controls transformation
    glScalef(k, k, k);
    glTranslatef(tx, ty, 0);
    glRotatef(alpha, 0, 0, 1);

    //drawing
    //draw outline/border first (black)
    glColor3f(0.0, 0.0, 0.0);
    //drawMouth();
    //drawHead();
    drawBones();
    //draw coloured sections (scaled down/coloured)
    /*
    glPushMatrix();
    glScalef(0.95, 0.95, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    drawMouth();
    drawHead();
    glPopMatrix();

    drawEyes();

    glPopMatrix();
    */
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
    std::cout<<"CS3241 Lab 1\n\n";
    std::cout<<"+++++CONTROL BUTTONS+++++++\n\n";
    std::cout<<"Scale Up/Down: Q/E\n";
    std::cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
    std::cout<<"Move Up/Down: W/S\n";
    std::cout<<"Move Left/Right: Z/C\n";

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (800, 800);
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
