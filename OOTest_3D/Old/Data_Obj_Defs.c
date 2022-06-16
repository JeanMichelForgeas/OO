
/*------------------------------------------------------------------------*/
/*                              Définition Simple face                    */

static OOTexture Mur_Textures[] = {
	{ 0, 0, 128, 128, (FLOAT)1, (FLOAT)1, OOF_TEXTURE_STRETCH, 0, NULL }
};
static UWORD Mur_Faces[] = {
    4, 6,40, OOF_LINES|OOF_MAP, 0,    0,3,2,1,
    ENDFACES
};
static OO3DPoint Mur_Coords[] = {
    (FLOAT)-.80, (FLOAT)+.80, (FLOAT)0,
    (FLOAT)+.80, (FLOAT)+.80, (FLOAT)0,
    (FLOAT)+.80, (FLOAT)-.80, (FLOAT)0,
    (FLOAT)-.80, (FLOAT)-.80, (FLOAT)0,
};

/*------------------------------------------------------------------------*/
/*                              Définition petite pyramide                */

static UWORD Pyramide_Faces[] = {
    3, 6,0, OOF_LINES, 0,    0,1,3,
    3, 6,0, OOF_LINES, 0,    2,0,3,
    3, 6,0, OOF_LINES, 0,    2,3,1,
    3, 6,0, OOF_LINES, 0,    2,1,0,
    ENDFACES
};
static OO3DPoint Pyramide_Coords[] = {
    (FLOAT)-.40, (FLOAT)-.20, (FLOAT) .20,
    (FLOAT) .40, (FLOAT)-.20, (FLOAT) .20,
    (FLOAT)   0, (FLOAT) .40, (FLOAT) .20,
    (FLOAT)   0, (FLOAT)   0, (FLOAT)-.40,
};

/*------------------------------------------------------------------------*/
/*                              Définition balle Boing                    */

static UWORD Boing_Faces[] = {
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    1,0,3,4,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    3,2,6,7,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    5,4,8,9,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    8,7,10,11,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    16,15,12,13,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    19,18,14,15,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    21,20,16,17,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    23,22,19,20,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    0,12,14,2,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    6,18,22,10,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    11,23,21,9,
    4, 0,3, OOF_HIDE|OOF_FILL, 0,    5,17,13,1,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    0,2,3,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    5,1,4,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    4,3,7,8,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    7,6,10,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    9,8,11,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    15,14,12,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    17,16,13,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    20,19,15,16,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    22,18,19,
    3, 0,2, OOF_HIDE|OOF_FILL, 0,    21,23,20,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    1,13,12,0,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    2,14,18,6,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    10,22,23,11,
    4, 0,2, OOF_HIDE|OOF_FILL, 0,    9,21,17,5,
    ENDFACES
};
static OO3DPoint Boing_Coords[] = {
    (FLOAT)-.50, (FLOAT) .17, (FLOAT)-.17,
    (FLOAT)-.50, (FLOAT) .17, (FLOAT) .17,
    (FLOAT)-.17, (FLOAT) .17, (FLOAT)-.50,
    (FLOAT)-.17, (FLOAT) .50, (FLOAT)-.17,
    (FLOAT)-.17, (FLOAT) .50, (FLOAT) .17,
    (FLOAT)-.17, (FLOAT) .17, (FLOAT) .50,
    (FLOAT) .17, (FLOAT) .17, (FLOAT)-.50,
    (FLOAT) .17, (FLOAT) .50, (FLOAT)-.17,
    (FLOAT) .17, (FLOAT) .50, (FLOAT) .17,
    (FLOAT) .17, (FLOAT) .17, (FLOAT) .50,
    (FLOAT) .50, (FLOAT) .17, (FLOAT)-.17,
    (FLOAT) .50, (FLOAT) .17, (FLOAT) .17,
    (FLOAT)-.50, (FLOAT)-.17, (FLOAT)-.17,
    (FLOAT)-.50, (FLOAT)-.17, (FLOAT) .17,
    (FLOAT)-.17, (FLOAT)-.17, (FLOAT)-.50,
    (FLOAT)-.17, (FLOAT)-.50, (FLOAT)-.17,
    (FLOAT)-.17, (FLOAT)-.50, (FLOAT) .17,
    (FLOAT)-.17, (FLOAT)-.17, (FLOAT) .50,
    (FLOAT) .17, (FLOAT)-.17, (FLOAT)-.50,
    (FLOAT) .17, (FLOAT)-.50, (FLOAT)-.17,
    (FLOAT) .17, (FLOAT)-.50, (FLOAT) .17,
    (FLOAT) .17, (FLOAT)-.17, (FLOAT) .50,
    (FLOAT) .50, (FLOAT)-.17, (FLOAT)-.17,
    (FLOAT) .50, (FLOAT)-.17, (FLOAT) .17,
};

