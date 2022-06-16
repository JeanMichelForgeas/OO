
/*----------------------------------------------------------------------------*\
     This is a lib which reads 3d-studio binary files from version 3.0
     and higher
     (v1.05)
     author: Martin van Velsen
             ( and some great help by Gert van der Spoel )
     email:  vvelsen@ronix.ptf.hro.nl
\*----------------------------------------------------------------------------*/
#include "OOTypes.h"
#include "OO_pool.h"
#include "3dsfile.h"
#include "OO_class_prv.h"
#include "OO_list.h"
/*#include "LPolygon.h"
#include "L3Point.h"
#include "PolySet.h"*///++++
#define TRUE_3DS               0
#define FALSE_3DS              1
//>------ global vars

char *viewports [11]={
                      "Bogus",
                      "Top",
                      "Bottom",
                      "Left",
                      "Right",
                      "Front",
                      "Back",
                      "User",
                      "Camera",
                      "Light",
                      "Disabled"
                     };
FILE *bin3ds;
unsigned long current_chunk=0L;
unsigned char views_read=0;
unsigned int numb_faces=0,numb_vertices=0;
char temp_name [100];
float trans_mat [4][4]; // translation matrix for objects

/*typedef struct point
{
    float x;
    float y;
    float z;
} POINT3D;
 POINT3D *Storage;
 CPolySet   *PolySet;*///++++
