#include "visualizer.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

extern int faces[6][9];

void update_front();
void update_back();
void update_up();
void update_down();
void update_right();
void update_left();

extern float c[6][3];

enum moves
{
    F,
    F_PRIME,
    F_2,
    B,
    B_PRIME,
    B_2,
    U,
    U_PRIME,
    U_2,
    D,
    D_PRIME,
    D_2,
    R,
    R_PRIME,
    R_2,
    L,
    L_PRIME,
    L_2,

} move;

t_queue         *q;

SDL_GLContext   *context;
SDL_Window      *window;

void Quit(int returnCode) {
    queue_destroy(q);
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
//    glEnable(GL_POLYGON_OFFSET_FILL);
//    glPolygonOffset(1.0f, 1.0f);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glViewport(0, 0, (GLsizei) SCREEN_WIDTH, (GLsizei) SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ratio = (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT;
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return (1);
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


/*

clockwise
0 1 2      6 3 0
3 4 5  =>  7 4 1
6 7 8      8 5 2

anti-clockwise
0 1 2      2 5 8
3 4 5  =>  1 4 7
6 7 8      0 3 6

twice
0 1 2      8 7 6
3 4 5  =>  5 4 3
6 7 8      2 1 0

*/



void rotate_ints(int *face, int k)
{
    int rot[3][9] = {
        {6, 3, 0, 7, 4, 1, 8, 5, 2},
        {2, 5, 8, 1, 4, 7, 0, 3, 6},
        {8, 7, 6, 5, 4, 3, 2, 1, 0}
        };

    int copy[9], i;

    for (i=0; i<9; i++)
        copy[i] = face[i];
    for (i=0; i<9; i++)
        face[i] = copy[rot[k][i]];
}



void rotate_Z(int f, float angle)   //F, B
{
    for (int idx=0; idx<9; idx++)
    {
        int i=faces[f][idx];
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
                rotateZ(cubes[i][j][k], angle);
        }
    }
}

void rotate_Y(int f, float angle)   //U, D
{
    for (int idx=0; idx<9; idx++)
    {
        int i=faces[f][idx];
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
                rotateY(cubes[i][j][k], angle);
        }
    }
}

