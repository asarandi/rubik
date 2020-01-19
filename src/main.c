#include <stdint.h>
#include <sys/time.h>

#if defined(__APPLE__)

# include <OpenGL/gl.h>
# include <OpenGL/glu.h>

#else
# include <GL/gl.h>
# include <GL/glu.h>
#endif

#include "SDL.h"
#include "common.h"
#include "move.h"
#include "queue.h"
#include "solver.h"
#include "utils.h"

# define SCREEN_WIDTH  640
# define SCREEN_HEIGHT 480
# define CUBE_INNER_COLOR 0.08f, 0.08f, 0.08f
# define MOVE_DURATION 500000    /* half second */
# define ABS(i) ((i) < 0 ? -(i) : (i))
# define PERSPECTIVE_UNIT 5.625f
# define ONE_DEGREE -0.0174533f

SDL_GLContext *context;
SDL_Window *window;

t_move moves[] = {
        {SDLK_f, KMOD_NONE,  "F",  1},
        {SDLK_f, KMOD_LCTRL, "F'", -1},
        {SDLK_f, KMOD_LALT,  "F2", 2},
        {SDLK_b, KMOD_NONE,  "B",  -1},
        {SDLK_b, KMOD_LCTRL, "B'", 1},
        {SDLK_b, KMOD_LALT,  "B2", -2},
        {SDLK_u, KMOD_NONE,  "U",  1},
        {SDLK_u, KMOD_LCTRL, "U'", -1},
        {SDLK_u, KMOD_LALT,  "U2", 2},
        {SDLK_d, KMOD_NONE,  "D",  -1},
        {SDLK_d, KMOD_LCTRL, "D'", 1},
        {SDLK_d, KMOD_LALT,  "D2", -2},
        {SDLK_r, KMOD_NONE,  "R",  1},
        {SDLK_r, KMOD_LCTRL, "R'", -1},
        {SDLK_r, KMOD_LALT,  "R2", 2},
        {SDLK_l, KMOD_NONE,  "L",  -1},
        {SDLK_l, KMOD_LCTRL, "L'", 1},
        {SDLK_l, KMOD_LALT,  "L2", -2}
};

typedef float t_cube[6][4][3];

t_cube cube_array[27];
t_cube cube_array_copy[27];

float cube_colors[6][3] = {
        {0.00f, 0.61f, 0.28f},  //F - green
        {0.00f, 0.27f, 0.68f},  //B - blue
        {1.00f, 1.00f, 1.00f},  //U - white
        {1.00f, 0.84f, 0.00f},  //D - yellow
        {0.72f, 0.07f, 0.20f},  //R - red
        {1.00f, 0.35f, 0.00f}   //L - orange
};

int faces_array[6][9] = {
        {6,  15, 24, 3,  12, 21, 0,  9,  18}, // 0 F green
        {26, 17, 8,  23, 14, 5,  20, 11, 2},  // 1 B blue
        {8,  17, 26, 7,  16, 25, 6,  15, 24}, // 2 U white
        {0,  9,  18, 1,  10, 19, 2,  11, 20}, // 3 D yellow
        {24, 25, 26, 21, 22, 23, 18, 19, 20}, // 4 R red
        {8,  7,  6,  5,  4,  3,  2,  1,  0}   // 5 L orange
};

t_state cube_state = {
        0,
        NULL,
        0,
        0,
        0,
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 0, 0, 0, 0, 0, 0, 0},
};

int faces_array_copy[6][9];

int color_enabled_table[27][6];
float perspective_x = 35.264f;
float perspective_y = -45.0f;
t_queue *q;

void rotate_x(float *xyz, float angle) {
    float y, z;

    y = xyz[1];
    z = xyz[2];
    xyz[1] = (y * cosf(angle)) + (z * -sinf(angle));
    xyz[2] = (y * sinf(angle)) + (z * cosf(angle));
}

void rotate_y(float *xyz, float angle) {
    float x, z;

    x = xyz[0];
    z = xyz[2];
    xyz[0] = (x * cosf(angle)) + (z * sinf(angle));
    xyz[2] = (x * -sinf(angle)) + (z * cosf(angle));
}

void rotate_z(float *xyz, float angle) {
    float x, y;

    x = xyz[0];
    y = xyz[1];
    xyz[0] = (x * cosf(angle)) + (y * -sinf(angle));
    xyz[1] = (x * sinf(angle)) + (y * cosf(angle));
}

