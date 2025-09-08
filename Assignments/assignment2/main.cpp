// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <time.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

#define DEG_TO_RAD(x) ((x) * (M_PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / M_PI))

//planets radius relative to earth
#define EARTH_RADIUS 0.1

#define MERCURY_RADIUS EARTH_RADIUS * 0.38
#define VENUS_RADIUS EARTH_RADIUS * 0.95
#define MARS_RADIUS EARTH_RADIUS * 0.53
#define JUPITER_RADIUS EARTH_RADIUS * 11.2
#define SATURN_RADIUS EARTH_RADIUS * 9.14
#define URANUS_RADIUS EARTH_RADIUS * 4.01
#define NEPTUNE_RADIUS EARTH_RADIUS * 3.88
#define SUN_RADIUS EARTH_RADIUS * 50.0 //scaled down by approx half

//planets distances from sun relative to earth
#define EARTH_DIST 10.0

#define MERCURY_DIST EARTH_DIST * 0.67
#define VENUS_DIST EARTH_DIST * 0.83
#define MARS_DIST EARTH_DIST * 1.17
#define JUPITER_DIST EARTH_DIST * 1.56
#define SATURN_DIST EARTH_DIST * 1.94
#define URANUS_DIST EARTH_DIST * 2.33
#define NEPTUNE_DIST EARTH_DIST * 2.78

//orbiting speed for each planet
#define EARTH_SPEED 0.01

#define MERCURY_SPEED EARTH_SPEED * 1.59
#define VENUS_SPEED EARTH_SPEED * 1.18
#define MARS_SPEED EARTH_SPEED * 0.81
#define JUPITER_SPEED EARTH_SPEED * 0.44
#define SATURN_SPEED EARTH_SPEED * 0.32
#define URANUS_SPEED EARTH_SPEED * 0.23
#define NEPTUNE_SPEED EARTH_SPEED * 0.18

typedef struct {
    float x, y, z;
} Vec3;


//colors
#define MERCURY_COLOR (Vec3){1.0, 0.8, 0.0}
#define VENUS_COLOR (Vec3){1.0, 0.4, 0.1}
#define EARTH_COLOR (Vec3){0.2, 0.2, 1.0}
#define MARS_COLOR (Vec3){1.0, 0.1, 0.0}
#define JUPITER_COLOR (Vec3){0.6, 0.5, 0.0}
#define SATURN_COLOR (Vec3){0.8, 0.7, 0.0}
#define URANUS_COLOR (Vec3){0.3, 0.5, 1.0}
#define NEPTUNE_COLOR (Vec3){0.0, 0.0, 1.0}


#define MAX_BODIES 20

typedef struct Body {
    Vec3 pos, color;
    float radius, o_speed, o_rad, o_angle; //orbiting_xxx
    struct Body* anchor;
    int depth;
    std::string tag;
    bool has_ring;
} Body;



int g_body_count = 0; //global bodies counter
int camera_mode = 0; //0=default, 1=top-down

Body g_bodies[MAX_BODIES];

int current_focus_index = 0;
Vec3 current_focus;
float k = 1.0;

void drawCircle(float radius) {
    std::vector<Vec3> vertices;
    float angle = 1.0;
    int triangle_count = 360.0 - 2; //n vertices = n-2 triangles - eg pentagon needs only 3 triangles
    std::vector<Vec3> temp;

    for (int i = 0; i < 360; i++) {
        float current_angle = angle * i;
        float x = radius * cos(DEG_TO_RAD(current_angle));
        float y = radius * sin(DEG_TO_RAD(current_angle));
        temp.push_back((Vec3){x, y, 0.0f});
    }

    //all triangles are generated from a common vertex - in this case temp[0]
    for (int i = 0; i < triangle_count; i++) {
        vertices.push_back(temp[0]);	    //the origin vertex
        vertices.push_back(temp[i + 1]);	//current vertex
        vertices.push_back(temp[i + 2]);	//next vertex
    }


    glBegin(GL_POLYGON);
    for (Vec3 vertex : vertices) {
        glVertex2f(vertex.x, vertex.y);
        //std::cout << vertex.x << " - " << vertex.y << "\n";
    }
    glEnd();
}

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


Body createBody(Vec3 pos, Vec3 color, float radius, float o_speed, float o_rad,
                float o_angle, Body* anchor, std::string tag, bool has_ring) {

    Body body = {
        .pos = pos,
        .color = color,
        .radius = radius,
        .o_speed = o_speed,
        .o_rad = o_rad,
        .o_angle = o_angle,
        .anchor = anchor,
        .depth = (anchor==nullptr) ? 0 : anchor->depth + 1,
        .tag = tag,
        .has_ring = has_ring
    };

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

    drawSphere(body->radius);

    glColor3f(1.0, 1.0, 1.0);
    if (body->has_ring) {
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        drawCircle(body->radius*2);
    }

    glPopMatrix();
}
//wrapper functions
Body* createSun(Vec3 pos, Vec3 color, float radius, std::string tag) {
    g_bodies[g_body_count] = createBody(pos, color, radius, 0.0, 0.0, 0.0, NULL, tag, false);
    return &g_bodies[g_body_count++];
}

