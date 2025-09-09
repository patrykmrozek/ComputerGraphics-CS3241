///////////////////////////////////
/// Name: Patryk Mrozek		   ///
/////////////////////////////////

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
//planets
#define MERCURY_RADIUS EARTH_RADIUS * 0.38
#define VENUS_RADIUS EARTH_RADIUS * 0.95
#define MARS_RADIUS EARTH_RADIUS * 0.53
#define JUPITER_RADIUS EARTH_RADIUS * 11.2
#define SATURN_RADIUS EARTH_RADIUS * 9.14
#define URANUS_RADIUS EARTH_RADIUS * 4.01
#define NEPTUNE_RADIUS EARTH_RADIUS * 3.88
#define SUN_RADIUS EARTH_RADIUS * 50.0 //scaled down by approx half
//moons
#define MOON_RADIUS EARTH_RADIUS * 0.20
#define IO_RADIUS EARTH_RADIUS * 0.29
#define EUROPA_RADIUS EARTH_RADIUS * 0.25
#define GANYMEDE_RADIUS EARTH_RADIUS * 0.41
#define CALLISTO_RADIUS EARTH_RADIUS * 0.38
#define TITAN_RADIUS EARTH_RADIUS * 0.40

#define CLOCK_RADIUS 30.0

//planets distances from sun relative to earth
#define EARTH_DIST 10.0
//planets
#define MERCURY_DIST EARTH_DIST * 0.67
#define VENUS_DIST EARTH_DIST * 0.83
#define MARS_DIST EARTH_DIST * 1.17
#define JUPITER_DIST EARTH_DIST * 1.56
#define SATURN_DIST EARTH_DIST * 2.20
#define URANUS_DIST EARTH_DIST * 2.60
#define NEPTUNE_DIST EARTH_DIST * 2.88
//moons
#define MOON_DIST EARTH_RADIUS * 3
#define IO_DIST JUPITER_RADIUS * 1.7
#define EUROPA_DIST JUPITER_RADIUS * 2.2
#define GANYMEDE_DIST JUPITER_RADIUS * 2.6
#define CALLISTO_DIST JUPITER_RADIUS * 3
#define TITAN_DIST SATURN_RADIUS * 3.2

//orbiting speed
#define EARTH_SPEED 0.01
//planets
#define MERCURY_SPEED EARTH_SPEED * 1.59
#define VENUS_SPEED EARTH_SPEED * 1.18
#define MARS_SPEED EARTH_SPEED * 0.81
#define JUPITER_SPEED EARTH_SPEED * 0.44
#define SATURN_SPEED EARTH_SPEED * 0.32
#define URANUS_SPEED EARTH_SPEED * 0.23
#define NEPTUNE_SPEED EARTH_SPEED * 0.1
//moons
#define MOON_SPEED EARTH_SPEED * 5
#define IO_SPEED JUPITER_SPEED * 8
#define EUROPA_SPEED JUPITER_SPEED * 6
#define GANYMEDE_SPEED JUPITER_SPEED * 4
#define CALLISTO_SPEED JUPITER_SPEED * 3
#define TITAN_SPEED SATURN_SPEED * 4

typedef struct {
    float x, y, z;
} Vec3;


//colors
//planets
#define MERCURY_COLOR (Vec3){1.0, 0.8, 0.0}
#define VENUS_COLOR (Vec3){1.0, 0.4, 0.1}
#define EARTH_COLOR (Vec3){0.2, 0.2, 1.0}
#define MARS_COLOR (Vec3){1.0, 0.1, 0.0}
#define JUPITER_COLOR (Vec3){0.6, 0.5, 0.0}
#define SATURN_COLOR (Vec3){0.8, 0.7, 0.0}
#define URANUS_COLOR (Vec3){0.3, 0.5, 1.0}
#define NEPTUNE_COLOR (Vec3){0.0, 0.0, 1.0}
//moons
#define MOON_COLOR (Vec3){0.8, 0.8, 0.8}
#define IO_COLOR (Vec3){1.0, 1.0, 0.6}
#define EUROPA_COLOR (Vec3){0.7, 0.9, 1.0}
#define GANYMEDE_COLOR (Vec3){0.6, 0.6, 0.8}
#define CALLISTO_COLOR (Vec3){0.4, 0.4, 0.4}
#define TITAN_COLOR (Vec3){0.9, 0.7, 0.4}