/*------------------------------------------------------------------------*/
/*                              Définition Cube                           */

/*        4_________5
 *       /|         /
 *     0/_________1/|
 *      | |   0   | |
 *      |7|_______|_|6
 *      |/        |/
 *     3|_________|2
 */

static OOTexture Cube_Textures[] = {
	{ 0, 0, 128, 128, (FLOAT)1, (FLOAT)1, OOF_TEXTURE_STRETCH, 0, NULL }
};

static UWORD Cube_Faces[] = {
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    0,3,2,1,
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    4,7,3,0,
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    4,0,1,5,
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    1,2,6,5,
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    3,7,6,2,
    4, 4,7, OOF_LINES|OOF_HIDE|OOF_MAP, 0,    5,6,7,4,
    ENDFACES
};

static OO3DPoint Cube_Coords[] = {
    (FLOAT)-.80, (FLOAT)+.80, (FLOAT)-.80,
    (FLOAT)+.80, (FLOAT)+.80, (FLOAT)-.80,
    (FLOAT)+.80, (FLOAT)-.80, (FLOAT)-.80,
    (FLOAT)-.80, (FLOAT)-.80, (FLOAT)-.80,
    (FLOAT)-.80, (FLOAT)+.80, (FLOAT)+.80,
    (FLOAT)+.80, (FLOAT)+.80, (FLOAT)+.80,
    (FLOAT)+.80, (FLOAT)-.80, (FLOAT)+.80,
    (FLOAT)-.80, (FLOAT)-.80, (FLOAT)+.80,
};

/*------------------------------------------------------------------------*/
/*                              Définition vaisseau                       */

static UWORD Ship_Faces[] = {
    4, 1,7,  OOF_HIDE|OOF_FILL,  0,    0,1,2,3,       /* dessus cokpit */
    3, 12,14,OOF_HIDE|OOF_FILL,  1,    1,4,2,         /* cokpit droit */
    3, 12,14,OOF_HIDE|OOF_FILL,  2,    3,5,0,         /* cokpit gauche */
    4, 12,15,OOF_HIDE|OOF_FILL,  3,    2,4,5,3,       /* cokpit avant */
    4, 1,7,  OOF_HIDE|OOF_FILL,  4,    4,6,7,5,       /* pointe avant */
    3, 1,6,  OOF_HIDE|OOF_FILL,  5,    1,6,4,         /* pointe avant droite */
    3, 1,6,  OOF_HIDE|OOF_FILL,  6,    0,5,7,         /* pointe avant gauche */
    3, 1,5,  OOF_HIDE|OOF_FILL,  7,    1,9,6,         /* pointe avant droite latérale */
    3, 1,5,  OOF_HIDE|OOF_FILL,  8,    0,7,8,         /* pointe avant gauche latérale */
    4, 1,1,  OOF_HIDE|OOF_FILL,  9,    10,12,13,11,   /* dessous */
    4, 1,2,  OOF_HIDE|OOF_FILL, 10,    7,6,12,10,     /* dessous avant */
    4, 1,2,  OOF_HIDE|OOF_FILL, 11,    11,13,15,14,   /* dessous arrière */
    3, 1,3,  OOF_HIDE|OOF_FILL, 12,    8,7,10,        /* dessous avant gauche */
    3, 1,2,  OOF_HIDE|OOF_FILL, 13,    8,10,11,       /* dessous gauche */
    3, 1,3,  OOF_HIDE|OOF_FILL, 14,    8,11,14,       /* dessous arrière gauche */
    3, 1,3,  OOF_HIDE|OOF_FILL, 15,    6,9,12,        /* dessous avant droit */
    3, 1,2,  OOF_HIDE|OOF_FILL, 16,    9,13,12,       /* dessous droit */
    3, 1,3,  OOF_HIDE|OOF_FILL, 17,    9,15,13,       /* dessous arrière droit */
    3, 1,4,  OOF_HIDE|OOF_FILL, 18,    8,14,17,       /* côté gauche */
    3, 1,5,  OOF_HIDE|OOF_FILL, 19,    0,8,17,        /* côté gauche dessus */
    3, 1,4,  OOF_HIDE|OOF_FILL, 20,    9,16,15,       /* côté droit */
    3, 1,5,  OOF_HIDE|OOF_FILL, 21,    9,1,16,        /* côté droit dessus */
    4, 1,6,  OOF_HIDE|OOF_FILL, 22,    1,0,17,16,     /* dessus arrière */
    4, 1,3,  OOF_HIDE|OOF_FILL, 23,    14,15,16,17,   /* derrière */
    4, 1,8,  OOF_HIDE|OOF_FILL, 24,    18,19,20,21,   /* tuyère */
    4, 1,6,			  OOF_FILL, 25,    8,25,26,27,    /* aileron avant gauche */
    4, 1,6,			  OOF_FILL, 26,    22,9,24,23,    /* aileron avant droit */
    ENDFACES
};

