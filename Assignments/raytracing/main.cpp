// CS3241Lab5.cpp
// PATRYK MROZEK
//
// How to run: (with provided Makefile)
// >make
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
    if (t_ca < 0) {return NULL;}; //behind camera
    //(P_ca = O+t_ca*D)
    Vector3 P_ca = r.origin + (r.dir * t_ca);
    double d_sq = L.lengthsqr() - (t_ca*t_ca);
    if (d_sq > (s.radius*s.radius)) {return DBL_MAX;}

    double t_h = sqrt((s.radius*s.radius) - d_sq);
    double t1, t2;
    t1 = t_ca - t_h;
    t2 = t_ca + t_h;
    return min(t1, t2);
}

void rayTrace(Ray ray, Color* c)
{
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

  Sphere S = gObjs[best_idx];
  Vector3 P = ray.origin + ray.dir * best_t;
  Vector3 N = P - S.center;
  N.normalize();

  Vector3 L = lightPos - P;
  L.normalize();
  double NdotL = max(0.0, dot_prod(N, L));
  double r = S.ambientR[0] * ambientL[0] + S.diffuseR[0] * diffuseL[0] * NdotL; 
  double g = S.ambientR[1] * ambientL[1] + S.diffuseR[1] * diffuseL[1] * NdotL;
  double b = S.ambientR[2] * ambientL[2] + S.diffuseR[2] * diffuseL[2] * NdotL;

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
    double r, g, b;
    
    cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
    long long int time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time
    
    ray.origin = cameraPos;
    
    Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
    Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
    Vector3 currPt;
    
    for(x=0;x<WINWIDTH;x++)
        for (y = 0; y < WINHEIGHT; y++)
        {
            currPt = startingPt + leftVector*x + upVector*y;
            ray.dir = currPt-cameraPos;
            ray.dir.normalize();
            Color c = (Color){r, g, b};
            rayTrace(ray, c);
            drawInPixelBuffer(x, y, r, g, b);
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


void setScene(int i = 0)
{
    if (i > NUM_SCENE)
    {
        cout << "Warning: Invalid Scene Number" << endl;
        return;
    }
    
    if (i == 0)
    {
        
        ((Sphere*)objList[0])->set(Vector3(-130, 80, 120), 100);
        ((Sphere*)objList[1])->set(Vector3(130, -80, -80), 100);
        ((Sphere*)objList[2])->set(Vector3(-130, -80, -80), 100);
        ((Sphere*)objList[3])->set(Vector3(130, 80, 120), 100);
        
        objList[0]->ambientR[0] = 0.1;
        objList[0]->ambientR[1] = 0.4;
        objList[0]->ambientR[2] = 0.4;
        objList[0]->diffuseR[0] = 0;
        objList[0]->diffuseR[1] = 1;
        objList[0]->diffuseR[2] = 1;
        objList[0]->specularR[0] = 0.2;
        objList[0]->specularR[1] = 0.4;
        objList[0]->specularR[2] = 0.4;
        objList[0]->speN = 300;
        
        objList[1]->ambientR[0] = 0.6;
        objList[1]->ambientR[1] = 0.6;
        objList[1]->ambientR[2] = 0.2;
        objList[1]->diffuseR[0] = 1;
        objList[1]->diffuseR[1] = 1;
        objList[1]->diffuseR[2] = 0;
        objList[1]->specularR[0] = 0.0;
        objList[1]->specularR[1] = 0.0;
        objList[1]->specularR[2] = 0.0;
        objList[1]->speN = 50;
        
        objList[2]->ambientR[0] = 0.1;
        objList[2]->ambientR[1] = 0.6;
        objList[2]->ambientR[2] = 0.1;
        objList[2]->diffuseR[0] = 0.1;
        objList[2]->diffuseR[1] = 1;
        objList[2]->diffuseR[2] = 0.1;
        objList[2]->specularR[0] = 0.3;
        objList[2]->specularR[1] = 0.7;
        objList[2]->specularR[2] = 0.3;
        objList[2]->speN = 650;
        
        objList[3]->ambientR[0] = 0.3;
        objList[3]->ambientR[1] = 0.3;
        objList[3]->ambientR[2] = 0.3;
        objList[3]->diffuseR[0] = 0.7;
        objList[3]->diffuseR[1] = 0.7;
        objList[3]->diffuseR[2] = 0.7;
        objList[3]->specularR[0] = 0.6;
        objList[3]->specularR[1] = 0.6;
        objList[3]->specularR[2] = 0.6;
        objList[3]->speN = 650;
        
    }
    
    if (i == 1)
    {
        
        // Step 5
        
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
    
    objList = new RtObject*[NUM_OBJECTS];
    
    // create four spheres
    objList[0] = new Sphere(Vector3(-130, 80, 120), 100);
    objList[1] = new Sphere(Vector3(130, -80, -80), 100);
    objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
    objList[3] = new Sphere(Vector3(130, 80, 120), 100);
    
    setScene(0);
    
    setScene(sceneNo);
    renderScene();
    
    glutMainLoop();
    
    for (int i = 0; i < NUM_OBJECTS; i++)
        delete objList[i];
    delete[] objList;
    
    delete[] pixelBuffer;
    
    return 0;
}