t_cube *make_cube(float x, float y, float z) {
    int coords[6][4][3] = {
            {
                    {0, 0, 0}, //F
                    {0, 1, 0},
                    {1, 1, 0},
                    {1, 0, 0},
            },
            {
                    {0, 0, -1}, //B
                    {0, 1, -1},
                    {1, 1, -1},
                    {1, 0, -1},
            },
            {
                    {0, 1, 0}, //U
                    {0, 1, -1},
                    {1, 1, -1},
                    {1, 1, 0},
            },
            {
                    {0, 0, 0}, //D
                    {0, 0, -1},
                    {1, 0, -1},
                    {1, 0, 0},
            },
            {
                    {1, 0, 0}, //R
                    {1, 1, 0},
                    {1, 1, -1},
                    {1, 0, -1},
            },
            {
                    {0, 0, 0}, //L
                    {0, 1, 0},
                    {0, 1, -1},
                    {0, 0, -1},
            },
    };

    static t_cube t;
    float f[3], u = 2.0f;
    int i, j, k;

    f[0] = x;
    f[1] = y;
    f[2] = z;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 3; k++)
                t[i][j][k] = f[k] + (float) coords[i][j][k] * u;
        }
    }
    return &t;
}

void update_neighbors(int f) {
    int i, j, k;

    for (i = 0; i < 6; i++) {
        if (i == f)
            continue;
        for (j = 0; j < 9; j++) {
            for (k = 0; k < 9; k++) {
                if (faces_array_copy[i][j] == faces_array_copy[f][k])
                    faces_array[i][j] = faces_array[f][k];
            }
        }
    }
}

void rotate_face_ints(int f, int k) {
    int i, copy[9], rotations[3][9] = {
            {6, 3, 0, 7, 4, 1, 8, 5, 2},    //clockwise
            {2, 5, 8, 1, 4, 7, 0, 3, 6},    //anti-clockwise
            {8, 7, 6, 5, 4, 3, 2, 1, 0}     //twice
    };

    for (i = 0; i < 9; i++)
        copy[i] = faces_array[f][i];
    for (i = 0; i < 9; i++)
        faces_array[f][i] = copy[rotations[k][i]];
}

void rotate_face_floats(int f, float angle) {
    int idx, i, j, k;
    void (*func[])(float *, float) =
            {
                    &rotate_z,
                    &rotate_y,
                    &rotate_x
            };

    for (idx = 0; idx < 9; idx++) {
        i = faces_array[f][idx];
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 4; k++)
                func[f / 2](cube_array[i][j][k], angle);
        }
    }
}

void mark_enabled_colors(int *res, int i, int j, int k) {
    res[0] = k == 3;
    res[1] = k == -1;
    res[2] = j == 1;
    res[3] = j == -3;
    res[4] = i == 1;
    res[5] = i == -3;
}

void init_cubes() {
    int idx, i, j, k;
    t_cube *t;

    idx = 0;
    for (i = -3; i < 3; i += 2) {
        for (j = -3; j < 3; j += 2) {
            for (k = 3; k > -3; k -= 2) {
                t = make_cube((float) i, (float) j, (float) k);
                memcpy(cube_array[idx], t, sizeof(*t));
                mark_enabled_colors(color_enabled_table[idx], i, j, k);
                ++idx;
            }
        }
    }
}

uint64_t get_time_stamp() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ull + tv.tv_usec;
}

void process_moves() {
    static int i = -1;  //current_move
    static uint64_t move_started_time;
    uint64_t t;
    float p, k;
    int d;

    if (i == -1) {
        if (queue_is_empty(q))
            return;
        i = (intptr_t) (int) queue_dequeue(q);
        move_started_time = get_time_stamp();
        memcpy(cube_array_copy, cube_array, sizeof(cube_array));
        return;
    }

    t = get_time_stamp();
    d = moves[i].dir;

    if (t > move_started_time + MOVE_DURATION) {
        memcpy(cube_array, cube_array_copy, sizeof(cube_array));
        rotate_face_floats(i / 3, (float) d * 90 * ONE_DEGREE);
        rotate_face_ints(i / 3, i % 3);
        move_state(&cube_state, i / 3, i % 3);
        update_neighbors(i / 3);
        //printf("%s\n", moves[i].str);
        i = -1;
        return;
    }

    memcpy(cube_array, cube_array_copy, sizeof(cube_array));
    p = (float) (t - move_started_time) / ((float) MOVE_DURATION / 100);
    k = roundf(((float) ABS(d) * 90.0f) / ((float) 100.0f) * p);
    rotate_face_floats(i / 3, d < 0 ? -k * ONE_DEGREE : k * ONE_DEGREE);
}

