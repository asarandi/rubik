int faces[6][9] = {
    { 6, 15, 24,  3, 12, 21,  0,  9, 18}, // 0 F green
    {26, 17,  8, 23, 14,  5, 20, 11,  2}, // 1 B blue
    { 8, 17, 26,  7, 16, 25,  6, 15, 24}, // 2 U white
    { 0,  9, 18,  1, 10, 19,  2, 11, 20}, // 3 D yellow
    {24, 25, 26, 21, 22, 23, 18, 19, 20}, // 4 R red
    { 8,  7,  6,  5,  4,  3,  2,  1,  0}  // 5 L orange
};

int fcopy[6][9];

void copy_faces()
{
    int i, j;

    for (i=0; i<6; i++)
    {
        for (j=0; j<9; j++)
            fcopy[i][j] = faces[i][j];
    }
}

void update_neighbors(int f)
{
    int i, j, k;

    for (i=0; i<6; i++)
    {
        if (i == f)
            continue ;
        for (j=0; j<9; j++)
        {
            for (k=0; k<9; k++)
            {
                if (fcopy[i][j] == fcopy[f][k])
                    faces[i][j] = faces[f][k];
            }
        }
    }
}
