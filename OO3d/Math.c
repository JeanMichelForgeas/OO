#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_3d_prv.h"

#ifdef AMIGA
void OO3DMath_Rotate( OO3DPoint *co, OO3DPoint *angle )
{
  FLOAT x, y, z, sina, cosa;

    if (FTST(angle->X))
        {
        sina = FSIN( angle->X );
        cosa = FCOS( angle->X );
        y = FADD(FMUL(co->Y, cosa), FMUL(co->Z, sina));
        z = FSUB(FMUL(co->Z, cosa), FMUL(co->Y, sina));
        co->Y = y;
        co->Z = z;
        }

    if (FTST(angle->Z))
        {
        sina = FSIN( angle->Z );
        cosa = FCOS( angle->Z );
        x = FADD(FMUL(co->X, cosa), FMUL(co->Y, sina));
        y = FSUB(FMUL(co->Y, cosa), FMUL(co->X, sina));
        co->X = x;
        co->Y = y;
        }

    if (FTST(angle->Y))
        {
        sina = FSIN( angle->Y );
        cosa = FCOS( angle->Y );
        x = FSUB(FMUL(co->X, cosa), FMUL(co->Z, sina));
        z = FADD(FMUL(co->Z, cosa), FMUL(co->X, sina));
        co->X = x;
        co->Z = z;
        }
}

FLOAT OO3DMath_PlanZ( FLOAT a, FLOAT x, FLOAT b, FLOAT y,  FLOAT d, FLOAT c )
{
    return( FNEG(FDIV( FADD(FADD(FMUL(a,x),FMUL(b,y)),d), c )) );
}

FLOAT OO3DMath_Normalize( OO3DPoint *n )
{
    FLOAT len;
    if (! FZERO(len = OO3DMath_Modulo( n )))
        OO3DMath_VecDiv( n, len );
    return len;
}

FLOAT OO3DMath_Normale( OO3DPoint *p0, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *n, BOOL normalize )
{
    OO3DPoint U, V;   // !!! Il faudrait faire une boucle pour trouver des points qui ne soient pas égaux
    FLOAT len = (FLOAT)0;

    U.X = FSUB( p1->X, p0->X ); // premier vecteur
    U.Y = FSUB( p1->Y, p0->Y );
    U.Z = FSUB( p1->Z, p0->Z );
    V.X = FSUB( p2->X, p0->X ); // deuxieme vecteur
    V.Y = FSUB( p2->Y, p0->Y );
    V.Z = FSUB( p2->Z, p0->Z );

    n->X = FSUB( FMUL(U.Y, V.Z), FMUL(U.Z, V.Y) );
    n->Y = FSUB( FMUL(U.Z, V.X), FMUL(U.X, V.Z) );
    n->Z = FSUB( FMUL(U.X, V.Y), FMUL(U.Y, V.X) );

    // Normalisation de la normale, est-ce normal ?
    if (normalize == TRUE) len = OO3DMath_Normalize( n );

    return len;
}

FLOAT OO3DMath_Modulo( OO3DPoint *p )
{
    return( FSQRT( FADD( FADD(FMUL(p->X,p->X), FMUL(p->Y,p->Y)), FMUL(p->Z,p->Z) ) ) );
}
#endif

#ifdef _WIN32
void OO3DMath_Rotate( OO3DPoint *co, OO3DPoint *angle )
{
  FLOAT x, y, z, sina, cosa;

    if (angle->X != (FLOAT)0)
        {
        sina = FSIN( angle->X );
        cosa = FCOS( angle->X );
        y = co->Y * cosa + co->Z * sina;
        z = co->Z * cosa - co->Y * sina;
        co->Y = y;
        co->Z = z;
        }

    if (angle->Z != (FLOAT)0)
        {
        sina = FSIN( angle->Z );
        cosa = FCOS( angle->Z );
        x = co->X * cosa + co->Y * sina;
        y = co->Y * cosa - co->X * sina;
        co->X = x;
        co->Y = y;
        }

    if (angle->Y != (FLOAT)0)
        {
        sina = FSIN( angle->Y );
        cosa = FCOS( angle->Y );
        x = co->X * cosa - co->Z * sina;
        z = co->Z * cosa + co->X * sina;
        co->X = x;
        co->Z = z;
        }
}

