
#include <stdio.h>
#include <stdlib.h>

#include "ootypes.h"
#include "ooprotos.h"

#include "oo_array.h"
#include "oo_buf.h"
#include "oo_list.h"
#include "oo_pooli.h"
#include "oo_tags.h"
#include "oo_text.h"
#include "oo_pictures.h"
#include "oo_init.h"
#include "oo_class.h"
#include "oo_gui.h"
#include "oo_gfx.h"

#include "oo_class_prv.h"

static void Test_Arrays();
static void Test_Buffers();
static void Test_Class();
static void Test_Bitmaps();
static void Test_Lists();
static void Test_Memory();
static void Test_Tags();
static void Test_Text();
static void Test_Tree1();
static void Test_Float();

struct OptionChoice {
	char	c1, c2;
	void	(*func)();
	};
static struct OptionChoice OptionChoices[] = { 
	'a','A', Test_Arrays,
	'b','B', Test_Buffers,
	'c','C', Test_Class,
	'd','D', Test_Bitmaps,
	'F','f', Test_Float,
	'l','L', Test_Lists,
	'm','M', Test_Memory,
	't','T', Test_Tags,
	'x','X', Test_Text,
	'1','1', Test_Tree1,
	}; 

void main( int argc, char **argv )
{
    if (OO_InitV( OOA_Console, TRUE, TAG_END ) == OOV_ERROR) goto END_ERROR;

	for (;;)
		{
		char i, opt = 0;
		void (*func)();

		if (argc ==	2)
			{
			opt = *argv[1];
			}
		else{
			printf( "===================================\n" );
			printf( "	T E S T   M E N U\n" );
			printf( "\n" );
			printf( "	   A - Arrays\n" );
			printf( "	   B - Buffers\n" );
			printf( "	   C - Classe\n" );
			printf( "	   D - Bitmaps\n" );
			printf( "	   F - Float\n" );
			printf( "	   L - Lists\n" );
			printf( "	   M - Memory\n" );
			printf( "	   T - Tags\n" );
			printf( "	   X - Text\n" );
			printf( "	   1 - Tree 1\n" );
			printf( "	   Q - Quit\n" );
			printf( "\n" );
			printf( "Enter option ==> " );
			opt = (char) getchar();
			fflush( stdin );
			}
		if (opt == 'q' || opt == 'Q' || opt == '\n' || opt == '\r')
			break;

		for (func=NULL, i=0; i < sizeof(OptionChoices)/sizeof(struct OptionChoice); i++)
			if (opt == OptionChoices[i].c1 || opt == OptionChoices[i].c2)
				{ func = OptionChoices[i].func; break; }

		if (func != NULL)
			{
			printf( "\n" );
			(*func)();
			printf( "------------ Press RETURN..." );
			getchar();
			fflush( stdin );
			}
		else{ 
			printf( "*** Invalid choice\n" );
			fflush( stdout );
			if (argc > 1) break;
			}
		printf( "\n\n" );
		}

  END_ERROR:
    OO_Cleanup();
}


//****************************************************************
//**************************   ARRAYS   **************************
//****************************************************************

static void print_array( ULONG *ar )
{
	ULONG i, count = OOArray_GetCount( &ar );
	printf( "*** %2d :", count );
	for (i=0; i < count; i++)
		printf( " %d", OOArray_Get( &ar, i ) );
	printf( "\n" );
}

static void Test_Arrays()
{
	ULONG *ar=0;

	if (ar = OOArray_Alloc( NULL, 3, 4 ))
		{
		ar[0] = 10;
		ar[1] = 20;
		ar[2] = 30;
		printf( "---  3 : 10 20 30\n" );
		print_array( ar );

		OOArray_SetCount( &ar, 5 );
		OOArray_Set( &ar, 0, 100 );
		OOArray_Set( &ar, 1, 200 );
		OOArray_Set( &ar, 2, 300 );
		OOArray_Set( &ar, 3, 400 );
		OOArray_Set( &ar, 4, 500 );
		printf( "---  5 : 100 200 300 400 500\n" );
		print_array( ar );

		OOArray_Insert( &ar, 0, 1000 );
		OOArray_Insert( &ar, 3, 3000 );
		OOArray_AddTail( &ar, 6000 );
		printf( "---  8 : 1000 100 200 3000 300 400 500 6000\n" );
		print_array( ar );

		OOArray_Remove( &ar, 0 );
		OOArray_Remove( &ar, 2 );
		OOArray_RemoveTail( &ar );
		printf( "---  5 : 100 200 300 400 500\n" );
		print_array( ar );

		OOArray_Insert( &ar, 9, 999 );
		printf( "--- 10 : 100 200 300 400 500 0 0 0 0 999\n" );
		print_array( ar );

		OOArray_SetCount( &ar, 5 );
		printf( "---  5 : 100 200 300 400 500\n" );
		print_array( ar );

		OOArray_Free( &ar );
		}
}


//****************************************************************
//**************************   BUFFERS   *************************
//****************************************************************

