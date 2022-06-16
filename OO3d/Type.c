/*****************************************************************
 *
 *       Project:	OO
 *       Function:	Partage des types d'objets et de leurs
 *					ressources
 *
 *       Created:   06/08/1999     Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1999 Jean-Michel Forgeas
 *
 ****************************************************************/


/******* Includes ************************************************/

#include "ootypes.h"
#include "ooprotos.h"
#include "oo_pool.h"
#include "oo_list.h"
#include "oo_3d_prv.h"


/******* Imported ************************************************/


/******* Exported ************************************************/

void OO3DType_RemoveMaterial( OO3DMaterial *node, BOOL force );
void OO3DType_RemoveTexture( OO3DTexture *node, BOOL force );
void OO3DType_RemoveImage( OOBitMap *bitmap, BOOL force );
void OO3DType_RemoveType( OO3DType *node, BOOL force );


/******* Statics *************************************************/

static OOList TypesList;
static OOList ImagesList;
static OOList TexturesList;
static OOList MaterialsList;

static OONameNode *search_node_name( OOList *list, UBYTE *name );
static OOImageNode *search_node_bitmap( OOList *list, OOBitMap *bitmap );
static UBYTE *get_name( UBYTE *pathname );


/*****************************************************************
 *
 *		Init / Cleanup
 *
 ****************************************************************/

BOOL OO3DType_Init( void )
{
	OOList_Init( &TypesList );
	OOList_Init( &ImagesList );
	OOList_Init( &TexturesList );
	OOList_Init( &MaterialsList );
	return TRUE;
}

void OO3DType_Cleanup( void )
{
	OONode *node, *succ;

    for (node=TypesList.lh_Head; succ = node->ln_Succ; node = succ)
		OO3DType_RemoveType( (OO3DType*)node, TRUE );

    for (node=MaterialsList.lh_Head; succ = node->ln_Succ; node = succ)
		OO3DType_RemoveMaterial( (OO3DMaterial*)node, TRUE );

    for (node=TexturesList.lh_Head; succ = node->ln_Succ; node = succ)
		OO3DType_RemoveTexture( (OO3DTexture*)node, TRUE );

    for (node=ImagesList.lh_Head; succ = node->ln_Succ; node = succ)
		{
		OOList_Remove( node );
		OOGfx_FreeBMap( ((OOImageNode*)node)->BitMap );
		OOPool_Free( OOGlobalPool, node );
		}
}


/*****************************************************************
 *
 *		Code
 *
 ****************************************************************/

//======================= Images =========================

OOBitMap *OO3DType_AddImage( UBYTE *filename )
{
	OOBitMap *image = NULL;
	UBYTE *name;
	OOImageNode *node;

    if ((name = get_name( filename )) != NULL)
        {
		// Si existe déjà, renvoi l'existant
		if ((node = (OOImageNode*) search_node_name( &ImagesList, name )) != NULL)
			{
			image = node->BitMap;
			node->Node.UseCount++;
			OOASSERT( image != NULL );
			}
		// Sinon on commence par allouer la mémoire
		else{
			if ((node = OOPool_Alloc( OOGlobalPool, sizeof(OOImageNode) + strlen(name) + 1 )) != NULL)
				{
				node->Node.Name = (UBYTE*)node + sizeof(OOImageNode);
				strcpy( node->Node.Name, name );
				node->Node.UseCount = 1;

				if ((image = OOGfx_LoadPicture( filename )) != NULL)
					{
					node->BitMap = image;
					OOList_AddTail( &ImagesList, (OONode*)node );
					}
				else{
					OOPool_Free( OOGlobalPool, node );
					node = NULL;
					OOTRACE( "OO3DType_AddImage : erreur OOGfx_LoadPicture( \"%s\" )", filename );
					}
				}
			}
        }
	return image;
}

