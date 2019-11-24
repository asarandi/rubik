#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

#define TRUE  1
#define FALSE 0

extern float f[6][9][4][3];
extern float c[6][3];

SDL_GLContext   *context;
SDL_Window      *window;

void Quit(int returnCode) {
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)        
        SDL_DestroyWindow(window);
    SDL_Quit();
    exit(returnCode);
}

int initGL(GLvoid) {
    GLfloat ratio;

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glViewport(0, 0, (GLsizei) SCREEN_WIDTH, (GLsizei) SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ratio = (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT;
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return (TRUE);
}

void rotateX(float *xyz, float angle)
{
    float y, z;
    
    y = xyz[1];
    z = xyz[2];
    xyz[1] = (y * cosf(angle)) + (z * -sinf(angle));
    xyz[2] = (y * sinf(angle)) + (z * cosf(angle));
}

void rotateY(float *xyz, float angle)
{
    float x, z;
    
    x = xyz[0];
    z = xyz[2];
    xyz[0] = (x * cosf(angle)) + (z * sinf(angle));
    xyz[2] = (x * -sinf(angle)) + (z * cosf(angle));
}

void rotateZ(float *xyz, float angle)
{
    float x, y;
    
    x = xyz[0];
    y = xyz[1];
    xyz[0] = (x * cosf(angle)) + (y * -sinf(angle));
    xyz[1] = (x * sinf(angle)) + (y * cosf(angle));
}

typedef float t_cube[6][4][3];

t_cube cubes[27];

int front[9] = { 6, 15, 24,  3, 12, 21,  0,  9, 18};
int  back[9] = {26, 17,  8, 23, 14,  5, 20, 19,  2};
int    up[9] = { 8, 17, 26,  7, 16, 25,  6, 15, 24};
int  down[9] = { 0,  9, 18,  1, 10, 19,  2, 11, 20};
int right[9] = {24, 25, 26, 21, 22, 23, 18, 19, 20};
int  left[9] = { 8,  7,  6,  5,  4,  3,  2,  1,  0};

void rotate_single_cube(int neg)
{
    float angle = 0.0174533;

    int i = 18;

    for (int j=0; j<6; j++)
    {
        for (int k=0; k<4; k++)
        {
            rotateX(&cubes[i][j][k][0], neg ? -angle : angle);
            rotateY(&cubes[i][j][k][0], neg ? -angle : angle);
            rotateZ(&cubes[i][j][k][0], neg ? -angle : angle);
        }
    }
}

t_cube *make_cube(float x, float y, float z)
{
    static t_cube  t;
    static float u = 2.0f;

    t[0][0][0] = x;    t[0][0][1] = y;    t[0][0][2] = z;   //A
    t[0][1][0] = x;    t[0][1][1] = y+u;  t[0][1][2] = z;   //B    
    t[0][2][0] = x+u;  t[0][2][1] = y+u;  t[0][2][2] = z;   //C
    t[0][3][0] = x+u;  t[0][3][1] = y;    t[0][3][2] = z;   //D

    t[1][0][0] = x;    t[1][0][1] = y;    t[1][0][2] = z-u; //E
    t[1][1][0] = x;    t[1][1][1] = y+u;  t[1][1][2] = z-u; //F
    t[1][2][0] = x+u;  t[1][2][1] = y+u;  t[1][2][2] = z-u; //G
    t[1][3][0] = x+u;  t[1][3][1] = y;    t[1][3][2] = z-u; //H

    t[2][0][0] = x;    t[2][0][1] = y+u;  t[2][0][2] = z;   //B    
    t[2][1][0] = x;    t[2][1][1] = y+u;  t[2][1][2] = z-u; //F
    t[2][2][0] = x+u;  t[2][2][1] = y+u;  t[2][2][2] = z-u; //G
    t[2][3][0] = x+u;  t[2][3][1] = y+u;  t[2][3][2] = z;   //C

    t[3][0][0] = x;    t[3][0][1] = y;    t[3][0][2] = z;   //A
    t[3][1][0] = x;    t[3][1][1] = y;    t[3][1][2] = z-u; //E
    t[3][2][0] = x+u;  t[3][2][1] = y;    t[3][2][2] = z-u; //H
    t[3][3][0] = x+u;  t[3][3][1] = y;    t[3][3][2] = z;   //D

    t[4][0][0] = x+u;  t[4][0][1] = y;    t[4][0][2] = z;   //D
    t[4][1][0] = x+u;  t[4][1][1] = y+u;  t[4][1][2] = z;   //C
    t[4][2][0] = x+u;  t[4][2][1] = y+u;  t[4][2][2] = z-u; //G
    t[4][3][0] = x+u;  t[4][3][1] = y;    t[4][3][2] = z-u; //H

    t[5][0][0] = x;    t[5][0][1] = y;    t[5][0][2] = z;   //A
    t[5][1][0] = x;    t[5][1][1] = y+u;  t[5][1][2] = z;   //B    
    t[5][2][0] = x;    t[5][2][1] = y+u;  t[5][2][2] = z-u; //F
    t[5][3][0] = x;    t[5][3][1] = y;    t[5][3][2] = z-u; //E
    return &t;
}

float angle = 0.0174533f * 5;

void rotate_down(int neg)
{
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
            {
                rotateY(&cubes[0*9+i][j][k][0], neg ? -angle : angle);
                rotateY(&cubes[1*9+i][j][k][0], neg ? -angle : angle);
                rotateY(&cubes[2*9+i][j][k][0], neg ? -angle : angle);
            }
        }
    }
}

