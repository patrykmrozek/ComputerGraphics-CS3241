// CS3241Lab5.cpp
// PATRYK MROZEK
//
// How to run: (with provided Makefile)
// >make
// >./main
//
// OR
//
// >make run

#include <cfloat>
#include <iostream>
#include "vector3D.h"
#include <chrono>
#include <float.h>
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>

using namespace std;

#define WINWIDTH 600
#define WINHEIGHT 400
#define NUM_OBJECTS 4
#define MAX_RT_LEVEL 50
#define NUM_SCENE 2

float* pixelBuffer = new float[WINWIDTH * WINHEIGHT * 3];

typedef struct Ray {
  Vector3 origin, dir;
} Ray;

typedef struct Color {
  double r, g, b;
} Color;

struct Sphere {
    Vector3 center;
    double radius;
    Color col;
    double ambientR[3];
    double diffuseR[3];
    double specularR[3];
    double speN = 300;
};


static Sphere gObjs[NUM_OBJECTS];
static int gNumObjs = 0;


struct Light {
  Vector3 pos;
  Color I; //intensity
};


// Camera Settings
Vector3 cameraPos(0,0,-500);

// assume the the following two vectors are normalised
Vector3 lookAtDir(0,0,1);
Vector3 upVector(0,1,0);
Vector3 leftVector(1, 0, 0);
float focalLen = 500;

// Light Settings

Vector3 lightPos(900,1000,-1500);
double ambientL[3] = { 0.4,0.4,0.4 };
double diffuseL[3] = { 0.7,0.7, 0.7 };
double specularL[3] = { 0.5,0.5, 0.5 };

Color bgColor = { 0.1,0.1,0.4 };

int sceneNo = 0;

double intersect(Ray r, Sphere s)
{
  /*
   * Ray from O to C (L = C - O) dir to sphere center
   * Project L onto dir D = dist from O to C (t_ca = L*D)
   * if -t_ca, sphere behind (ignore) else its in front 
   * t_ca - how far along ray the center would be if ray was pointing directly at center
   * pythagoras to get how far from center (between points O, C and now P_ca (P_ca = O+t_ca*D))
   * d^2 = ||L||^2 - t_ca^2, which gives us d, dist from P_ca to C, and if d < r, ray cuts sphere
   * to find half dist between two points along the ray: t_h = sqrt(r^2 - d^2)
   * t1 = t_ca - t_h, t2 = t_ca + t_h
   * t = min(t1, t2)
   * final intersect point p = O + t*D
   */ 

    Vector3 L = s.center - r.origin;
    double t_ca = dot_prod(L, r.dir);
    //(P_ca = O+t_ca*D)
    Vector3 P_ca = r.origin + (r.dir * t_ca);
    double d_sq = L.lengthsqr() - (t_ca*t_ca);
    double r_sq = s.radius * s.radius;
    if (d_sq > (r_sq)) {return DBL_MAX;}

    double t_h = sqrt((r_sq) - d_sq);

    double t_min = 1e-4; //tiny num
    double t1, t2;
    t1 = t_ca - t_h;
    t2 = t_ca + t_h;
    if (t1 >= t_min) return t1;
    if (t2 >= t_min) return t2;
  return DBL_MAX;
}

Vector3 reflect(Vector3 incident, Vector3 normal)
{
  //incident ray, unit surface normal
  return incident - normal * (2.0 * dot_prod(incident, normal));
}