/*----------------------------------------------------------------------------*/
unsigned char ReadChar (void)
{
 return (fgetc (bin3ds));
}
/*----------------------------------------------------------------------------*/
unsigned int ReadInt (void)
{
 return (ReadChar () | (ReadChar () << 8));
}
/*----------------------------------------------------------------------------*/
unsigned long ReadLong (void)
{
 unsigned long temp1,temp2;

 temp1=(ReadChar () | (ReadChar () << 8));
 temp2=(ReadChar () | (ReadChar () << 8));

 return (temp1+(temp2*0x10000L));
}
/*----------------------------------------------------------------------------*/
unsigned long ReadChunkPointer (void)
{
 return (ReadLong ());
}
/*----------------------------------------------------------------------------*/
unsigned long GetChunkPointer (void)
{
 return (ftell (bin3ds)-2); // compensate for the already read Marker
}
/*----------------------------------------------------------------------------*/
void ChangeChunkPointer (unsigned long temp_pointer)
{
 fseek (bin3ds,temp_pointer,SEEK_SET);
}
/*----------------------------------------------------------------------------*/
int ReadName (void)
{
 unsigned int teller=0;
 unsigned char letter;

 strcpy (temp_name,"Default name");

 letter=ReadChar ();
 if (letter==0) return (-1); // dummy object
 temp_name [teller]=letter;
 teller++;

 do
 {
  letter=ReadChar ();
  temp_name [teller]=letter;
  teller++;
 }
 while ((letter!=0) && (teller<12));

 temp_name [teller-1]=0;

 #ifdef __DEBUG__
  OOTRACE ("     Found name : %s\n",temp_name);
 #endif
 return (0);
}
/*----------------------------------------------------------------------------*/
int ReadLongName (void)
{
 unsigned int teller=0;
 unsigned char letter;

 strcpy (temp_name,"Default name");

 letter=ReadChar ();
 if (letter==0) return (-1); // dummy object
 temp_name [teller]=letter;
 teller++;

 do
 {
  letter=ReadChar ();
  temp_name [teller]=letter;
  teller++;
 }
 while (letter!=0);

 temp_name [teller-1]=0;

 #ifdef __DEBUG__
   OOTRACE ("Found name : %s\n",temp_name);
 #endif
 return (0);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadUnknownChunk (unsigned int chunk_id)
{
 unsigned long current_pointer;
 unsigned long temp_pointer;

 chunk_id=chunk_id;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadRGBColor ( OOInst_3DSpace *inst )
{
 float rgb_val [3];
 int i;
 for (i=0;i<3;i++)
  fread (&(rgb_val [i]),sizeof (float),1,bin3ds);

 #ifdef __DEBUG__
 OOTRACE ("     Found Color (RGB) def of: R:%5.2f,G:%5.2f,B:%5.2f\n",
          rgb_val [0],
          rgb_val [1],
          rgb_val [2]);
 #endif

 return (12L);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadTrueColor ( OOInst_3DSpace *inst )
{
 unsigned char true_c_val [3];
 int i;
 for (i=0;i<3;i++)
  true_c_val [i]=ReadChar ();

 #ifdef __DEBUG__
 OOTRACE ("     Found Color (24bit) def of: R:%d,G:%d,B:%d\n",
          true_c_val [0],
          true_c_val [1],
          true_c_val [2]);
 #endif

 return (3L);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadBooleanChunk (unsigned char *boolean)
{
 unsigned long current_pointer;
 unsigned long temp_pointer;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 *boolean=ReadChar ();

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadSpotChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 float target [4];
 float hotspot,falloff;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 fread (&(target [0]),sizeof (float),1,bin3ds);
 fread (&(target [1]),sizeof (float),1,bin3ds);
 fread (&(target [2]),sizeof (float),1,bin3ds);
 fread (&hotspot,sizeof (float),1,bin3ds);
 fread (&falloff,sizeof (float),1,bin3ds);

 #ifdef __DEBUG__
 OOTRACE ("      The target of the spot is at: X:%5.2f Y:%5.2f Y:%5.2f\n",
          target [0],
          target [1],
          target [2]);
 OOTRACE ("      The hotspot of this light is : %5.2f\n",hotspot);
 OOTRACE ("      The falloff of this light is : %5.2f\n",falloff);
 #endif

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadLightChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS,boolean;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L; // 2 id + 4 pointer
 float light_coors [3];

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 fread (&(light_coors [0]),sizeof (float),1,bin3ds);
 fread (&(light_coors [1]),sizeof (float),1,bin3ds);
 fread (&(light_coors [2]),sizeof (float),1,bin3ds);

 #ifdef __DEBUG__
 OOTRACE ("     Found light at coordinates: X: %5.2f, Y: %5.2f,Z: %5.2f\n",
          light_coors [0],
          light_coors [1],
          light_coors [2]);
 #endif

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case LIT_UNKNWN01 :
                           #ifdef __DEBUG__
                           OOTRACE (">>>>> Found Light unknown chunk id of %0X\n",LIT_UNKNWN01);
                           #endif
                           tellertje+=ReadUnknownChunk (LIT_UNKNWN01);
                           break;
        case LIT_OFF      :
                           #ifdef __DEBUG__
                           OOTRACE (">>>>> Light is (on/off) chunk: %0X\n",LIT_OFF);
                           #endif
                           tellertje+=ReadBooleanChunk (&boolean);
                           #ifdef __DEBUG__
                           if (boolean==TRUE_3DS)
                             OOTRACE ("     Light is on\n");
                           else
                             OOTRACE ("     Light is off\n");
                           #endif
                           break;
        case LIT_SPOT     :
                           #ifdef __DEBUG__
                           OOTRACE (">>>>> Light is SpotLight: %0X\n",TRI_VERTEXL);
                           #endif
                           tellertje+=ReadSpotChunk ( inst );
                           break;
        case COL_RGB      :
                           #ifdef __DEBUG__
                           OOTRACE (">>>>> Found Color def (RGB) chunk id of %0X\n",temp_int);
                           #endif
                           tellertje+=ReadRGBColor ( inst );
                           break;
        case COL_TRU      :
                           #ifdef __DEBUG__
                           OOTRACE (">>>>> Found Color def (24bit) chunk id of %0X\n",temp_int);
                           #endif
                           tellertje+=ReadTrueColor ( inst );
                           break;
        default           :break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadCameraChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 float camera_eye [3];
 float camera_focus [3];
 float rotation,lens;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 fread (&(camera_eye [0]),sizeof (float),1,bin3ds);
 fread (&(camera_eye [1]),sizeof (float),1,bin3ds);
 fread (&(camera_eye [2]),sizeof (float),1,bin3ds);

 #ifdef __DEBUG__
 OOTRACE ("     Found Camera viewpoint at coordinates: X: %5.2f, Y: %5.2f,Z: %5.2f\n",
          camera_eye [0],
          camera_eye [1],
          camera_eye [2]);
 #endif

 fread (&(camera_focus [0]),sizeof (float),1,bin3ds);
 fread (&(camera_focus [1]),sizeof (float),1,bin3ds);
 fread (&(camera_focus [2]),sizeof (float),1,bin3ds);

 #ifdef __DEBUG__
 OOTRACE ("     Found Camera focus coors at coordinates: X: %5.2f, Y: %5.2f,Z: %5.2f\n",
          camera_focus [0],
          camera_focus [1],
          camera_focus [2]);
 #endif

 fread (&rotation,sizeof (float),1,bin3ds);
 fread (&lens,sizeof (float),1,bin3ds);
 #ifdef __DEBUG__
 OOTRACE ("     Rotation of camera is:  %5.4f\n",rotation);
 OOTRACE ("     Lens in used camera is: %5.4fmm\n",lens);
 #endif

 if ((temp_pointer-38)>0) // this means more chunks are to follow
 {
  #ifdef __DEBUG__
  OOTRACE ("     **** found extra cam chunks ****\n");
  #endif
  if (ReadInt ()==CAM_UNKNWN01)
  {
   #ifdef __DEBUG__
   OOTRACE ("     **** Found cam 1 type ch ****\n");
   #endif
   ReadUnknownChunk (CAM_UNKNWN01);
  }
  if (ReadInt ()==CAM_UNKNWN02)
  {
   #ifdef __DEBUG__
   OOTRACE ("     **** Found cam 2 type ch ****\n");
   #endif
   ReadUnknownChunk (CAM_UNKNWN02);
  }
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadVerticesChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 float vertices [3]; // x,y,z
 unsigned int i;
 OO3DPoint *coords; //OO

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();
 numb_vertices  =ReadInt ();

  //OO
  {
    coords = OOPool_Alloc( OOGlobalPool, sizeof(OO3DPoint) * numb_vertices );
    if (coords == NULL) return temp_pointer;
    inst->CurrLoadInfo->DefCoords = coords;
  }

 #ifdef __DEBUG__
 OOTRACE ("      Found (%d) number of vertices\n",numb_vertices);
 #endif
//Storage = new POINT3D[ numb_vertices ];
 for (i=0;i<numb_vertices;i++)
 {
  fread (&(vertices [0]),sizeof (float),1,bin3ds);
  fread (&(vertices [1]),sizeof (float),1,bin3ds);
  fread (&(vertices [2]),sizeof (float),1,bin3ds);
  coords->X = vertices[0]; //OO
  coords->Y = vertices[1]; //OO
  coords->Z = vertices[2]; //OO
  coords++;
/*
  #ifdef __DEBUG__
  OOTRACE ("      Vertex nr%4d: X: %5.2f  Y: %5.2f  Z:%5.2f\n",
           i,
           vertices [0],
           vertices [1],
           vertices [2]);
  #endif
*/
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/ unsigned long current_pointer;
unsigned long ReadSmoothingChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long smoothing;
 unsigned int i;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 for (i=0;i<numb_faces;i++)
 {
  smoothing=ReadLong();
  smoothing=smoothing; // compiler warnig depressor *>:)
  #ifdef __DEBUG__
  OOTRACE ("      The smoothing group for face [%5d] is %d\n",i,smoothing);
  #endif
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadFacesChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned int temp_diff;
 unsigned int faces [6]; // a,b,c,Diff (Diff= AB: BC: CA: )
 unsigned int i;
 OOFaceDef *fd; //OO

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();
 numb_faces     =ReadInt ();
 #ifdef __DEBUG__
 OOTRACE ("      Found (%d) number of faces\n",numb_faces);
 #endif

  //OO
  {
    fd = OOPool_Alloc( OOGlobalPool, (sizeof(OOFaceDef)+2*sizeof(UWORD)) * numb_faces + sizeof(UWORD) );
    if (fd == NULL) return temp_pointer;
    inst->CurrLoadInfo->DefFaces = fd;
  }

 for (i=0;i<numb_faces;i++, fd=(OOFaceDef*)(((UBYTE*)fd)+(sizeof(OOFaceDef)+2*sizeof(UWORD))))
 {
  //OOTRACE( "--> fd=%x, >>> %x  (%d)\n", fd, (OOFaceDef*)(((UBYTE*)fd)+(sizeof(OOFaceDef)+2*sizeof(UWORD))), (sizeof(OOFaceDef)+2*sizeof(UWORD)) );
  faces [0]=ReadInt ();
  faces [1]=ReadInt ();
  faces [2]=ReadInt ();
  temp_diff=ReadInt () & 0x000F;
  faces [3]=(temp_diff & 0x0004) >> 2;
  faces [4]=(temp_diff & 0x0002) >> 1;
  faces [5]=(temp_diff & 0x0001);

    fd->NumPoints = 3;
    fd->LineColor = 0;
    fd->FillColor = 9;
    fd->Flags = OOF_LINES|OOF_HIDE;
    fd->TextureIndex = 0;
    fd->CoordIndex[0] = faces[0]; //OO
    fd->CoordIndex[1] = faces[1]; //OO
    fd->CoordIndex[2] = faces[2]; //OO
/*
  #ifdef __DEBUG__
  OOTRACE ("      Face nr:%d, A: %d  B: %d  C:%d , AB:%d  BC:%d  CA:%d\n",
           i,
           faces [0],
           faces [1],
           faces [2],
           faces [3],
           faces [4],
           faces [5]);
  #endif
*/
 }
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 *((UWORD*)fd)++ = ENDFACES; //OO
 OOTRACE ("      Found (%d) number of faces\n",numb_faces);

 if (ReadInt ()==TRI_SMOOTH)
  ReadSmoothingChunk ( inst );
 #ifdef __DEBUG__
 else
  OOTRACE ("      No smoothing groups found, assuming autosmooth\n");
 #endif

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadTranslationChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 int i,j;
 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 for (j=0;j<4;j++)
 {
   for (i=0;i<3;i++)
    fread (&(trans_mat [j][i]),sizeof (float),1,bin3ds);
 }

 trans_mat [0][3]=0;
 trans_mat [1][3]=0;
 trans_mat [2][3]=0;
 trans_mat [3][3]=1;

 #ifdef __DEBUG__
 OOTRACE ("     The translation matrix is:\n");
 for (i=0;i<4;i++)
     OOTRACE ("      | %5.2f %5.2f %5.2f %5.2f |\n",
              trans_mat [i][0],
              trans_mat [i][1],
              trans_mat [i][2],
              trans_mat [i][3]);
 #endif

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadObjChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS,boolean=TRUE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L; // 2 id + 4 pointer

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 //OO
 if ((inst->CurrLoadInfo = (OOLoadObjInfo*) OOPool_Alloc( OOGlobalPool, sizeof(OOLoadObjInfo) )) == NULL)
     return temp_pointer;
 inst->CurrLoadInfo->DefCoords = NULL;
 inst->CurrLoadInfo->DefFaces = NULL;

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case TRI_VERTEXL :
                          #ifdef __DEBUG__
                          OOTRACE (">>>>> Found Object vertices chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadVerticesChunk ( inst );
                          break;
        case TRI_FACEL1  :
                          #ifdef __DEBUG__
                          OOTRACE (">>>>> Found Object faces (1) chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadFacesChunk ( inst );
                          break;
        case TRI_FACEL2  :
                          #ifdef __DEBUG__
                          OOTRACE (">>>>> Found Object faces (2) chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadUnknownChunk (temp_int);
                          break;
/*        case TRI_TRANSL  :
                          #ifdef __DEBUG__
                          OOTRACE (">>>>> Found Object translation chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadTranslationChunk ( inst );
                          break;*/
        case TRI_VISIBLE :
                          #ifdef __DEBUG__
                          OOTRACE (">>>>> Found Object vis/invis chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadBooleanChunk (&boolean);

                          #ifdef __DEBUG__
                          if (boolean==TRUE_3DS)
                             OOTRACE ("      Object is (visible)\n");
                          else
                             OOTRACE ("      Object is (not visible)\n");
                          #endif
                          break;
        default:          break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

OOTRACE( "3DSFILE : DefCoords=%x, DefFaces=%x\n", inst->CurrLoadInfo->DefCoords, inst->CurrLoadInfo->DefFaces );
 if (inst->CurrLoadInfo->DefCoords != NULL && inst->CurrLoadInfo->DefFaces != NULL)
 {
     OObject *obj = OO_NewV( &Class_3DSolid,
        D3A_DefFaces , (ULONG)inst->CurrLoadInfo->DefFaces,
        D3A_DefCoords, (ULONG)inst->CurrLoadInfo->DefCoords,
        D3A_CenterSelf, TRUE,
        OOA_Parent   , OOINST_OBJ(&Class_3DSpace,inst),
        TAG_END );

     if (obj != NULL) OOList_AddTail( &inst->LoadList, (OONode*)inst->CurrLoadInfo );
     else OOPool_Free( OOGlobalPool, inst->CurrLoadInfo );
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadObjectChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L; // 2 id + 4 pointer

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 if (ReadName ()==-1)
 {
  #ifdef __DEBUG__
  OOTRACE (">>>>* Dummy Object found\n");
  #endif
 }

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case OBJ_UNKNWN01:tellertje+=ReadUnknownChunk (OBJ_UNKNWN01);break;
        case OBJ_UNKNWN02:tellertje+=ReadUnknownChunk (OBJ_UNKNWN02);break;
        case OBJ_TRIMESH :
                          #ifdef __DEBUG__
                          OOTRACE (">>>> Found Obj/Mesh chunk id of %0X\n",OBJ_TRIMESH);
                          #endif
                          tellertje+=ReadObjChunk ( inst );
                          break;
        case OBJ_LIGHT   :
                          #ifdef __DEBUG__
                          OOTRACE (">>>> Found Light chunk id of %0X\n",OBJ_LIGHT);
                          #endif
                          tellertje+=ReadLightChunk ( inst );
                          break;
        case OBJ_CAMERA  :
                          #ifdef __DEBUG__
                          OOTRACE (">>>> Found Camera chunk id of %0X\n",OBJ_CAMERA);
                          #endif
                          tellertje+=ReadCameraChunk ( inst );
                          break;
        default:          break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadBackgrChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L; // 2 id + 4 pointer

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case COL_RGB :
                      #ifdef __DEBUG__
                      OOTRACE (">> Found Color def (RGB) chunk id of %0X\n",temp_int);
                      #endif
                      tellertje+=ReadRGBColor ( inst );
                      break;
        case COL_TRU :
                      #ifdef __DEBUG__
                      OOTRACE (">> Found Color def (24bit) chunk id of %0X\n",temp_int);
                      #endif
                      tellertje+=ReadTrueColor ( inst );
                      break;
        default:      break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadAmbientChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L; // 2 id + 4 pointer

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case COL_RGB :
                      #ifdef __DEBUG__
                      OOTRACE (">>>> Found Color def (RGB) chunk id of %0X\n",temp_int);
                      #endif
                      tellertje+=ReadRGBColor ( inst );
                      break;
        case COL_TRU :
                      #ifdef __DEBUG__
                      OOTRACE (">>>> Found Color def (24bit) chunk id of %0X\n",temp_int);
                      #endif
                      tellertje+=ReadTrueColor ( inst );
                      break;
        default:      break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long FindCameraChunk ( OOInst_3DSpace *inst )
{
 long temp_pointer=0L;
 int i;

 for (i=0;i<12;i++)
  ReadInt ();

 temp_pointer=11L;
 temp_pointer=ReadName ();

 #ifdef __DEBUG__
 if (temp_pointer==-1)
   OOTRACE (">>>>* No Camera name found\n");
 #endif

 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadViewPortChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned int port,attribs;
 int i;

 views_read++;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 attribs=ReadInt ();
 if (attribs==3)
 {
  #ifdef __DEBUG__
  OOTRACE ("<Snap> active in viewport\n");
  #endif
 }
 if (attribs==5)
 {
  #ifdef __DEBUG__
  OOTRACE ("<Grid> active in viewport\n");
  #endif
 }

 for (i=1;i<6;i++) ReadInt (); // read 5 ints to get to the viewport

 port=ReadInt ();
 if ((port==0xFFFF) || (port==0))
 {
   FindCameraChunk ( inst );
   port=CAMERA;
 }

 #ifdef __DEBUG__
 OOTRACE ("Reading [%s] information with id:%d\n",viewports [port],port);
 #endif

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadViewChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case EDIT_VIEW_P1 :
                           #ifdef __DEBUG__
                           OOTRACE (">>>> Found Viewport1 chunk id of %0X\n",temp_int);
                           #endif
                           tellertje+=ReadViewPortChunk ( inst );
                           break;
        case EDIT_VIEW_P2 :
                           #ifdef __DEBUG__
                           OOTRACE (">>>> Found Viewport2 (bogus) chunk id of %0X\n",temp_int);
                           #endif
                           tellertje+=ReadUnknownChunk (EDIT_VIEW_P2);
                           break;
       case EDIT_VIEW_P3 :
                           #ifdef __DEBUG__
                           OOTRACE (">>>> Found Viewport chunk id of %0X\n",temp_int);
                           #endif
                           tellertje+=ReadViewPortChunk ( inst );
                           break;
        default           :break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;

   if (views_read>3)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadMatDefChunk ( OOInst_3DSpace *inst )
{
 unsigned long current_pointer;
 unsigned long temp_pointer;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 if (ReadLongName ()==-1)
 {
   #ifdef __DEBUG__
   OOTRACE (">>>>* No Material name found\n");
   #endif
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadMaterialChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case MAT_NAME01  :
                          #ifdef __DEBUG__
                          OOTRACE (">>>> Found Material def chunk id of %0X\n",temp_int);
                          #endif
                          tellertje+=ReadMatDefChunk ( inst );
                          break;
        default:break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadEditChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case EDIT_UNKNW01:tellertje+=ReadUnknownChunk (EDIT_UNKNW01);break;
        case EDIT_UNKNW02:tellertje+=ReadUnknownChunk (EDIT_UNKNW02);break;
        case EDIT_UNKNW03:tellertje+=ReadUnknownChunk (EDIT_UNKNW03);break;
        case EDIT_UNKNW04:tellertje+=ReadUnknownChunk (EDIT_UNKNW04);break;
        case EDIT_UNKNW05:tellertje+=ReadUnknownChunk (EDIT_UNKNW05);break;
        case EDIT_UNKNW06:tellertje+=ReadUnknownChunk (EDIT_UNKNW06);break;
        case EDIT_UNKNW07:tellertje+=ReadUnknownChunk (EDIT_UNKNW07);break;
        case EDIT_UNKNW08:tellertje+=ReadUnknownChunk (EDIT_UNKNW08);break;
        case EDIT_UNKNW09:tellertje+=ReadUnknownChunk (EDIT_UNKNW09);break;
        case EDIT_UNKNW10:tellertje+=ReadUnknownChunk (EDIT_UNKNW10);break;
        case EDIT_UNKNW11:tellertje+=ReadUnknownChunk (EDIT_UNKNW11);break;
        case EDIT_UNKNW12:tellertje+=ReadUnknownChunk (EDIT_UNKNW12);break;
        case EDIT_UNKNW13:tellertje+=ReadUnknownChunk (EDIT_UNKNW13);break;

        case EDIT_MATERIAL :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Materials chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadMaterialChunk ( inst );
                            break;
        case EDIT_VIEW1    :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found View main def chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadViewChunk ( inst );
                            break;
        case EDIT_BACKGR   :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Backgr chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadBackgrChunk ( inst );
                            break;
        case EDIT_AMBIENT  :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Ambient chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadAmbientChunk ( inst );
                            break;
        case EDIT_OBJECT   :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Object chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadObjectChunk ( inst );
                            break;
        default:            break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadKeyfChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case KEYF_UNKNWN01 :tellertje+=ReadUnknownChunk (temp_int);break;
        case KEYF_UNKNWN02 :tellertje+=ReadUnknownChunk (temp_int);break;
        case KEYF_FRAMES   :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Keyframer frames chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadUnknownChunk (temp_int);
                            break;
        case KEYF_OBJDES   :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found Keyframer object description chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadUnknownChunk (temp_int);
                            break;
        case EDIT_VIEW1    :
                            #ifdef __DEBUG__
                            OOTRACE (">>> Found View main def chunk id of %0X\n",temp_int);
                            #endif
                            tellertje+=ReadViewChunk ( inst );
                            break;
        default:            break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
unsigned long ReadMainChunk ( OOInst_3DSpace *inst )
{
 unsigned char end_found=FALSE_3DS;
 unsigned int temp_int;
 unsigned long current_pointer;
 unsigned long temp_pointer;
 unsigned long tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE_3DS)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case KEYF3DS :
                      #ifdef __DEBUG__
                      OOTRACE (">> Found *Keyframer* chunk id of %0X\n",KEYF3DS);
                      #endif
                      tellertje+=ReadKeyfChunk ( inst );
                      break;
        case EDIT3DS :
                      #ifdef __DEBUG__
                      OOTRACE (">> Found *Editor* chunk id of %0X\n",EDIT3DS);
                      #endif
                      tellertje+=ReadEditChunk ( inst );
                      break;
        default:      break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
    end_found=TRUE_3DS;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); // move to the new chunk position
 return (temp_pointer);
}
/*----------------------------------------------------------------------------*/
int ReadPrimaryChunk ( OOInst_3DSpace *inst )
{
 unsigned char version;

 if (ReadInt ()==MAIN3DS)
 {
  #ifdef __DEBUG__
  OOTRACE ("> Found Main chunk id of %0X\n",MAIN3DS);
  #endif
  //>---------- find version number
  fseek (bin3ds,28L,SEEK_SET);
  version=ReadChar ();
  if (version<3)
  {
   #ifdef __DEBUG__
   OOTRACE ("Sorry this lib can only read 3ds files of version 3.0 and higher\n");
   OOTRACE ("The version of the file you want to read is: %d\n",version);
   #endif
   return (1);
  }
  fseek (bin3ds,2,SEEK_SET);
  ReadMainChunk ( inst );
 }
 else
  return (1);

 return (0);
}
/*----------------------------------------------------------------------------*/
/*                      Test Main for the 3ds-bin lib                         */
/*----------------------------------------------------------------------------*/
int ReadFile3DS( OOInst_3DSpace *inst, char *FileName)
{

 bin3ds=fopen (FileName,"rb");
 if (bin3ds==NULL)
  return (-1);

 #ifdef __DEBUG__
 OOTRACE ("\nLoading 3ds binary file : %s\n",FileName);
 #endif
 while (ReadPrimaryChunk ( inst )==0);

 return (0);
}
/*----------------------------------------------------------------------------*/
//#endif
