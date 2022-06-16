
static OOFltTagItem TL_FltSpace[] = {
    D3A_Gravity,    (FLOAT)10,
    TAGFLT_END
    };
static OOTagItem TL_Space[] = {
    OOV_TAG_MORE, (ULONG)TL_FltSpace,
    };

static OOFltTagItem TL_FltSoleil[] = {
    D3A_X, (FLOAT)-1.3e9F,
    D3A_Y, (FLOAT)0,
    D3A_Z, (FLOAT)2.e9F,
    TAGFLT_END
    };
static OOTagItem TL_Soleil[] = {
    OOV_TAG_MORE,	(ULONG)TL_FltSoleil,
    };

static OOFltTagItem TL_FltLight[] = {
    D3A_X, (FLOAT)-.5F,
    D3A_Y, (FLOAT) .5F,
    D3A_Z, (FLOAT) 1.2F,
    TAGFLT_END
    };
static OOTagItem TL_Light[] = {
    OOV_TAG_MORE,	(ULONG)TL_FltLight,
    };

static OOFltTagItem TL_FltShuttle[] = {
    D3A_X, (FLOAT)- 70.5F,	
    D3A_Y, (FLOAT)   0.F,	D3A_RotSIy, (FLOAT)10,
    D3A_Z, (FLOAT) 150.F,	D3A_RotSIz, (FLOAT)10,
    TAGFLT_END
    };
static OOTagItem TL_Shuttle[] = {
    OOA_FileName,	(ULONG)"Objects\\Shuttle.off",
	D3A_CenterSelf, TRUE,
    OOV_TAG_MORE,	(ULONG)TL_FltShuttle,
    };