int draw_gl_scene() {
    int t[8] = {0, 1, 2, 3, 0, 3, 1, 2};
    int i, j, k;

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -18.0f);
    glRotatef(perspective_x, 1.0f, 0.0f, 0.0f);
    glRotatef(perspective_y, 0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    for (i = 0; i < 27; i++) {
        for (j = 0; j < 6; j++) {
            if (color_enabled_table[i][j])
                glColor4f(cube_colors[j][0], cube_colors[j][1], cube_colors[j][2], 1.0f);
            else
                glColor4f(CUBE_INNER_COLOR, 1.0f);

            for (k = 0; k < 4; k++)
                glVertex3f(cube_array[i][j][k][0], cube_array[i][j][k][1], cube_array[i][j][k][2]);
        }
    }
    glEnd();
    glLineWidth(10.0f);
    glBegin(GL_LINES);
    glColor4f(CUBE_INNER_COLOR, 0.2f);
    for (i = 0; i < 27; i++) {
        for (j = 0; j < 8; j++)
            glVertex3f(cube_array[i][0][t[j]][0], cube_array[i][0][t[j]][1], cube_array[i][0][t[j]][2]);
        for (j = 0; j < 8; j++)
            glVertex3f(cube_array[i][1][t[j]][0], cube_array[i][1][t[j]][1], cube_array[i][1][t[j]][2]);
        for (j = 0; j < 4; j++) {
            glVertex3f(cube_array[i][0][j][0], cube_array[i][0][j][1], cube_array[i][0][j][2]);
            glVertex3f(cube_array[i][1][j][0], cube_array[i][1][j][1], cube_array[i][1][j][2]);
        }
    }
    glEnd();
    glDisable(GL_BLEND);
    return (1);
}

void quit(int exit_code) {
    queue_destroy(q);
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    exit(exit_code);
}

void handle_key_press(SDL_Keysym *keysym) {
    int i;

    for (i = 0; i < 18; i++) {
        if (moves[i].sdlk != keysym->sym)
            continue;
        if ((!moves[i].kmod) && (keysym->mod))
            continue;
        if ((moves[i].kmod) && (keysym->mod != moves[i].kmod))
            continue;
        queue_enqueue(q, (void *) (intptr_t) i);
        break;
    }

    switch (keysym->sym) {
        case SDLK_p:
            print_cube(&cube_state);
            break;
        case SDLK_s:
            solve(&cube_state);
            break;
        case SDLK_ESCAPE:
            quit(0);
            break;
        case SDLK_RIGHT:
            perspective_y += PERSPECTIVE_UNIT;
            break;
        case SDLK_LEFT:
            perspective_y -= PERSPECTIVE_UNIT;
            break;
        case SDLK_UP:
            perspective_x += PERSPECTIVE_UNIT;
            break;
        case SDLK_DOWN:
            perspective_x -= PERSPECTIVE_UNIT;
            break;
        default:
            break;
    }
}

void read_input(int argc, char **argv) {
    int i, j;
    char *s, *delim = " \t\n\r\f\v";

    for (i = 1; i < argc; i++) {
        s = argv[i];
        for (j = 0; s[j]; j++)
            s[j] = (char) toupper(s[j]);
        s = strtok(s, delim);
        while (s) {
            for (j = 0; j < 18; j++) {
                if (!strcmp(s, moves[j].str)) {
                    queue_enqueue(q, (void *) (intptr_t) j);
                    break;
                }
            }
            s = strtok(NULL, delim);
        }
    }
}

int init_gl() {
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

void init_window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        quit(1);
    }

    if (!(window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SDL_WINDOW_OPENGL))) {
        fprintf(stderr, "CreateWindow() failed: %s\n", SDL_GetError());
        quit(1);
    }

    if (!(context = SDL_GL_CreateContext(window))) {
        fprintf(stderr, "SDL_GL_CreateContext() failed: %s\n", SDL_GetError());
        quit(1);
    }
}

int main(int ac, char **av) {
    int done = 0;
    SDL_Event event;

    init_window();
    init_gl();
    init_cubes();
    memcpy(faces_array_copy, faces_array, sizeof(faces_array));
    q = queue_init();
    read_input(ac, av);

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    handle_key_press(&event.key.keysym);
                    break;
                case SDL_QUIT:
                    done = 1;
                    break;
                default:
                    break;
            }
        }
        process_moves();
        draw_gl_scene();
        SDL_GL_SwapWindow(window);
    }
    quit(0);
    return (0);
}