static void Test_Buffers()
{
	char *b1, *b2=NULL, *b3=NULL;

	b1 = OOBuf_Alloc( NULL, 10, 4 );
	if (b1 != NULL)
		{
		strcpy( b1, "0123456789" );
		OOBuf_Clone( b1, &b2 );
		OOBuf_Copy( b1, &b3, 3,4 );
		if (b2 != NULL && b3 != NULL)
			{
			char* b4;
			UWORD uw = 100;
			ULONG ul = 777888999;

			OOBuf_SetDatas( b1, uw, ul );

			printf( "--- 10:'0123456789' - 10:'0123456789' - 4:'3456'\n" );
			printf( "*** %d:'%s' - %d:'%s' - %d:'%s'\n", OOBuf_Length(b1), b1, OOBuf_Length(b2), b2 , OOBuf_Length(b3), b3 );

			OOBuf_Set( "ABCDEF", 6, &b1 );
			OOBuf_Cut( &b2, 0, 2 );
			OOBuf_Cut( &b2, 2, 3 );
			OOBuf_Cut( &b2, 4, 20 );
			OOBuf_Paste( "abcedf", 3, &b3, 0, 0 );
			OOBuf_Paste( "...___===@@@&&&", 12, &b3, MAXULONG, 0 );
			OOBuf_Paste( "###", 3, &b3, 25, '°' );
			printf( "--- 6:'ABCDEF' - 4:'2378' - 28:'abc3456...___===@@@°°°°°°###'\n" );
			printf( "*** %d:'%s' - %d:'%s' - %d:'%s'\n", OOBuf_Length(b1), b1, OOBuf_Length(b2), b2 , OOBuf_Length(b3), b3 );

			OOBuf_Resize( &b1, 3, FALSE );
			OOBuf_Truncate( b3, 5 );
			printf( "--- 3:'ABC' - 4:'2378' - 5:'abc34'\n" );
			printf( "*** %d:'%s' - %d:'%s' - %d:'%s'\n", OOBuf_Length(b1), b1, OOBuf_Length(b2), b2 , OOBuf_Length(b3), b3 );

			b4=NULL;
			OOBuf_FormatV( &b4, 0, "+++ %d:'%s' - %d:'%s' - %d:'%s'", OOBuf_Length(b1), b1, OOBuf_Length(b2), b2 , OOBuf_Length(b3), b3 );
			printf( "%s\n", b4 );

			OOBuf_Truncate( b4, 0 );
		    OOBuf_FormatV( &b4, 0, "--- %6.4f %ld %hd %s", (double)3.7, 10, (short)65740, "ouf!" );
            printf( "%s\n", b4 );
            printf( "*** %6.4f %ld %hd %s\n", (double)3.7, 10, (short)65740, "ouf!" );
			OOBuf_Free( &b4 );

			ul = OOBuf_GetDatas( b1, &uw );
			printf( "--- 100 777888999\n" );
			printf( "*** %d %d\n", uw, ul );
			}
		OOBuf_Free( &b1 );
		OOBuf_Free( &b2 );
		OOBuf_Free( &b3 );
		}
}


//****************************************************************
//**************************   BITMAPS   *************************
//****************************************************************

static void Test_Bitmaps()
{
	BITMAPINFO	*dib;
	HBITMAP		hbm;
	
	if ((dib = OOGfx_DIB_Alloc( 320, 200, 4, TRUE )) != NULL)
		{
		RGBQUAD *rgb = dib->bmiColors;
		ULONG size = OOGfx_DIB_PaletteSize( dib );
		ULONG num = OOGfx_DIB_NumTableColors( dib );
		if ((hbm = OOGfx_DIB_ToBitmap( dib, FALSE )) != NULL)
			{
			}
		}
}


//****************************************************************
//**************************   FLOAT   ***************************
//****************************************************************

#include "oo_3d.h"

static void Test_Float()
{
	FLOAT f1, f2;
	SLONG i;
	ULONG t1, t2;

	f1 = -2.345F;
	f2 = (FLOAT)fabs(f1);
	printf( "abs(%f) -> %f\n", f1, f2 );

	f1 = 2.4F;
	f2 = 2.3F;
	printf( "max(%f,%f) -> %f\n", f1, f2, __max(f1,f2) );

	t1 = GetTickCount();
	f1 = 7652987.623F;
	for (i=0; i < 1000000; i++)
		f2 = FSQRT(f1);
	t2 = GetTickCount();
	printf( "FSQRT-> %d ms\n", t2-t1 );

	t1 = GetTickCount();
	f1 = 7652987.623F;
	for (i=0; i < 1000000; i++)
		f2 = FSIN(f1);
	t2 = GetTickCount();
	printf( "FSIN-> %d ms\n", t2-t1 );

	t1 = GetTickCount();
	f1 = 7652987.623F;
	for (i=0; i < 1000000; i++)
		f2 = FADD(f1,98764.1654F);
	t2 = GetTickCount();
	printf( "FADD-> %d ms\n", t2-t1 );
}


//****************************************************************
//**************************   LISTS   ***************************
//****************************************************************

struct TestNode {
	OONode	Node;
	ULONG	Val;
	};

static void print_list( OOList *list )
{
	ULONG count;
	OONode *node, *next;

	for (count=0, node=list->lh_Head; next = node->ln_Succ; node = next, count++) ;
	printf( "*** %d :", count );
	for (node=list->lh_Head; node->ln_Succ; node = node->ln_Succ)
		printf( " %d", ((struct TestNode*)node)->Val );
	printf( "\n" );
}

static void Test_Lists()
{
	OOList L;
	struct TestNode N1, N2, N3, N4, N5, *n;

	N1.Val = 1;
	N2.Val = 2;
	N3.Val = 3;
	N4.Val = 4;
	N5.Val = 5;
	OOList_Init( &L );
	OOList_AddTail( &L, (OONode*)&N4 );
	OOList_AddHead( &L, (OONode*)&N1 );
	OOList_Insert( &L, (OONode*)&N3, (OONode*)&N4 );
	OOList_InsertAfter( &L, (OONode*)&N2, (OONode*)&N1 );
	OOList_AddTail( &L, (OONode*)&N5 );

	n = (struct TestNode*) OOList_GetHead( &L );
	printf( "First node : 1 == %d\n", n->Val );
	printf( "--- 5 : 1 2 3 4 5\n" );
	print_list( &L );

	OOList_RemHead( &L );
	OOList_RemTail( &L );
	OOList_Remove( (OONode*)&N3 );
	printf( "--- 2 : 2 4\n" );
	print_list( &L );
}


//****************************************************************
//**************************   MEMORY   **************************
//****************************************************************

