/*
                ---------------- 
                | U0 | U1 | U2 | 
                ---------------- 
                | U3 | U4 | U5 | 
                ---------------- 
                | U6 | U7 | U8 | 
---------------- --------------- --------------- ---------------
| L0 | L1 | L2  | F0 | F1 | F2 |  R0 | R1 | R2 |  B0 | B1 | B2 |
---------------- --------------- --------------- ---------------
| L3 | L4 | L5  | F3 | F4 | F5 |  R3 | R4 | R5 |  B3 | B4 | B5 |
---------------- --------------- --------------- ---------------
| L6 | L7 | L8  | F6 | F7 | F8 |  R6 | R7 | R8 |  B6 | B7 | B8 |
---------------- --------------- --------------- ---------------
                | D0 | D1 | D2 | 
                ---------------- 
                | D3 | D4 | D5 | 
                ---------------- 
                | D6 | D7 | D8 | 
                ---------------- 
*/


/*
    6 cube faces,   F = orange
                    B = red
                    U = white
                    D = yellow
                    R = green
                    L = blue


    9 squares per face: ABC
                        DEF
                        GHI
    4 points per square TL,TR,BR,BL
    3 floats per point
*/

float c[6][3] = {
    {1.00f, 0.65f, 0.00f}, 
    {1.00f, 0.00f, 0.00f}, 
    {1.00f, 1.00f, 1.00f}, 
    {1.00f, 1.00f, 0.00f}, 
    {0.00f, 1.00f, 0.00f}, 
    {0.00f, 0.00f, 1.00f} 
};