static OOFltTagItem TL_FltSolid1[] = {
    D3A_X, (FLOAT)0,  D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,     D3A_RotSIx, (FLOAT)150,
    D3A_Y, (FLOAT)0,  D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,
    D3A_Z, (FLOAT)2,  D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Solid1[] = {
    D3A_DefFaces , (ULONG)Pyramide_Faces,
    D3A_DefCoords, (ULONG)Pyramide_Coords,
	D3A_CenterSelf, TRUE,
    OOV_TAG_MORE , (ULONG)TL_FltSolid1,
    };

static OOFltTagItem TL_FltBalle1[] = {
    D3A_X,  (FLOAT)0, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,	   D3A_RotSIx, (FLOAT)150,
    D3A_Y,  (FLOAT)0, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,
    D3A_Z,  (FLOAT)4, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Balle1[] = {
    D3A_DefFaces	, (ULONG)Boing_Faces,
    D3A_DefCoords	, (ULONG)Boing_Coords,
    //D3A_Sensitivity	, D3F_SENS_GRAVITY | D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltBalle1,
    };

static OOFltTagItem TL_FltBalle2[] = {
    D3A_X,  (FLOAT)2, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,
    D3A_Y,  (FLOAT)0, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,     D3A_RotSIy, (FLOAT)150,
    D3A_Z,  (FLOAT)4, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Balle2[] = {
    D3A_DefFaces	, (ULONG)Boing_Faces,
    D3A_DefCoords	, (ULONG)Boing_Coords,
    //D3A_Sensitivity	, D3F_SENS_GRAVITY | D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltBalle2,
    };

static OOFltTagItem TL_FltBalle3[] = {
    D3A_X,  (FLOAT)-2, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 0, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,
    D3A_Z,  (FLOAT) 4, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,     D3A_RotSIz, (FLOAT)150,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Balle3[] = {
    D3A_DefFaces	, (ULONG)Boing_Faces,
    D3A_DefCoords	, (ULONG)Boing_Coords,
    //D3A_Sensitivity	, D3F_SENS_GRAVITY | D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltBalle3,
    };

static OOFltTagItem TL_FltBalle4[] = {
    D3A_X,  (FLOAT)0, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,
    D3A_Y,  (FLOAT)2, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,     D3A_RotSIy, (FLOAT)150,
    D3A_Z,  (FLOAT)4, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,     D3A_RotSIz, (FLOAT) 50,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Balle4[] = {
    D3A_DefFaces	, (ULONG)Boing_Faces,
    D3A_DefCoords	, (ULONG)Boing_Coords,
    //D3A_Sensitivity	, D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltBalle4,
    };

static OOFltTagItem TL_FltBalle5[] = {
    D3A_X,  (FLOAT) 0, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,     D3A_RotSIx, (FLOAT) 50,
    D3A_Y,  (FLOAT)-2, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,     D3A_RotSIy, (FLOAT)150,
    D3A_Z,  (FLOAT) 4, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Balle5[] = {
    D3A_DefFaces	, (ULONG)Boing_Faces,
    D3A_DefCoords	, (ULONG)Boing_Coords,
    //D3A_Sensitivity	, D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltBalle5,
    };

static OOFltTagItem TL_FltSolid3[] = {
    D3A_X, (FLOAT)0, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,
    D3A_Y, (FLOAT)0, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,
    D3A_Z, (FLOAT)7, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,     D3A_RotSIz, (FLOAT)60,
    D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Solid3[] = {
    D3A_DefFaces	, (ULONG)Cube_Faces,
    D3A_DefCoords	, (ULONG)Cube_Coords,
    //D3A_Sensitivity	, D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltSolid3,
    };

static OOFltTagItem TL_FltShip[] = {
    D3A_X, (FLOAT)- 2  ,  D3A_RotSIx, (FLOAT)-15,
    D3A_Y, (FLOAT)- 1.5,  D3A_RotSIy, (FLOAT)-30,
    D3A_Z, (FLOAT) 11  ,  D3A_RotSIz, (FLOAT)-20,
    D3A_Masse, (FLOAT)1000,
    TAGFLT_END
    };
static OOTagItem TL_Ship[] = {
    D3A_DefFaces	, (ULONG)Ship_Faces,
    D3A_DefCoords	, (ULONG)Ship_Coords,
    //D3A_Sensitivity	, D3F_SENS_GRAVITY | D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltShip,
    };

static OOFltTagItem TL_FltCamera1[] = {
    D3A_X, (FLOAT)0,   D3A_WindowWidth,  (FLOAT) 1,
    D3A_Y, (FLOAT)0,   D3A_WindowHeight, (FLOAT).625,   D3A_RotSAy, (FLOAT)0,
    D3A_Z, (FLOAT)0,   D3A_ObserverDist, (FLOAT) 1,
    D3A_Masse,   (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera1[] = {
    D3A_DefFaces , (ULONG)Camera_Faces,
    D3A_DefCoords, (ULONG)Camera_Coords,
    D3A_Collide	  , FALSE,
	D3A_CenterSelf, TRUE ,
    OOV_TAG_MORE, (ULONG)TL_FltCamera1,
    };

static OOTagItem TL_Screen1[] = {
    OOA_AreaAddr  , (ULONG)&P.area1,
    D3A_ConnectObj, (ULONG)&P.camera1,
    D3A_FillColor , 0,
	TAG_END
    };

static OOFltTagItem TL_FltCamera2[] = {
    D3A_X, (FLOAT)2.5, D3A_WindowWidth , (FLOAT)1    ,
    D3A_Y, (FLOAT)0  , D3A_WindowHeight, (FLOAT)0.625,    D3A_RotSAy, (FLOAT)-35, /* PI=180 */
    D3A_Z, (FLOAT)0  , D3A_ObserverDist, (FLOAT)1    ,
    D3A_Masse  , (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera2[] = {
    D3A_DefFaces , (ULONG)Camera_Faces,
    D3A_DefCoords, (ULONG)Camera_Coords,
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_Collide	  , FALSE,
	D3A_CenterSelf, TRUE ,
    D3A_Visible	  , FALSE,
    OOV_TAG_MORE, (ULONG)TL_FltCamera2,
    };

static OOTagItem TL_Screen2[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_ConnectObj, (ULONG)&P.camera2,
    D3A_FillColor , 255,
	TAG_END
    };

//****************************************************************
//**********************   Test rotations   **********************
//****************************************************************

static OOFltTagItem TL_FltPseudoSoleil[] = {
    D3A_X,  (FLOAT)-4, 
    D3A_Y,  (FLOAT) 0, 
    D3A_Z,  (FLOAT) 8, 
	D3A_RotCx, (FLOAT)-4,   D3A_RotAx, (FLOAT)0,   D3A_RotIx, (FLOAT)0,
	D3A_RotCy, (FLOAT) 0,   D3A_RotAy, (FLOAT)0,   D3A_RotIy, (FLOAT)100,
	D3A_RotCz, (FLOAT) 9,   D3A_RotAz, (FLOAT)0,   D3A_RotIz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_PseudoSoleil[] = {
    D3A_DefFaces , (ULONG)Boing_Faces,
    D3A_DefCoords, (ULONG)Boing_Coords,
	D3A_CenterAbs, TRUE ,
    OOV_TAG_MORE , (ULONG)TL_FltPseudoSoleil,
    };

static OOFltTagItem TL_FltPseudoTerre[] = {
    D3A_X,  (FLOAT)-4  ,
    D3A_Y,  (FLOAT) 0  ,
    D3A_Z,  (FLOAT) 5,
	D3A_RotAx, (FLOAT)0,   D3A_RotIx, (FLOAT)50, 
	D3A_RotAy, (FLOAT)0,   D3A_RotIy, (FLOAT)0, 
	D3A_RotAz, (FLOAT)0,   D3A_RotIz, (FLOAT)0,  
    TAGFLT_END
    };
static OOTagItem TL_PseudoTerre[] = {
    D3A_DefFaces , (ULONG)Boing_Faces,
    D3A_DefCoords, (ULONG)Boing_Coords,
	D3A_AttachObj, (ULONG)&P.soleil,
	D3A_CenterObj, TRUE,
	D3A_Audible  , TRUE,
	OOA_SoundFileName, (ULONG)"Sounds\\Plane.wav",
    OOV_TAG_MORE , (ULONG)TL_FltPseudoTerre,
    };

static OOFltTagItem TL_FltPseudoLune[] = {
    D3A_X,  (FLOAT)-4,
    D3A_Y,  (FLOAT) 0,
    D3A_Z,  (FLOAT) 6.5,
	D3A_RotAx, (FLOAT)0,	D3A_RotIx, (FLOAT)0,  
	D3A_RotAy, (FLOAT)0,	D3A_RotIy, (FLOAT)300,
	D3A_RotAz, (FLOAT)0,	D3A_RotIz, (FLOAT)0,  
    TAGFLT_END
    };
static OOTagItem TL_PseudoLune[] = {
    D3A_DefFaces , (ULONG)Boing_Faces,
    D3A_DefCoords, (ULONG)Boing_Coords,
	D3A_AttachObj, (ULONG)&P.terre,
	D3A_CenterObj, TRUE,
    OOV_TAG_MORE , (ULONG)TL_FltPseudoLune,
    };

static OOFltTagItem TL_FltMur1[] = {
    D3A_X,  (FLOAT)-4.6,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 0, 		D3A_RotSAy, (FLOAT)-90,
    D3A_Z,  (FLOAT) 8, 		D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Mur1[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMur1,
    };

static OOFltTagItem TL_FltMur2[] = {
    D3A_X,  (FLOAT)-4.6,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 0, 		D3A_RotSAy, (FLOAT)-90,
    D3A_Z,  (FLOAT) 11,		D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Mur2[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMur2,
    };

static OOFltTagItem TL_FltMur3[] = {
    D3A_X,  (FLOAT)-4.6,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 0, 		D3A_RotSAy, (FLOAT)-90,
    D3A_Z,  (FLOAT) 5,		D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Mur3[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMur3,
    };

static OOFltTagItem TL_FltMur4[] = {
    D3A_X,  (FLOAT)-4.6,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 3, 		D3A_RotSAy, (FLOAT)-90,
    D3A_Z,  (FLOAT) 8,		D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Mur4[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMur4,
    };

static OOFltTagItem TL_FltMur5[] = {
    D3A_X,  (FLOAT)-4.6,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT)-3,		D3A_RotSAy, (FLOAT)-90,
    D3A_Z,  (FLOAT) 8,		D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Mur5[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMur5,
    };

static OOFltTagItem TL_FltCamera3[] = {
    D3A_X, (FLOAT)2.5, D3A_WindowWidth , (FLOAT)1    ,
    D3A_Y, (FLOAT)0  , D3A_WindowHeight, (FLOAT)0.625,    D3A_RotSAy, (FLOAT)-35, D3A_RotSIy, (FLOAT)-5,
    D3A_Z, (FLOAT)0  , D3A_ObserverDist, (FLOAT)1    ,
	D3A_RotCx, (FLOAT)-4,   D3A_RotIx, (FLOAT)0,
	D3A_RotCy, (FLOAT) 0,   D3A_RotIy, (FLOAT)5,
	D3A_RotCz, (FLOAT) 9,   D3A_RotIz, (FLOAT)0,
    D3A_Masse  , (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera3[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_Collide	  , FALSE,
	D3A_CenterAbs , TRUE ,
	D3A_CenterSelf, TRUE ,
    OOV_TAG_MORE, (ULONG)TL_FltCamera3,
    };

static OOTagItem TL_Screen3[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_ConnectObj, (ULONG)&P.camera3,
    D3A_FillColor , 0,
	TAG_END
    };

static OOTagItem TL_Mike[] = {
	D3A_AttachObj, (ULONG)&P.camera3,
    OOV_TAG_MORE, (ULONG)TL_FltCamera3,	// Pour commencer au même endroit que la caméra
    };

static OOTagItem TL_Speaker[] = {
    D3A_ConnectObj, (ULONG)&P.mike,
	TAG_END
    };


//****************************************************************
//**********************   Test Simple      **********************
//****************************************************************

static OOFltTagItem TL_FltMurS[] = {
    D3A_X,  (FLOAT) 2,	D3A_RotSAx, (FLOAT)0,
    D3A_Y,  (FLOAT) 0, 	D3A_RotSAy, (FLOAT)0,
    D3A_Z,  (FLOAT) 3,	D3A_RotSAz, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_MurS[] = {
    D3A_DefFaces	, (ULONG)Mur_Faces,
    D3A_DefCoords	, (ULONG)Mur_Coords,
    D3A_DefTextures	, (ULONG)Mur_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
	D3A_AttachObj	, (ULONG)&P.soleil,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltMurS,
    };

//****************************************************************
//**********************   Test 3DS		    **********************
//****************************************************************

static OOFltTagItem TL_FltCamera4[] = {
    D3A_X, (FLOAT)0, D3A_WindowWidth , (FLOAT)1    ,
    D3A_Y, (FLOAT)0  , D3A_WindowHeight, (FLOAT)0.625,    
    D3A_Z, (FLOAT)-400  , D3A_ObserverDist, (FLOAT)1,
    D3A_Masse  , (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera4[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_Collide	  , FALSE,
	D3A_CenterAbs , TRUE ,
	D3A_CenterSelf, TRUE ,
    OOV_TAG_MORE, (ULONG)TL_FltCamera4,
    };
static OOTagItem TL_Screen4[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_ConnectObj, (ULONG)&P.camera4,
    D3A_FillColor , 255,
	TAG_END
    };

//****************************************************************
//**********************   Test Cube Texture**********************
//****************************************************************

static OOFltTagItem TL_FltCamera5[] = {
    D3A_X, (FLOAT)0,   D3A_WindowWidth,  (FLOAT) 1,
    D3A_Y, (FLOAT)0,   D3A_WindowHeight, (FLOAT).625,   D3A_RotSAy, (FLOAT)0,
    D3A_Z, (FLOAT)0,   D3A_ObserverDist, (FLOAT) 1,
    D3A_Masse,   (FLOAT)0,
    D3A_Profile, (FLOAT)0,
    TAGFLT_END
    };
static OOTagItem TL_Camera5[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_Collide	  , FALSE,
	D3A_CenterAbs , TRUE ,
	D3A_CenterSelf, TRUE ,
    OOV_TAG_MORE, (ULONG)TL_FltCamera5,
    };

static OOTagItem TL_Screen5[] = {
    OOA_AreaAddr  , (ULONG)&P.area2,
    D3A_ConnectObj, (ULONG)&P.camera5,
    D3A_FillColor , 255,
	TAG_END
    };
static OOFltTagItem TL_FltCube[] = {
    D3A_X, (FLOAT)0, D3A_SpeedVx, (FLOAT)0,    D3A_SpeedAx, (FLOAT)0,
    D3A_Y, (FLOAT)0, D3A_SpeedVy, (FLOAT)0,    D3A_SpeedAy, (FLOAT)0,
    D3A_Z, (FLOAT)7, D3A_SpeedVz, (FLOAT)0,    D3A_SpeedAz, (FLOAT)0,
	//D3A_RotSIx, (FLOAT)60,
	//D3A_RotSIz, (FLOAT)60,
	D3A_Masse, (FLOAT)100,
    TAGFLT_END
    };
static OOTagItem TL_Cube[] = {
    D3A_DefFaces	, (ULONG)Cube_Faces,
    D3A_DefCoords	, (ULONG)Cube_Coords,
    D3A_DefTextures	, (ULONG)Cube_Textures,
    D3A_ImageFileName,(ULONG)"Textures\\De2.bmp",
    //D3A_Sensitivity	, D3F_SENS_LUMINOSITY,
	D3A_CenterSelf	, TRUE,
    OOV_TAG_MORE	, (ULONG)TL_FltCube,
    };
