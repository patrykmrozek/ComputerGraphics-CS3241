////////////////////////////////////
/// Name: Patryk Mrozek
/// Functions: glNewList() - glEndList() - glCallList();
///////////////////////////////////

// CS3241 Assignment 1: Doodle
#include <cmath>
#include <vector>
#include <iostream>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

#define DEG_TO_RAD(x) ((x) * (M_PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / M_PI))

#define BONE_COUNT 4
#define BONE_RADIUS_OUTLINE 1.3f
#define BONE_RADIUS_FILL 1.1f
#define BONE_DIST_FROM_CENTER 9.0f
#define BONE_LENGTH 2.5f
#define BONE_ROTATION_OFFSET 45.0f
#define BONE_Y_OFFSET -1.5f
#define BONE_BALL_DIST_OUTLINE 0.8f
#define BONE_BALL_DIST_FILL 0.9f
#define BONE_BODY_SCALE_FACTOR 1.6f


#define HEAD_SCALE_FACTOR 0.95f
#define HEAD_RADIUS_OUTLINE 5.0f
#define HEAD_RADIUS_FILL 4.8f

#define MOUTH_RADIUS_OUTLINE 4.5f
#define MOUTH_RADIUS_FILL 4.2f
#define MOUTH_SCALE_X_FACTOR 0.75f
#define MOUTH_SCALE_Y_FACTOR 0.9f
#define MOUTH_Y_OFFSET -5.0f

#define EYE_RADIUS 1.5f
#define EYE_Y_OFFSET -1.5f
#define EYE_X_SPACING 3.75f

#define BRIM_LENGTH_OUTLINE 7
#define BRIM_RADIUS_OUTLINE 0.5
#define BRIM_LENGTH_FILL 6.9
#define BRIM_RADIUS_FILL 0.35
#define BRIM_Y_OFFSET 1

#define HAT_RADIUS_OUTLINE HEAD_RADIUS_OUTLINE
#define HAT_RADIUS_FILL HEAD_RADIUS_FILL

#define CIRCLE_NUM_VERTICES 50
#define CIRCLE_ANGLE_FULL 360.0f
#define CIRCLE_ANGLE_HALF 180.0f

GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1;
float tx = 0.0, ty=1;


//display list - a pre compiled list of OpenGL commands stored on the GPU
GLuint head_list_fill, head_list_outline, mouth_list_outline, mouth_list_fill, eye_list,
    bone_list_outline, bone_list_fill, brim_list_outline, brim_list_fill, hat_list_outline, hat_list_fill;


typedef struct {
    float x, y, z;
} vec3;



std::vector<vec3> createCircle(float radius, int vertex_count, float circle_angle) {
    std::vector<vec3> vertices;
    float angle = circle_angle / vertex_count;
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
    std::vector<vec3> circle_vertices = createCircle(radius, vertex_count, CIRCLE_ANGLE_FULL);
    glBegin(GL_POLYGON);
    for (vec3 vertex : circle_vertices) {
        glVertex2f(vertex.x, vertex.y);
        //std::cout << vertex.x << " - " << vertex.y << "\n";
    }
    glEnd();
}

void drawHead(float head_radius) {
    drawCircle(head_radius, CIRCLE_NUM_VERTICES);
}

void drawMouth(float mouth_radius) {
    glPushMatrix();
    glTranslatef(0.0, MOUTH_Y_OFFSET, 0.0);
    glScalef(MOUTH_SCALE_X_FACTOR, MOUTH_SCALE_Y_FACTOR, 0.0);
    drawCircle(mouth_radius, CIRCLE_NUM_VERTICES);
    glPopMatrix();
}

void drawEye() {
    glColor3f(0.0, 0.0, 0.0);
    drawCircle(EYE_RADIUS,  CIRCLE_NUM_VERTICES);
}

void drawEyes() {
    glPushMatrix();
    glScalef(1, 1, 0.0);
    glTranslatef(-EYE_X_SPACING/2, EYE_Y_OFFSET, 0);
    drawEye();
    glTranslatef(EYE_X_SPACING, 0, 0);
    drawEye();
    glPopMatrix();
}