float f[6][9][4][3] = {
    {
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
    },

    {
        {   {-3.0f,  3.0f, -3.0f},     //A-TL
            {-1.0f,  3.0f, -3.0f},     //A-TR
            {-1.0f,  1.0f, -3.0f},     //A-BR
            {-3.0f,  1.0f, -3.0f} },   //A-BL
        {   {-1.0f,  3.0f, -3.0f},     //B-TL
            { 1.0f,  3.0f, -3.0f},     //B-TR
            { 1.0f,  1.0f, -3.0f},     //B-BR
            {-1.0f,  1.0f, -3.0f} },   //B-BL
        {   { 1.0f,  3.0f, -3.0f},     //C-TL
            { 3.0f,  3.0f, -3.0f},     //C-TR
            { 3.0f,  1.0f, -3.0f},     //C-BR
            { 1.0f,  1.0f, -3.0f} },   //C-BL
        {   {-3.0f,  1.0f, -3.0f},     //D-TL
            {-1.0f,  1.0f, -3.0f},     //D-TR
            {-1.0f, -1.0f, -3.0f},     //D-BR
            {-3.0f, -1.0f, -3.0f} },   //D-BL
        {   {-1.0f,  1.0f, -3.0f},     //E-TL
            { 1.0f,  1.0f, -3.0f},     //E-TR
            { 1.0f, -1.0f, -3.0f},     //E-BR
            {-1.0f, -1.0f, -3.0f} },   //E-BL
        {   { 1.0f,  1.0f, -3.0f},     //F-TL
            { 3.0f,  1.0f, -3.0f},     //F-TR
            { 3.0f, -1.0f, -3.0f},     //F-BR
            { 1.0f, -1.0f, -3.0f} },   //F-BL
        {   {-3.0f, -1.0f, -3.0f},     //G-TL
            {-1.0f, -1.0f, -3.0f},     //G-TR
            {-1.0f, -3.0f, -3.0f},     //G-BR
            {-3.0f, -3.0f, -3.0f} },   //G-BL
        {   {-1.0f, -1.0f, -3.0f},     //H-TL
            { 1.0f, -1.0f, -3.0f},     //H-TR
            { 1.0f, -3.0f, -3.0f},     //H-BR
            {-1.0f, -3.0f, -3.0f} },   //H-BL
        {   { 1.0f, -1.0f, -3.0f},     //I-TL
            { 3.0f, -1.0f, -3.0f},     //I-TR
            { 3.0f, -3.0f, -3.0f},     //I-BR
            { 1.0f, -3.0f, -3.0f} }    //I-BL
    },

    {
        {   {-3.0f, 3.0f,  3.0f},     //A-TL
            {-1.0f, 3.0f,  3.0f},     //A-TR
            {-1.0f, 3.0f,  1.0f},     //A-BR
            {-3.0f, 3.0f,  1.0f} },   //A-BL
        {   {-1.0f, 3.0f,  3.0f},     //B-TL
            { 1.0f, 3.0f,  3.0f},     //B-TR
            { 1.0f, 3.0f,  1.0f},     //B-BR
            {-1.0f, 3.0f,  1.0f} },   //B-BL
        {   { 1.0f, 3.0f,  3.0f},     //C-TL
            { 3.0f, 3.0f,  3.0f},     //C-TR
            { 3.0f, 3.0f,  1.0f},     //C-BR
            { 1.0f, 3.0f,  1.0f} },   //C-BL
        {   {-3.0f, 3.0f,  1.0f},     //D-TL
            {-1.0f, 3.0f,  1.0f},     //D-TR
            {-1.0f, 3.0f, -1.0f},     //D-BR
            {-3.0f, 3.0f, -1.0f} },   //D-BL
        {   {-1.0f, 3.0f,  1.0f},     //E-TL
            { 1.0f, 3.0f,  1.0f},     //E-TR
            { 1.0f, 3.0f, -1.0f},     //E-BR
            {-1.0f, 3.0f, -1.0f} },   //E-BL
        {   { 1.0f, 3.0f,  1.0f},     //F-TL
            { 3.0f, 3.0f,  1.0f},     //F-TR
            { 3.0f, 3.0f, -1.0f},     //F-BR
            { 1.0f, 3.0f, -1.0f} },   //F-BL
        {   {-3.0f, 3.0f, -1.0f},     //G-TL
            {-1.0f, 3.0f, -1.0f},     //G-TR
            {-1.0f, 3.0f, -3.0f},     //G-BR
            {-3.0f, 3.0f, -3.0f} },   //G-BL
        {   {-1.0f, 3.0f, -1.0f},     //H-TL
            { 1.0f, 3.0f, -1.0f},     //H-TR
            { 1.0f, 3.0f, -3.0f},     //H-BR
            {-1.0f, 3.0f, -3.0f} },   //H-BL
        {   { 1.0f, 3.0f, -1.0f},     //I-TL
            { 3.0f, 3.0f, -1.0f},     //I-TR
            { 3.0f, 3.0f, -3.0f},     //I-BR
            { 1.0f, 3.0f, -3.0f} }    //I-BL
    },

    {
        {   {-3.0f, -3.0f,  3.0f},     //A-TL
            {-1.0f, -3.0f,  3.0f},     //A-TR
            {-1.0f, -3.0f,  1.0f},     //A-BR
            {-3.0f, -3.0f,  1.0f} },   //A-BL
        {   {-1.0f, -3.0f,  3.0f},     //B-TL
            { 1.0f, -3.0f,  3.0f},     //B-TR
            { 1.0f, -3.0f,  1.0f},     //B-BR
            {-1.0f, -3.0f,  1.0f} },   //B-BL
        {   { 1.0f, -3.0f,  3.0f},     //C-TL
            { 3.0f, -3.0f,  3.0f},     //C-TR
            { 3.0f, -3.0f,  1.0f},     //C-BR
            { 1.0f, -3.0f,  1.0f} },   //C-BL
        {   {-3.0f, -3.0f,  1.0f},     //D-TL
            {-1.0f, -3.0f,  1.0f},     //D-TR
            {-1.0f, -3.0f, -1.0f},     //D-BR
            {-3.0f, -3.0f, -1.0f} },   //D-BL
        {   {-1.0f, -3.0f,  1.0f},     //E-TL
            { 1.0f, -3.0f,  1.0f},     //E-TR
            { 1.0f, -3.0f, -1.0f},     //E-BR
            {-1.0f, -3.0f, -1.0f} },   //E-BL
        {   { 1.0f, -3.0f,  1.0f},     //F-TL
            { 3.0f, -3.0f,  1.0f},     //F-TR
            { 3.0f, -3.0f, -1.0f},     //F-BR
            { 1.0f, -3.0f, -1.0f} },   //F-BL
        {   {-3.0f, -3.0f, -1.0f},     //G-TL
            {-1.0f, -3.0f, -1.0f},     //G-TR
            {-1.0f, -3.0f, -3.0f},     //G-BR
            {-3.0f, -3.0f, -3.0f} },   //G-BL
        {   {-1.0f, -3.0f, -1.0f},     //H-TL
            { 1.0f, -3.0f, -1.0f},     //H-TR
            { 1.0f, -3.0f, -3.0f},     //H-BR
            {-1.0f, -3.0f, -3.0f} },   //H-BL
        {   { 1.0f, -3.0f, -1.0f},     //I-TL
            { 3.0f, -3.0f, -1.0f},     //I-TR
            { 3.0f, -3.0f, -3.0f},     //I-BR
            { 1.0f, -3.0f, -3.0f} }    //I-BL
    },

    {
        {   {3.0f, -3.0f,  3.0f},     //A-TL
            {3.0f, -1.0f,  3.0f},     //A-TR
            {3.0f, -1.0f,  1.0f},     //A-BR
            {3.0f, -3.0f,  1.0f} },   //A-BL
        {   {3.0f, -1.0f,  3.0f},     //B-TL
            {3.0f,  1.0f,  3.0f},     //B-TR
            {3.0f,  1.0f,  1.0f},     //B-BR
            {3.0f, -1.0f,  1.0f} },   //B-BL
        {   {3.0f,  1.0f,  3.0f},     //C-TL
            {3.0f,  3.0f,  3.0f},     //C-TR
            {3.0f,  3.0f,  1.0f},     //C-BR
            {3.0f,  1.0f,  1.0f} },   //C-BL
        {   {3.0f, -3.0f,  1.0f},     //D-TL
            {3.0f, -1.0f,  1.0f},     //D-TR
            {3.0f, -1.0f, -1.0f},     //D-BR
            {3.0f, -3.0f, -1.0f} },   //D-BL
        {   {3.0f, -1.0f,  1.0f},     //E-TL
            {3.0f,  1.0f,  1.0f},     //E-TR
            {3.0f,  1.0f, -1.0f},     //E-BR
            {3.0f, -1.0f, -1.0f} },   //E-BL
        {   {3.0f,  1.0f,  1.0f},     //F-TL
            {3.0f,  3.0f,  1.0f},     //F-TR
            {3.0f,  3.0f, -1.0f},     //F-BR
            {3.0f,  1.0f, -1.0f} },   //F-BL
        {   {3.0f, -3.0f, -1.0f},     //G-TL
            {3.0f, -1.0f, -1.0f},     //G-TR
            {3.0f, -1.0f, -3.0f},     //G-BR
            {3.0f, -3.0f, -3.0f} },   //G-BL
        {   {3.0f, -1.0f, -1.0f},     //H-TL
            {3.0f,  1.0f, -1.0f},     //H-TR
            {3.0f,  1.0f, -3.0f},     //H-BR
            {3.0f, -1.0f, -3.0f} },   //H-BL
        {   {3.0f,  1.0f, -1.0f},     //I-TL
            {3.0f,  3.0f, -1.0f},     //I-TR
            {3.0f,  3.0f, -3.0f},     //I-BR
            {3.0f,  1.0f, -3.0f} }    //I-BL
    },

    {
        {   {-3.0f, -3.0f,  3.0f},     //A-TL
            {-3.0f, -1.0f,  3.0f},     //A-TR
            {-3.0f, -1.0f,  1.0f},     //A-BR
            {-3.0f, -3.0f,  1.0f} },   //A-BL
        {   {-3.0f, -1.0f,  3.0f},     //B-TL
            {-3.0f,  1.0f,  3.0f},     //B-TR
            {-3.0f,  1.0f,  1.0f},     //B-BR
            {-3.0f, -1.0f,  1.0f} },   //B-BL
        {   {-3.0f,  1.0f,  3.0f},     //C-TL
            {-3.0f,  3.0f,  3.0f},     //C-TR
            {-3.0f,  3.0f,  1.0f},     //C-BR
            {-3.0f,  1.0f,  1.0f} },   //C-BL
        {   {-3.0f, -3.0f,  1.0f},     //D-TL
            {-3.0f, -1.0f,  1.0f},     //D-TR
            {-3.0f, -1.0f, -1.0f},     //D-BR
            {-3.0f, -3.0f, -1.0f} },   //D-BL
        {   {-3.0f, -1.0f,  1.0f},     //E-TL
            {-3.0f,  1.0f,  1.0f},     //E-TR
            {-3.0f,  1.0f, -1.0f},     //E-BR
            {-3.0f, -1.0f, -1.0f} },   //E-BL
        {   {-3.0f,  1.0f,  1.0f},     //F-TL
            {-3.0f,  3.0f,  1.0f},     //F-TR
            {-3.0f,  3.0f, -1.0f},     //F-BR
            {-3.0f,  1.0f, -1.0f} },   //F-BL
        {   {-3.0f, -3.0f, -1.0f},     //G-TL
            {-3.0f, -1.0f, -1.0f},     //G-TR
            {-3.0f, -1.0f, -3.0f},     //G-BR
            {-3.0f, -3.0f, -3.0f} },   //G-BL
        {   {-3.0f, -1.0f, -1.0f},     //H-TL
            {-3.0f,  1.0f, -1.0f},     //H-TR
            {-3.0f,  1.0f, -3.0f},     //H-BR
            {-3.0f, -1.0f, -3.0f} },   //H-BL
        {   {-3.0f,  1.0f, -1.0f},     //I-TL
            {-3.0f,  3.0f, -1.0f},     //I-TR
            {-3.0f,  3.0f, -3.0f},     //I-BR
            {-3.0f,  1.0f, -3.0f} }    //I-BL
    }
};