void rotate_front(int neg)
{
    for (int idx=0; idx<9; idx++)
    {
        int i=front[idx];
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
                rotateZ(cubes[i][j][k], neg ? -angle : angle);
        }
    }
}


void init_cubes()
{
    int idx = 0;
    t_cube *t;
    for (int i=-3; i<3; i+=2)
    {
        for (int j=-3; j<3; j+=2)
        {
            for (int k=3; k>-3; k-=2)
            {
                t = make_cube((float)i, (float)j, (float)k);
                memcpy(cubes[idx++], t, sizeof(*t));
            }
        }
    }
}

int drawGLScene(GLvoid) {
    //static GLfloat rquad;

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(35.264f, 1.0f, 0.0f, 0.0f);
    glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);

    for (int i=0; i<27; i++)
    {
        for (int j=0; j<6; j++)
        {
            glColor3f(c[j][0], c[j][1], c[j][2]);
            for (int k=0; k<4; k++)
            {
                glVertex3f(cubes[i][j][k][0], cubes[i][j][k][1], cubes[i][j][k][2]);
            }
        }
    }
    glEnd();
    return (TRUE);
}

void handleKeyPress(SDL_Keysym *keysym) {
    switch (keysym->sym) {
        case SDLK_ESCAPE:
            Quit(0);
            break;
//        case SDLK_u:
//            rotate_up();
//            break;
        case SDLK_a:
            rotate_single_cube(0);
            break;
        case SDLK_z:
            rotate_single_cube(1);
            break;
        case SDLK_f:
            rotate_front(0);
            break;
        case SDLK_v:
            rotate_front(1);
            break;
        case SDLK_d:
            rotate_down(0);
            break;
        case SDLK_c:
            rotate_down(1);
            break;
        default:
            break;
    }
    return;
}

int main(int argc, char **argv) {
    int done = FALSE;
    SDL_Event event;
    init_cubes();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        Quit(1);
    }

    if (!(window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL)))
    {
        fprintf(stderr, "CreateWindow() failed: %s\n", SDL_GetError());
        Quit(1);
    }

    if (!(context = SDL_GL_CreateContext(window)))
    {
        fprintf(stderr, "SDL_GL_CreateContext() failed: %s\n", SDL_GetError());
        Quit(1);
    }

    initGL();
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    handleKeyPress(&event.key.keysym);
                    break;
                case SDL_QUIT:
                    done = TRUE;
                    break;
                default:
                    break;
            }
        }
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGLScene();
        SDL_GL_SwapWindow(window);
    }
    Quit(0);
    return (0);
}
