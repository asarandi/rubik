#include "common.h"
#include "move.h"
#include "utils.h"

extern t_move moves[];

void print_cube(t_state *cube) {
    char *edge_orientation_names[2][12] = {
            {"UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL"},
            {"FU", "RU", "BU", "LU", "FD", "RD", "BD", "LD", "RF", "LF", "RB", "LB"},
    };
    char *corner_orientation_names[3][8] = {
            {"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR"},
            {"RUF", "BUR", "LUB", "FUL", "FDR", "LDF", "BDL", "RDB"},
            {"FRU", "RBU", "BLU", "LFU", "RFD", "FLD", "LBD", "BRD"},
    };
    int i, j, k;

    for (i = 0; i < 12; i++) {
        j = cube->edge_orientations[i];
        k = cube->edges[i];
        printf("%s%s", edge_orientation_names[j][k], " ");
    }
    for (i = 0; i < 8; i++) {
        j = cube->corner_orientations[i];
        k = cube->corners[i];
        printf("%s%s", corner_orientation_names[j][k], i + 1 < 8 ? " " : "\n");
    }

    for (i = 0; i < 12; i++)
        printf("%02x%s", cube->edges[i], " ");
    for (i = 0; i < 8; i++)
        printf("%02x%s", cube->corners[i], i + 1 < 8 ? " " : "\n");
    for (i = 0; i < 12; i++)
        printf("%02x%s", cube->edge_orientations[i], " ");
    for (i = 0; i < 8; i++)
        printf("%02x%s", cube->corner_orientations[i], i + 1 < 8 ? " " : "\n");
    printf("move = %s\n", moves[(int) cube->move].str);
}