#define BG_COLOR (Vec3){0.0, 0.0, 0.1}

#define MAX_BODIES 25

typedef struct Body {
    Vec3 pos, color;
    float radius, o_speed, o_rad, o_angle; //orbiting_xxx
    struct Body* anchor;
    int depth;
    std::string tag;
    bool has_ring;
} Body;


int g_body_count = 0; //global bodies counter
bool camera_mode = false; //false/0=default, true/1=top-down

Body g_bodies[MAX_BODIES];

int current_focus_index = 0;
Vec3 current_focus;
float zoom_factor = 1.0;

bool wireframe_mode = false;

bool clock_mode = false;
time_t seconds = 0;
struct tm* timeinfo;
float timer = 0.1;
float clock_angle = 0.0;



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
    for (int i = 0; i < vertices.size(); i++) {
        glVertex2f(vertices[i].x, vertices[i].y);
        //std::cout << vertex.x << " - " << vertex.y << "\n";
    }
    glEnd();
}

void drawSphere(double r){
    //x = (r*sin(Phi)*cos(Theta))
    //y = (r*sin(Phi)*sin(Theta))
    //z = (r*cos(Phi))
    int n = 20;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 2*n; j++) {
            float phi1 = (i*M_PI)/n;
            float theta1 = j*M_PI/n;
            float theta2 = (j+1)*M_PI/n;
            float phi2 = (i+1)*M_PI/n;
            glBegin(GL_QUADS);
            glVertex3f(r*sin(phi1)*cos(theta1),
                       r*cos(phi1),
                       r*sin(phi1)*sin(theta1));

            glVertex3f(r*sin(phi1)*cos(theta2),
                       r*cos(phi1),
                       r*sin(phi1)*sin(theta2));

            glVertex3f(r*sin(phi2)*cos(theta2),
                       r*cos(phi2),
                       r*sin(phi2)*sin(theta2));

            glVertex3f(r*sin(phi2)*cos(theta1),
                       r*cos(phi2),
                       r*sin(phi2)*sin(theta1));

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

    glColor3f(body->color.x/2, body->color.y/2, body->color.z/2);
    if (body->has_ring) {
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        drawCircle(body->radius*2);
        glPopMatrix();
    }

    glPopMatrix();
}

void renderClock() {
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(
        glutGet(GLUT_WINDOW_WIDTH)-135.0,
        100.0,
        0.0);
    drawCircle(CLOCK_RADIUS);
    glColor3f(BG_COLOR.x, BG_COLOR.y, BG_COLOR.z);
    drawCircle(CLOCK_RADIUS-3.0);

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(CLOCK_RADIUS*cos(DEG_TO_RAD(-clock_angle)*0.8), //-clock_angle for clockwise
               CLOCK_RADIUS*sin(DEG_TO_RAD(-clock_angle)*0.8),
               0.0);
    glEnd();
    glPopMatrix();

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

    //moons
    Body* moon = createMoon(earth, MOON_COLOR, MOON_RADIUS, MOON_SPEED, MOON_DIST, "moon", false);
    Body* io = createMoon(jupiter, IO_COLOR, IO_RADIUS, IO_SPEED, IO_DIST, "io", false);
    Body* europa = createMoon(jupiter, EUROPA_COLOR, EUROPA_RADIUS, EUROPA_SPEED, EUROPA_DIST, "europa", false);
    Body* ganymede = createMoon(jupiter, GANYMEDE_COLOR, GANYMEDE_RADIUS, GANYMEDE_SPEED, GANYMEDE_DIST, "ganymede", false);
    Body* callisto = createMoon(jupiter, CALLISTO_COLOR, CALLISTO_RADIUS, CALLISTO_SPEED, CALLISTO_DIST, "callisto", false);
    Body* titan = createMoon(saturn, TITAN_COLOR, TITAN_RADIUS, TITAN_SPEED, TITAN_DIST, "titan", false);

}

void updateCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float camera_dist = g_bodies[current_focus_index].radius * 10 * zoom_factor;
    float min_dist = g_bodies[current_focus_index].radius*2;
    float max_dist = (100/min_dist) * g_bodies[current_focus_index].radius;
    if (camera_dist < min_dist) camera_dist = min_dist;
    if (camera_dist > max_dist) camera_dist = max_dist;


    //std::cout << "Camera Dist " << current_focus_index << ": " << camera_dist << "\n";
    //std::cout << "Max dist: " << max_dist << " - ";

    if (camera_mode == 0) {
        gluLookAt( current_focus.x, current_focus.y, current_focus.z + camera_dist,  // eye position
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

void renderUI() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1); //orthographic screen coords - text will be like an overlay

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    const char* curr_planet_tag = g_bodies[current_focus_index].tag.c_str(); //convert std::string to c_str()
    glColor3f(1.0, 1.0, 1.0);
    renderBitmapString(
        glutGet(GLUT_WINDOW_WIDTH)/2,
        glutGet(GLUT_WINDOW_HEIGHT)-50.0,
        GLUT_BITMAP_HELVETICA_18,
        curr_planet_tag);

    const char* curr_view_tag;
    if (!camera_mode) {
        curr_view_tag = "default view";
    } else {
        curr_view_tag = "top down view";
    }
    glColor3f(3.0, 0.0, 1.0);
    renderBitmapString(
        50.0f,
        50.0f,
        GLUT_BITMAP_HELVETICA_18,
        curr_view_tag);

    const char* clock_mode_tag;
    if (clock_mode) {
        clock_mode_tag = "clock mode: on";
        renderClock();
        glColor3f(0.0, 1.0, 0.0);
    } else {
        glColor3f(1.0, 0.0, 0.0);
        clock_mode_tag = "clock mode: off";
    }

    renderBitmapString(
        glutGet(GLUT_WINDOW_WIDTH)-200.0f,
        50.0f,
        GLUT_BITMAP_HELVETICA_18,
        clock_mode_tag);



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
    glClearColor (BG_COLOR.x, BG_COLOR.y, BG_COLOR.z, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    createSolarSystem();
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!wireframe_mode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    updateCamera();
    glPushMatrix();

    for (int i = 0; i < g_body_count; i++) {
        renderBody(&g_bodies[i]);
    }
    glPopMatrix();

    renderUI();
    glFlush ();
}

void idle() {
    /*
    alpha += 0.2;
    ty = sin(alpha * 0.005);
    tx = cos(alpha * 0.01);
*/

    if (!clock_mode) {
        for (int i = 0; i < g_body_count; i++) {
            updateBody(&g_bodies[i]);
        }

        if (current_focus_index < g_body_count) {
            current_focus = g_bodies[current_focus_index].pos;
        }
    } else {
        seconds = time(NULL);
        timeinfo = localtime(&seconds);

        for (int i = 0; i < g_body_count; i++) {
            updateBody(&g_bodies[i]);
        }
        if (current_focus_index < g_body_count) {
            current_focus = g_bodies[current_focus_index].pos;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        clock_angle++;
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
            if(zoom_factor>0.1) {
                zoom_factor-=0.1;
            }
            glutPostRedisplay();
            break;

        case 'r':
            if (zoom_factor < 100.0) {
                zoom_factor+=0.1;
            }
            glutPostRedisplay();
            break;

		case 't':
            clock_mode = !clock_mode;
            break;
        case 'L':
        case 'l':
            camera_mode = !camera_mode; //switches between 0 and 1
            reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            glutPostRedisplay();
            break;
        case 'f':
        case 'F':
            wireframe_mode = !wireframe_mode;
            glutPostRedisplay();
            break;

		default:
			break;
	}
}

int main(int argc, char **argv)
{
    std::cout<<"+++++CONTROL BUTTONS+++++++\n\n";
    std::cout<<"Toggle Time Mode: T\n";
    std::cout<<"Exit: ESC or q/Q\n";
    std::cout<<"Switch between planet views (1-8)\n";
    std::cout<<"Switch view to the sun (0)\n";
    std::cout<<"Zoom in: E - Zoom out: R\n";
    std::cout<<"Toggle top-down view: L\n";
    std::cout<<"Toggle wireframe mode: F\n";

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
