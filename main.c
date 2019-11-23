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

void handleKeyPress(SDL_Keysym *keysym) {
    switch (keysym->sym) {
        case SDLK_ESCAPE:
            Quit(0);
            break;
        default:
            break;
    }
    return;
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

float f [9][4][3] = {
    {   {-3.0f,  3.0f, 3.0f},     //A-TL
        {-1.0f,  3.0f, 3.0f},     //A-TR
        {-1.0f,  1.0f, 3.0f},     //A-BR
        {-3.0f,  1.0f, 3.0f} },   //A-BL
    {   {-1.0f,  3.0f, 3.0f},     //B-TL
        { 1.0f,  3.0f, 3.0f},     //B-TR
        { 1.0f,  1.0f, 3.0f},     //B-BR
        {-1.0f,  1.0f, 3.0f} },   //B-BL
    {   { 1.0f,  3.0f, 3.0f},     //C-TL
        { 3.0f,  3.0f, 3.0f},     //C-TR
        { 3.0f,  1.0f, 3.0f},     //C-BR
        { 1.0f,  1.0f, 3.0f} },   //C-BL
    {   {-3.0f,  1.0f, 3.0f},     //D-TL
        {-1.0f,  1.0f, 3.0f},     //D-TR
        {-1.0f, -1.0f, 3.0f},     //D-BR
        {-3.0f, -1.0f, 3.0f} },   //D-BL
    {   {-1.0f,  1.0f, 3.0f},     //E-TL
        { 1.0f,  1.0f, 3.0f},     //E-TR
        { 1.0f, -1.0f, 3.0f},     //E-BR
        {-1.0f, -1.0f, 3.0f} },   //E-BL
    {   { 1.0f,  1.0f, 3.0f},     //F-TL
        { 3.0f,  1.0f, 3.0f},     //F-TR
        { 3.0f, -1.0f, 3.0f},     //F-BR
        { 1.0f, -1.0f, 3.0f} },   //F-BL
    {   {-3.0f, -1.0f, 3.0f},     //G-TL
        {-1.0f, -1.0f, 3.0f},     //G-TR
        {-1.0f, -3.0f, 3.0f},     //G-BR
        {-3.0f, -3.0f, 3.0f} },   //G-BL
    {   {-1.0f, -1.0f, 3.0f},     //H-TL
        { 1.0f, -1.0f, 3.0f},     //H-TR
        { 1.0f, -3.0f, 3.0f},     //H-BR
        {-1.0f, -3.0f, 3.0f} },   //H-BL
    {   { 1.0f, -1.0f, 3.0f},     //I-TL
        { 3.0f, -1.0f, 3.0f},     //I-TR
        { 3.0f, -3.0f, 3.0f},     //I-BR
        { 1.0f, -3.0f, 3.0f} }    //I-BL
};

float c[9][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {0.5f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.5f}
};

//  1.00f, 0.65f, 0.00f     //orange    // FRONT
//  1.00f, 0.00f, 0.00f     //red       // BACK

//  1.00f, 1.00f, 1.00f     //white     // UP
//  1.00f, 1.00f, 0.00f     //yellow    // DOWN

//  0.00f, 0.00f, 1.00f     //blue      // LEFT
//  0.00f, 1.00f, 0.00f     //green     // RIGHT

int drawGLScene(GLvoid) {
    static GLfloat rquad;

    glLoadIdentity(); glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(rquad, 1.0f, 0.5f, 0.0f);    rquad -= 0.5f;
    glBegin(GL_QUADS);
    for (int i=0; i<9; i++) {
        glColor3f(1.00f, 0.65f, 0.00f);             for (int j=0; j<4; j++) glVertex3f(f[i][j][0], f[i][j][1],  3.0f); }
    for (int i=0; i<9; i++) {
        glColor3f(1.00f, 0.00f, 0.00f);             for (int j=0; j<4; j++) glVertex3f(f[i][j][0], f[i][j][1], -3.0f); }
    glEnd();        

//    glLoadIdentity();
//    glTranslatef(0.0f, 0.0f, -18.0f);
    glBegin(GL_QUADS);
    for (int i=0; i<9; i++) {
        glColor3f(1.00f, 1.00f, 1.00f);             for (int j=0; j<4; j++) glVertex3f(f[i][j][0], 3.0f, f[i][j][1]); }
    for (int i=0; i<9; i++) {
        glColor3f(1.00f, 1.00f, 0.00f);             for (int j=0; j<4; j++) glVertex3f(f[i][j][0], -3.0f, f[i][j][1]); }
    glEnd();        

//    glLoadIdentity();
//    glTranslatef(0.0f, 0.0f, -18.0f);
    glBegin(GL_QUADS);
    for (int i=0; i<9; i++) {
        glColor3f(0.00f, 1.00f, 0.00f);             for (int j=0; j<4; j++) glVertex3f(3.0, f[i][j][0], f[i][j][1]); }
    for (int i=0; i<9; i++) {
        glColor3f(0.00f, 0.00f, 1.00f);             for (int j=0; j<4; j++) glVertex3f(-3.0, f[i][j][0], f[i][j][1]); }
    glEnd();

    return (TRUE);
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