void drawBoneBody(float length, float width) {
    float half_width = width/2;
    float half_length = length/2;

    glBegin(GL_POLYGON);
    glVertex2f(-half_length, -half_width);
    glVertex2f(half_length, -half_width);
    glVertex2f(half_length, half_width);
    glVertex2f(-half_length, half_width);
    glEnd();

}

void drawBones(int bone_count, float bone_radius, float bone_dist_from_center, float bone_ball_dist) {
    /*
     * drawCricle,
     * translate it to the edge of the screen,
     * rotate it about the center - ends up in the corner
     *
     */
    float angle_step = 2 * M_PI / bone_count;
    glPushMatrix();
    glTranslatef(0, BONE_Y_OFFSET, 0);//shift all bones down
    glRotatef(BONE_ROTATION_OFFSET, 0, 0, 1);//rotate all bones by 45 deg so that theyre in the corners

    for (int i = 0; i < bone_count; i++) {
        glPushMatrix();
        //angle at current i
        float angle_current = angle_step * i;

        //calculate the angle to face the origin ()
        float dangle = RAD_TO_DEG(angle_current); //dangle = degree angle :)
        glRotatef(dangle, 0, 0, 1.0); //rotate coord system - posx is bone direction

        glPushMatrix();
        glTranslatef(bone_dist_from_center/2, 0, 0); //go to between the end of the bone and the center
        drawBoneBody(bone_dist_from_center, bone_radius*BONE_BODY_SCALE_FACTOR);//draw a rectangle extending outward both ways
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, bone_radius*bone_ball_dist, 0); //start at center - move up 0.75 radius
        drawCircle(bone_radius, CIRCLE_NUM_VERTICES); //firsst 'bone-ball'
        glTranslatef(0, -bone_radius*(2*bone_ball_dist), 0); //move down 1.5 radius (0.75 each side)
        drawCircle(bone_radius, CIRCLE_NUM_VERTICES); //second 'bone-ball'
        glPopMatrix();

        glPushMatrix();
        glTranslatef(bone_dist_from_center, bone_radius*bone_ball_dist, 0); //move to where the end of the bone should be
        drawCircle(bone_radius, CIRCLE_NUM_VERTICES);
        glTranslatef(0, -bone_radius*(2*bone_ball_dist), 0); // lower ball
        drawCircle(bone_radius, CIRCLE_NUM_VERTICES);
        glPopMatrix();

        glPopMatrix();
    }

    glPopMatrix();
}

void drawBrim(float brim_radius, float brim_length) {
    //draw small circle on one side - rectangle to the other side - another circle
    glPushMatrix();
    glTranslatef(-brim_length, BRIM_Y_OFFSET, 0); //left side circle
    drawCircle(brim_radius, CIRCLE_NUM_VERTICES/2);
    glTranslatef(2*brim_length, 0, 0); //right side circle
    drawCircle(brim_radius, CIRCLE_NUM_VERTICES/2);

    glBegin(GL_POLYGON);
    glVertex2f(0, brim_radius);//TR
    glVertex2f(0, -brim_radius);//BR
    glVertex2f(-brim_length*2, -brim_radius);//BL
    glVertex2f(-brim_length*2, brim_radius);//TL
    glEnd();

    glPopMatrix();

}

