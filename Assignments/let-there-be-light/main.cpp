// CS3241 Assignment 3: Let there be light
//////////////////////////////////////////
/// Patryk Mrozek
/////////////////////////////////////////
/// compile on Mac: g++ -o main main.cpp -framework GLUT -framework OpenGL -DGL_SILENCE_DEPRECATION -framework Cocoa && ./main

#include <OpenGL/OpenGL.h>
#include <cmath>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif



#define BENDER_HEAD_RADIUS 1.0f
#define BENDER_HEAD_SIZE 2.5f
#define BENDER_EYE_COVER_SIZE 0.7f
#define BENDER_EYE_COVER_RADIUS 0.3f
#define BENDER_EYE_SIZE 0.33f
#define BENDER_EYE_DIST BENDER_HEAD_RADIUS/3.3

//camera setup
// constants
#define INITIAL_CAMERA_X 0.0f
#define INITIAL_CAMERA_Y 0.0f
#define INITIAL_CAMERA_Z 0.0f
#define INITIAL_LOOKAT_X 0.0f
#define INITIAL_LOOKAT_Y 0.0f
#define INITIAL_LOOKAT_Z -6.0f
#define INITIAL_UP_X 0.0f
#define INITIAL_UP_Y 1.0f
#define INITIAL_UP_Z 0.0f
#define INITIAL_NEAR 1.0f
#define INITIAL_FAR 20.0f
#define INITIAL_FOVY 40.0f

//global vars
double camera_x = 0.0f, camera_y = 0.0f, camera_z = 0.0f;
double lookat_x = 0.0f, lookat_y = 0.0f, lookat_z = -6.0f;
double up_x = 0.0f, up_y = 1.0f, up_z = 0.0f;
double near_plane = 1.0f;
double far_plane = 20.0f;
double fovy = 40.0f;



using namespace std;

// global variable

bool m_Smooth = false;
bool m_Highlight = false;
GLfloat angle = 0;   /* in degrees */
GLfloat angle2 = 0;   /* in degrees */
GLfloat zoom = 1.0;
int mouseButton = 0;
int moving, startx, starty;

#define NO_OBJECT 4;
int current_object = 0;

using namespace std;