Body* createPlanet(Body* sun, Vec3 color, float radius, float o_speed, float o_rad,
                   std::string tag, bool has_ring) {
    Vec3 pos = {sun->pos.x + o_rad, sun->pos.y, sun->pos.z};
    g_bodies[g_body_count] = createBody(pos, color, radius, o_speed, o_rad, 0.0, sun, tag, has_ring);
    return &g_bodies[g_body_count++];
}

Body* createMoon(Body* planet, Vec3 color, float radius, float o_speed, float o_rad,
                 std::string tag, bool has_ring) {
    Vec3 pos = {planet->pos.x + o_rad, planet->pos.y, planet->pos.z};
    g_bodies[g_body_count] = createBody(pos, color, radius, o_speed, o_rad, 0.0, planet, tag, false);
    return &g_bodies[g_body_count++];
}

void createSolarSystem() {
    Vec3 sun_pos = (Vec3){0.0, 0.0, 0.0};
    Vec3 sun_color = (Vec3){1.0, 1.0, 0.0};
    Body* sun = createSun(sun_pos, sun_color, SUN_RADIUS, "sun");

    //mercury - venus - earth - mars - jupiter - saturn - uranus - neptune
    Body* mercury = createPlanet(sun, MERCURY_COLOR, MERCURY_RADIUS, MERCURY_SPEED, MERCURY_DIST, "mercury", false);
    Body* venus = createPlanet(sun, VENUS_COLOR, VENUS_RADIUS, VENUS_SPEED, VENUS_DIST, "venus", false);
    Body* earth = createPlanet(sun, EARTH_COLOR, EARTH_RADIUS, EARTH_SPEED, EARTH_DIST, "earth", false);
    Body* mars = createPlanet(sun, MARS_COLOR, MARS_RADIUS, MARS_SPEED, MARS_DIST, "mars", false);
    Body* jupiter = createPlanet(sun, JUPITER_COLOR, JUPITER_RADIUS, JUPITER_SPEED, JUPITER_DIST, "jupiter", false);
    Body* saturn = createPlanet(sun, SATURN_COLOR, SATURN_RADIUS, SATURN_SPEED, SATURN_DIST, "saturn", true);
    Body* uranus = createPlanet(sun, URANUS_COLOR, URANUS_RADIUS, URANUS_SPEED, URANUS_DIST, "uranus", false);
    Body* neptune = createPlanet(sun, NEPTUNE_COLOR, NEPTUNE_RADIUS, NEPTUNE_SPEED, NEPTUNE_DIST, "neptune", false);


    /*
    Vec3 m1_color = (Vec3){0.9, 0.9, 0.9};
    Body* m1 = createMoon(earth, m1_color, 0.2, 0.1, 1.5);
*/
}

void updateCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float camera_dist = g_bodies[current_focus_index].radius * 10 * k;
    if (camera_dist < 10.0f) camera_dist = 10.0f;
    if (camera_dist > 50.0f) camera_dist = 50.0f;
    if (camera_mode == 0) {
        gluLookAt( current_focus.x, current_focus.y, current_focus.z+camera_dist,  // eye position
                  current_focus.x, current_focus.y, current_focus.z,  // look at center
                  0.0, 1.0, 0.0); // up vector
    } else {
        gluLookAt( current_focus.x,current_focus.y+camera_dist, current_focus.z,
                  current_focus.x, current_focus.y, current_focus.z,
                  0.0, 0.0, 1.0);
    }
}

void renderBitmapString(float x, float y, void* font, const char *string) {
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void renderText() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1); //orthographic screen coords - text will be like an overlay

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    const char* curr_planet_tag = g_bodies[current_focus_index].tag.c_str(); //convert std::string to c_str()

    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0, 1.0, 1.0);
    renderBitmapString(50.0f, 50.0f, GLUT_BITMAP_HELVETICA_18, curr_planet_tag);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w/h, 0.1, SUN_RADIUS*20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

    renderText();

    updateCamera();
    glPushMatrix();

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

    if (current_focus_index < g_body_count) {
        current_focus = g_bodies[current_focus_index].pos;
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
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            current_focus_index = key - '0'; // - '0' converts to ASCII
            if (current_focus_index < g_body_count) {
                current_focus = g_bodies[current_focus_index].pos;
                reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
                glutPostRedisplay();
            }
            break;

        case 'e':
            if(k>0.1)
                k-=0.1;
            glutPostRedisplay();
            break;

        case 'r':
            k+=0.1;
            glutPostRedisplay();
            break;

    /*
		case 't':
			clockMode = !clockMode;
			if (clockMode)
				cout << "Current Mode: Clock mode." << endl;
			else
				cout << "Current Mode: Solar mode." << endl;
			break;
*/
        case 'L':
        case 'l':
            camera_mode = (camera_mode+1)%2; //switches between 0 and 1
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
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
    glutInitWindowSize(800, 800);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);	
    //glutMouseFunc(mouse);

    for (int i = 0; i < g_body_count; i++) {
        std::cout << "BODY " << i << ": " << g_bodies[i].tag << "\n";
    }

	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
