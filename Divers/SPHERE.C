/*% cc -g sphere.c -o sphere -lm
 *
 * sphere - generate a triangle mesh approximating a sphere by
 *  recursive subdivision. First approximation is an octahedron;
 *  each level of refinement increases the number of triangles by
 *  a factor of 4.
 * Level 3 (128 triangles) is a good tradeoff if gouraud
 *  shading is used to render the database.
 *
 * Usage: sphere [level] [-p] [-c]
 *	level is an integer >= 1 setting the recursion level (default 1).
 *	-p causes generation of a PPHIGS format ASCII archive
 *	    instead of the default generic output format.
 *	-c causes triangles to be generated with vertices in counterclockwise
 *	    order as viewed from the outside in a RHS coordinate system.
 *	    The default is clockwise order.
 *
 *  The subroutines print_object() and print_triangle() should
 *  be changed to generate whatever the desired database format is.
 *
 * Jon Leech (leech@cs.unc.edu) 3/24/89
 */
#include <stdio.h>
#include <math.h>

typedef struct {
    double  x, y, z;
} point;

typedef struct {
    point     pt[3];	/* Vertices of triangle */
    double    area;	/* Unused; might be used for adaptive subdivision */
} triangle;

typedef struct {
    int       npoly;	/* # of triangles in object */
    triangle *poly;	/* Triangles */
} object;

/* Six equidistant points lying on the unit sphere */
#define XPLUS {  1,  0,  0 }	/*  X */
#define XMIN  { -1,  0,  0 }	/* -X */
#define YPLUS {  0,  1,  0 }	/*  Y */
#define YMIN  {  0, -1,  0 }	/* -Y */
#define ZPLUS {  0,  0,  1 }	/*  Z */
#define ZMIN  {  0,  0, -1 }	/* -Z */

/* Vertices of a unit octahedron */
triangle octahedron[] = {
    { { XPLUS, ZPLUS, YPLUS }, 0.0 },
    { { YPLUS, ZPLUS, XMIN  }, 0.0 },
    { { XMIN , ZPLUS, YMIN  }, 0.0 },
    { { YMIN , ZPLUS, XPLUS }, 0.0 },
    { { XPLUS, YPLUS, ZMIN  }, 0.0 },
    { { YPLUS, XMIN , ZMIN  }, 0.0 },
    { { XMIN , YMIN , ZMIN  }, 0.0 },
    { { YMIN , XPLUS, ZMIN  }, 0.0 }
};

/* A unit octahedron */
object oct = {
    sizeof(octahedron) / sizeof(octahedron[0]),
    &octahedron[0]
};

int PPHIGSflag = 0;

/* Forward declarations */
point *normalize(/* point *p */);
point *midpoint(/* point *a, point *b */);
void print_object(/* object *obj, int level */);
void print_triangle(/* triangle *t */);
void pphigs_header(/* int */);
void pphigs_trailer();

extern char *malloc(/* unsigned */);

main(ac, av)
int ac;
char *av[];
{
    object *old,
	   *new;
    int     ccwflag = 0,	/* Reverse vertex order if true */
	    i,
	    level,		/* Current subdivision level */
	    maxlevel = 1;	/* Maximum subdivision level */

    /* Parse arguments */
    for (i = 1; i < ac; i++) {
	if (!strcmp(av[i], "-p"))
	    PPHIGSflag = 1;
	else if (!strcmp(av[i], "-c"))
	    ccwflag = 1;
	else if ((maxlevel = atoi(av[i])) < 1) {
	    fprintf(stderr, "%s: # of levels must be >= 1\n", av[0]);
	    exit(1);
	}
    }

    if (ccwflag) {
	/* Reverse order of points in each triangle */
	for (i = 0; i < oct.npoly; i++) {
	    point tmp;
			  tmp = oct.poly[i].pt[0];
	    oct.poly[i].pt[0] = oct.poly[i].pt[2];
	    oct.poly[i].pt[2] = tmp;
	}
    }

    old = &oct;

    /* Subdivide each starting triangle (maxlevel - 1) times */
    for (level = 1; level < maxlevel; level++) {
	/* Allocate a new object */
	new = (object *)malloc(sizeof(object));
	if (new == NULL) {
	    fprintf(stderr, "%s: Out of memory on subdivision level %d\n",
		av[0], level);
	    exit(1);
	}
	new->npoly = old->npoly * 4;

	/* Allocate 4* the number of points in the current approximation */
	new->poly  = (triangle *)malloc(new->npoly * sizeof(triangle));
	if (new->poly == NULL) {
	    fprintf(stderr, "%s: Out of memory on subdivision level %d\n",
		av[0], level);
	    exit(1);
	}

	/* Subdivide each triangle in the old approximation and normalize
	 *  the new points thus generated to lie on the surface of the unit
	 *  sphere.
	 * Each input triangle with vertices labelled [0,1,2] as shown
	 *  below will be turned into four new triangles:
	 *
	 *			Make new points
	 *			    a = (0+2)/2
	 *			    b = (0+1)/2
	 *			    c = (1+2)/2
	 *	  1
	 *	 /\		Normalize a, b, c
	 *	/  \
	 *    b/____\ c		Construct new triangles
	 *    /\    /\		    [0,b,a]
	 *   /	\  /  \		    [b,1,c]
	 *  /____\/____\	    [a,b,c]
	 * 0	  a	2	    [a,c,2]
	 */
	for (i = 0; i < old->npoly; i++) {
	    triangle
		 *oldt = &old->poly[i],
		 *newt = &new->poly[i*4];
	    point a, b, c;

	    a = *normalize(midpoint(&oldt->pt[0], &oldt->pt[2]));
	    b = *normalize(midpoint(&oldt->pt[0], &oldt->pt[1]));
	    c = *normalize(midpoint(&oldt->pt[1], &oldt->pt[2]));

	    newt->pt[0] = oldt->pt[0];
	    newt->pt[1] = b;
	    newt->pt[2] = a;
	    newt++;

	    newt->pt[0] = b;
	    newt->pt[1] = oldt->pt[1];
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = b;
	    newt->pt[2] = c;
	    newt++;

	    newt->pt[0] = a;
	    newt->pt[1] = c;
	    newt->pt[2] = oldt->pt[2];
	}

	if (level > 1) {
	    free(old->poly);
	    free(old);
	}

	/* Continue subdividing new triangles */
	old = new;
    }

    /* Print out resulting approximation */
    print_object(old, maxlevel);
}