/*
void			OOIPool_Reset			( void *pool );
void *			OOIPool_Resize		( void *pool, void *oldblock, ULONG newsize );
ULONG			OOIPool_GetLength		( void *ptr );
ULONG			OOIPool_GetCount		( void *pool );
void			OOIPool_EnableFree	( void *pool );
void			OOIPool_DisableFree	( void *pool );
BOOL			OOIPool_IsFreeEnabled	( void *pool );
BOOL			OOIPool_WritePool		( void *pool, FILE *file );
BOOL			OOIPool_ReadPool		( void *pool, FILE *file );
ULONG			OOIPool_GetId			( void *ptr );
ULONG			OOIPool_SetId			( void *pool, void *ptr, ULONG newid );
void *			OOIPool_GetAddress	( void *pool, ULONG id );
ULONG			OOIPool_GetLastId		( void *pool );
*/
static void Test_Memory()
{
	void *pool;

	if (pool = OOIPool_Create( 100, 0xff, 0 ))
		{
		ULONG id1, id2, id3, id4;

		OOIPool_Alloc( pool, 4, &id1, TRUE );
		OOIPool_Alloc( pool, 10, &id2, TRUE );
		OOIPool_Alloc( pool, 50, &id3, TRUE );
		OOIPool_Alloc( pool, 120, &id4, TRUE );

		id1 = OOIPool_FreeId( pool, id1 );
		OOIPool_Alloc( pool, 4, &id1, TRUE );

		id1 = OOIPool_FreeId( pool, id1 );
		id2 = OOIPool_FreeId( pool, id2 );
		id3 = OOIPool_FreeId( pool, id3 );
		id4 = OOIPool_FreeId( pool, id4 );

		OOIPool_Delete( pool );
		}
}


//****************************************************************
//**************************   TAGS   ****************************
//****************************************************************

static OOTagList TagsB[] =
    {
    { 3001,		4001 },
    { 3002,     4002 },
    { 3003,     4003 },
    { 3004,     4004 },
    { 3005,     4005 },
    { 3006,     4006 },
    { TAG_END }
    };
static OOTagList TagsA[] =
    {
    { 1000,     2000 },
    { 1001,     2001 },
    { 1002,     2002 },
    { TAG_END }
    };

static ULONG tagarray[] =
    { 1001, 1003, 1005, 1007, 3001, 3003, 3005, 3007, TAG_END };

#define TAG_USER	100
#define tag1  TAG_USER + 1
#define tag2  TAG_USER + 2
#define tag3  TAG_USER + 3
#define tag4  TAG_USER + 4
#define tag5  TAG_USER + 5

static OOTagItem boolmap[] =
    {
    { tag1,  0x0001 },
    { tag2,  0x0002 },
    { tag3,  0x0004 },
    { tag4,  0x0008 },
    { TAG_END }
    };
static OOTagItem boolexample[] =
    {
    { tag1,  TRUE },
    { tag2,  FALSE },
    { tag5,  45 },
    { tag3,  TRUE },
    { TAG_END }
    };

#define MY_SIZE    7000
#define MY_WEIGHT  7001
#define HIS_TALL   7002

static OOTagItem list[] = {
               { MY_SIZE,   71 },
               { MY_WEIGHT, 200 },
               { TAG_END } };
static OOTagItem map[] = {
               { MY_SIZE,   HIS_TALL },
               { TAG_END } };

#define ATTR_Size   0x8000L
#define ATTR_Color  0x8001L
#define ATTR_Shape  0x8002L
#define ATTR_Title  (0x8003L|TAGT_BYTE|TAGT_STR)

char toto[] = "OldList";
static OOTagItem ChangeList[] = {
               { ATTR_Size,  100 },
               { ATTR_Shape, 400 },
               { ATTR_Title, (ULONG)"ChangeList" },
               { TAG_END } };
static OOTagItem OldList[] = {
               { ATTR_Size,  100 },
               { ATTR_Color, 200 },
               { ATTR_Shape, 300 },
               { ATTR_Title, (ULONG)toto },
               { TAG_END } };


static ULONG print_tagitem( OOTagItem *ti, ULONG parm )
{
    printf( "     { %ld,\t%ld },\n", ti->ti_Tag, ti->ti_Data );
    return 10;
}