void rotate_X(int f, float angle)   //R, L
{
    for (int idx=0; idx<9; idx++)
    {
        int i=faces[f][idx];
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
                rotateX(cubes[i][j][k], angle);
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

float perspective_x = 35.264f;
float perspective_y = -45.0f;

int drawGLScene(GLvoid) {
    //static GLfloat rquad;

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(perspective_x, 1.0f, 0.0f, 0.0f);
    glRotatef(perspective_y, 0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    for (int i=0; i<27; i++)
    {
        for (int j=0; j<6; j++)
        {
            glColor3f(c[j][0], c[j][1], c[j][2]);
            for (int k=0; k<4; k++)
                glVertex3f(cubes[i][j][k][0], cubes[i][j][k][1], cubes[i][j][k][2]);
        }
    }
    glEnd();

    glLineWidth(10.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    int t[8] = {0,1,2,3,0,3,1,2};
    for (int i=0; i<27; i++)
    {
        for (int j=0; j<8; j++)
            glVertex3f(cubes[i][0][t[j]][0], cubes[i][0][t[j]][1], cubes[i][0][t[j]][2]);
        for (int j=0; j<8; j++)
            glVertex3f(cubes[i][1][t[j]][0], cubes[i][1][t[j]][1], cubes[i][1][t[j]][2]);
        for (int j=0; j<4; j++)
        {
            glVertex3f(cubes[i][0][j][0], cubes[i][0][j][1], cubes[i][0][j][2]);
            glVertex3f(cubes[i][1][j][0], cubes[i][1][j][1], cubes[i][1][j][2]);
        }
    }
    glEnd();

    return (1);
}

typedef struct s_move
{
    enum moves m;
    int     sdlk;
    int     kmod;
    char    *str;
    int     face;
    void    (*f)(int, float);
    int     dir;
    int     rot;
    void    (*u)();
} t_move;

t_move  all_moves[] = {

//
//    //move   sdlk,   kmod,        str,  face func     rot int  update
    { F,       SDLK_f, KMOD_NONE  , "F" , 0, &rotate_Z,  1, 0, &update_front },
    { F_PRIME, SDLK_f, KMOD_LSHIFT, "F'", 0, &rotate_Z, -1, 1, &update_front },
    { F_2,     SDLK_f, KMOD_LCTRL , "F2", 0, &rotate_Z,  2, 2, &update_front },
    { B,       SDLK_b, KMOD_NONE  , "B" , 1, &rotate_Z, -1, 0, &update_back  },
    { B_PRIME, SDLK_b, KMOD_LSHIFT, "B'", 1, &rotate_Z,  1, 1, &update_back  },
    { B_2,     SDLK_b, KMOD_LCTRL , "B2", 1, &rotate_Z,  2, 2, &update_back  },
    { U,       SDLK_u, KMOD_NONE  , "U" , 2, &rotate_Y,  1, 0, &update_up    },
    { U_PRIME, SDLK_u, KMOD_LSHIFT, "U'", 2, &rotate_Y, -1, 1, &update_up    },
    { U_2,     SDLK_u, KMOD_LCTRL , "U2", 2, &rotate_Y,  2, 2, &update_up    },
    { D,       SDLK_d, KMOD_NONE  , "D" , 3, &rotate_Y, -1, 0, &update_down  },
    { D_PRIME, SDLK_d, KMOD_LSHIFT, "D'", 3, &rotate_Y,  1, 1, &update_down  },
    { D_2,     SDLK_d, KMOD_LCTRL , "D2", 3, &rotate_Y,  2, 2, &update_down  },
    { R,       SDLK_r, KMOD_NONE  , "R" , 4, &rotate_X,  1, 0, &update_right },
    { R_PRIME, SDLK_r, KMOD_LSHIFT, "R'", 4, &rotate_X, -1, 1, &update_right },
    { R_2,     SDLK_r, KMOD_LCTRL , "R2", 4, &rotate_X,  2, 2, &update_right },
    { L,       SDLK_l, KMOD_NONE  , "L" , 5, &rotate_X, -1, 0, &update_left  },
    { L_PRIME, SDLK_l, KMOD_LSHIFT, "L'", 5, &rotate_X,  1, 1, &update_left  },
    { L_2,     SDLK_l, KMOD_LCTRL , "L2", 5, &rotate_X,  2, 2, &update_left  }
};


uint64_t get_time_stamp()
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ull + tv.tv_usec;
}

#define MOVE_DURATION 1000000    /* half second */

void animate()
{

    float angle = -0.0174533; // one degree
    //float angle = -1.5708f;

    static int i = -1;  //current_move
    static uint64_t move_started_time;
    static t_cube cubes_copy[27];
    uint64_t t;

    if ((i == -1) && (!queue_is_empty(q)))
    {
        i = queue_dequeue(q);
        move_started_time = get_time_stamp();
        memcpy(cubes_copy, cubes, sizeof(cubes));
    }
    if (i == -1)
        return ;

    t = get_time_stamp();
    if (t > move_started_time + MOVE_DURATION)
    {
        memcpy(cubes, cubes_copy, sizeof(cubes));
        int f = all_moves[i].face;
        all_moves[i].f(f, all_moves[i].dir * 90 * angle);
        rotate_ints(faces[f], all_moves[i].rot);
        all_moves[i].u();
        printf("%s\n", all_moves[i].str);
        i = -1;
        return animate();
    }

    memcpy(cubes, cubes_copy, sizeof(cubes));
    int f = all_moves[i].face;
    int d = all_moves[i].dir;
    d = d < 0 ? -d : d;
    float p = (t - move_started_time) / (MOVE_DURATION / 100);
    float k = roundf(((float)d * 90.0f) / ((float)100.0f) * p);
    k = all_moves[i].dir < 0 ? -k : k;
//    printf("p = %f, k = %f\n", p, k);

    all_moves[i].f(f, k * angle);

    //animate
}

void handleKeyPress(SDL_Keysym *keysym) {

    for (int i=0; i<18; i++)
    {
        if (all_moves[i].sdlk != keysym->sym)
            continue ;
        if ((!all_moves[i].kmod) && (keysym->mod))
            continue ;
        if ((all_moves[i].kmod) && (keysym->mod != all_moves[i].kmod))
            continue ;
        queue_enqueue(q, all_moves[i].m); // i      
        break ;
    }

    switch (keysym->sym) {
        case SDLK_ESCAPE:
            Quit(0);
            break;

        case SDLK_RIGHT: perspective_y += 11.25f; break ;
        case SDLK_LEFT:  perspective_y -= 11.25f; break ;
        case SDLK_UP:    perspective_x += 11.25f; break ;
        case SDLK_DOWN:  perspective_x -= 11.25f; break ;
        default:
            break;
    }
    return;
}

int main(int argc, char **argv) {
    int done = 0;
    char *delim = " \t\n\r\f\v";
    SDL_Event event;
    init_cubes();

    q = queue_init();
    for (int i=1; i<argc; i++)
    {
        char *s = argv[i];
        for (int j=0; s[j]; j++)
            s[j] = toupper(s[j]);
        s = strtok(s, delim);
        while (s)
        {
            for (int j=0; j<18; j++)
            {
                if (!strcmp(s, all_moves[j].str))
                {
                    queue_enqueue(q, all_moves[j].m);
                    break ;
                }
            }
            s = strtok(NULL, delim);
        }
    }

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
                    done = 1;
                    break;
                default:
                    break;
            }
        }
        animate();
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGLScene();
        SDL_GL_SwapWindow(window);
    }    
    Quit(0);
    return (0);
}
