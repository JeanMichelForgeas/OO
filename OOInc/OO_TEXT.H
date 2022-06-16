#ifndef OO_TEXT_H
#define OO_TEXT_H

struct _OOTextItem {
    UWORD   CodeId;		// Le code d'une erreur, ou Id d'une ressource à charger, ou au choix
    UBYTE   Severity;	// si (Flags & OOF_TEXT_ERROR) -> sévérité de l'erreur (code secondaire : 0 = OK,  5 = WARNING, 10 = ERROR, 20 = FATAL ERROR)
    UBYTE   Flags;		// Indique où le texte doit être cherché, si buffer alloué, type du message, etc...
    char	*Text;      // Texte, OOV_ENDTABLE indique la fin de la table 
    };
typedef	struct _OOTextItem			OOTextItem;

// Flags 
#define OOF_TEXT_FORCEUSE	0x01  // Si 1 et si Text!=NULL on ne va pas voir dans le catalogue ou la resource
#define OOF_TEXT_ALLOCATED	0x02  // Si 1 c'est un buffer alloué 
#define OOF_TEXT_ERROR		0x04  // Si 1 c'est un message d'erreur 
#define OOF_TEXT_RESOURCE	0x08  // Si le texte est dans une resource 
#define OOF_TEXT_CATALOG	0x10  // Si le texte est dans un catalogue

//==============================================================================
//==============================================================================
//============================                  ================================
//============================      PROTOS      ================================
//============================                  ================================
//==============================================================================
//==============================================================================

void			OOStr_SetTable	( OOTextItem *table );
char *			OOStr_Get		( ULONG index );
ULONG			OOStr_GetSize	( ULONG index, void *vfont, void *vsize );
char *			OOStr_Draw		( ULONG index, void *vrp, void *vbox, ULONG place );
char *			OOStr_Set		( ULONG index, char *newstr, BOOL alloc );
BOOL			OOStr_IsEqual	( UBYTE *s1, UBYTE *s2, BOOL case_dependant );

#endif // OO_TEXT_H
