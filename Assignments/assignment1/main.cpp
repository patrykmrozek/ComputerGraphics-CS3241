//////////////////////////////////////////////////////////////
/// Name: Patryk Mrozek					   				  ///
/// Functions: glNewList() - glEndList() - glCallList(); ///
///////////////////////////////////////////////////////////

// CS3241 Assignment 1: Doodle
#include <cmath>
#include <vector>
#include <iostream>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

#define DEG_TO_RAD(x) ((x) * (M_PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / M_PI))
#define CLAMP(x, y) (if (x >= y) x -= x))

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

#define NOSE_RADIUS 0.55f
#define NOSE_Y_OFFSET -3.7
#define NOSE_X_SCALE_FACTOR 1.2
#define NOSE_Y_SCALE_FACTOR 0.8

#define MOUTH_RADIUS_OUTLINE 4.5f
#define MOUTH_RADIUS_FILL 4.2f
#define MOUTH_SCALE_X_FACTOR 0.75f
#define MOUTH_SCALE_Y_FACTOR 0.9f
#define MOUTH_Y_OFFSET -5.0f

#define TEETH_MOUTH_RATIO 0.8
#define TEETH_RADIUS_OUTLINE (MOUTH_RADIUS_OUTLINE*TEETH_MOUTH_RATIO)
#define TEETH_RADIUS_FILL (MOUTH_RADIUS_FILL*TEETH_MOUTH_RATIO)
#define TEETH_SCALE_X_FACTOR (MOUTH_SCALE_X_FACTOR*1.3165f)
#define TEETH_SCALE_Y_FACTOR (MOUTH_SCALE_Y_FACTOR/1.5f)
#define TEETH_Y_OFFSET_MIDDLE (MOUTH_Y_OFFSET*0.75f)
#define TEETH_Y_OFFSET_BOTTOM (MOUTH_Y_OFFSET*0.975f)
#define TEETH_START_ANGLE 200.0f
#define TEETH_END_ANGLE 340.0f

#define EYE_RADIUS 1.4f
#define EYE_Y_OFFSET -1.5f
#define EYE_X_SPACING 3.78f

#define BRIM_LENGTH_OUTLINE 7.0f
#define BRIM_RADIUS_OUTLINE 0.5f
#define BRIM_LENGTH_FILL 6.9f
#define BRIM_RADIUS_FILL 0.35f
#define BRIM_Y_OFFSET 1.0f

#define HAT_RADIUS_OUTLINE HEAD_RADIUS_OUTLINE
#define HAT_RADIUS_FILL HEAD_RADIUS_FILL

#define CIRCLE_NUM_VERTICES 360
#define CIRCLE_ANGLE_FULL 360.0f
#define CIRCLE_ANGLE_HALF 180.0f

#define RIBBON_NUM_VECTORS 100
#define RIBBON_OUTLINE_RADIUS 1.35f
#define RIBBON_FILL_RADIUS 1.2f

typedef struct {
    float x, y, z;
} vec3;

#define HAT_COLOR (vec3){1.0, 0.8, 0.0}
#define RIBBON_COLOR (vec3){0.9, 0.0, 0.2}

GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1;
float tx = 0.0, ty=1;


//display list - a pre compiled list of OpenGL commands stored on the GPU
GLuint head_list_fill, head_list_outline, mouth_list_outline, mouth_list_fill, eye_list,
    bone_list_outline, bone_list_fill, brim_list_outline, brim_list_fill,hat_list_outline, hat_list_fill,
    teeth_bottom_list_outline, teeth_bottom_list_fill, teeth_middle_list_outline, teeth_middle_list_fill,
    nose_list, ribbon_list_outline, ribbon_list_fill;


  /////////////////////////////////////
 /*			DRAW FUNCTIONS			*/
/////////////////////////////////////

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

