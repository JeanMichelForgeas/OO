#include "ootypes.h"
#include "ooprotos.h"

#include "oo_class.h"


// Texte

#define OOTEXT_TITLE_COMPAGNY	1
#define OOTEXT_TITLE_AUTHOR		2
#define OOTEXT_TITLE_APPL		3
#define OOTEXT_TITLE_WINDOW1	4
#define OOTEXT_COPYRIGHT		5
#define OOTEXT_TITLE_GROUPH3D	6
#define OOTEXT_TITLE_WAITVBL	7
#define OOTEXT_TITLE_CLEARSCR	8
#define OOTEXT_TITLE_RADIOTEST	9
#define OOTEXT_LIST_RADIOTEST	10


// Images

#define OOPIC_ICON_APPL	1
#define OOPIC_ICON_LOGO	2


// Share objects

struct ApplObjects {
    OObject *Appl;

    OObject *GUI;
    OObject *Screen;
    OObject *Window;
    OObject *TestType;
	OObject *ButStart;

	OObject *H3D;
	OObject *Screen3D;
	OObject *Area3D;
	OObject *Space;
    OObject *Camera1;
	};