static OO3DPoint Ship_Coords[] = {
    (FLOAT) 1.20, (FLOAT) 1.00, (FLOAT)-1.00,  /*  0  dessus avant */
    (FLOAT)-1.20, (FLOAT) 1.00, (FLOAT)-1.00,  /*  1 */
    (FLOAT) -.50, (FLOAT) 1.00, (FLOAT)-2.00,  /*  2 */
    (FLOAT)  .50, (FLOAT) 1.00, (FLOAT)-2.00,  /*  3  cokpit */
    (FLOAT) -.50, (FLOAT)  .40, (FLOAT)-2.50,  /*  4 */
    (FLOAT)  .50, (FLOAT)  .40, (FLOAT)-2.50,  /*  5 */
    (FLOAT) -.50, (FLOAT)   .0, (FLOAT)-4.00,  /*  6  avant */
    (FLOAT)  .50, (FLOAT)   .0, (FLOAT)-4.00,  /*  7 */
    (FLOAT) 1.50, (FLOAT)   .0, (FLOAT)-1.00,  /*  8 */
    (FLOAT)-1.50, (FLOAT)   .0, (FLOAT)-1.00,  /*  9 */
    (FLOAT)  .50, (FLOAT)-1.00, (FLOAT)-2.00,  /* 10  dessous */
    (FLOAT)  .50, (FLOAT)-1.00, (FLOAT) -.50,  /* 11 */
    (FLOAT) -.50, (FLOAT)-1.00, (FLOAT)-2.00,  /* 12 */
    (FLOAT) -.50, (FLOAT)-1.00, (FLOAT) -.50,  /* 13 */
    (FLOAT) 1.00, (FLOAT) -.30, (FLOAT) 3.00,  /* 14  arrière */
    (FLOAT)-1.00, (FLOAT) -.30, (FLOAT) 3.00,  /* 15 */
    (FLOAT)-1.50, (FLOAT)  .30, (FLOAT) 3.00,  /* 16 */
    (FLOAT) 1.50, (FLOAT)  .30, (FLOAT) 3.00,  /* 17 */

    (FLOAT) 1.00, (FLOAT) -.20, (FLOAT) 3.01,  /* 18  tuyère */
    (FLOAT)-1.00, (FLOAT) -.20, (FLOAT) 3.01,  /* 19 */
    (FLOAT)-1.20, (FLOAT)  .20, (FLOAT) 3.01,  /* 20 */
    (FLOAT) 1.20, (FLOAT)  .20, (FLOAT) 3.01,  /* 21 */
    (FLOAT)-1.00, (FLOAT)   .0, (FLOAT)-2.50,  /* 22  aileron avant droit */
    (FLOAT)-2.50, (FLOAT)   .0, (FLOAT)-2.00,  /* 23 */
    (FLOAT)-2.50, (FLOAT)   .0, (FLOAT)-1.00,  /* 24 */
    (FLOAT) 1.00, (FLOAT)   .0, (FLOAT)-2.50,  /* 25  aileron avant gauche */
    (FLOAT) 2.50, (FLOAT)   .0, (FLOAT)-2.00,  /* 26 */
    (FLOAT) 2.50, (FLOAT)   .0, (FLOAT)-1.00,  /* 27 */
};

//=====================================================================
//								Caméra