void drawCircleSlice(float radius, float start_angle, float end_angle) {
    //basically a rewrite of drawCircle but it draws a slice between 2 angles: 0-360=full - 0-180=half...
    std::vector<vec3> vertices;
    float vertex_count = CIRCLE_NUM_VERTICES;
    float angle_range = end_angle - start_angle;

    for (int i = 0; i <= vertex_count; i++) {
        float current_angle = start_angle + (angle_range * i / vertex_count);
        float x = radius * cos(DEG_TO_RAD(current_angle));
        float y = radius * sin(DEG_TO_RAD(current_angle));
        vertices.push_back({x, y, 0.0f});
    }

    glBegin(GL_POLYGON);
    for (vec3 vertex : vertices) {
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();

}

void drawHead(float head_radius) {
    drawCircle(head_radius, CIRCLE_NUM_VERTICES);
}

void drawNose(float nose_radius) {
    glPushMatrix();
    glTranslatef(0.0, NOSE_Y_OFFSET, 0.0);
    glScalef(NOSE_X_SCALE_FACTOR, NOSE_Y_SCALE_FACTOR, 1.0);
    drawCircle(nose_radius, CIRCLE_NUM_VERTICES/2);
    glPopMatrix();
}

void drawMouth(float mouth_radius) {
    //chin
    glPushMatrix();
    glTranslatef(0.0, MOUTH_Y_OFFSET, 0.0);
    glScalef(MOUTH_SCALE_X_FACTOR, MOUTH_SCALE_Y_FACTOR, 0.0);
    drawCircle(mouth_radius, CIRCLE_NUM_VERTICES);
   glPopMatrix();
}

void drawTeeth(float teeth_radius, float teeth_y_offset) {
    //bottom
    glPushMatrix();
    glTranslatef(0.0, teeth_y_offset, 0.0);
    glScalef(TEETH_SCALE_X_FACTOR, TEETH_SCALE_Y_FACTOR, 0.0);
    drawCircleSlice(teeth_radius, TEETH_START_ANGLE, TEETH_END_ANGLE);
    glPopMatrix();
/*
    //middle
    glPushMatrix();
    glTranslatef(0.0, TEETH_Y_OFFSET_MIDDLE, 0.0);
    glScalef(TEETH_SCALE_X_FACTOR, TEETH_SCALE_Y_FACTOR, 0.0);
    drawCircleSlice(teeth_radius, 200.0, 340.0);
    glPopMatrix();
*/
}

void drawTooth(float tooth_width, float tooth_height, float y_offset, float tooth_angle) {
    glPushMatrix();
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        float current_angle = (-tooth_angle + (tooth_angle * i));//one tooth to the left - one in the middle - one on the right
        glRotatef(current_angle, 0.0, 0.0, 1.0);
        glTranslatef(0.0, y_offset, 0.0);
        glBegin(GL_POLYGON);
        glVertex2f(-tooth_width, 0.0);
        glVertex2f(tooth_width, 0.0);
        glVertex2f(tooth_width, -tooth_height);
        glVertex2f(-tooth_width, -tooth_height);
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();
}

void drawEye() {
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
    glPushMatrix();
    glTranslatef(0, BRIM_Y_OFFSET, 0);
    drawCircleSlice(hat_radius, start_angle, end_angle);
    glPopMatrix();
}

//essentially another copy of drawCircle, but Im only going to look at the first few vertices and the last few
//and skip the middle section of vertices - so I can then use accurate coordinates following a curve
//then use these vertices to create the 'ribbon' by joining the first batch with the second
void drawRibbon(float hat_radius, float ribbon_height, float vertex_count) {
    std::vector<vec3> vertices;
/*
    float start_angle_front = 0.0f;
    float end_angle_front = 20.0f;
    float start_angle_back = 160.0f;
    float end_angle_back = 180.0f;
*/
    for (int i = 0; i < vertex_count; i++) {
        float current_angle = 10.0f + (10.0f * i / vertex_count); //first 10 deg (0 - 10)
        float x = hat_radius * cos(DEG_TO_RAD(current_angle));
        float y = hat_radius * sin(DEG_TO_RAD(current_angle));
        vertices.push_back({x, y, 0.0f});
    }

    //reversed the loop so that when drawing, they connect at the same height
    for (int i = vertex_count-1; i >= 0; i--) {
        float current_angle = 160.0f + (10.0f * i / vertex_count); //last 10 deg (170 - 180)
        float x = hat_radius * cos(DEG_TO_RAD(current_angle));
        float y = hat_radius * sin(DEG_TO_RAD(current_angle));
        vertices.push_back({x, y, 0.0f});
    }

    glPushMatrix();
    glTranslatef(0, BRIM_Y_OFFSET*1.2f, 0);
    glScalef(1.01, 1.0, 1.0);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < vertices.size(); i++) {
        glVertex2f(vertices[i].x, vertices[i].y - 1.4f);
        glVertex2f(vertices[i].x, vertices[i].y - 1.4f + ribbon_height);
    }

    //connect back to start
   glVertex2f(vertices[0].x, vertices[0].y - 1.4f);

   glVertex2f(vertices[0].x, vertices[0].y - 1.4f + ribbon_height);

    glEnd();
    glPopMatrix();
}


  /////////////////////////////////////
 /*			DISPLAY LIST			*/