static void Test_Tags()
{
	OOTagItem *ti, *tagptr;
	OOTagItem *tic;

	printf( "Size of OOTagItem : %d\n", sizeof(OOTagItem) );
    printf( "OOTag_NextItem...\n" );
    tagptr = &TagsA[0];
    for (ti=tagptr; ti = OOTag_NextItem( &tagptr ); ) 
		print_tagitem( ti, 0 );

    printf("OOTag_Join...\n");
    OOTag_Join( TagsA, TagsB );
    OOTag_ApplyList( TagsA, print_tagitem, 0, 0, 0 );
	printf( "(Press RETURN)" ); getchar(); fflush( stdin );

    printf("OOTag_FindTag...\n");
    if (ti = OOTag_FindTag( TagsA, 3001 )) print_tagitem( ti, 0 ); else printf("    %ld not found\n", 3001);
    if (ti = OOTag_FindTag( TagsB, 3001 )) print_tagitem( ti, 0 ); else printf("    %ld not found\n", 3001);
    if (ti = OOTag_FindTag( TagsB, 3000 )) print_tagitem( ti, 0 ); else printf("    %ld not found\n", 3000);

    printf("OOTag_GetData...\n");
    printf("    (3001/1) %ld\n", OOTag_GetData( TagsA, 3001, 1 ) );
    printf("    (3004/4) %ld\n", OOTag_GetData( TagsA, 3004, 4 ) );
	printf( "(Press RETURN)" ); getchar(); fflush( stdin );

    printf("OOTag_InArray...\n");
    printf("    (3001) -> %ld (1)\n", OOTag_InArray( tagarray, 3001 ) );
    printf("    (3004) -> %ld (0)\n", OOTag_InArray( tagarray, 3004 ) );

    printf("OOTag_PackBool...\n");
    printf("    Flags (0x800002) => %lx (0x800005)\n", OOTag_PackBool( boolexample, boolmap, 0x800002 ) );

    printf("OOTag_MapTags...\n");
    OOTag_MapTags( list, map, 1 );
    OOTag_ApplyList( list, print_tagitem, 0, 0, 0 );
	printf( "(Press RETURN)" ); getchar(); fflush( stdin );

    printf("OOTag_Clone...\n");
    if (tic = OOTag_Clone( TagsA ))
        {
        OOTag_ApplyList( tic, print_tagitem, 0, 1, 10 );
        printf("...\n");
        OOTag_ApplyList( tic, print_tagitem, 0, 0, 0 );

        printf("OOTag_FilterList OOV_FILTER_AND...\n");
        printf( "    %ld valids -> (4)\n", OOTag_FilterList( tic, tagarray, OOV_FILTER_AND ) );
        OOTag_ApplyList( tic, print_tagitem, 0, 0, 0 );

        printf("OOTag_Copy...\n");
        OOTag_Copy( TagsA, tic );
        printf("OOTag_FilterList OOV_FILTER_NOT...\n");
        printf( "    %ld valids -> (5)\n", OOTag_FilterList( tic, tagarray, OOV_FILTER_NOT ) );
        OOTag_ApplyList( tic, print_tagitem, 0, 0, 0 );
        OOTag_Free( tic );
        }
	printf( "(Press RETURN)" ); getchar(); fflush( stdin );

    printf("OOTag_FilterChanges...\n");
    if (tic = OOTag_Clone( ChangeList ))
        {
        printf("OOTag_FilterChanges OOV_FILTER_KEEP...\n");
        OOTag_FilterChanges( tic, OldList, OOV_FILTER_KEEP );
        printf("    ChangeList:\n");
        OOTag_ApplyList( tic, print_tagitem, 0, 0, 0 );
        printf("    OldList:\n");
        OOTag_ApplyList( OldList, print_tagitem, 0, 0, 0 );

        OOTag_Copy( ChangeList, tic );
        printf("OOTag_FilterChanges OOV_FILTER_UPDATE...\n");
        OOTag_FilterChanges( tic, OldList, OOV_FILTER_UPDATE );
        printf("    ChangeList:\n");
        OOTag_ApplyList( tic, print_tagitem, 0, 0, 0 );
        printf("    OldList:\n");
        OOTag_ApplyList( OldList, print_tagitem, 0, 0, 0 );

        OOTag_Free( tic );
        }
}


//****************************************************************
//**************************   TEXT   ****************************
//****************************************************************

#define TEXT_1			1
#define TEXT_RESOURCE	2

#include "resource.h"

static OOTextItem text_table[] = 
	{
		{ 0 },
		{ 100, 0, 0, "Text 1" },
		{ IDS_STRING1, 0, OOF_TEXT_RESOURCE, NULL },
		{ 0, 0, 0, (char*)OOV_ENDTABLE },
	};


static void Test_Text()
{
	OOStr_SetTable( text_table );
	
	printf( "Text 1 : %s\n", OOStr_Get( TEXT_1 ) );
	printf( "Text from resource : %s\n", OOStr_Get( TEXT_RESOURCE ) );
	OOStr_Set( TEXT_1, "Text 3", FALSE );
	printf( "Text 3 : %s\n", OOStr_Get( TEXT_1 ) );

	OOStr_SetTable( NULL );
}


//****************************************************************
//**************************   CLASSEE   *************************
//****************************************************************

#define OOM_UP         (TAGT_PUB+1)
#define OOM_DOWN       (TAGT_PUB+2)

#define OOA_CurrVal    (TAGT_PUB+1)
#define OOA_Limit      (TAGT_PUB+2)

static ULONG SetGet_Currval( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		OOTagList *inst_tl = OOINST_ATTRS(sgp->Class,sgp->Obj);
		OOTagItem *oldlimitem = OOTag_FindTag( inst_tl, OOA_Limit );
		OOTagItem *newlimitem = OOTag_FindTag( sgp->ExtAttr, OOA_Limit );
		ULONG limit, new_value;

		// Get eventually a new limit in the new taglist
		limit = (newlimitem != NULL) ? newlimitem->ti_Data : oldlimitem->ti_Data;
		// Get the new value for the attr
		new_value = sgp->ExtAttr->ti_Data;
		// Eventually correct the new value
		if (new_value > limit)
			{
			new_value = limit;
			// In this case we must retest the difference
			if (new_value == sgp->Attr->ti_Data)
				return OOV_NOCHANGE;
			}
		// Set the attr and the passed tagitem to the new corrected value
		printf( "OOA_CurrVal : set to %d\n", new_value );
		sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data = new_value;
		return OOV_CHANGED;
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return OOV_NOCHANGE;
}

static ULONG SetGet_Limit( OOSetGetParms *sgp )
{
	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		OOTagList *currvalitem, *inst_tl = OOINST_ATTRS(sgp->Class,sgp->Obj);
		ULONG new_limit;

		// Get the new value for the attr
		new_limit = sgp->ExtAttr->ti_Data;

		// Reset current value attribute if limit is set lower
		currvalitem = OOTag_FindTag( inst_tl, OOA_CurrVal );
		if (currvalitem->ti_Data > new_limit)
			{
			printf( "OOA_CurrVal : set to %d\n", new_limit );
			currvalitem->ti_Data = new_limit;
			// Notify ourself because we change an attribute other than the
			// one that is passed here, and because the value is changed by 
			// hand instead of OOM_SET or OOPrv_MSet()
			OO_DoNotifyV( sgp->Obj, OOA_CurrVal, currvalitem->ti_Data, TAG_END );
			}

		// Set the attr and the passed tagitem to the new corrected value
		printf( "OOA_Limit   : set to %d\n", new_limit );
		sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data = new_limit;
		return OOV_CHANGED;
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return OOV_NOCHANGE;
}