void drawHat(float hat_radius, float start_angle, float end_angle) {
    //basically a rewrite of drawCircle but it draws a slice between 2 angles: 0-360=full - 0-180=half...
    std::vector<vec3> vertices;
    float vertex_count = CIRCLE_NUM_VERTICES;
    float angle_range = end_angle - start_angle;

    for (int i = 0; i <= vertex_count; i++) {
        float current_angle = start_angle + (angle_range * i / vertex_count);
        float x = hat_radius * cos(DEG_TO_RAD(current_angle));
        float y = hat_radius * sin(DEG_TO_RAD(current_angle));
        vertices.push_back({x, y, 0.0f});
    }

    vertices.insert(vertices.begin(), {0, 0, 0}); //make it start at 0,0

    glPushMatrix();
    glTranslatef(0, BRIM_Y_OFFSET, 0);
    glBegin(GL_POLYGON);
    for (vec3 vertex : vertices) {
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();
    glPopMatrix();

}

void createDisplayList() {
    //head
    head_list_outline = glGenLists(1);
    glNewList(head_list_outline, GL_COMPILE);
    drawHead(HEAD_RADIUS_OUTLINE);
    glEndList();

    head_list_fill = glGenLists(1);
    glNewList(head_list_fill, GL_COMPILE);
    drawHead(HEAD_RADIUS_FILL);
    glEndList();

    //mouth
    mouth_list_outline = glGenLists(1);
    glNewList(mouth_list_outline, GL_COMPILE);
    drawMouth(MOUTH_RADIUS_OUTLINE);
    glEndList();

    mouth_list_fill = glGenLists(1);
    glNewList(mouth_list_fill, GL_COMPILE);
    drawMouth(MOUTH_RADIUS_FILL);
    glEndList();

    //eyes
    eye_list = glGenLists(1);
    glNewList(eye_list, GL_COMPILE);
    drawEyes();
    glEndList();

    //bones
    bone_list_outline = glGenLists(1);
    glNewList(bone_list_outline, GL_COMPILE);
    drawBones(BONE_COUNT, BONE_RADIUS_OUTLINE, BONE_DIST_FROM_CENTER, BONE_BALL_DIST_OUTLINE);
    glEndList();

    bone_list_fill= glGenLists(1);
    glNewList(bone_list_fill, GL_COMPILE);
    drawBones(BONE_COUNT, BONE_RADIUS_FILL, BONE_DIST_FROM_CENTER, BONE_BALL_DIST_FILL);
    glEndList();

    //brim
    brim_list_outline = glGenLists(1);
    glNewList(brim_list_outline, GL_COMPILE);
    drawBrim(BRIM_RADIUS_OUTLINE, BRIM_LENGTH_OUTLINE);
    glEndList();

    brim_list_fill = glGenLists(1);
    glNewList(brim_list_fill, GL_COMPILE);
    drawBrim(BRIM_RADIUS_FILL, BRIM_LENGTH_FILL);
    glEndList();

    //hat
    hat_list_outline = glGenLists(1);
    glNewList(hat_list_outline, GL_COMPILE);
    drawHat(HAT_RADIUS_OUTLINE, 0, 180);

    hat_list_fill = glGenLists(1);
    glNewList(hat_list_fill, GL_COMPILE);
    drawHat(HEAD_RADIUS_FILL, 0, 180);
    glEndList();

}

void drawHeadOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(head_list_outline);
}

void drawHeadFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(head_list_fill);
}

void drawMouthOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(mouth_list_outline);
}

void drawMouthFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(mouth_list_fill);
}

void drawEyesFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(eye_list);
}

void drawBonesOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(bone_list_outline);
}

void drawBonesFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(bone_list_fill);
}

void drawBrimOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(brim_list_outline);
}

void drawBrimFillFromList() {
    glColor3f(1.0, 1.0, 0.0);
    glCallList(brim_list_fill);
}

void drawHatOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(hat_list_outline);
}

void drawHatFillFromList() {
    glColor3f(1.0, 1.0, 0.0);
    glCallList(hat_list_fill);
}




void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    //controls transformation
    glScalef(k, k, k);
    glTranslatef(tx, ty, 0);
    glRotatef(alpha, 0, 0, 1);

    //bones first
    drawBonesOutlineFromList();
    drawBonesFillFromList();

    //then mouth
    drawMouthOutlineFromList();
    drawMouthFillFromList();

    //head
    drawHeadOutlineFromList();
    drawHeadFillFromList();

    //hat
    drawHatOutlineFromList();
    drawHatFillFromList();

    //brim
    drawBrimOutlineFromList();
    drawBrimFillFromList();

    drawEyesFromList();

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
    createDisplayList();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