/////////////////////////////////////


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

    //nose
    nose_list = glGenLists(1);
    glNewList(nose_list, GL_COMPILE);
    drawNose(NOSE_RADIUS);
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

    //teeth
    teeth_bottom_list_outline = glGenLists(1);
    glNewList(teeth_bottom_list_outline, GL_COMPILE);
    drawTeeth(TEETH_RADIUS_OUTLINE, TEETH_Y_OFFSET_BOTTOM);
    glEndList();

    teeth_bottom_list_fill = glGenLists(1);
    glNewList(teeth_bottom_list_fill, GL_COMPILE);
    drawTeeth(TEETH_RADIUS_FILL, TEETH_Y_OFFSET_BOTTOM);
    glEndList();

    teeth_middle_list_outline = glGenLists(1);
    glNewList(teeth_middle_list_outline, GL_COMPILE);
    drawTeeth(TEETH_RADIUS_OUTLINE, TEETH_Y_OFFSET_MIDDLE);
    glEndList();

    teeth_middle_list_fill = glGenLists(1);
    glNewList(teeth_middle_list_fill, GL_COMPILE);
    drawTeeth(TEETH_RADIUS_FILL, TEETH_Y_OFFSET_MIDDLE);
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
    glEndList();

    hat_list_fill = glGenLists(1);
    glNewList(hat_list_fill, GL_COMPILE);
    drawHat(HEAD_RADIUS_FILL, 0, 180);
    glEndList();

    //ribbon
    ribbon_list_outline = glGenLists(1);
    glNewList(ribbon_list_outline, GL_COMPILE);
    drawRibbon(HEAD_RADIUS_FILL, RIBBON_OUTLINE_RADIUS, RIBBON_NUM_VECTORS);
    glEndList();

    ribbon_list_fill = glGenLists(1);
    glNewList(ribbon_list_fill, GL_COMPILE);
    drawRibbon(HEAD_RADIUS_FILL, RIBBON_FILL_RADIUS, RIBBON_NUM_VECTORS);
    glEndList();

}

  /////////////////////////////////////////
 /*			FromList Functions			*/
/////////////////////////////////////////

void drawHeadOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(head_list_outline);
}

void drawHeadFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(head_list_fill);
}

void drawNoseFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(nose_list);
}

void drawMouthOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(mouth_list_outline);
}

void drawMouthFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(mouth_list_fill);
}


void drawTeethBottomOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(teeth_bottom_list_outline);
}

void drawTeethBottomFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(teeth_bottom_list_fill);
}

void drawTeethMiddleOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(teeth_middle_list_outline);
}

void drawTeethMiddleFillFromList() {
    glColor3f(1.0, 1.0, 1.0);
    glCallList(teeth_middle_list_fill);
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
    glColor3f(HAT_COLOR.x, HAT_COLOR.y, 0.0);
    glCallList(brim_list_fill);
}

void drawHatOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(hat_list_outline);
}

void drawHatFillFromList() {
    glColor3f(HAT_COLOR.x, HAT_COLOR.y, 0.0);
    glCallList(hat_list_fill);
}

void drawRibbonOutlineFromList() {
    glColor3f(0.0, 0.0, 0.0);
    glCallList(ribbon_list_outline);
}

void drawRibbonFillFromList() {
    glColor3f(RIBBON_COLOR.x, RIBBON_COLOR.y, RIBBON_COLOR.z);
    glCallList(ribbon_list_fill);
}


  /////////////////////////////
 /*			DISPLAY			*/
/////////////////////////////

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

    //teeth
    drawTeethBottomOutlineFromList();
    drawTeethBottomFillFromList();
    drawTeethMiddleOutlineFromList();
    drawTeethMiddleFillFromList();



    //head
    drawHeadOutlineFromList();
    drawHeadFillFromList();

    //hat
    drawHatOutlineFromList();
    drawHatFillFromList();

    //ribbon
    drawRibbonOutlineFromList();
    drawRibbonFillFromList();

    //brim
    drawBrimOutlineFromList();
    drawBrimFillFromList();

    drawEyesFromList();
    drawNoseFromList();

    glColor3f(0.0, 0.0, 0.0);
    drawTooth(0.1, 2, MOUTH_Y_OFFSET, 13.5);

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