static ULONG Dispatcher_MyClass( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
    switch (method)
        {
        //============== Required standard methods
        case OOM_NEW:
            if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
                {
				// Init here default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) == OOV_ERROR) { OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
                }
            return (ULONG)obj;
        case OOM_DELETE:
           return OO_DoBaseMethod( cl, obj, method, attrlist );
        case OOM_GET: // Do not call base method
            return OOPrv_MGet( cl, obj, attrlist );
        case OOM_SET: // Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

        //============== Custom methods
        case OOM_UP:
			{
			OOTagList *inst_tl = OOINST_ATTRS(cl,obj);
			ULONG newval = OOTag_GetData( inst_tl, OOA_CurrVal, 0 ) + 1;
			OOTagItem change_tl[] = { OOA_CurrVal, newval, TAG_END };
			return OOPrv_MSet( cl, obj, change_tl, OOF_MSET );
			}

        case OOM_DOWN:
			{
			OOTagList *inst_tl = OOINST_ATTRS(cl,obj);
			ULONG newval = OOTag_GetData( inst_tl, OOA_CurrVal, 0 );
			if (newval > 0)
				{
				newval--;
				{
				OOTagItem change_tl[] = { OOA_CurrVal, newval, TAG_END };
				return OOPrv_MSet( cl, obj, change_tl, OOF_MSET );
				}
				}
			return OOV_OK;
			}

		default:
            return OO_DoBaseMethod( cl, obj, method, attrlist );
			break;
		}
}