void OO3DType_RemoveImage( OOBitMap *bitmap, BOOL force )
{
	OOImageNode *node;
	if ((node = (OOImageNode*) search_node_bitmap( &ImagesList, bitmap )) != NULL)
		{
		if (--node->Node.UseCount == 0 || force == TRUE)
			{
			OOList_Remove( (OONode*)node );
			OOGfx_FreeBMap( node->BitMap );
			OOPool_Free( OOGlobalPool, node );
			}
		}
}

//======================= Textures =========================

OO3DTexture *OO3DType_AddTexture( OO3DefTexture *def )
{
	OO3DTexture *node = NULL;

	if (def != NULL)
		{
		UBYTE *name = def->Name;
		// Si existe déjà, renvoi l'existant
		if ((node = (OO3DTexture*) search_node_name( &TexturesList, name )) != NULL)
			{
			node->Node.UseCount++;
			}
		// Sinon on commence par allouer la mémoire
		else{
			if ((node = OOPool_Alloc( OOGlobalPool, sizeof(OO3DTexture) + strlen(name) + 1 )) != NULL)
				{
				node->Node.Name = (UBYTE*)node + sizeof(OO3DTexture);
				strcpy( node->Node.Name, name );
				node->Node.UseCount = 1;

				// On pointe sur une éventuelle image
				if ((node->BitMap = OO3DType_AddImage( def->FileName )) != NULL)
					{
					node->XOffs			= def->XOffs		;
					node->YOffs			= def->YOffs		;		
					node->Width			= def->Width		;
					node->Height		= def->Height		;		
					node->RealWidh		= def->RealWidh		;
					node->RealHeight	= def->RealHeight	;
					node->Flags			= def->Flags		;				
					
					OOList_AddTail( &TexturesList, (OONode*)node );
					}
				else{
					OOPool_Free( OOGlobalPool, node );
					node = NULL;
					}
				}
			}
		}
	return node;
}

void OO3DType_RemoveTexture( OO3DTexture *node, BOOL force )
{
	if (node != NULL)
		{
		if (--node->Node.UseCount == 0 || force == TRUE)
			{
			OOList_Remove( (OONode*)node );
			OO3DType_RemoveImage( node->BitMap, FALSE );
			OOPool_Free( OOGlobalPool, node );
			}
		}
}

//======================= Materials =========================

OO3DMaterial *OO3DType_AddMaterial( OO3DefMaterial *def )
{
	OO3DMaterial *node = NULL;

	if (def != NULL)
		{
		UBYTE *name = def->Name;
		// Si existe déjà, renvoi l'existant
		if ((node = (OO3DMaterial*) search_node_name( &MaterialsList, name )) != NULL)
			{
			node->Node.UseCount++;
			}
		// Sinon on commence par allouer la mémoire
		else{
			if ((node = OOPool_Alloc( OOGlobalPool, sizeof(OO3DMaterial) + strlen(name) + 1 )) != NULL)
				{
				node->Node.Name = (UBYTE*)node + sizeof(OO3DMaterial);
				strcpy( node->Node.Name, name );
				node->Node.UseCount = 1;

				// On pointe sur d'éventuelles textures : on permet NULL
				node->DisplayTexture	= OO3DType_AddTexture( def->DisplayTexture	);
				node->TranspTexture 	= OO3DType_AddTexture( def->TranspTexture 	);
				node->BumpTexture   	= OO3DType_AddTexture( def->BumpTexture   	);

				node->FillColor      	= def->FillColor	;
				node->Reflection     	= def->Reflection	;
				node->Flags          	= def->Flags		;		
				
				OOList_AddTail( &MaterialsList, (OONode*)node );
				}
			}
		}
	return node;
}

void OO3DType_RemoveMaterial( OO3DMaterial *node, BOOL force )
{
	if (node != NULL)
		{
		if (--node->Node.UseCount == 0 || force == TRUE)
			{
			OOList_Remove( (OONode*)node );
			OO3DType_RemoveTexture( node->DisplayTexture, FALSE );
			OO3DType_RemoveTexture( node->TranspTexture , FALSE );
			OO3DType_RemoveTexture( node->BumpTexture   , FALSE );
			OOPool_Free( OOGlobalPool, node );
			}
		}
}