void rayTrace(Ray ray, Color* c, int depth)
{
  if (depth <= 0) {
    *c = bgColor;
    return;
  }

  double best_t = DBL_MAX;
  int best_idx = -1;

  for (int i = 0; i < gNumObjs; i++) {
    double t = intersect(ray, gObjs[i]);
    if (t < best_t) {
      best_t = t;
      best_idx = i;
    }
  }

  //miss
  if (best_idx < 0) {
    c->r = bgColor.r;
    c->g = bgColor.g;
    c->b = bgColor.b;
    return;
  }

  Sphere &S = gObjs[best_idx];
  Vector3 P = ray.origin + ray.dir * best_t;
  Vector3 N = P - S.center;
  N.normalize();

  Vector3 L = lightPos - P;
  L.normalize();
  double NdotL = max(0.0, dot_prod(N, L));

  //specular
  Vector3 V = cameraPos - P;
  V.normalize();
  Vector3 R = reflect(L * -1.0, N); //reflect incoming light ray about N
  
  double RdotV = max(0.0, dot_prod(R, V));
  double spec = pow(RdotV, max(1.0, S.speN)); //shininess exponent

  double r = S.ambientR[0] * ambientL[0] + S.diffuseR[0] * diffuseL[0] * NdotL + S.specularR[0] * specularL[0] * spec; 
  double g = S.ambientR[1] * ambientL[1] + S.diffuseR[1] * diffuseL[1] * NdotL + S.specularR[1] * specularL[1] * spec;
  double b = S.ambientR[2] * ambientL[2] + S.diffuseR[2] * diffuseL[2] * NdotL + S.specularR[2] * specularL[2] * spec;



  double kr = 0.3;
  if (kr > 0.0) {

    Color r_c;
    Vector3 new_o = ray.origin + (ray.dir * best_t); //point on ray at t
    Vector3 ref_dir = reflect(ray.dir, N);
    ref_dir.normalize();
    Ray ref_ray = (Ray){new_o, ref_dir};
    rayTrace(ref_ray, &r_c, depth-1);

    r = (1.0 - kr) * r + kr * r_c.r;
    g = (1.0 - kr) * g + kr * r_c.g;
    b = (1.0 - kr) * b + kr * r_c.b; 
  }

  c->r = min(1.0, max(0.0, r));
  c->g = min(1.0, max(0.0, g));
  c->b  = min(1.0, max(0.0, b));


}


void drawInPixelBuffer(int x, int y, double r, double g, double b)
{
    pixelBuffer[(y*WINWIDTH + x) * 3] = (float)r;
    pixelBuffer[(y*WINWIDTH + x) * 3 + 1] = (float)g;
    pixelBuffer[(y*WINWIDTH + x) * 3 + 2] = (float)b;
}

