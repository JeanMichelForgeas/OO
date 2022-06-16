/******************************************************************
 *
 *       Project:    Test 3D
 *       Function:   Data texte et images
 *
 *       Created:    06/08/99    Jean-Michel Forgeas
 *
 *       This code is CopyRight © 1999 Jean-Michel Forgeas
 *
 *****************************************************************/


//******* Includes ************************************************

#include "ootypes.h"
#include "ooprotos.h"

#include "oo_text.h"
#include "oo_pictures.h"

#include "resource.h"
#include "data.h"


//******* Imported ************************************************


//******* Exported ************************************************

OOTextItem Text_Table[] = {
		{ 0 }, // 0 reserved
		{ 0, 0, 0, "JM Forgeas" },
		{ 0, 0, 0, "Jean-Michel Forgeas" },
		{ 0, 0, 0, "Test 3D" },
		{ 0, 0, 0, "Test du moteur 3D JMF" },
		{ 0, 0, 0, "Copyright © 1999\nJean-Michel Forgeas\nAll Rights Reserved" },
		{ 0, 0, 0, " Handle3D " },
		{ 0, 0, 0, "Sync VBlank" },
		{ 0, 0, 0, "Clear Screen" },
		{ 0, 0, 0, "Tests" },
		{ 0, 0, 0, "Simple\0""2\0""3\0" },
		{ 0, 0, 0, (char*)OOV_ENDTABLE }, // end marker
	};

OOPicItem Images_Table[] = {
		{ 0 }, // 0 reserved
		{ MAKEINTRESOURCE(IDI_APPL), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ MAKEINTRESOURCE(IDI_LOGO), 0, OOF_PIC_RESOURCE|OOF_PIC_ICON, NULL },
		{ NULL, 0, 0, (void*)OOV_ENDTABLE }, // end marker
	};

struct ApplObjects ApplObj;


//******* Statics *************************************************