static OOAttrDesc attrs[] = {
    { OOA_CurrVal, 0, "CurrVal"	, SetGet_Currval, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { OOA_Limit  , 5, "Limit"	, SetGet_Limit	, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT },
    { TAG_END }
	};

struct Inst_MyClass { ULONG Toto; };

OOClass Class_MyClass = { 
	Dispatcher_MyClass, 
	&Class_Root,		// Base class
	"Class_MyClass",	// Class Name
	0,					// InstOffset
	sizeof(struct Inst_MyClass),// InstSize : Taille de l'instance
	attrs, 				// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};

static void Test_Class()
{
	OObject *obj1, *obj2;
	ULONG res;

	OO_InitClass( &Class_MyClass );

    if ((obj1 = OO_NewV( &Class_MyClass, OOA_CurrVal, 10, OOA_Limit, 100, TAG_END )) == NULL) goto END_ERROR;
    if ((obj2 = OO_NewV( &Class_MyClass, OOA_CurrVal, 20, OOA_Limit, 200, TAG_END )) == NULL) goto END_ERROR;

    if (OO_AddNotifyV(	obj1, OON_TRIG_EVERYVALUE, OOA_CurrVal, 0,0, 
						&obj2, OON_ACT_DOMETHODVALUE, OOM_SET, OOA_CurrVal, 0, TAG_END ) != NULL)
		{
	    OO_SetAttrsV( obj1, OOA_CurrVal, 5, TAG_END );
		printf( "obj2 OOA_CurrVal=%ld\n", OO_GetAttr( obj2, OOA_CurrVal ) );
		}
    OO_Delete( obj2 );
    obj2 = NULL;

    OO_SetAttr( obj1, OOA_CurrVal, 15 );
	OO_GetAttrsV( obj1, OOA_CurrVal, &res, TAG_END );
    printf( "obj1 OOA_CurrVal=%ld\n", res );
    OO_DoMethod( obj1, OOM_UP, NULL );
    printf( "obj1 OOA_CurrVal=%ld\n", OO_GetAttr( obj1, OOA_CurrVal ) );

    OO_Delete( obj1 );

  END_ERROR:;
}


//****************************************************************
//**************************   TREE 1   **************************
//****************************************************************

#define OOTEXT_TITLE_APPL		1
#define OOTEXT_TITLE_WINCTRL	2
#define OOTEXT_COPYRIGHT		3
#define OOTEXT_LONGLIST			4
#define OOTEXT_TITLE_BUTTON		5
#define OOTEXT_TITLE_GROUP		6
#define OOTEXT_TITLE_LIST		7
static OOTextItem text1_table[] = {
		{ 0 }, // 0 reserved
		{ 0, 0, 0, "Test 1" },
		{ 0, 0, 0, "Fenêtre des contrôles" },
		{ 0, 0, 0, "Copyright © 1998 Jean-Michel Forgeas\nAll Rights Reserved" },
		{ 0, 0, 0, "aaaaaaaaaa\0bbbbbbbbbbbbbbbbbb\0ccccccccccccccccc\0ddddddddddddddddddddd\0eeeeeeeeeeeeeeeeeeeee\0ffffffffffffffffffffff\0ggggggggggggggggggggggggg\0hhhhhhhhhhhhhhhhhhh\0" },
		{ 0, 0, 0, "Bouton" },
		{ 0, 0, 0, " Group " },
		{ 0, 0, 0, " List " },
		{ 0, 0, 0, (char*)OOV_ENDTABLE }, // end marker
	};

#define OOPIC_ICON_APPL	1
#define OOPIC_ICON_LOGO	2
static OOPicItem images_table[] = {
		{ 0 }, // 0 reserved
		{ MAKEINTRESOURCE(IDI_APPL), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ MAKEINTRESOURCE(IDI_LOGO), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ NULL, 0, 0, (void*)OOV_ENDTABLE }, // end marker
	};

struct OP {
    OObject *appl, *dbg, *gui;
    OObject *os1, *ow1, *ar1, *but1, *but2, *but3, *boxarea;
    OObject *radio1, *radio2, *radio3, *radio4;
	OObject *editnum1, *dispnum2, *edittext1;
	OObject *scroll1, *scrollv3;
	OObject *list;
	OObject *testobj;
    };
struct OP P;

OOTagList ApplTags[] = {
	OOA_TitleIndex	, OOTEXT_TITLE_APPL,
	OOA_ImageIndex	, OOPIC_ICON_APPL,
	TAG_END
	};

OOTagList WinTags[] = {
	OOA_TitleIndex		, OOTEXT_TITLE_WINCTRL,
	OOA_HorizGroup		, TRUE,
	OOA_Margins			, FALSE,
	OOA_Borderless		, FALSE,
	OOA_WinCtlDrag		, FALSE,
	OOA_WinCtlSize		, TRUE,
	OOA_WinCtlClose		, TRUE,
	OOA_WinCtlZoom		, TRUE,
	OOA_WinCtlIcon		, TRUE,
	TAG_END
	};

OOTagList AreaTags[] = {
	OOA_WidthHeight		, W2L(40,20),
	TAG_END
	};

OOTagList BoxAreaTags[] = {
	OOA_WidthHeight		, W2L(190,35),
	TAG_END
	};

OOTagList VertGroupTags[] = {
	OOA_VertGroup		, TRUE,
	OOA_Spacing			, TRUE,
	OOA_ExpandHeight	, TRUE,
	TAG_END
	};
OOTagList VertGroup2Tags[] = {
	OOA_VertGroup		, TRUE,
	OOA_Margins			, TRUE,
	OOA_ExpandHeight	, TRUE,
	TAG_END
	};
OOTagList HorizGroupTags[] = {
	OOA_HorizGroup		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

OOTagList VertGroupBoxTags[] = {
	OOA_TitleIndex		, OOTEXT_TITLE_GROUP,
	OOA_FontName		, (ULONG)"System",
	OOA_FontHeightP100	, 150,
	OOA_VertGroup		, TRUE,
	OOA_Margins			, TRUE,
	OOA_Spacing			, TRUE,
	OOA_ExpandHeight	, TRUE,
	TAG_END
	};
OOTagList HorizGroupBoxTags[] = {
	OOA_TitleIndex		, OOTEXT_TITLE_GROUP,
	OOA_FontBold		, TRUE,
	OOA_HorizGroup		, TRUE,
	OOA_Margins			, TRUE,
	OOA_Spacing			, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

OOTagList Button1Tags[] = {
	OOA_WidthHeight		, W2L(40,14),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"System",
	OOA_SameWidth, TRUE, OOA_SameHeight, TRUE,
	TAG_END
	};
OOTagList Button2Tags[] = {
	OOA_WidthHeight		, W2L(60,20),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"Courrier new",
	OOA_SameWidth, TRUE, OOA_SameHeight, TRUE,
	TAG_END
	};
OOTagList Button3Tags[] = {
	OOA_WidthHeight		, W2L(100,18),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_SameWidth, TRUE, OOA_SameHeight, TRUE,
	OOA_FontHeight		, 28,
	TAG_END
	};

OOTagList Button4Tags[] = {
	OOA_WidthHeight		, W2L(40,14),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"System",
	OOA_FontItalic		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};
OOTagList Button5Tags[] = {
	OOA_WidthHeight		, W2L(60,20),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"Courrier new",
	OOA_FontBold		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};
OOTagList Button6Tags[] = {
	OOA_WidthHeight		, W2L(100,18),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontHeight		, 28,
	OOA_FontItalic		, TRUE,
	OOA_ExpandWidth		, TRUE,
	TAG_END
	};

OOTagList Button7Tags[] = {
	OOA_WidthHeight		, W2L(40,14),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"System",
	OOA_FontUnderline	, TRUE,
	TAG_END
	};
OOTagList Button8Tags[] = {
	OOA_WidthHeight		, W2L(60,20),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontName		, (ULONG)"Courrier new",
	OOA_FontItalic		, TRUE,
	OOA_FontUnderline	, TRUE,
	TAG_END
	};
OOTagList Button9Tags[] = {
	OOA_WidthHeight		, W2L(100,18),
	OOA_TitleIndex		, OOTEXT_TITLE_BUTTON,
	OOA_FontHeight		, 28,
	OOA_FontItalic		, TRUE,
	OOA_FontUnderline	, TRUE,
	TAG_END
	};

OOTagList CheckerH1Tags[] = { OOA_Title, (ULONG)"Checker", OOA_Selected	  , TRUE , OOA_3States, FALSE, OOA_TitlePlace, OOV_LEFT  , TAG_END };
OOTagList CheckerH2Tags[] = { OOA_Title, (ULONG)"Checker", OOA_Selected   , FALSE, OOA_3States, FALSE, OOA_TitlePlace, OOV_RIGHT , TAG_END };
OOTagList CheckerH3Tags[] = { OOA_Title, (ULONG)"Checker", OOA_Determinate, TRUE , OOA_3States, TRUE , OOA_TitlePlace, OOV_TOP   , TAG_END };

OOTagList ScrollerV1Tags[] = { OOA_Title, (ULONG)"Scroller", OOA_Vertical, TRUE, OOA_TitlePlace, OOV_LEFT  , TAG_END };
OOTagList ScrollerV2Tags[] = { OOA_Title, (ULONG)"Scroller", OOA_Vertical, TRUE, OOA_TitlePlace, OOV_RIGHT , TAG_END };
OOTagList ScrollerV3Tags[] = { OOA_Title, (ULONG)"Scroller", OOA_Vertical, TRUE, OOA_TitlePlace, OOV_TOP   , OOA_Total, 10, OOA_Visible, 2, TAG_END };
OOTagList ScrollerV4Tags[] = { OOA_Title, (ULONG)"Scroller", OOA_Vertical, TRUE, OOA_TitlePlace, OOV_BOTTOM, TAG_END };

OOTagList Radio1Tags[] = { OOA_Title, (ULONG)"Radio", OOA_Selected,  0, OOA_TitlePlace, OOV_LEFT  , OOA_TextPlace, OOV_RIGHT, OOA_List, (ULONG)"Option 1\0Option 2\0Option 3\0", TAG_END };
OOTagList Radio2Tags[] = { OOA_Title, (ULONG)"Radio", OOA_Selected,  1, OOA_TitlePlace, OOV_RIGHT , OOA_TextPlace, OOV_LEFT , OOA_List, (ULONG)"Lundi\0Mardi\0Mercredi\0Jeudi\0Vendredi\0Samedi\0Dimanche\0", TAG_END };
OOTagList Radio3Tags[] = { OOA_Title, (ULONG)"Radio", OOA_Selected,  0, OOA_TitlePlace, OOV_TOP   , OOA_TextPlace, OOV_LEFT , OOA_List, (ULONG)"Choix 1\0", TAG_END };
OOTagList Radio4Tags[] = { OOA_Title, (ULONG)"Radio", OOA_Selected, -1, OOA_TitlePlace, OOV_TOP   , OOA_TextPlace, OOV_RIGHT, OOA_List, (ULONG)"Choix 2\0", TAG_END };

OOTagList Scroller1Tags[] = {
	OOA_Min			, -2,
	OOA_Max			, +4,
	OOA_Pos			, 0,
	OOA_Horizontal	, TRUE,
	TAG_END
	};
OOTagList Scroller2Tags[] = {
	OOA_Total		, 30,
	OOA_Visible		, 10,
	OOA_Pos			, 10,
	OOA_Vertical	, TRUE,
	TAG_END
	};

OOTagList ListTags[] = {
	TAG_END
	};

static ULONG veriftextfunc( OObject *obj, UBYTE **text )
{
	return OOV_OK;
}
OOTagList EditText1Tags[] = {
	OOA_WidthHeight	, W2L(80,0),
	OOA_Title		, (ULONG)"Text",
	OOA_TitlePlace	, OOV_LEFT,
	OOA_Text    	, (ULONG)"Merci mon mimi",
	OOA_VerifyFunc  , (ULONG)veriftextfunc,
	//OOA_BoxType   , OOV_BOX_NULL,
	OOA_TextPlace 	, OOV_LEFT,
	//OOA_NumDigits , 5,
	OOA_FullWidth	, TRUE,
	TAG_END
	};

static ULONG veriffunc( OObject *obj, SLONG *valptr )
{
	if (*valptr > 2) *valptr = 2;
	else if (*valptr < -1) *valptr = -1;
	return OOV_OK;
	//return OOV_ERROR;
}
OOTagList EditNum1Tags[] = {
	OOA_Title		, (ULONG)"Distance",
	OOA_TitlePlace	, OOV_RIGHT,
	OOA_Min			, -2,
	OOA_Max			, +4,
	OOA_Number    	, 0,
	//OOA_VerifFunc  	, (ULONG)veriffunc,
	//OOA_BoxType    	, OOV_BOX_NULL,
	//OOA_TextPlace 	, OOV_RIGHT,
	OOA_NumDigits 	, 1,
	TAG_END
	};
OOTagList DisplayNum2Tags[] = {
	OOA_TitlePlace 	, OOV_BOTTOM,
	OOA_Title		, (ULONG)"Distance",
	//OOA_BoxType   	, OOV_BOX_NULL,
	OOA_FmtString 	, (ULONG)"%+1d %%",
	OOA_NumDigits 	, 1,
	TAG_END
	};

OOTagList DisplayTextTags[] = {
	OOA_TextIndex	, OOTEXT_COPYRIGHT,
	OOA_ExpandWidth	, TRUE,
	OOA_ExpandHeight, TRUE,
	//OOA_FullWidth	, TRUE,
	//OOA_FullHeight	, TRUE,
	//OOA_TextPlace 	, OOV_LEFT,
	TAG_END
	};

OOTagList DisplayImageTags[] = {
	OOA_ImageIndex	, OOPIC_ICON_LOGO,
	//OOA_ImagePlace 	, OOV_RIGHT,
	TAG_END
	};


static OONotifyItem NF_Radio1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_Radio2[] = {
    { OON_TRIG_EVERYVALUE, OOA_Selected, 0,0, OF(OP,scrollv3), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };
static ULONG func_radio3( void ) { return OO_SetAttrsV( P.radio4, OOA_Selected, -1, OOA_ListIndex, OOTEXT_LONGLIST, TAG_END ); }
static OONotifyItem NF_Radio3[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)func_radio3 },
    OOV_ENDTABLE
    };
static ULONG func_radio4( void ) { OO_SetAttr( P.radio3, OOA_Selected, -1 ); return OO_SetAttr( P.radio4, OOA_List, (ULONG)"Choix 2\0" ); }
static OONotifyItem NF_Radio4[] = {
    { OON_TRIG_EQUAL, OOA_PressedButton, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)func_radio4 },
    OOV_ENDTABLE
    };

static OONotifyItem NF_EditNum1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Number, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    OOV_ENDTABLE
    };

static OONotifyItem NF_Scroller1[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,editnum1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,scrollv3), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,radio1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Selected, 0, TAG_END },
    OOV_ENDTABLE
    };