void setupLighting()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    // Lights, material properties
    GLfloat ambientProperties[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat diffuseProperties[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularProperties[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { -100.0f,100.0f,100.0f,1.0f };

    glClearDepth(1.0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularProperties);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

    // Default : lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

}

//helper to change colors
void setMaterial(float r, float g, float b) {
    GLfloat diffuse[] = {r, g, b, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
}


void drawSphere(double r)
{
    int i, j;
    int n = 20;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (i = 0; i < 2 * n; i++)
        for (j = 0; j < n; j++)
        {
            glBegin(GL_POLYGON);
            double x1 = r*sin(i * M_PI / n) * sin(j * M_PI / n);
            double y1 = r*cos(i * M_PI / n) * sin(j * M_PI / n);
            double z1 = r*cos(j * M_PI / n);
            glNormal3d(x1/r, y1/r, z1/r);
            glVertex3d(x1, y1, z1);

            double x2 = r*sin((i + 1) * M_PI / n) * sin(j * M_PI / n);
            double y2 = r*cos((i + 1) * M_PI / n) * sin(j * M_PI / n);
            double z2 = r*cos(j * M_PI / n);
            glNormal3d(x2/r, y2/r, z2/r);
            glVertex3d(x2, y2, z2);

            double x3 = r*sin((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
            double y3 = r*cos((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
            double z3 = r*cos((j + 1) * M_PI / n);
            glNormal3d(x3/r, y3/r, z3/r);
            glVertex3d(x3, y3, z3);

            double x4 = r*sin(i * M_PI / n) * sin((j + 1) * M_PI / n);
            double y4 = r*cos(i * M_PI / n) * sin((j + 1) * M_PI / n);
            double z4 = r*cos((j + 1) * M_PI / n);
            glNormal3d(x4/r, y4/r, z4/r);
            glVertex3d(x4, y4, z4);

            glEnd();
        }

}

void drawSquare(double size) {
    glBegin(GL_QUADS);
    glVertex3d(-size/2, -size/2, 0);
    glVertex3d(size/2, -size/2, 0);
    glVertex3d(size/2, size/2, 0);
    glVertex3d(-size/2, size/2, 0);
    glEnd();
}

void drawCube(double size) {
    //around x axis - 4 faces
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i*90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, size/2);
        drawSquare(size);
        glPopMatrix();
    }

    //around y axis - 2 faces (90, 270)
    for (int i = 1; i < 3; i++) {
        glPushMatrix();
        glRotatef(90 + (i-1)*180 , 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, size/2);
        drawSquare(size);
        glPopMatrix();
    }
}

void drawCylinder(double radius, double height, int sections, bool include_ends) {

    if (include_ends) {
        //top of the cylinder, normal pointing straight up
        glBegin(GL_POLYGON);
        glNormal3d(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < sections; i++) {
            double angle = 2 * M_PI * i / sections;
            glVertex3d(radius*cos(angle), height/2, radius*sin(angle));
        }
        glEnd();

        //bottom face - normal pointing down
        glBegin(GL_POLYGON);
        glNormal3d(0.0f, -1.0f, 0.0f);
        for (int i = 0; i < sections; i++) {
            double angle = 2 * M_PI * i / sections;
            glVertex3d(radius*cos(angle), -height/2, radius*sin(angle));
        }
        glEnd();
    }

    for (int i = 0; i < sections; i++) {
        //calc angles for two consecutive sections
        double angle1 = 2.0f * M_PI * i / sections;
        double angle2 = 2.0f * M_PI * (i+1) / sections;

        //points on bottom face, can use height to judge the y coord
        double x1 = radius * cos(angle1);
        double z1 = radius * sin(angle1);

        double x2 = radius * cos(angle2);
        double z2 = radius * sin(angle2);

        glBegin(GL_QUADS);
        //BL
        glNormal3d(cos(angle1), 0, sin(angle1)); //pointing away
        glVertex3d(x1, -height/2, z1);
        //BR
        glNormal3d(cos(angle2), 0, sin(angle2));
        glVertex3d(x2, -height/2, z2);
        //TR
        glNormal3d(cos(angle2), 0, sin(angle2));
        glVertex3d(x2, height/2, z2);
        //TL
        glNormal3d(cos(angle1), 0, sin(angle1));
        glVertex3d(x1, height/2, z1);

        glEnd();
    }
}

void drawComp1(double radius, double size) {
    glPushMatrix();
    glScalef(1/size, 1/size, 1/size);
    setMaterial(1.0f, 0.0f, 1.0f);
    drawSphere(radius);

    for (int i = 0; i < 360; i += 45) {
        for (int j = -45; j <= 45; j += 90) {
            glPushMatrix();
            glRotatef(i, 0.0f, 1.0f, 0.0f);
            glRotatef(j, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, size/2);
            setMaterial(0.0f, 0.0f, 1.0f);
            drawCylinder(radius * 0.5, size, 20, true);
            glTranslatef(0.0f, -size/2, 0.0f);
            setMaterial(1.0f, 1.0f, 0.01f);
            drawSphere(radius/2);
            glTranslatef(0.0f, size, 0.0f);
            //drawSphere(radius);
            /*
            glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, size/2);

            drawCylinder(radius*0.3, size, 20);
            */
            glPopMatrix();
        }
    }

    glPopMatrix();
}

void drawBenderHead() {


    setMaterial(0.7f, 0.7f, 0.8f);

    drawCylinder(BENDER_HEAD_RADIUS, BENDER_HEAD_SIZE, 50, true);

    glPushMatrix();
    glTranslatef(0.0f, BENDER_HEAD_SIZE/2, 0.0f);
    drawSphere(BENDER_HEAD_RADIUS);
    glPopMatrix();
}

void drawBenderAntenna() {
    glPushMatrix();
    glTranslatef(0.0f, BENDER_HEAD_SIZE*0.9, 0.0f);
    glPushMatrix();
    glScalef(1.3f, 1.0f, 1.1f);
    drawSphere(0.1f);
    glPopMatrix();
    glPushMatrix();
    glScalef(1.0f, 10.0f, 1.0f);
    drawSphere(0.05f);
    glPopMatrix();
    glTranslatef(0.0f, 0.5f, 0.0);
    drawSphere(0.08f);
    glPopMatrix();
}

void drawBenderEyeCover() {

    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, BENDER_HEAD_RADIUS*0.9, -BENDER_HEAD_SIZE/6);
    glPushMatrix();
    glScalef(2.5f, 1.0f, 1.3f);
    drawCylinder(BENDER_EYE_COVER_RADIUS, BENDER_EYE_COVER_SIZE, 50, false);
    setMaterial(0.0f, 0.0f, 0.0f);
    glScalef(0.99f, 0.99f, 0.99f);
    drawCylinder(0.3f, 0.3f, 50, true);
    glPopMatrix();
    glPopMatrix();
}

void drawBenderEyes() {
    setMaterial(1.0f, 1.0f, 0.3f);
    glPushMatrix();
    glTranslatef(-BENDER_EYE_DIST, BENDER_EYE_COVER_RADIUS*1.4f, BENDER_HEAD_RADIUS);
    drawSphere(BENDER_EYE_SIZE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, BENDER_EYE_SIZE);
    setMaterial(0.0f, 0.0f, 0.0f);
    drawSquare(0.1f);
    glPopMatrix();
    setMaterial(1.0f, 1.0f, 0.3f);
    glTranslatef(2*BENDER_EYE_DIST, 0.0f, 0.0f);
    drawSphere(BENDER_EYE_SIZE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, BENDER_EYE_SIZE);
    setMaterial(0.0f, 0.0f, 0.0f);
    drawSquare(0.1f);
    glPopMatrix();
    glPopMatrix();
}

void drawBenderMouth() {

    glPushMatrix();

    glTranslatef(0.0f, -BENDER_EYE_COVER_RADIUS*1.5, BENDER_HEAD_RADIUS/5);
    setMaterial(1.0f, 1.0f, 0.3f);
    glScalef(1.7f, 1.0f, 1.7f);
    drawCylinder(BENDER_HEAD_RADIUS/2, BENDER_EYE_COVER_SIZE, 50, false);
    setMaterial(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.001f);
    drawCylinder(BENDER_HEAD_RADIUS/2, 0.01f, 50, false);

    glPopMatrix();
}

void bender() {
    glPushMatrix();

    glTranslatef(0.0f, -0.4f, 0.0f);
    glScalef(0.8f, 0.8f, 0.8f);
    drawBenderHead();
    drawBenderAntenna();
    drawBenderEyeCover();
    drawBenderEyes();
    drawBenderMouth();

    glPopMatrix();

}

void updateProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, 1.0, near_plane, far_plane);
    glMatrixMode(GL_MODELVIEW);
}

void resetCamera() {
    camera_x = INITIAL_CAMERA_X;
    camera_y = INITIAL_CAMERA_Y;
    camera_z = INITIAL_CAMERA_Z;
    lookat_x = INITIAL_LOOKAT_X;
    lookat_y = INITIAL_LOOKAT_Y;
    lookat_z = INITIAL_LOOKAT_Z;
    up_x = INITIAL_UP_X;
    up_y = INITIAL_UP_Y;
    up_z = INITIAL_UP_Z;
    near_plane = INITIAL_NEAR;
    far_plane = INITIAL_FAR;
    fovy = INITIAL_FOVY;

    updateProjection();
    glutPostRedisplay();
}

void bestAngle() {
    up_x = INITIAL_UP_X;
    up_y = INITIAL_UP_Y;
    up_z = INITIAL_UP_Z;
    near_plane = INITIAL_NEAR;
    far_plane = INITIAL_FAR;
    fovy = INITIAL_FOVY;

    switch (current_object) {
    case 0: //sphere
        camera_x = 2.0f; camera_y = 1.0f; camera_z = 2.0f;
        lookat_x = 0.0f; lookat_y = 0.0f; lookat_z = -6.0f;
        break;
    case 1: //cylinder
        camera_x = 1.5f; camera_y = 2.0f; camera_z = 1.5f;
        lookat_x = 0.0f; lookat_y = 0.0f; lookat_z = -6.0f;
        break;
    case 2: //comp1
        camera_x = 1.0f; camera_y = 1.0f; camera_z = 1.0f;
        lookat_x = 0.0f; lookat_y = 0.0f; lookat_z = -6.0f;
        fovy = 20.0f; // Wider view for complex object
        break;
    case 3: //bender
        camera_x = 1.0f; camera_y = 0.5f; camera_z = 2.0f;
        lookat_x = 0.0f; lookat_y = -0.5f; lookat_z = -6.0f;
        break;
    default:
        camera_x = 2.0f; camera_y = 1.0f; camera_z = 2.0f;
        lookat_x = 0.0f; lookat_y = 0.0f; lookat_z = -6.0f;
        break;
    }

    updateProjection();
    glutPostRedisplay();
}



void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setMaterial(1.0f, 0.0f, 0.0f);

    if (m_Smooth) {
        glShadeModel(GL_SMOOTH);
    } else {
        glShadeModel(GL_FLAT);
    }

    if (m_Highlight) {
        GLfloat mat_spec[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat mat_shine[] = {100.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine);
    } else {
        GLfloat mat_spec[] = {0.0f, 0.0f, 0.0f, 0.0f};
        GLfloat mat_shine[] = {0.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera_x, camera_y, camera_z,
            lookat_x, lookat_y, lookat_z,
            up_x, up_y, up_z);


    glPushMatrix();
    glTranslatef(0, 0, -6);

    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);

    glScalef(zoom, zoom, zoom);

    switch (current_object) {
    case 0:
        drawSphere(1);
        break;
    case 1:
        //drawCube(1);
        drawCylinder(1.0f, 3.0f, 50, true);
        break;
    case 2:
        drawComp1(1.0f, 10.0f);
        break;
    case 3:
        bender();
        break;
    default:
        break;
    };
    glPopMatrix();
    glutSwapBuffers();
}




void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'p':
    case 'P':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 'w':
    case 'W':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 'v':
    case 'V':
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case 's':
    case 'S':
        m_Smooth = !m_Smooth;
        break;
    case 'h':
    case 'H':
        m_Highlight = !m_Highlight;
        break;

    case '1':
    case '2':
    case '3':
    case '4':
        current_object = key - '1';
        break;
    case 'n':
        near_plane -= 0.1f;
        if (near_plane < 0.1f) near_plane = 0.1f;
        updateProjection();
        cout << "NEAR PLANE: " << near_plane << "\n";
        break;
    case 'N':
        near_plane += 0.1f;
        updateProjection();
        break;
    case 'f':
        far_plane -= 1.0f;
        if (far_plane < near_plane + 1.0f) far_plane = near_plane + 1.0f;
        updateProjection();
        cout << "FAR PLANE: " << far_plane << "\n";
        break;
    case 'F':
        far_plane += 1.0f;
        updateProjection();
        break;
    case 'o':
        fovy -= 2.0f;
        if (fovy < 10.0f) fovy = 10.0f;
        updateProjection();
        break;
    case 'O':
        fovy += 2.0f;
        if (fovy > 150.0f) fovy = 150.0f;
        updateProjection();
        break;
    case 'r':
        cout << "RESETTING!\n";
        resetCamera();
        break;
    case 'R':
        cout << "BEST ANGLE!\n";
        bestAngle();
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



void
mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouseButton = button;
        moving = 1;
        startx = x;
        starty = y;
    }
    if (state == GLUT_UP) {
        mouseButton = button;
        moving = 0;
    }
}