//======================= Types =========================

static ULONG compute_maxpolys_per_vertex( OO3DefType *def )
{
	UWORD *table, p, n;
	ULONG size = sizeof(UWORD) * def->NumVertices;
	ULONG num = 0;

	if ((table = OOPool_Alloc( OOGlobalPool, size ) ) != NULL)
		{
		// mise à 0 des compteurs
		memset( table, 0, size );

		// remplissage
		for (p=0; p < def->NumPolygons; p++)
			{
			OO3DefPolygon *poly = &def->Polygons[p];
			for (n=0; n < poly->NumVertices; n++)
				table[poly->VerticesIndex[n]]++;
			}
		// tire le max
		for (n=0; n < def->NumVertices; n++)
			if (num < table[n]) num = table[n];

		OOPool_Free( OOGlobalPool, table );
		}
	return num;
}

OO3DType *OO3DType_AddType( OO3DefType *def )
{
	OO3DType *node = NULL;

	if (def != NULL)
		{
		UBYTE *name = def->Name;
		// Si existe déjà, renvoi l'existant
		if ((node = (OO3DType*) search_node_name( &TypesList, name )) != NULL)
			{
			node->Node.UseCount++;
			}
		// Sinon on commence par allouer la mémoire
		else{
			OO3DefPolygon *src;
			OO3DPolygon *dst;
			ULONG i, size, v_size, p_size, c_size;
			v_size = sizeof(OO3DPoint) * def->NumVertices;
			p_size = sizeof(OO3DPolygon) * def->NumPolygons;
			c_size = sizeof(UWORD) * def->NumVertices * (compute_maxpolys_per_vertex( def ) + 1); // 1 pour nombre en tête
			size = sizeof(OO3DType) + v_size + p_size + c_size;

			if ((node = OOPool_Alloc( OOGlobalPool, size + strlen(name) + 1 )) != NULL)
				{
				node->Node.Name = (UBYTE*)node + size;
				strcpy( node->Node.Name, name );
				node->Node.UseCount = 1;
				
				//------------------- Recopie des infos données

				node->NumVertices	= def->NumVertices	;
				node->NumPolygons	= def->NumPolygons	;
				node->DimVertPols	= (UWORD) (c_size / (sizeof(UWORD) * def->NumVertices));
				node->Vertices		= (void*) ((ULONG)node + sizeof(OO3DType));
				node->Polygons		= (void*) ((ULONG)node + sizeof(OO3DType) + v_size);
				node->VertPols		= (void*) ((ULONG)node + sizeof(OO3DType) + v_size + p_size);
				
				// Recopie des sommets
				memcpy( node->Vertices, def->Vertices, sizeof(OO3DPoint) * def->NumVertices );

				// Recopie des polygones et création des matériaux
				for (i=0, src=def->Polygons, dst = node->Polygons; i < node->NumPolygons; i++, src++, dst++)
					{
					dst->Flags = src->Flags	;
					dst->NumVertices = src->NumVertices	;
					dst->VerticesIndex[0] = src->VerticesIndex[0];
					dst->VerticesIndex[1] = src->VerticesIndex[1];
					dst->VerticesIndex[2] = src->VerticesIndex[2];
					dst->VerticesIndex[3] = src->VerticesIndex[3];
					// On pointe sur un éventuel matériau : on permet NULL
					dst->Material = OO3DType_AddMaterial( src->Material );
					}

				//------------------- Calculs pour sphère englobante
                {
                UWORD n, i, ind;
                FLOAT flt, minx=MAXFLT, maxx=MINFLT, miny=MAXFLT, maxy=MINFLT, minz=MAXFLT, maxz=MINFLT;
				OO3DPoint P;
				OO3DPolygon *poly;

                // Calcul de largeur, hauteur, profondeur
                for (n=0, poly=node->Polygons; n < node->NumPolygons; n++, poly++)
                    {
                    for (i=0; i < poly->NumVertices; i++)
                        {
                        ind = poly->VerticesIndex[i];
                        flt = node->Vertices[ind].X;	if (FCMP(flt,minx) < 0) minx = flt;
														if (FCMP(flt,maxx) > 0) maxx = flt;
                        flt = node->Vertices[ind].Y;	if (FCMP(flt,miny) < 0) miny = flt;
														if (FCMP(flt,maxy) > 0) maxy = flt;
                        flt = node->Vertices[ind].Z;	if (FCMP(flt,minz) < 0) minz = flt;
														if (FCMP(flt,maxz) > 0) maxz = flt;
                        }
                    }
                node->Size.X = FSUB(maxx,minx);
                node->Size.Y = FSUB(maxy,miny);
                node->Size.Z = FSUB(maxz,minz);

				// Calcul cu centre
				P.X = FDIV(node->Size.X,(FLOAT)2);
				P.Y = FDIV(node->Size.Y,(FLOAT)2);
				P.Z = FDIV(node->Size.Z,(FLOAT)2);
                node->Center.X = FADD(minx,P.X);
                node->Center.Y = FADD(miny,P.Y);
                node->Center.Z = FADD(minz,P.Z);

                // Calcul du rayon de la sphère englobante
                node->R = OO3DMath_Modulo( &P );
                }

				//------------------- Correspondance sommets / polygones
				{
				UWORD i, p, n, *vp;

				// mise à 0 des compteurs
				memset( node->VertPols, 0, c_size );

				// remplissage
				for (p=0; p < def->NumPolygons; p++)
					{
					OO3DefPolygon *poly = &def->Polygons[p];
					for (n=0; n < poly->NumVertices; n++)
						{
						i = poly->VerticesIndex[n];
						vp = node->VertPols + i * node->DimVertPols;
						(*vp)++;
						vp[*vp] = p;
						}
					}
				}

				//------------------- Ajout du node

				OOList_AddTail( &TypesList, (OONode*)node );
				}
			}
		}
	return node;
}

