#ifndef OO_PICTURES_H
#define OO_PICTURES_H

struct _OOPicItem {
	UBYTE	*Name;		// Nom d'un fichier, d'une resource, ou id d'une resource
    UWORD   CodeId;		// Un code éventuel
    UWORD   Flags;		// Indique où l'image doit être cherchée, si buffer alloué, type du message, etc...
    void	*Picture;	// Image, OOV_ENDTABLE indique la fin de la table 
    };
typedef	struct _OOPicItem	OOPicItem;

// Flags
#define OOF_PIC_LOADFILE	0x01  // Si 1 Name doit contenir un nom de fichier à charger
#define OOF_PIC_ALLOCATED	0x02  // Si 1 un buffer a été alloué 
#define OOF_PIC_RESOURCE	0x04  // Si l'image est dans une resource 
#define OOF_PIC_ICON		0x08  // Si l'image est une icône
#define OOF_PIC_POINTER		0x10  // Si l'image est un pointeur
#define OOF_PIC_BITMAP		0x20  // Si l'image est une bitmap
#define OOF_PIC_PRELOAD		0x40  // Si l'image doit être pré-chargée


//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

void			OOPic_SetTable	( OOPicItem *table );
void *			OOPic_Get		( ULONG index );
void *			OOPic_GetSized	( ULONG index, ULONG width, ULONG height );
ULONG			OOPic_GetSize	( ULONG index, void *size );
OOPicItem *		OOPic_GetItem	( ULONG index );
void *			OOPic_Draw		( ULONG index, void *rp, void *b, ULONG place );
void *			OOPic_Set		( ULONG index, void *newpic, BOOL alloc );

#endif // OO_PICTURES_H