void motion(int x, int y)
{
    if (moving) {
        if (mouseButton == GLUT_LEFT_BUTTON)
        {
            angle = angle + (x - startx);
            angle2 = angle2 + (y - starty);
        }
        else zoom += ((y - starty) * 0.001);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }

}

int main(int argc, char** argv)
{
    cout << "CS3241 Lab 3" << endl << endl;

    cout << "1-4: Draw different objects" << endl;
    cout << "S: Toggle Smooth Shading" << endl;
    cout << "H: Toggle Highlight" << endl;
    cout << "W: Draw Wireframe" << endl;
    cout << "P: Draw Polygon" << endl;
    cout << "V: Draw Vertices" << endl;
    cout << "n/N: Decrease/Increase near plane" << endl;
    cout << "f/F: Decrease/Increase far plane" << endl;
    cout << "o/O: Decrease/Increase field of view" << endl;
    cout << "r: Reset camera to initial position" << endl;
    cout << "R: Set best viewing angle for current object" << endl;
    cout << "Q: Quit" << endl << endl;

    cout << "Left mouse click and drag: rotate the object" << endl;
    cout << "Right mouse click and drag: zooming" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("CS3241 Assignment 3");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    setupLighting();
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_MODELVIEW);
    updateProjection();
    glutMainLoop();

    return 0;
}
