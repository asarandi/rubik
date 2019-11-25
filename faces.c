int faces[6][9] = {
    { 6, 15, 24,  3, 12, 21,  0,  9, 18}, // 0 F green
    {26, 17,  8, 23, 14,  5, 20, 11,  2}, // 1 B blue
    { 8, 17, 26,  7, 16, 25,  6, 15, 24}, // 2 U white
    { 0,  9, 18,  1, 10, 19,  2, 11, 20}, // 3 D yellow
    {24, 25, 26, 21, 22, 23, 18, 19, 20}, // 4 R red
    { 8,  7,  6,  5,  4,  3,  2,  1,  0}  // 5 L orange
};

void update_front()
{
    faces[2][6] = faces[0][0];  //up
    faces[2][7] = faces[0][1];
    faces[2][8] = faces[0][2];

    faces[4][0] = faces[0][2];  //right
    faces[4][3] = faces[0][5];
    faces[4][6] = faces[0][8];

    faces[3][0] = faces[0][6];  //down
    faces[3][1] = faces[0][7];
    faces[3][2] = faces[0][8];

    faces[5][2] = faces[0][0];  //left
    faces[5][5] = faces[0][3];
    faces[5][8] = faces[0][6];
}

void update_right()
{
    faces[2][8] = faces[4][0];  //up
    faces[2][5] = faces[4][1];
    faces[2][2] = faces[4][2];

    faces[1][0] = faces[4][2];  //right
    faces[1][3] = faces[4][5];
    faces[1][6] = faces[4][8];

    faces[3][2] = faces[4][6];  //down
    faces[3][5] = faces[4][7];
    faces[3][8] = faces[4][8];

    faces[0][2] = faces[4][0];  //left
    faces[0][5] = faces[4][3];
    faces[0][8] = faces[4][6];
}

void update_back()
{
    faces[2][2] = faces[1][0];  //up
    faces[2][1] = faces[1][1];
    faces[2][0] = faces[1][2];

    faces[5][0] = faces[1][2];  //right
    faces[5][3] = faces[1][5];
    faces[5][6] = faces[1][8];

    faces[3][8] = faces[1][6];  //down
    faces[3][7] = faces[1][7];
    faces[3][6] = faces[1][8];

    faces[4][2] = faces[1][0];  //left
    faces[4][5] = faces[1][3];
    faces[4][8] = faces[1][6];
}

void update_left()
{
    faces[2][0] = faces[5][0];  //up
    faces[2][3] = faces[5][1];
    faces[2][6] = faces[5][2];

    faces[0][0] = faces[5][2];  //right
    faces[0][3] = faces[5][5];
    faces[0][6] = faces[5][8];

    faces[3][6] = faces[5][6];  //down
    faces[3][3] = faces[5][7];
    faces[3][0] = faces[5][8];

    faces[1][2] = faces[5][0];  //left
    faces[1][5] = faces[5][3];
    faces[1][8] = faces[5][6];
}


void update_up()
{
    faces[1][2] = faces[2][0];
    faces[1][1] = faces[2][1];
    faces[1][0] = faces[2][2];

    faces[4][2] = faces[2][2];
    faces[4][1] = faces[2][5];
    faces[4][0] = faces[2][8];

    faces[0][0] = faces[2][6];
    faces[0][1] = faces[2][7];
    faces[0][2] = faces[2][8];

    faces[5][0] = faces[2][0];
    faces[5][1] = faces[2][3];
    faces[5][2] = faces[2][6];
}

void update_down()
{
    faces[0][6] = faces[3][0];
    faces[0][7] = faces[3][1];
    faces[0][8] = faces[3][2];

    faces[4][6] = faces[3][2];
    faces[4][7] = faces[3][5];
    faces[4][8] = faces[3][8];

    faces[1][8] = faces[3][6];
    faces[1][7] = faces[3][7];
    faces[1][6] = faces[3][8];

    faces[5][8] = faces[3][0];
    faces[5][7] = faces[3][3];
    faces[5][6] = faces[3][6];
}