void renderScene()
{
    int x, y;
    Ray ray;
    
    cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
    long long int time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time
    
    ray.origin = cameraPos;
    
    Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
    Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
    Vector3 currPt;
    
    for(x=0;x<WINWIDTH;x++)
        for (y = 0; y < WINHEIGHT; y++)
        {
          currPt = startingPt + leftVector*(x+0.5) + upVector*(y+0.5); //pixel centers at +0.5
          ray.dir = currPt-cameraPos;
          ray.dir.normalize();
          Color c;
          rayTrace(ray, &c, MAX_RT_LEVEL); //pass by pointer for color
          drawInPixelBuffer(x, y, c.r, c.g, c.b);
        }
    
    long long int time2 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the ending time
    
    cout << "Done! \nRendering time = " << time2 - time1 << "ms" << endl << endl;
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_DOUBLEBUFFER);
    glDrawPixels(WINWIDTH, WINHEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
    glutSwapBuffers();
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


void set3(double a[3], double x, double y, double z)
{
  a[0] = x;
  a[1] = y;
  a[2] = z;
}

void addSphere(Vector3 c, double r,
               double a0, double a1, double a2,
               double d0, double d1, double d2,
               double s0, double s1, double s2,
               double shininess)
{
  if (gNumObjs >= NUM_OBJECTS) {
    return;
  }
  Sphere &S = gObjs[gNumObjs++];
  S.center = c;
  S.radius = r;
  set3(S.ambientR, a0, a1, a2);
  set3(S.diffuseR, d0, d1, d2);
  set3(S.specularR, s0, s1, s2);
  S.speN = shininess;
}

void setScene(int i = 0)
{
    if (i >= NUM_SCENE)
    {
        cout << "Warning: Invalid Scene Number" << endl;
        return;
    }
    
      gObjs[i].ambientR[0]=0.1; gObjs[i].ambientR[1]=0.4; gObjs[i].ambientR[2]=0.4;
      gObjs[i].diffuseR[0]=0.0; gObjs[i].diffuseR[1]=1.0; gObjs[i].diffuseR[2]=1.0;
      gObjs[i].specularR[0]=0.2; gObjs[i].specularR[1]=0.4; gObjs[i].specularR[2]=0.4; gObjs[i].speN=300;
    
    if (i == 1)
    {

        
    }
}

void keyboard (unsigned char key, int x, int y)
{
    //keys to control scaling - k
    //keys to control rotation - alpha
    //keys to control translation - tx, ty
    switch (key) {
        case 's':
        case 'S':
            sceneNo = (sceneNo + 1 ) % NUM_SCENE;
            setScene(sceneNo);
            cout << "Scene No: " << sceneNo << "\n";
            renderScene();
            glutPostRedisplay();
            break;
        case 'q':
        case 'Q':
            exit(0);
            
        default:
            break;
    }
}

int main(int argc, char **argv)
{
    
    
    cout<<"<<CS3241 Lab 5>>\n\n"<<endl;
    cout << "S to go to next scene" << endl;
    cout << "Q to quit"<<endl;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (WINWIDTH, WINHEIGHT);
    
    glutCreateWindow ("CS3241 Lab 5: Ray Tracing");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutKeyboardFunc(keyboard);
    setScene(0);
    
    setScene(sceneNo);
    
    gNumObjs = 4;

    gObjs[0].center = Vector3(-130,  80, 120); gObjs[0].radius = 100;
    gObjs[1].center = Vector3( 130, -80, -80); gObjs[1].radius = 100;
    gObjs[2].center = Vector3(-130, -80, -80); gObjs[2].radius = 100;
    gObjs[3].center = Vector3( 130,  80, 120); gObjs[3].radius = 100;

    // Copy the material arrays you already set in setScene(0)
    gObjs[0].ambientR[0]=0.1; gObjs[0].ambientR[1]=0.4; gObjs[0].ambientR[2]=0.4;
    gObjs[0].diffuseR[0]=0.0; gObjs[0].diffuseR[1]=1.0; gObjs[0].diffuseR[2]=1.0;
    gObjs[0].specularR[0]=0.2; gObjs[0].specularR[1]=0.4; gObjs[0].specularR[2]=0.4; gObjs[0].speN=300;

    gObjs[1].ambientR[0]=0.6; gObjs[1].ambientR[1]=0.6; gObjs[1].ambientR[2]=0.2;
    gObjs[1].diffuseR[0]=1.0; gObjs[1].diffuseR[1]=1.0; gObjs[1].diffuseR[2]=0.0;
    gObjs[1].specularR[0]=0.0; gObjs[1].specularR[1]=0.0; gObjs[1].specularR[2]=0.0; gObjs[1].speN=50;

    gObjs[2].ambientR[0]=0.1; gObjs[2].ambientR[1]=0.6; gObjs[2].ambientR[2]=0.1;
    gObjs[2].diffuseR[0]=0.1; gObjs[2].diffuseR[1]=1.0; gObjs[2].diffuseR[2]=0.1;
    gObjs[2].specularR[0]=0.3; gObjs[2].specularR[1]=0.7; gObjs[2].specularR[2]=0.3; gObjs[2].speN=650;

    gObjs[3].ambientR[0]=0.3; gObjs[3].ambientR[1]=0.3; gObjs[3].ambientR[2]=0.3;
    gObjs[3].diffuseR[0]=0.7; gObjs[3].diffuseR[1]=0.7; gObjs[3].diffuseR[2]=0.7;
    gObjs[3].specularR[0]=0.6; gObjs[3].specularR[1]=0.6; gObjs[3].specularR[2]=0.6; gObjs[3].speN=650;
      
    renderScene();
    
    glutMainLoop();
   
    delete[] pixelBuffer;
    
    return 0;
}
