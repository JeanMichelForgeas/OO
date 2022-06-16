#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_3d_prv.h"


static void InterplanX( FLOAT plan, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *res )
{
  FLOAT dx, dy, dz, px;

    dx = FSUB((p2->X), (p1->X)); // Ne peut pas être nul sinon les points seraient du même côté
    dy = FSUB((p2->Y), (p1->Y));
    dz = FSUB((p2->Z), (p1->Z));
    px = FSUB((p2->X), (plan ));

    res->X = plan;
    res->Y = FSUB( p2->Y, FDIV( FMUL(dy, px), dx ) );
    res->Z = FSUB( p2->Z, FDIV( FMUL(dz, px), dx ) );
}

static ULONG ClipLineX( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa )
{
  ULONG num = 0;

    if (FTST(plan) > 0) // Borne droite
        {
        if (FCMP(src[0].X,plan) <= 0)
            {
            dst[0] = src[0];
            if (FCMP(src[1].X,plan) <= 0) { dst[1] = src[1]; num = 1; }       // on reste dans le clip
            else { InterplanX( plan, &src[0], &src[1], &dst[1] ); num = 2; }  // on sort du clip
            }
        else if (FCMP(src[1].X,plan) <= 0)
            {
            dst[1] = src[1];
            InterplanX( plan, &src[0], &src[1], &dst[0] );                    // on rentre dans le clip
            num = 1; fa->_P.Flags |= OOF_CLIPPED;
            }
        }
    else{               // Borne gauche
        if (FCMP(src[0].X,plan) >= 0)
            {
            dst[0] = src[0];
            if (FCMP(src[1].X,plan) >= 0) { dst[1] = src[1]; num = 1; }       // on reste dans le clip
            else { InterplanX( plan, &src[0], &src[1], &dst[1] ); num = 2; }  // on sort du clip
            }
        else if (FCMP(src[1].X,plan) >= 0)
            {
            dst[1] = src[1];
            InterplanX( plan, &src[0], &src[1], &dst[0] );                    // on rentre dans le clip
            num = 1; fa->_P.Flags |= OOF_CLIPPED;
            }
        }
    return num;
}

ULONG IntOO_ClipPolygoneX( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints )
{
  ULONG i, num=0;

    if (numpoints == 1)
        {
        if (FTST(plan) > 0)
             if (FCMP(src[0].X,plan) <= 0) { *dst = *src; num = 1; }
        else if (FCMP(src[0].X,plan) >= 0) { *dst = *src; num = 1; }
        }
    else if (numpoints == 2)
        {
        num = ClipLineX( plan, src, dst, fa );
        }
    else{
        for (i=0; i < numpoints; i++)
            num += ClipLineX( plan, &src[i], dst + num, fa );
        if (num) dst[num] = dst[0];  // Le premier à la fin pour faciliter l'examen des segments
        }
    return num;
}

//******************************************************************************

static void InterplanY( FLOAT plan, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *res )
{
  FLOAT dx, dy, dz, py;

    dx = FSUB((p2->X), (p1->X));
    dy = FSUB((p2->Y), (p1->Y)); // Ne peut pas être nul sinon les points seraient du même côté
    dz = FSUB((p2->Z), (p1->Z));
    py = FSUB((p2->Y), (plan ));

    res->X = FSUB( p2->X, FDIV( FMUL(dx, py), dy ) );
    res->Y = plan;
    res->Z = FSUB( p2->Z, FDIV( FMUL(dz, py), dy ) );
}

