#ifndef OO_PROTOS_H
#define OO_PROTOS_H

#include <string.h> // pour proto memmove() 
#include <stdlib.h> // pour proto malloc() et free() 

//============================================//
#ifdef AMIGA

#include <exec/types.h>
#include <dos/dos.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <pragmas/exec_pragmas.h>

void kprintf( APTR fmt, ... );

#endif
//============================================//
#ifdef _WIN32

#endif
//============================================//
#ifdef _MAC      //+++ A Revoir

#endif
//============================================//

double  OOMath_Sqrt( double f );


#endif // OO_PROTOS_H
