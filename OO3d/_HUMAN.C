
#include <oo/prvoo.h>
#include <oo/oo3d.h>


static ULONG Dispatcher_3DHuman( OOClass *cl, OObject *obj, OOMsg *msg )
{
  ULONG retval = 1;

    switch (msg->MethodId)
        {

        /*--------------- */
        default:
            retval = OO_DoSuperMethodA( cl, obj, msg );
            break;
        }

    return( retval );
}

static OOAttr attrs[] = {
    0 };

extern OOClass Class_3DSolid;
OOClass Class_3DHuman = {
    { 0,0, 0, Dispatcher_3DHuman, 0 },
    &Class_3DSolid,0,0,
    sizeof(OOInst_Root)+sizeof(OOInst_3DSolid),
    sizeof(OOInst_3DHuman),0, attrs
    };