void OO3DMath_RotateArray( OO3DPoint *co, OO3DPoint *angle, ULONG count )
{
	ULONG choice = 0;
	FLOAT sinax, cosax;
	FLOAT sinay, cosay;
	FLOAT sinaz, cosaz;

	if (angle->X != (FLOAT)0)
		{
		sinax = FSIN( angle->X );
		cosax = FCOS( angle->X );
		choice |= 4;
		}
	if (angle->Y != (FLOAT)0)
		{
		sinay = FSIN( angle->Y );
		cosay = FCOS( angle->Y );
		choice |= 2;
		}
	if (angle->Z != (FLOAT)0)
		{
		sinaz = FSIN( angle->Z );
		cosaz = FCOS( angle->Z );
		choice |= 1;
		}

	if (choice)
		{
		FLOAT x, y, z;

		#define ROT_LOOP10 for (; count > 10; count-=10)
		#define ROT_LOOP for (; count; count--, co++)
		#define ROT_X	y = co->Y * cosax + co->Z * sinax;\
						z = co->Z * cosax - co->Y * sinax;\
						co->Y = y;						  \
						co->Z = z;
		#define ROT_Y	x = co->X * cosay + co->Y * sinay;\
						y = co->Y * cosay - co->X * sinay;\
						co->X = x;						  \
						co->Y = y;
		#define ROT_Z	x = co->X * cosaz - co->Z * sinaz;\
						z = co->Z * cosaz + co->X * sinaz;\
						co->X = x;						  \
						co->Z = z;

		switch (choice)
			{
			case 1: ROT_LOOP { ROT_Z			} break;
			case 2: ROT_LOOP { ROT_Y			} break;
			case 3: ROT_LOOP { ROT_Y ROT_Z		} break;
			case 4: ROT_LOOP { ROT_X			} break;
			case 5: ROT_LOOP { ROT_X ROT_Z		} break;
			case 6: ROT_LOOP { ROT_X ROT_Y		} break;
			case 7: ROT_LOOP { ROT_X ROT_Y ROT_Z} break;
			}
		}
}

void OO3DMath_AddArray( OO3DPoint *co, OO3DPoint *add, ULONG count )
{
	ULONG choice = 0;

	if (add->X != (FLOAT)0)	choice |= 4;
	if (add->Y != (FLOAT)0)	choice |= 2;
	if (add->Z != (FLOAT)0)	choice |= 1;

	if (choice)
		{
		#define ROT_LOOP10 for (; count > 10; count-=10)
		#define ROT_LOOP for (; count; count--, co++)
		#define ADD_X	co->X = FADD(co->X, add->X);
		#define ADD_Y	co->Y = FADD(co->Y, add->Y);
		#define ADD_Z	co->Z = FADD(co->Z, add->Z);

		switch (choice)
			{
			case 1: ROT_LOOP { ADD_Z			} break;
			case 2: ROT_LOOP { ADD_Y			} break;
			case 3: ROT_LOOP { ADD_Y ADD_Z		} break;
			case 4: ROT_LOOP { ADD_X			} break;
			case 5: ROT_LOOP { ADD_X ADD_Z		} break;
			case 6: ROT_LOOP { ADD_X ADD_Y		} break;
			case 7: ROT_LOOP { ADD_X ADD_Y ADD_Z} break;
			}
		}
}

