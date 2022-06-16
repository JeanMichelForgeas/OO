/******************************************************************
 *
 *       Project:    Test 3D
 *       Function:   Test du moteur 3D JMF
 *
 *       Created:    06/08/99    Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1999 Jean-Michel Forgeas
 *
 *****************************************************************/


//******* Includes ************************************************

#include "ootypes.h"
#include "ooprotos.h"

//#include "oo_tags.h"
//#include "oo_buf.h"
#include "oo_text.h"
#include "oo_pictures.h"
#include "oo_init.h"
#include "oo_class.h"
#include "oo_gui.h"
#include "oo_3d.h"

#include "data.h"


//******* Imported ************************************************

extern OOTextItem Text_Table[];
extern OOPicItem Images_Table[];

extern struct ApplObjects ApplObj;

extern void * GUI_Init( void );


//******* Exported ************************************************


//******* Statics *************************************************

static OOTagList ApplTags[] = {
	OOA_CompagnyIndex	, OOTEXT_TITLE_COMPAGNY,
	OOA_AuthorIndex		, OOTEXT_TITLE_AUTHOR,
	OOA_NameIndex		, OOTEXT_TITLE_APPL,
	OOA_ImageIndex		, OOPIC_ICON_APPL,
	TAG_END
	};

static OOTreeItem ApplTree[] =
    {
    OBJECT( 0, OF(ApplObjects,Appl), &Class_Application, ApplTags, NULL ),
    OBJECTEND
    };


/******************************************************************
 *
 *      Code
 *
 *****************************************************************/

void main( int argc, char **argv )
{
    if (OO_InitV( OOA_Console, TRUE, TAG_END ) == OOV_ERROR) goto END_ERROR;
	if (OO_Init3D( NULL ) == OOV_ERROR) goto END_ERROR;

	OOStr_SetTable(	Text_Table );
	OOPic_SetTable(	Images_Table );

    if (! OO_NewTree( 0, &ApplObj, ApplTree )) goto END_ERROR;
    if (! (GUI_Init())) goto END_ERROR;

    if (! OO_DoMethodV( ApplObj.GUI, OOM_PRESENT, OOA_Object, ApplObj.GUI, TAG_END )) goto END_ERROR;

	OO_SetAttr( ApplObj.TestType, OOA_Selected, 0 );

	OO_DoMethodV( ApplObj.H3D, D3M_START3D, TAG_END );

    OO_DoMethodV( ApplObj.Appl, OOM_WAITEVENTS, OOA_EventMask, OOF_BREAK_C, TAG_END );

    OO_DoMethodV( ApplObj.H3D, D3M_STOP3D, TAG_END );
	
  END_ERROR:
    OO_Delete( ApplObj.Appl );
    OO_Cleanup3D();
    OO_Cleanup();
}