static OONotifyItem NF_ScrollerV3[] = {
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,dispnum2), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Number, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,scroll1), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Pos, 0, TAG_END },
    { OON_TRIG_EVERYVALUE, OOA_Pos, 0,0, OF(OP,radio2), OON_ACT_DOMETHODVALUE, OOM_SET, OOA_Selected, 0, TAG_END },
    OOV_ENDTABLE
    };

static ULONG close_func1( void ) { printf( "Fermeture demandée, d'accord...\n" ); return OOV_OK; }
static ULONG close_func2( void ) { printf( "Fermeture demandée, PAS d'accord !\n" ); return OOV_CANCEL; }
static OONotifyItem NF_Win1[] = {
    { OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, 0, OON_ACT_CALLFUNC, (ULONG)close_func1 },
    //{ OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, OF(OP,ow1), OON_ACT_DOMETHOD, OOM_DELETE, TAG_END },
    { OON_TRIG_EQUAL, OOA_WinCloseAsk, TRUE,0, OF(OP,appl), OON_ACT_DOMETHOD, OOM_SETEVENT, OOA_EventMask, OOF_BREAK_C, TAG_END },
    OOV_ENDTABLE
    };

static OOTreeItem Tree1[] =
    {
    OBJECT( 0, OF(OP,appl), &Class_Application, ApplTags, NULL ),
		OBJECT( 1, OF(OP,gui),  &Class_HandleGUI, NULL, NULL ),
			OBJECT( 2, OF(OP,os1),  &Class_AreaScreen, NULL, NULL ),
				OBJECT( 3, OF(OP,ow1),  &Class_AreaWindow, WinTags, NF_Win1 ),
					// OBJECT( 4, OF(OP,ar1),  &Class_Area, AreaTags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroup, VertGroupTags, NULL ),
					  OBJECT( 5, OF(OP,testobj),  &Class_AreaGroup, VertGroup2Tags, NULL ),
						OBJECT( 6, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, NULL ),
							OBJECT( 7, OF(OP,but1), &Class_CtrlButton, Button1Tags, NULL ),
							OBJECT( 7, OF(OP,but2), &Class_CtrlButton, Button2Tags, NULL ),
							OBJECT( 7, OF(OP,but3), &Class_CtrlButton, Button3Tags, NULL ),
						OBJECT( 6, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button4Tags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button5Tags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button6Tags, NULL ),
						OBJECT( 6, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button7Tags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button8Tags, NULL ),
							OBJECT( 7, OOV_NOOFFS, &Class_CtrlButton, Button9Tags, NULL ),
						OBJECT( 6, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, NULL ),
							  OBJECT( 7, OOV_NOOFFS,  &Class_CtrlChecker, CheckerH1Tags, NULL ),
							  OBJECT( 7, OOV_NOOFFS,  &Class_CtrlChecker, CheckerH2Tags, NULL ),
							  OBJECT( 7, OF(OP,scroll1) ,  &Class_CtrlScroller, Scroller1Tags, NF_Scroller1 ),
							  OBJECT( 7, OF(OP,editnum1),  &Class_CtrlEditNum, EditNum1Tags, NF_EditNum1 ),
						OBJECT( 6, OOV_NOOFFS,  &Class_AreaGroup, HorizGroupTags, NULL ),
							  OBJECT( 7, OF(OP,radio1),  &Class_CtrlRadio, Radio1Tags, NF_Radio1 ),
							  OBJECT( 7, OF(OP,radio2),  &Class_CtrlRadio, Radio2Tags, NF_Radio2 ),
							  OBJECT( 7, OOV_NOOFFS,  &Class_AreaGroupBox, VertGroupBoxTags, NULL ),
								OBJECT( 8, OOV_NOOFFS,  &Class_AreaGroupBox, HorizGroupBoxTags, NULL ),
								  OBJECT( 9, OF(OP,radio3),  &Class_CtrlRadio, Radio3Tags, NF_Radio3 ),
								  OBJECT( 9, OF(OP,radio4),  &Class_CtrlRadio, Radio4Tags, NF_Radio4 ),
								  OBJECT( 9, OOV_NOOFFS,  &Class_CtrlDisplayImage, DisplayImageTags, NULL ),
								OBJECT( 8, OOV_NOOFFS,  &Class_CtrlDisplayText, DisplayTextTags, NULL ),
								OBJECT( 8, OF(OP,edittext1),  &Class_CtrlEditText, EditText1Tags, NULL ),
						// OBJECT( 6, OF(OP,boxarea),  &Class_Area, BoxAreaTags, NULL ),
						 OBJECT( 6, OF(OP,list),  &Class_CtrlList, ListTags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_CtrlScroller, ScrollerV1Tags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_CtrlScroller, ScrollerV2Tags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroup, VertGroupTags, NULL ),
						OBJECT( 5, OF(OP,scrollv3),  &Class_CtrlScroller, ScrollerV3Tags, NF_ScrollerV3 ),
						OBJECT( 5, OF(OP,dispnum2),  &Class_CtrlDisplayNum, DisplayNum2Tags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_AreaGroup, VertGroupTags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlChecker, CheckerH3Tags, NULL ),
						OBJECT( 5, OOV_NOOFFS,  &Class_CtrlScroller, ScrollerV4Tags, NULL ),
					OBJECT( 4, OOV_NOOFFS,  &Class_CtrlScroller, Scroller2Tags, NULL ),
    OBJECTEND
    };

