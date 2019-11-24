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

void rotate_front()
{
    for (int i=0; i<9; i++)
    {
        for (int j=0; j<4; j++)
            rotateZ(&f[0][i][j][0], 0.0174533f);
    }

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<4; j++)
        {
            rotateZ(&f[2][i][j][0], 0.0174533f);
            rotateZ(&f[3][i][j][0], 0.0174533f);
            rotateZ(&f[4][i][j][0], 0.0174533f);
            rotateZ(&f[5][i][j][0], 0.0174533f);
        }
    }
}


void rotate_up()
{
    for (int i=0; i<9; i++)
    {
        for (int j=0; j<4; j++)
            rotateY(&f[2][i][j][0], 0.0174533f);
    }

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<4; j++)
        {
            rotateY(&f[0][i][j][0], 0.0174533f);
            rotateY(&f[1][i][j][0], 0.0174533f);
            rotateY(&f[4][i*3+2][j][0], 0.0174533f);
            rotateY(&f[5][i*3+2][j][0], 0.0174533f);
        }
    }
}



int drawGLScene(GLvoid) {
    //static GLfloat rquad;

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(35.264f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    for (int i=0; i<6; i++)
    {
        glColor3f(c[i][0], c[i][1], c[i][2]);
        for (int j=0; j<9; j++)
        {
            for (int k=0; k<4; k++)
            {
                glVertex3f(f[i][j][k][0], f[i][j][k][1] , f[i][j][k][2]);
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
        case SDLK_u:
            rotate_up();
            break;
        case SDLK_f:
            rotate_front();
            break;
        default:
            break;
    }
    return;
}

int main(int argc, char **argv) {
    int done = FALSE;
    SDL_Event event;

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
