// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

int g_body_count = 0; //global bodies counter

GLfloat GPI = (GLfloat)M_PI;
float alpha = 0.0, k=1.0;
float tx = 0.0, ty=0.0;

#define EARTH_RADIUS 0.1
//planets size relative to earth
#define MERCURY_RADIUS EARTH_RADIUS * 0.38
#define VENUS_RADIUS EARTH_RADIUS * 0.95
#define MARS_RADIUS EARTH_RADIUS * 0.53
#define JUPITER_RADIUS EARTH_RADIUS * 11.2
#define SATURN_RADIUS EARTH_RADIUS * 9.14
#define URANUS_RADIUS EARTH_RADIUS * 4.01
#define NEPTUNE_RADIUS EARTH_RADIUS * 3.88
#define SUN_RADIUS EARTH_RADIUS * 50.0

//distances from the sun (in au - relative to earth)
//reference: https://www.jpl.nasa.gov/_edu/pdfs/scaless_reference.pdf
#define MERCURY_DIST 12.0
#define VENUS_DIST 15.0
#define EARTH_DIST 18.0
#define MARS_DIST 21.0
#define JUPITER_DIST 28.0
#define SATURN_DIST 35.0
#define URANUS_DIST 42.0
#define NEPTUNE_DIST 50.0


typedef struct {
    float x, y, z;
} Vec3;

typedef struct Body {
    Vec3 pos, color;
    float size, o_speed, o_rad, o_angle; //orbiting_xxx
    struct Body* anchor;
    int depth;
} Body;


#define MAX_BODIES 100
Body g_bodies[MAX_BODIES];


void drawSphere(double r) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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


Body createBody(Vec3 pos, Vec3 color, float size, float o_speed,
                float o_rad, float o_angle, Body* anchor) {

    Body body = {
        .pos = pos,
        .color = color,
        .size = size,
        .o_speed = o_speed,
        .o_rad = o_rad,
        .o_angle = o_angle,
        .anchor = anchor,
        .depth = (anchor==nullptr) ? 0 : anchor->depth + 1
    };

    g_body_count++;

    return body;
}

void updateBody(Body* body) {
    if (body->anchor != nullptr) { //if its not at depth 0 (not orbting)
        body->o_angle += body->o_speed;

        //new pos based on anchor + orbit (x and z only)
        body->pos.x = body->anchor->pos.x + body->o_rad * cos(body->o_angle);
        body->pos.y = body->anchor->pos.y;
        body->pos.z = body->anchor->pos.z + body->o_rad * sin(body->o_angle);
    }
}

void renderBody(const Body* body) {
    glPushMatrix();

    glTranslatef(body->pos.x, body->pos.y, body->pos.z);
    glColor3f(body->color.x, body->color.y, body->color.z);

    drawSphere(body->size);

    glPopMatrix();
}

//wrapper functions
Body* createSun(Vec3 pos, Vec3 color, float size) {
    g_bodies[g_body_count] = createBody(pos, color, size, 0.0, 0.0, 0.0, NULL);
    return &g_bodies[g_body_count++];
}

Body* createPlanet(Body* sun, Vec3 color, float size,
                  float o_speed, float o_rad) {
    Vec3 pos = {sun->pos.x + o_rad, sun->pos.y, sun->pos.z};
    g_bodies[g_body_count] = createBody(pos, color, size, o_speed, o_rad, 0.0, sun);
    return &g_bodies[g_body_count++];
}

Body* createMoon(Body* planet, Vec3 color, float size,
                float o_speed, float o_rad) {
    Vec3 pos = {planet->pos.x + o_rad, planet->pos.y, planet->pos.z};
    g_bodies[g_body_count] = createBody(pos, color, size, o_speed, o_rad, 0.0, planet);
    return &g_bodies[g_body_count++];
}

void createSolarSystem() {
    Vec3 sun_pos = (Vec3){0.0, 0.0, 0.0};
    Vec3 sun_color = (Vec3){1.0, 1.0, 0.0};
    Body* sun = createSun(sun_pos, sun_color, SUN_RADIUS);

    //mercury - venus - earth - mars - jupiter - saturn - uranus - neptune
    Vec3 mercury_color = (Vec3){1.0, 1.0, 1.0};
    Body* mercury = createPlanet(sun, mercury_color, 0.2, 0.04, MERCURY_DIST);

    Vec3 earth_color = (Vec3){0.0, 0.0, 1.0};
    Body* earth = createPlanet(sun, earth_color, 0.5, 0.05, EARTH_DIST);

    /*
    Vec3 m1_color = (Vec3){0.9, 0.9, 0.9};
    Body* m1 = createMoon(earth, m1_color, 0.2, 0.1, 1.5);
*/
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w/h, 0.1, SUN_RADIUS*20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0, 3.0, SUN_RADIUS*5,  // eye position
              0.0, 0.0, 0.0,  // look at center
              0.0, 1.0, 0.0); // up vector
}

void init(void) {
    glClearColor (0.0, 0.0, 0.1, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    createSolarSystem();
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
    glRotatef(alpha, 0, 0, 1);

    //glTranslatef(tx, ty, 0);

    for (int i = 0; i < g_body_count; i++) {
        renderBody(&g_bodies[i]);
    }

	glPopMatrix();
	glFlush ();
}

void idle() {
    /*
    alpha += 0.2;
    ty = sin(alpha * 0.005);
    tx = cos(alpha * 0.01);
*/
    for (int i = 0; i < g_body_count; i++) {
        updateBody(&g_bodies[i]);
    }

    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

        case 27: // press ESC to exit
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
    glutInitWindowSize (800, 800);
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