void OO3DType_RemoveType( OO3DType *node, BOOL force )
{
	if (node != NULL)
		{
		if (--node->Node.UseCount == 0 || force == TRUE)
			{
			OOList_Remove( (OONode*)node );
			{
			SLONG i;
			OO3DPolygon *poly;
			for (i=0, poly=node->Polygons; i < node->NumPolygons; i++, poly++)
				OO3DType_RemoveMaterial( poly->Material, FALSE );
			}
			OOPool_Free( OOGlobalPool, node );
			}
		}
}

void *OO3DType_SearchType( UBYTE *name )
{
	return (void*) search_node_name( &TypesList, name );
}


/*****************************************************************
 *
 *		Statics
 *
 ****************************************************************/

static OONameNode *search_node_name( OOList *list, UBYTE *name )
{
	OONode *node, *succ;
    for (node=list->lh_Head; succ = node->ln_Succ; node = succ)
        {
		if (stricmp( ((OONameNode*)node)->Name, name) == 0)
			return (OONameNode*)node;
        }
	return NULL;
}

static OOImageNode *search_node_bitmap( OOList *list, OOBitMap *bitmap )
{
	OONode *node, *succ;
    for (node=list->lh_Head; succ = node->ln_Succ; node = succ)
        {
		if (((OOImageNode*)node)->BitMap == bitmap)
			return (OOImageNode*)node;
        }
	return NULL;
}

static UBYTE *get_name( UBYTE *pathname )
{
	UBYTE c;
	ULONG len;

	if (pathname == NULL) return NULL;

	for (len = strlen( pathname ); len > 0; len--)
		{
		c = pathname[len-1];
		if (c == '/' || c == '\\' || c == ':') break;
		}

	if (pathname[len] == 0) return NULL;
	return pathname + len;
}