void OO3DMath_SubArray( OO3DPoint *co, OO3DPoint *sub, ULONG count )
{
	ULONG choice = 0;

	if (sub->X != (FLOAT)0)	choice |= 4;
	if (sub->Y != (FLOAT)0)	choice |= 2;
	if (sub->Z != (FLOAT)0)	choice |= 1;

	if (choice)
		{
		#define ROT_LOOP10 for (; count > 10; count-=10)
		#define ROT_LOOP for (; count; count--, co++)
		#define SUB_X	co->X = FSUB(co->X, sub->X);
		#define SUB_Y	co->Y = FSUB(co->Y, sub->Y);
		#define SUB_Z	co->Z = FSUB(co->Z, sub->Z);

		switch (choice)
			{
			case 1:
				ROT_LOOP { SUB_Z } 
				break;
			case 2:
				ROT_LOOP { SUB_Y } 
				break;
			case 3: 
				ROT_LOOP { SUB_Y SUB_Z } 
				break;
			case 4: 
				ROT_LOOP { SUB_X } 
				break;
			case 5: 
				ROT_LOOP { SUB_X SUB_Z } 
				break;
			case 6: 
				ROT_LOOP { SUB_X SUB_Y } 
				break;
			case 7: 
				ROT_LOOP { SUB_X SUB_Y SUB_Z; } 
				break;
			}
		}
}

FLOAT OO3DMath_PlanZ( void *p, FLOAT X, FLOAT Y )
{
/*
    OOPlan *plan = p;
    return (FLOAT) - (plan->N.X*X + plan->N.X*Y + plan->D) / plan->N.Z;
*/
	return X;
}

FLOAT OO3DMath_Normalize( OO3DPoint *n )
{
    FLOAT len;
    if (len = (FLOAT) OOMath_Sqrt( n->X*n->X + n->Y*n->Y + n->Z*n->Z ))
        OO3DMath_VecDiv( n, len );
    return len;
}

FLOAT OO3DMath_Normale( OO3DPoint *p0, OO3DPoint *p1, OO3DPoint *p2, OO3DPoint *n, BOOL normalize )
{
    OO3DPoint U, V;
    FLOAT len = (FLOAT)0;

    U.X = p1->X - p0->X; // premier vecteur
    U.Y = p1->Y - p0->Y;
    U.Z = p1->Z - p0->Z;
    V.X = p2->X - p0->X; // deuxieme vecteur
    V.Y = p2->Y - p0->Y;
    V.Z = p2->Z - p0->Z;

    OO3DMath_CrossProduct( &U, &V, n );

    // Normalisation de la normale, est-ce normal ?
    if (normalize == TRUE) len = OO3DMath_Normalize( n );

    return len;
}

/*
void OO3DMath_ProjectTexture( OOPlan *plan, OOTexture *t )
{
    OO3DPoint M, N, *p = &plan->Points[0], *p1 = &plan->Points[plan->NumPoints-1], *p2 = &plan->Points[1];

    M.X = p1->X - p->X; // M : vecteur horizontal
    M.Y = p1->Y - p->Y;
    M.Z = p1->Z - p->Z;
    N.X = p->X - p2->X; // N : vecteur vertical
    N.Y = p->Y - p2->Y;
    N.Z = p->Z - p2->Z;

    OO3DMath_CrossProduct( p , &N, &plan->A );
    OO3DMath_CrossProduct( &M, p , &plan->B );
    OO3DMath_CrossProduct( &N, &M, &plan->C );
    OO3DMath_VecMul( &plan->A, (FLOAT)t->Width );
    OO3DMath_VecMul( &plan->B, (FLOAT)t->Height );
}
*/

FLOAT OO3DMath_Modulo( OO3DPoint *p )
{
    return (FLOAT) OOMath_Sqrt( p->X*p->X + p->Y*p->Y + p->Z*p->Z );
}
#endif

/*  PI    180 */
/* ---- = ----     x * 180 / PI = angle */
/*   x     90 */

FLOAT OO3DMath_Deg2Rad( FLOAT deg )
{
    return( FDIV(FMUL(deg, PI), (FLOAT)180) );
}

FLOAT OO3DMath_Rad2Deg( FLOAT rad )
{
    return( FDIV(FMUL(rad, (FLOAT)180), PI) );
}
