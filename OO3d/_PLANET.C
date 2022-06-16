
#include <oo/prvoo.h>
#include <oo/oo3d.h>

struct PlanetConst {
    UBYTE *Name;
    UWORD *DefFace;
    ULONG ke;
    FLOAT l0, lp, p0, pp, o0, op;
    FLOAT e, i, a;
    FLOAT Rayon, Masse;
    };

static FLOAT UA = 1.49597870e11; /* unité de distance astronomique */

static UWORD Faces_Mercure[] = { 0, 1,1, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,1, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Venus  [] = { 0, 1,2, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,2, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Terre  [] = { 0, 1,3, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,3, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Mars   [] = { 0, 1,4, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,4, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Jupiter[] = { 0, 1,1, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,1, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Saturne[] = { 0, 1,2, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,2, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Uranus [] = { 0, 1,3, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,3, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Neptune[] = { 0, 1,4, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,4, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };
static UWORD Faces_Pluton [] = { 0, 1,5, OOF_ELLIPSE|OOF_FFILL, 0, 0,   0, 6,5, OOF_LINES|OOF_ELLIPSE, 0, 0,   ENDFACES };

static struct PlanetConst pctab[] = {
    "Mercure", Faces_Mercure, 5, 4.01165674, 0.0714254534 , 1.3249318  , 7.42288531e-7, 0.823044915, 5.66184809e-7,     0.205615  , 0.1222254,  0.387098  ,  2439000, 3.3021780995e23,
    "Venus"  , Faces_Venus  , 3, 3.60860786, 0.0279631195 , 2.27161584 , 6.557202e-7  , 1.32290721 , 4.36681379e-7,     0.006816  , 0.0592301,  0.723330  ,  6052000, 4.8689977443e24,
    "Terre"  , Faces_Terre  , 3, 4.86885775, 0.0172027914 , 4.90847672 , 8.18559419e-7, 0          , 0            ,     0.01675104, 0        ,  1.00000023,  6378140, 5.9742420693e24,
    "Mars"   , Faces_Mars   , 5, 2.17756004, 9.14676586e-3, 5.83378048 , 8.79296877e-7, 0.851615955, 3.71231532e-7,     0.093309  , 0.0322939,  1.523678  ,  3397200, 6.4191227963e23,
    "Jupiter", Faces_Jupiter, 4, 4.68359105, 1.45088209e-3, 0.222302587, 7.69341134e-7, 1.7357823  , 4.82932604e-7,     0.048335  , 0.0228418,  5.202561  , 71398000, 1.8991650396e27,
    "Saturne", Faces_Saturne, 4, 4.86552417, 5.84648298e-4, 1.59029166 , 9.35845545e-7, 1.96855686 , 4.17308224e-7,     0.055892  , 0.0435027,  9.554747  , 60000000, 5.6855795341e26,
    "Uranus" , Faces_Uranus , 4, 4.3369163 , 2.05408149e-4, 2.99433923 , 7.09301808e-7, 1.28250284 , 2.38237443e-7,     0.046344  , 0.0134820, 19.21814   , 25400000, 8.6978005160e25,
    "Neptune", Faces_Neptune, 3, 1.5123627 , 1.05071439e-4, 0.815801799, 6.80678408e-7, 2.28102316 , 5.25169572e-7,     0.008997  , 0.0310536, 30.10957   , 24300000, 1.0298747023e26,
    "Pluton" , Faces_Pluton , 7, 1.6406095 , 7.01213952e-5, 3.89784382 , 6.67239373e-7, 1.90345608 , 6.67239373e-7,     0.250236  , 0.2996812, 39.438712  ,  2500000, 6.6303333333e23,
    /*
    "Soleil" , 0, 0         , 0            , 0          , 0            , 0          , 0            ,     0         , 0        , 0          ,696000000, 1.9891000000e30,
    */
    };

static ULONG tags_array[] = { D3A_Planet_Id, D3A_Planet_Name, D3A_Planet_e, D3A_Planet_i, D3A_Planet_a, D3A_Planet_L, D3A_Planet_P, D3A_Planet_O, D3A_Planet_v, D3A_Planet_b, D3A_Planet_l, D3A_Planet_r, TAG_END };

static FLOAT num_days( ULONG J, ULONG M, ULONG A, ULONG H, ULONG T, ULONG S )
{
  ULONG N;
  FLOAT HS;

    N = A*365 + 31*(M-1) + J;
    if (M <= 2) A = A - 1;
    N = N + A/4 - A/100 + A/400;
    if (M > 2) N = N - ((M-1)*4+27)/10;
    N = N - 694325;
    /* { ULONG sem; sem = N % 7; kprintf( "N  =%ld\nsem=%ld\n", N, sem ); } */

    HS = FDIV( F2FLT(H*(1000000000/24) + T*(1000000000/1440) + S*(1000000000/86400)), (FLOAT)1000000000 );
    return( FADD(F2FLT(N),HS) );
}

static void compute_direct( OOInst_3DPlanet *id )
{
    switch (id->Id)
        {
        case D3ID_MERCURE:
            break;

        case D3ID_VENUS  :
           break;

        case D3ID_TERRE  :
           break;

        case D3ID_MARS   :
           break;

        case D3ID_JUPITER:
           break;

        case D3ID_SATURNE:
           break;

        case D3ID_URANUS :
           break;

        case D3ID_NEPTUNE:
           break;

        case D3ID_PLUTON :
           break;

        default: break;
        }

}

static ULONG Dispatcher_3DPlanet( OOClass *cl, OObject *obj, OOMsg *msg )
{
  ULONG retval = 1;

    switch (msg->MethodId)
        {
        case OOM_NEW:
            {
            ULONG *pl, planetid = OOTag_GetTagData( D3A_Planet_Id, D3ID_TERRE, msg->AttrList );
            OOTagItem *tl, TL[5];
            struct PlanetConst *pc = &pctab[planetid];

            TL[0].ti_Tag = D3A_DefFaces ; TL[0].ti_Data = (ULONG)pc->DefFace;
            TL[1].ti_Tag = D3A_R        ; *(FLOAT*)&TL[1].ti_Data = pc->Rayon;
            TL[2].ti_Tag = D3A_Masse    ; *(FLOAT*)&TL[2].ti_Data = pc->Masse;
            TL[3].ti_Tag = D3A_Sphere	; TL[3].ti_Data = TRUE;
            TL[4].ti_Tag = TAG_MORE     ; TL[4].ti_Data = (ULONG)msg->AttrList;
            if (msg->AttrList == 0) TL[4].ti_Tag = TAG_END;

            msg->AttrList = TL;
            if (retval = OO_DoBaseMethod( cl, obj, method, attrlist ))
                {
                OOInst_3DPlanet *id = OINST_DATA(cl,retval);
                /*------ Initialise les ti_Tags */
                for (pl=tags_array, tl=(OOTagItem*)&id->tag_Id; (tl->ti_Tag=*pl++) != TAG_END; tl++);
                /*------ Initialise les ti_Datas */
                id->Id   = planetid;
                id->Name = pc->Name;
                id->e    = pc->e;
                id->i    = pc->i;
                id->a    = pc->a;
                /*------ Prend en compte la TagList d'init */
                OOTag_ApplyTagChanges( (OOTagItem*)&id->tag_Id, msg->AttrList );
                }
            msg->AttrList = (APTR)TL[4].ti_Data;
            }
            break;

        case OOM_UPDATE:
        case OOM_SET:
            OO_DoBaseMethod( cl, obj, method, attrlist );
            {
            OOInst_3DPlanet *id = OINST_DATA(cl,obj);
            OOTag_ApplyTagChanges( (OOTagItem*)&id->tag_Id, msg->AttrList );
            }
            break;

        case OOM_GET:
            OO_DoBaseMethod( cl, obj, method, attrlist );
            {
            OOInst_3DPlanet *id = OINST_DATA(cl,obj);
            OOTagItem *mtstate, *mti, *ti;
            for (mtstate=msg->AttrList; mti = OOTag_NextTagItem( &mtstate ); )
                *((ULONG*)mti->ti_Data) = (ti = OOTag_FindTagItem( mti->ti_Tag, (OOTagItem*)&id->tag_Id )) ? ti->ti_Data : 0;
            }
            break;

        /*--------------- */
        default:
            retval = OO_DoBaseMethod( cl, obj, method, attrlist );
            break;
        }

    return( retval );
}

static OOAttr attrs[] = {
    0 };

extern OOClass Class_3DSolid;
OOClass Class_3DPlanet = {
    { 0,0, 0, Dispatcher_3DPlanet, 0 },
    &Class_3DSolid,0,0,
    sizeof(OOInst_Root)+sizeof(OOInst_3DSolid),
    sizeof(OOInst_3DPlanet),0, attrs
    };
