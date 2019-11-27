#include "visualizer.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"

void copy_faces();
void update_neighbors(int f);

extern int faces[6][9];
extern float c[6][3];

int ctable [27][6];

float perspective_x = 35.264f;
float perspective_y = -45.0f;

#define CUBE_INNER_COLOR 0.08f, 0.08f, 0.08f

#define MOVE_DURATION 500000    /* half second */
# define ABS(i) ((i) < 0 ? -(i) : (i))

typedef struct s_move
{
    int     sdlk;
    int     kmod;
    char    *str;
    int     dir;
} t_move;

t_move  all_moves[] = {

//  //sdlk,   kmod,        str,  func       rot
    {SDLK_f, KMOD_NONE  , "F" ,  1},
    {SDLK_f, KMOD_LSHIFT, "F'", -1},
    {SDLK_f, KMOD_LCTRL , "F2",  2},
    {SDLK_b, KMOD_NONE  , "B" , -1},
    {SDLK_b, KMOD_LSHIFT, "B'",  1},
    {SDLK_b, KMOD_LCTRL , "B2",  2},
    {SDLK_u, KMOD_NONE  , "U" ,  1},
    {SDLK_u, KMOD_LSHIFT, "U'", -1},
    {SDLK_u, KMOD_LCTRL , "U2",  2},
    {SDLK_d, KMOD_NONE  , "D" , -1},
    {SDLK_d, KMOD_LSHIFT, "D'",  1},
    {SDLK_d, KMOD_LCTRL , "D2",  2},
    {SDLK_r, KMOD_NONE  , "R" ,  1},
    {SDLK_r, KMOD_LSHIFT, "R'", -1},
    {SDLK_r, KMOD_LCTRL , "R2",  2},
    {SDLK_l, KMOD_NONE  , "L" , -1},
    {SDLK_l, KMOD_LSHIFT, "L'",  1},
    {SDLK_l, KMOD_LCTRL , "L2",  2}
};


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
    int c[6][4][3] = {
        {
            {0, 0,  0}, //F
            {0, 1,  0},
            {1, 1,  0},
            {1, 0,  0},
        },
        {
            {0, 0, -1}, //B
            {0, 1, -1},
            {1, 1, -1},
            {1, 0, -1},
        },
        {
            {0, 1,  0}, //U
            {0, 1, -1},
            {1, 1, -1},
            {1, 1,  0},
        },
        {
            {0, 0,  0}, //D
            {0, 0, -1},
            {1, 0, -1},
            {1, 0,  0},
        },
        {
            {1, 0,  0}, //R
            {1, 1,  0},
            {1, 1, -1},
            {1, 0, -1},
        },
        {
            {0, 0,  0}, //L
            {0, 1,  0},
            {0, 1, -1},
            {0, 0, -1},
        },
    };

    static t_cube  t;
    float u = 2.0f;
    float f[3];

    f[0] = x;
    f[1] = y;
    f[2] = z;

    for (int i=0; i<6; i++)
    {
        for (int j=0; j<4; j++)
        {
            for (int k=0; k<3; k++)
                t[i][j][k] = f[k] + c[i][j][k] * u;
        }
    }
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

void rotate(int f, float angle)   //F, B
{
    void (*func[])(float *, float) =
    {
        &rotateZ,
        &rotateY,
        &rotateX
    };

    for (int idx=0; idx<9; idx++)
    {
        int i=faces[f][idx];
        for (int j=0; j<6; j++)
        {
            for (int k=0; k<4; k++)
                func[f/2](cubes[i][j][k], angle);
        }
    }
}

int *colors(int i, int j, int k)
{
    static int  res[6];

    res[0] = k == 3;
    res[1] = k == -1;
    res[2] = j == 1;
    res[3] = j == -3;
    res[4] = i == 1;
    res[5] = i == -3;
    return res;
}

void init_cubes()
{
    int *c, idx = 0;
    t_cube *t;
    for (int i=-3; i<3; i+=2)
    {
        for (int j=-3; j<3; j+=2)
        {
            for (int k=3; k>-3; k-=2)
            {
                t = make_cube((float)i, (float)j, (float)k);
                memcpy(cubes[idx], t, sizeof(*t));
                c = colors(i, j, k);
                memcpy(ctable[idx++], c, sizeof(int) * 6);
            }
        }
    }
}

int drawGLScene(GLvoid) {

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(perspective_x, 1.0f, 0.0f, 0.0f);
    glRotatef(perspective_y, 0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    for (int i=0; i<27; i++)
    {
        for (int j=0; j<6; j++)
        {
            if (ctable[i][j])
                glColor3f(c[j][0], c[j][1], c[j][2]);
            else
                glColor3f(CUBE_INNER_COLOR);

            for (int k=0; k<4; k++)
                glVertex3f(cubes[i][j][k][0], cubes[i][j][k][1], cubes[i][j][k][2]);
        }
    }
    glEnd();

    glLineWidth(10.0f);
    glBegin(GL_LINES);
    glColor3f(CUBE_INNER_COLOR);
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

uint64_t get_time_stamp()
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ull + tv.tv_usec;
}

void animate()
{

    float p, k, angle = -0.0174533; // one degree
    static int d, i = -1;  //current_move
    static uint64_t move_started_time;
    static t_cube cubes_copy[27];
    uint64_t t;

    if (i == -1)
    {
        if (queue_is_empty(q))
            return ;
        i = queue_dequeue(q);
        move_started_time = get_time_stamp();
        memcpy(cubes_copy, cubes, sizeof(cubes));
    }

    t = get_time_stamp();
    d = all_moves[i].dir;

    if (t > move_started_time + MOVE_DURATION)
    {
        memcpy(cubes, cubes_copy, sizeof(cubes));
        rotate(i / 3, d * 90 * angle);
        rotate_ints(faces[i / 3], i % 3);
        update_neighbors(i / 3);
        printf("%s\n", all_moves[i].str);
        i = -1;
        return ;
    }

    memcpy(cubes, cubes_copy, sizeof(cubes));
    p = (t - move_started_time) / (MOVE_DURATION / 100);
    k = roundf(((float)ABS(d) * 90.0f) / ((float)100.0f) * p);
    rotate(i / 3, d < 0 ? -k * angle : k * angle);
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
        queue_enqueue(q, i);
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
    copy_faces();

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
                    queue_enqueue(q, i);
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
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGLScene();
        SDL_GL_SwapWindow(window);
    }
    Quit(0);
    return (0);
}