static UWORD Camera_Faces[] = {
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    0,1,2,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    0,2,3,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    0,4,5,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    0,5,1,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    1,5,6,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    1,6,2,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    2,6,7,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    2,7,3,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    3,7,4,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    3,4,0,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    4,7,6,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    4,6,5,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    8,17,9,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    8,16,17,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    9,18,10,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    9,17,18,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    10,19,11,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    10,18,19,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    11,20,12,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    11,19,20,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    12,21,13,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    12,20,21,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    13,22,14,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    13,21,22,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    14,23,15,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    14,22,23,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    15,16,8,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    15,23,16,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    16,24,17,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    17,24,18,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    18,24,19,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    19,24,20,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    20,24,21,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    21,24,22,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    22,24,23,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    23,24,16,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    25,27,26,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    25,28,27,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    26,31,30,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    26,27,31,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    27,32,31,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    27,28,32,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    28,29,32,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    28,25,29,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    29,31,32,
	3, 4,7, OOF_LINES|OOF_HIDE|OOF_FILL, 0,    29,30,31,
    ENDFACES
};

static OO3DPoint Camera_Coords[] = {
    (FLOAT) 0.296875, (FLOAT) 0.582031, (FLOAT) 0.253906,
    (FLOAT) 0.296875, (FLOAT) 0.578125, (FLOAT)-0.327881,
    (FLOAT)-0.281006, (FLOAT) 0.578125, (FLOAT)-0.327881,
    (FLOAT)-0.281006, (FLOAT) 0.582031, (FLOAT) 0.253906,
    (FLOAT) 0.296875, (FLOAT) 0.000000, (FLOAT) 0.253906,
    (FLOAT) 0.296875, (FLOAT) 0.000000, (FLOAT)-0.327881,
    (FLOAT)-0.281006, (FLOAT) 0.000000, (FLOAT)-0.327881,
    (FLOAT)-0.281006, (FLOAT) 0.000000, (FLOAT) 0.253906,
    (FLOAT) 0.167969, (FLOAT) 0.175781, (FLOAT) 0.250000,
    (FLOAT) 0.039063, (FLOAT) 0.093750, (FLOAT) 0.250000,
    (FLOAT)-0.109131, (FLOAT) 0.128906, (FLOAT) 0.250000,
    (FLOAT)-0.187256, (FLOAT) 0.257813, (FLOAT) 0.250000,
    (FLOAT)-0.152100, (FLOAT) 0.406250, (FLOAT) 0.250000,
    (FLOAT)-0.023193, (FLOAT) 0.488281, (FLOAT) 0.250000,
    (FLOAT) 0.125000, (FLOAT) 0.453125, (FLOAT) 0.250000,
    (FLOAT) 0.203125, (FLOAT) 0.320313, (FLOAT) 0.250000,
    (FLOAT) 0.167969, (FLOAT) 0.175781, (FLOAT) 0.558594,
    (FLOAT) 0.039063, (FLOAT) 0.093750, (FLOAT) 0.558594,
    (FLOAT)-0.109131, (FLOAT) 0.128906, (FLOAT) 0.558594,
    (FLOAT)-0.187256, (FLOAT) 0.257813, (FLOAT) 0.558594,
    (FLOAT)-0.152100, (FLOAT) 0.406250, (FLOAT) 0.558594,
    (FLOAT)-0.023193, (FLOAT) 0.488281, (FLOAT) 0.558594,
    (FLOAT) 0.125000, (FLOAT) 0.453125, (FLOAT) 0.558594,
    (FLOAT) 0.203125, (FLOAT) 0.320313, (FLOAT) 0.558594,
    (FLOAT) 0.007813, (FLOAT) 0.289063, (FLOAT) 0.558594,
    (FLOAT) 0.156250, (FLOAT) 0.578125, (FLOAT)-0.331787,
    (FLOAT) 0.156250, (FLOAT) 0.578125, (FLOAT) 0.121094,
    (FLOAT) 0.156250, (FLOAT) 0.781250, (FLOAT)-0.034912,
    (FLOAT) 0.156250, (FLOAT) 0.781250, (FLOAT)-0.331787,
    (FLOAT)-0.136475, (FLOAT) 0.578125, (FLOAT)-0.331787,
    (FLOAT)-0.136475, (FLOAT) 0.578125, (FLOAT) 0.121094,
    (FLOAT)-0.136475, (FLOAT) 0.781250, (FLOAT)-0.034912,
    (FLOAT)-0.136475, (FLOAT) 0.781250, (FLOAT)-0.331787,
};