static ULONG ClipLineY( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa )
{
  ULONG num = 0;

    if (FTST(plan) > 0) // Borne supérieure
        {
        if (FCMP(src[0].Y,plan) <= 0)
            {
            dst[0] = src[0];
            if (FCMP(src[1].Y,plan) <= 0) { dst[1] = src[1]; num = 1; }       // on reste dans le clip
            else { InterplanY( plan, &src[0], &src[1], &dst[1] ); num = 2; }  // on sort du clip
            }
        else if (FCMP(src[1].Y,plan) <= 0)
            {
            dst[1] = src[1];
            InterplanY( plan, &src[0], &src[1], &dst[0] );                    // on rentre dans le clip
            num = 1; fa->_P.Flags |= OOF_CLIPPED;
            }
        }
    else{               // Borne inférieure
        if (FCMP(src[0].Y,plan) >= 0)
            {
            dst[0] = src[0];
            if (FCMP(src[1].Y,plan) >= 0) { dst[1] = src[1]; num = 1; }       // on reste dans le clip
            else { InterplanY( plan, &src[0], &src[1], &dst[1] ); num = 2; }  // on sort du clip
            }
        else if (FCMP(src[1].Y,plan) >= 0)
            {
            dst[1] = src[1];
            InterplanY( plan, &src[0], &src[1], &dst[0] );                    // on rentre dans le clip
            num = 1; fa->_P.Flags |= OOF_CLIPPED;
            }
        }
    return num;
}

ULONG IntOO_ClipPolygoneY( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints )
{
  ULONG i, num=0;

    if (numpoints == 1)
        {
        if (FTST(plan) > 0)
             if (FCMP(src[0].Y,plan) <= 0) { *dst = *src; num = 1; }
        else if (FCMP(src[0].Y,plan) >= 0) { *dst = *src; num = 1; }
        }
    else if (numpoints == 2)
        {
        num = ClipLineY( plan, src, dst, fa );
        }
    else{
        for (i=0; i < numpoints; i++)
            num += ClipLineY( plan, &src[i], dst + num, fa );
        if (num) dst[num] = dst[0];  // Le premier à la fin pour faciliter l'examen des segments
        }
    return num;
}

//******************************************************************************

static void InterplanZ( FLOAT plan, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *res )
{
  FLOAT dx, dy, dz, pz;

    dx = FSUB((p2->X), (p1->X));
    dy = FSUB((p2->Y), (p1->Y));
    dz = FSUB((p2->Z), (p1->Z)); // Ne peut pas être nul sinon les points seraient du même côté
    pz = FSUB((p2->Z), (plan ));

    res->X = FSUB( p2->X, FDIV( FMUL(dx, pz), dz ) );
    res->Y = FSUB( p2->Y, FDIV( FMUL(dy, pz), dz ) );
    res->Z = plan;
}

static ULONG ClipLineZ( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa )
{
  ULONG num = 0;

    if (FCMP(src[0].Z,plan) >= 0)
        {
        dst[0] = src[0];
        if (FCMP(src[1].Z,plan) >= 0) { dst[1] = src[1]; num = 1; }     // on reste dans le clip
        else { InterplanZ( plan, &src[0], &src[1], &dst[1] ); num = 2; }  // on sort du clip
        }
    else if (FCMP(src[1].Z,plan) >= 0)
        {
        dst[1] = src[1];
        InterplanZ( plan, &src[0], &src[1], &dst[0] );                    // on rentre dans le clip
        num = 1; fa->_P.Flags |= OOF_CLIPPED;
        }

    return num;
}

ULONG IntOO_ClipPolygoneZ( FLOAT plan, OO3DPoint *src, OO3DPoint *dst, OOFace *fa, ULONG numpoints )
{
  ULONG i, num=0;

    if (numpoints == 1)
        {
        if (FCMP(src[0].Z,plan) >= 0) { *dst = *src; num = 1; }
        }
    else if (numpoints == 2)
        {
        num = ClipLineZ( plan, src, dst, fa );
        }
    else{
        for (i=0; i < numpoints; i++)
            num += ClipLineZ( plan, &src[i], dst + num, fa );
        if (num) dst[num] = dst[0];  // Le premier à la fin pour faciliter l'examen des segments
        }
    return num;
}