/* Normalize a point p */
point *normalize(p)
point *p;
{
    static point r;
    double mag;

    r = *p;
    mag = r.x * r.x + r.y * r.y + r.z * r.z;
    if (mag != 0.0) {
	mag = 1.0 / sqrt(mag);
	r.x *= mag;
	r.y *= mag;
	r.z *= mag;
    }

    return &r;
}

/* Return the midpoint on the line between two points */
point *midpoint(a, b)
point *a, *b;
{
    static point r;

    r.x = (a->x + b->x) * 0.5;
    r.y = (a->y + b->y) * 0.5;
    r.z = (a->z + b->z) * 0.5;

    return &r;
}

/* Write out all triangles in an object */
void print_object(obj, level)
object *obj;
int level;
{
    int i;

    if (PPHIGSflag)
	pphigs_header(level);

    /* Spit out coordinates for each triangle */
    for (i = 0; i < obj->npoly; i++)
	print_triangle(&obj->poly[i]);

    if (PPHIGSflag)
	pphigs_trailer();
}

/* Output a triangle */
void print_triangle(t)
triangle *t;
{
    int i;

    if (PPHIGSflag) {
	printf("\tpolygon 3 {\n");
	for (i = 0; i < 3; i++)
	    printf("\t\t%g\t%g\t%g %g\t%g\t%g ;\n",
		t->pt[i].x, t->pt[i].y, t->pt[i].z,	/* Point */
		t->pt[i].x, t->pt[i].y, t->pt[i].z);	/* Normal */
	printf("\t};\n");
    } else {
	/* Modify this to generate your favorite output format
	 * Triangle vertices are in t->pt[0..2].{x,y,z}
	 * A generic format is provided.
	 */
	printf("triangle\n");
	for (i = 0; i < 3; i++)
	    printf("\t%g %g %g\n", t->pt[i].x, t->pt[i].y, t->pt[i].z);
    }
}

/* Generate header for a PPHIGS ASCII archive */
void pphigs_header(level)
int level;
{
    int dx, dy, dz;

    printf("structure sphere%d posted {\n", level);
    printf("\tcolor polygon {\n");
    printf("\t\t200 100  50   0     50 100 200   0\n");
    printf("\t};\n");

    switch (level) {
	case 1:
	    dx = -2000; dy =  2000; dz = 0;
	    break;
	case 2:
	    dx =  2000; dy =  2000; dz = 0;
	    break;
	case 3:
	    dx = -2000; dy = -2000; dz = 0;
	    break;
	case 4:
	    dx =  2000; dy = -2000; dz = 0;
	    break;
	case 5:
	    dx =     0; dy =	 0; dz = 0;
	    break;
	default:
	    dx = dy = dz = 0;
	    break;
    }

    printf("\tmatrix Pre scale 1000 1000 1000;\n");
    printf("\tmatrix Pre translate %d %d %d ;\n", dx, dy, dz);
}

void pphigs_trailer() {
    printf("};\n");
}