static void Test_Tree1()
{
	OOStr_SetTable(	text1_table );
	OOPic_SetTable(	images_table );

    if (! OO_NewTree( 0, &P, Tree1 )) goto END_ERROR;
    if (! OO_DoMethodV( P.gui, OOM_PRESENT, OOA_Object, P.gui, TAG_END )) goto END_ERROR;
	/*
	{ struct { SWORD x,y; } pt; SWORD x, y; OORastPort *rp;
	OO_GetAttrsV( P.boxarea, OOA_LeftTop, &pt, OOA_RastPort, &rp, TAG_END );
	x = pt.x; y = pt.y;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_BUTTONUP ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_HEAVYUP  ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_LIGHTUP  ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_BORDERUP ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_GROOVEUP ); x += 50;
	x = pt.x; y = pt.y + 20;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_BUTTONDOWN ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_HEAVYDOWN  ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_LIGHTDOWN  ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_BORDERDOWN ); x += 50;
	OOGfx_DrawBox( rp, x, y, x+39, y+14, OOV_BOX_GROOVEDOWN ); x += 50;
	}
	*/
    //OO_DoMethodV( P.appl, OOM_SETEVENT, OOA_EventMask, OOF_BREAK_C, TAG_END );

    printf( "CTRL+C pour arrêter...\n" );
    OO_DoMethodV( P.appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

  END_ERROR:
    OO_Delete( P.appl );
}
