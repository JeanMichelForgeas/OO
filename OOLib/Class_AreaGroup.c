/*****************************************************************
 *
 *		 Project:	 OO
 *		 Function:	 
 *
 *		 Created:		 Jean-Michel Forgeas
 *
 *		 This code is CopyRight © 1991-1998 Jean-Michel Forgeas
 *
 ****************************************************************/


//****** Includes ***********************************************

#include "oo_class.h"
#include "oo_class_prv.h"
#include "oo_pool.h"
#include "oo_buf.h"
#include "oo_text.h"

#include "oo_gui_prv.h"

#define OOV_GRP_HSPACE	6
#define OOV_GRP_VSPACE	4

struct _OOTmp {
	OObject *Obj;
	SWORD	W;
	SWORD	H;
	};
typedef struct _OOTmp OOTmp;

#define INDEX_OOA_VertGroup 		0
#define INDEX_OOA_HorizGroup 		1
#define INDEX_OOA_Margins			2
#define INDEX_OOA_Spacing			3


//****** Imported ***********************************************


//****** Exported ***********************************************


//****** Statics ************************************************

static void		AreaGroup_AreaSetup		( OOClass *cl, OObject *obj, OOTagList *attrlist );

static void correction_VPC( OOInst_AreaGroup *inst, SWORD zeropourcent, SWORD totpourcent, SWORD numchilds, SWORD numext );
static void correction_HPC( OOInst_AreaGroup *inst, SWORD zeropourcent, SWORD totpourcent, SWORD numchilds, SWORD numext );
static SWORD VertAjust_SameWH( OOInst_AreaGroup *inst, SWORD numchilds, SWORD totw, SWORD diffh, SWORD samew, SWORD sameh );
static SWORD HorizAjust_SameWH( OOInst_AreaGroup *inst, SWORD numchilds, SWORD diffw, SWORD toth, SWORD samew, SWORD sameh );


/***************************************************************************
 *
 *		Class Dispatcher
 *
 ***************************************************************************/

static ULONG Dispatcher_AreaGroup( OOClass *cl, OObject *obj, ULONG method, OOTagList *attrlist )
{
	switch (method)
		{
		//---------------
		case OOM_NEW:
			if ((obj = (OObject*) OO_DoBaseMethod( cl, obj, method, attrlist )) != NULL)
				{
				OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
				// Init instance datas
				inst->Ptr = NULL;
				inst->Flags = OOF_GRP_SPACING;
				// Init default attrs & change defaults with new values
                if (OOPrv_MNew( cl, obj, attrlist ) != OOV_ERROR)
					{
					if (inst->Flags & OOF_GRP_SPACING)
						{
						inst->VSpacing = OOV_GRP_VSPACE;
						inst->HSpacing = OOV_GRP_HSPACE;
						}
					}
				else
					{ OO_DoMethod( obj, OOM_DELETE, NULL ); obj = NULL; }
				}
			return (ULONG)obj;

		case OOM_DELETE:
			return OO_DoBaseMethod( cl, obj, method, attrlist );

		//---------------
		case OOM_GET:
			// Do not call base method
			return OOPrv_MGet( cl, obj, attrlist );

		case OOM_SET:
			// Do not call base method
			return OOPrv_MSet( cl, obj, attrlist, OOF_MSET );

		//---------------
		case OOM_AREA_SETUP: // Obtention de _Border et _SubBorder 
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP) break;
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_ERROR)return OOV_ERROR;

			AreaGroup_AreaSetup( cl, obj, attrlist );

			// Allocs de calcul
			OOASSERT( OOGlobalPool != NULL );
			{
			OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
			if ((inst->Ptr = OOBuf_Alloc( OOGlobalPool, 0, 10*sizeof(OOTmp) )) == NULL) return OOV_ERROR;
			return OO_DoChildrenMethod( OOF_TESTCHILD, obj, method, attrlist );
			}

		case OOM_AREA_MINMAX: // Obtention de _MM, puis de _B.W et _B.H (taille d'ouverture) 
			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_ERROR) // OOResult = OOInst_Area*
				return OOV_ERROR;
			{
			OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
			OObject *child, **tab = obj->ChildrenTable.Table;
			ULONG childindex, childcount = obj->ChildrenTable.Count;
			SLONG maxw, maxh;
			SWORD i, minw, minh, addw, addh, numchilds, numext, totexp, theotot, maxasked, totpourcent, zeropourcent;
			SWORD defw, defh;
			OOInst_Area *ca, *ar = _DATA_AREA(obj);
			OOTmp *pc;

			addw = ar->_SubBorder.Left + ar->_SubBorder.Right;
			addh = ar->_SubBorder.Top + ar->_SubBorder.Bottom;
			minw = ar->_Border.Left + ar->_Border.Right;
			minh = ar->_Border.Top + ar->_Border.Bottom;
			ar->_MM.MinW = 0;
			ar->_MM.MinH = 0;
			defw = 0;
			defh = 0;

			//--- Obtention de _MM 
			if (inst->Flags & OOF_GRP_VERT)
				{	//----------------------- 
				maxw = MAXSWORD;
				maxh = 0;
				maxasked = 0;

				if (childcount == 0) maxh = MAXSWORD; // Pas d'enfants
				else{
					for	(numchilds=totexp=numext=totpourcent=zeropourcent=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						if (OO_DoMethod( child, method, attrlist ) == OOV_OK)
							{
							ca = (APTR)OOResult;
							OOASSERT( ca != NULL );
							if (ca->RI == ar->RI)
								{ // Même RenderInfo 
								OOTmp *tmp, TMP; TMP.Obj = child;
								if (tmp = (OOTmp*) OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									*(ULONG*)&tmp[numchilds++].W = W2L(ca->Flags,ca->_B.H);
									if (ca->Flags & OOF_EXPANDH)
										{
										totexp += ca->_B.H;
										numext++;
										if (ca->Weight) totpourcent += ca->Weight; else zeropourcent++;
										ar->Flags |= OOF_EXPANDH; // Flags initialisés dans area SETUP 
										}
									if (ca->Flags & OOF_EXPANDW)
										{
										ar->Flags |= OOF_EXPANDW;
										maxasked = MAXSWORD; // MAXSWORD évite le blocage sur la taille demandée 
										}
									}
								}
							}
						}
					// On est sûr qu'on a une liste composée seulement de ClassAreas de même RastPort 

					if (numchilds)
						{
						//--- Des sous-objets EXPAND ??? 
						if (numext > 1)
							{
							//--- Correction des pourcentages des EXPAND 
							correction_VPC( inst, zeropourcent, totpourcent, numchilds, numext );

							//--- Calcul de la plus grande taille des expand 
							for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
								{
								if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags
									{
									ca = _DATA_AREA(pc->Obj);
									theotot = (pc->H * 100) / ca->Weight;
									if (theotot > totexp) totexp = theotot;
									}
								}

							//--- Correction des tailles (ici on ne doit que agrandir) 
							for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
								{
								if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags 
									{
									ca = _DATA_AREA(pc->Obj);
									pc->H = ca->_B.H = (totexp * ca->Weight) / 100;
									}
								}
							}

						//--- On calcule la taille du groupe 
						for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
							{
							ca = _DATA_AREA(pc->Obj);

							ar->_MM.MinH += ca->_MM.MinH;
							maxh += ca->_MM.MaxH;
							if (ca->_MM.MinW > ar->_MM.MinW) ar->_MM.MinW = ca->_MM.MinW;
							if (ca->_MM.MaxW < maxw) maxw = ca->_MM.MaxW;
							if (maxasked < ca->_B.W) maxasked = ca->_B.W;

							defh += ca->_B.H;
							if (ca->_B.W > defw) defw = ca->_B.W;
							}
						if (maxw < ar->_MM.MinW) maxw = ar->_MM.MinW; // possible

						//--- On agrandit les groupes pour remplir l'espace inoccupé 
						addh += (numchilds - 1) * inst->VSpacing;
						if (maxw < maxasked) // Pour que le groupe ne se calque pas sur le plus petit 
							maxw = maxasked; // (le max d'un groupe est le +petit max des fils) 
						for	(childindex=0; childindex < childcount; childindex++)
							{
							child = tab[childindex];
							ca = _DATA_AREA(child);
							if (ca->Flags & OOF_ISGROUP)
								{
								if (ca->_MM.MaxW < maxasked && (ca->Flags & OOF_EXPANDW))
									ca->_MM.MaxW = ca->_B.W = maxasked;
								if (ca->Flags & OOF_EXPANDH)
									ca->_MM.MaxH = ca->_B.H = MAXSWORD;
								}
							}
						}
					}
				}
			else if (inst->Flags & OOF_GRP_HORIZ)
				{	//----------------------- 
				maxw = 0;
				maxh = MAXSWORD;
				maxasked = 0;
				if (childcount == 0) maxw = MAXSWORD; // Pas d'enfants;
				else{
					for	(numchilds=totexp=numext=totpourcent=zeropourcent=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						if (OO_DoMethod( child, method, attrlist ) == OOV_OK)
							{
							ca = (APTR)OOResult;
							OOASSERT( ca != NULL );
							if (ca->RI == ar->RI)
								{ // Même RenderInfo 
								OOTmp *tmp, TMP; TMP.Obj = child;
								if (tmp = (OOTmp*) OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									*(ULONG*)&tmp[numchilds++].W = W2L(ca->_B.W,ca->Flags);
									if (ca->Flags & OOF_EXPANDW)
										{
										totexp += ca->_B.W;
										numext++;
										if (ca->Weight) totpourcent += ca->Weight; else zeropourcent++;
										ar->Flags |= OOF_EXPANDW; // Flags initialisés dans area SETUP 
										}
									if (ca->Flags & OOF_EXPANDH)
										{
										ar->Flags |= OOF_EXPANDH;
										maxasked = MAXSWORD; // Flags initialisés dans area SETUP 
										}
									}
								}
							}
						}
					// On est sûr qu'on a une liste composée seulement de ClassAreas de même RastPort 

					if (numchilds)
						{
						//--- Des sous-objets EXPAND ??? 
						if (numext > 1)
							{
							//--- Correction des pourcentages 
							correction_HPC( inst, zeropourcent, totpourcent, numchilds, numext );

							//--- Calcul de la plus grande taille des expand 
							for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
								{
								if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags 
									{
									ca = _DATA_AREA(pc->Obj);
									theotot = (pc->W * 100) / ca->Weight;
									if (theotot > totexp) totexp = theotot;
									}
								}

							//--- Correction des tailles (ici on ne doit que agrandir) 
							for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
								{
								if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags 
									{
									ca = _DATA_AREA(pc->Obj);
									pc->W = ca->_B.W = (totexp * ca->Weight) / 100;
									}
								}
							}

						//--- On calcule la taille du groupe 
						for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
							{
							ca = _DATA_AREA(pc->Obj);

							ar->_MM.MinW += ca->_MM.MinW;
							maxw += ca->_MM.MaxW;
							if (ca->_MM.MinH > ar->_MM.MinH) ar->_MM.MinH = ca->_MM.MinH;
							if (ca->_MM.MaxH < maxh) maxh = ca->_MM.MaxH;
							if (maxasked < ca->_B.H) maxasked = ca->_B.H;

							defw += ca->_B.W;
							if (ca->_B.H > defh) defh = ca->_B.H;
							}
						if (maxh < ar->_MM.MinH) maxh = ar->_MM.MinH; // possible

						//--- On agrandit les groupes pour remplir l'espace inoccupé 
						addw += (numchilds - 1) * inst->HSpacing;
						if (maxh < maxasked) // Pour que le groupe ne se calque pas sur le plus petit 
							maxh = maxasked;
						for	(childindex=0; childindex < childcount; childindex++)
							{
							child = tab[childindex];
							ca = _DATA_AREA(child);
							if (ca->Flags & OOF_ISGROUP)
								{
								if (ca->_MM.MaxH < maxasked && (ca->Flags & OOF_EXPANDH))
									ca->_MM.MaxH = ca->_B.H = maxasked;
								if (ca->Flags & OOF_EXPANDW)
									ca->_MM.MaxW = ca->_B.W = MAXSWORD;
								}
							}
						}
					}
				}
			else{	//----------------------- 
				maxw = MAXSWORD;
				maxh = MAXSWORD;
				if (childcount != 0)
					{
					for	(numchilds=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						if (OO_DoMethod( child, method, attrlist ) == OOV_OK)
							{
							ca = (APTR)OOResult;
							OOASSERT( ca != NULL );
							if (ca->RI == ar->RI)
								{ // Même RenderInfo 
								OOTmp TMP; TMP.Obj = child;
								if (OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									numchilds++;
									}
								ar->Flags |= (ca->Flags & (OOF_EXPANDH|OOF_EXPANDW)); // Flags initialisés dans area SETUP 

								if (ca->_MM.MinW > ar->_MM.MinW) ar->_MM.MinW = ca->_MM.MinW;
								if (ca->_MM.MinH > ar->_MM.MinH) ar->_MM.MinH = ca->_MM.MinH;
								if (ca->_MM.MaxW < maxw) maxw = ca->_MM.MaxW;
								if (ca->_MM.MaxH < maxh) maxh = ca->_MM.MaxH;

								if (ca->_B.W > defw) defw = ca->_B.W;
								if (ca->_B.H > defh) defh = ca->_B.H;
								}
							}
						}
					// On est sûr qu'on a une liste composée seulement de ClassAreas de même RastPort 
					if (numchilds)
						{
						}
					}
				}
			ar->_MM.MinW += minw + addw;
			ar->_MM.MinH += minh + addh;
			maxw += minw + addw; ar->_MM.MaxW = (maxw > MAXSWORD) ? MAXSWORD : (SWORD)maxw;
			maxh += minh + addh; ar->_MM.MaxH = (maxh > MAXSWORD) ? MAXSWORD : (SWORD)maxh;

			//--- Obtention de _B.W et _B.H 
			defw += minw + addw;
			defh += minh + addh;
			ar->_B.W = MAX(defw,ar->AskedW);
			ar->_B.H = MAX(defh,ar->AskedH);

			//--- Limite la taille 
			if (! (ar->Flags & OOF_EXPANDW)) ar->_MM.MaxW = ar->_B.W;
			if (! (ar->Flags & OOF_EXPANDH)) ar->_MM.MaxH = ar->_B.H;

			//--- Evite que le max soit plus petit que le min 
			if (ar->_MM.MaxW < ar->_MM.MinW) ar->_MM.MaxW = ar->_MM.MinW;
			if (ar->_MM.MaxH < ar->_MM.MinH) ar->_MM.MaxH = ar->_MM.MinH;

			//--- Permet quand même le changement à cause de AskedW/H 
			/* Mis en commentaire car permet aux groupes de se retailler même si leurs fils ne s'étendent pas.
			if (ar->_MM.MinW != ar->_MM.MaxW) ar->Flags |= OOF_EXPANDW;
			if (ar->_MM.MinH != ar->_MM.MaxH) ar->Flags |= OOF_EXPANDH;
			*/
			OOResult = (OORESULT) ar;
			}
			break;

		case OOM_AREA_ISGROUP:
			OOResult = (OORESULT)_DATA_AREA(obj);
			break;

		case OOM_AREA_BOX: // Accorde _B et _SubB avec _MM
			{
			OOInst_Area *ar = _DATA_AREA(obj);
			if (ar->Flags & OOF_EXPANDW) ar->Flags |= OOF_AINSIDE_FULLW;
			if (ar->Flags & OOF_EXPANDH) ar->Flags |= OOF_AINSIDE_FULLH;
			}
			//--- On arrange l'areagroup dans lim 
			OO_DoBaseMethod( cl, obj, method, attrlist ); // OOResult = OOInst_Area*

			//--- On place les sous areas de cet areagroup 
			{ // économie de pile : les variables ne sont pas gardées dans la récursion pendant OO_DoBaseMethod() 
			OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
			OOInst_Area *ar = _DATA_AREA(obj);
			OObject *child, **tab = obj->ChildrenTable.Table;
			ULONG childindex, childcount = obj->ChildrenTable.Count;
			OOInst_Area *ca;
			OOBox BOX;
			SWORD nochg, samew, sameh, i, numext, numchilds, minw, minh, defw, defh, totexp, diff, spacing, spaceleft, zeropourcent, totpourcent;
			OOTmp *pc;

			OOTagItem SameRITags[] = { OOA_RenderInfo, 0, TAG_END };
			SameRITags[0].ti_Ptr = ar->RI;

			minw = minh = defw = defh = totexp = 0;

			*(ULONG*)&BOX.X = (ar->Flags & OOF_ANEWRENDERINFO) ? 0 : *(ULONG*)&ar->_B.X;
#ifdef AMIGA
			BOX.X += ar->_Border.Left + ar->_SubBorder.Left;
			BOX.Y += ar->_Border.Top + ar->_SubBorder.Top;
#endif
#ifdef _WIN32
			if (obj->Class == &Class_AreaWindow)
				{
				BOX.X += ar->_SubBorder.Left;
				BOX.Y += ar->_SubBorder.Top;
				}
			else{
				BOX.X += ar->_Border.Left + ar->_SubBorder.Left;
				BOX.Y += ar->_Border.Top + ar->_SubBorder.Top;
				}
#endif
			BOX.W = ar->_B.W - (ar->_Border.Left + ar->_Border.Right + ar->_SubBorder.Left + ar->_SubBorder.Right);
			BOX.H = ar->_B.H - (ar->_Border.Top + ar->_Border.Bottom + ar->_SubBorder.Top + ar->_SubBorder.Bottom);

			OOBuf_Truncate( inst->Ptr, 0 );

			if (childcount == 0)	// Pas d'enfants
				break;		// OOResult contient (OOInst_Area*)

			if (inst->Flags & OOF_GRP_VERT)
				{	//---------------------------------------------------------------------------- 
					for	(samew=sameh=zeropourcent=totpourcent=numext=numchilds=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						OO_DoMethod( child, OOM_AREA_SAMERI, SameRITags ); // OOResult == (OOInst_Area*)
						if ((ca = (APTR)OOResult) != NULL)
							{
							if (ca->StatusFlags & OOF_AREA_SETUP)
								{
								OOTmp *tmp, TMP; TMP.Obj = child;
								if (tmp = (OOTmp*) OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									*(ULONG*)&tmp[numchilds++].W = W2L(ca->Flags,ca->_B.H);
									defh += ca->_B.H;
									if (ca->Flags & OOF_EXPANDH)
										{
										totexp += ca->_B.H;
										numext++;
										if (ca->Weight) totpourcent += ca->Weight; else zeropourcent++;
										}
									else if (ca->Flags & (OOF_SAMEW|OOF_SAMEH))
										{
										if (ca->Flags & OOF_SAMEW) if (ca->_B.W > samew) samew = ca->_B.W;
										if (ca->Flags & OOF_SAMEH) if (ca->_B.H > sameh) sameh = ca->_B.H;
										}
									}
								}
							}
						else OO_DoMethodV( child, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END ); // autres RastPorts 
						}

					// On est sûr qu'on a une liste composée seulement de ClassAreas dans le même RastPort 
					if (numchilds)
						{
						SWORD offs, new, subext;

						//--- Same width / height 
						diff = BOX.H - ((numchilds-1) * inst->VSpacing) - defh; // positif si agrandit 
						diff = VertAjust_SameWH( inst, numchilds, BOX.W, diff, samew, sameh );
						// defh n'est plus valide 

						//--- Constantes d'espacement 
						spacing = inst->VSpacing;
						spaceleft = 0;
						// Pour répartir du haut jusqu'en bas 
						if (numchilds > 1 && numext == 0)
							{ spacing += diff / (numchilds - 1); spaceleft = diff % (numchilds - 1); }

						//--- Ajout ou retrait alloué à chacun 
						if ((subext = numext) > 1) // Sinon Weight == 0 car pas corrigé 
						  while (diff && subext)
							{
							nochg = 0;
							for (pc=inst->Ptr, i=0; diff && i < numchilds; i++, pc++)
								{
								if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags 
									{
									ca = _DATA_AREA(pc->Obj);
									new = (((totexp+diff) * ca->Weight) / 100);
									if (new != pc->H)
										{
										if (new < ca->_MM.MinH) { offs = ca->_MM.MinH - pc->H; pc->W = 0; subext--; }
										else if (new > ca->_MM.MaxH) { offs = ca->_MM.MaxH - pc->H; pc->W = 0; subext--; }
										else offs = new - pc->H;

										if (offs == 0) { nochg++; } else { pc->H += offs; totexp += offs; diff -= offs; }
										}
									else nochg++;
									}
								else nochg++;
								}
							if (nochg == numchilds) break;
							}
						while (diff && subext) // trouve le subext qui reste pour lui mettre le diff restant 
							{
							nochg = 0;
							for (pc=inst->Ptr, i=0; diff && i < numchilds; i++, pc++)
								{
								if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags
									{
									ca = _DATA_AREA(pc->Obj);
									offs = diff / subext;
									if (offs == 0) offs = (diff > 0) ? 1 : -1;

									new = pc->H + diff;
									if (new < ca->_MM.MinH) { offs = ca->_MM.MinH - pc->H; pc->W = 0; subext--; }
									else if (new > ca->_MM.MaxH) { offs = ca->_MM.MaxH - pc->H; pc->W = 0; subext--; }

									if (offs == 0) { nochg++; } else { pc->H += offs; diff -= offs; }
									}
								else nochg++;
								}
							if (nochg == numchilds) break;
							}

						//--- Répartition vers les objets 
						for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
							{
							BOX.H = pc->H;
							OO_DoMethodV( pc->Obj, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END );
							BOX.Y += pc->H + spacing;
							if (spaceleft > 0) { BOX.Y++; spaceleft--; }
							else if (spaceleft < 0) { BOX.Y--; spaceleft++; }
							}
						}
				}
			else if (inst->Flags & OOF_GRP_HORIZ)
				{	//---------------------------------------------------------------------------- 
					for	(samew=sameh=zeropourcent=totpourcent=numext=numchilds=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						OO_DoMethod( child, OOM_AREA_SAMERI, SameRITags ); // OOResult == (OOInst_Area*)
						if ((ca = (APTR)OOResult) != NULL)
							{
							if (ca->StatusFlags & OOF_AREA_SETUP)
								{
								OOTmp *tmp, TMP; TMP.Obj = child;
								if (tmp = (OOTmp*) OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									*(ULONG*)&tmp[numchilds++].W = W2L(ca->_B.W,ca->Flags);
									defw += ca->_B.W;
									if (ca->Flags & OOF_EXPANDW)
										{
										totexp += ca->_B.W;
										numext++;
										if (ca->Weight) totpourcent += ca->Weight; else zeropourcent++;
										}
									else if (ca->Flags & (OOF_SAMEW|OOF_SAMEH))
										{
										if (ca->Flags & OOF_SAMEW) if (ca->_B.W > samew) samew = ca->_B.W;
										if (ca->Flags & OOF_SAMEH) if (ca->_B.H > sameh) sameh = ca->_B.H;
										}
									}
								}
							}
						else OO_DoMethodV( child, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END ); // autres RastPorts 
						}

					// On est sûr qu'on a une liste composée seulement de ClassAreas dans le même RastPort 
					if (numchilds)
						{
						SWORD offs, new, subext;

						//--- Same width / height 
						diff = BOX.W - ((numchilds-1) * inst->HSpacing) - defw; // positif si agrandit 
						diff = HorizAjust_SameWH( inst, numchilds, diff, BOX.H, samew, sameh );
						// defw n'est plus valide 

						//--- Constantes d'espacement 
						spacing = inst->HSpacing;
						spaceleft = 0;
						// Pour répartir de la gauche jusqu'à la droite 
						if (numchilds > 1 && numext == 0)
							{ spacing += diff / (numchilds - 1); spaceleft = diff % (numchilds - 1); }

						//--- Ajout ou retrait alloué à chacun 
						if ((subext = numext) > 1) // Sinon Weight == 0 car pas corrigé 
						  while (diff && subext)
							{
							nochg = 0;
							for (pc=inst->Ptr, i=0; diff && i < numchilds; i++, pc++)
								{
								if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags
									{
									ca = _DATA_AREA(pc->Obj);
									new = (((totexp+diff) * ca->Weight) / 100);
									if (new != pc->W)
										{
										if (new < ca->_MM.MinW) { offs = ca->_MM.MinW - pc->W; pc->H = 0; subext--; }
										else if (new > ca->_MM.MaxW) { offs = ca->_MM.MaxW - pc->W; pc->H = 0; subext--; }
										else offs = new - pc->W;

										if (offs == 0) { nochg++; } else { pc->W += offs; totexp += offs; diff -= offs; }
										}
									else nochg++;
									}
								else nochg++;
								}
							if (nochg == numchilds) break;
							}
						while (diff && subext) // trouve les subext qui restent pour leur mettre le diff restant
							{
							nochg = 0;
							for (pc=inst->Ptr, i=0; diff && i < numchilds; i++, pc++)
								{
								if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags
									{
									ca = _DATA_AREA(pc->Obj);
									offs = diff / subext;
									if (offs == 0) offs = (diff > 0) ? 1 : -1;
									new = pc->W + offs;
									if (new < ca->_MM.MinW) { offs = ca->_MM.MinW - pc->W; pc->H = 0; subext--; }
									else if (new > ca->_MM.MaxW) { offs = ca->_MM.MaxW - pc->W; pc->H = 0; subext--; }

									if (offs == 0) { nochg++; } else { pc->W += offs; diff -= offs; }
									}
								else nochg++;
								}
							if (nochg == numchilds) break;
							}

						//--- Répartition vers les objets 
						for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
							{
							BOX.W = pc->W;
							OO_DoMethodV( pc->Obj, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END );
							BOX.X += pc->W + spacing;
							if (spaceleft > 0) { BOX.X++; spaceleft--; }
							else if (spaceleft < 0) { BOX.X--; spaceleft++; }
							}
						}
				}
			else{	//---------------------------------------------------------------------------- 
					for	(samew=sameh=numext=numchilds=0,childindex=0; childindex < childcount; childindex++)
						{
						child = tab[childindex];
						OO_DoMethod( child, OOM_AREA_SAMERI, SameRITags ); // OOResult == (OOInst_Area*)
						if ((ca = (APTR)OOResult) != NULL)
							{
							if (ca->StatusFlags & OOF_AREA_SETUP)
								{
								OOTmp TMP; TMP.Obj = child;
								if (OOBuf_Paste( (UBYTE*)&TMP, sizeof(OOTmp), (UBYTE**)&inst->Ptr, MAXULONG, 0 ))
									{
									numchilds++;
									if (ca->Flags & (OOF_SAMEW|OOF_SAMEH))
										{
										if (ca->Flags & OOF_SAMEW) if (ca->_B.W > samew) samew = ca->_B.W;
										if (ca->Flags & OOF_SAMEH) if (ca->_B.H > sameh) sameh = ca->_B.H;
										}
									}
								}
							}
						else OO_DoMethodV( child, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END ); // autres RastPorts 
						}

					// On est sûr qu'on a une liste composée seulement de ClassAreas dans le même RastPort 
					if (numchilds)
						{
						for (pc=inst->Ptr, i=0; i < numchilds; i++, pc++)
							{
							OO_DoMethodV( pc->Obj, OOM_AREA_BOX, OOA_Flags, attrlist[0].ti_Data, OOA_OOBox, &BOX, TAG_END );
							}
						}
				}
			OOResult = (OORESULT) ar;
			}
			break;

		case OOM_AREA_START:
			if ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED) break;

			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_ERROR)
				return OOV_ERROR;
			return OO_DoChildrenMethod( OOF_TESTCHILD, obj, method, attrlist );

		case OOM_AREA_REFRESH:
			OOASSERT( attrlist != NULL && attrlist[0].ti_Tag == OOA_Flags && attrlist[1].ti_Tag == OOA_RastPort );
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;

			if (OO_DoBaseMethod( cl, obj, method, attrlist ) == OOV_ERROR)
				return OOV_ERROR;
			/*
			if (! (_DATA_AREA(obj)->Flags & OOF_ANEWRENDERINFO))
				{
				OOTagList	*attrs	= OOINST_ATTRS(cl,obj);
				OOInst_Area *ar = _DATA_AREA(obj);
				RECT _R;
				HDC dc;
				HPEN  oldpen;

				dc = GetDC( ar->RI->wi );
				oldpen = GetCurrentObject( dc, OBJ_PEN );  

				_R.left		= ar->_B.X;
				_R.top		= ar->_B.Y;
				_R.right	= ar->_B.X + ar->_B.W;
				_R.bottom	= ar->_B.Y + ar->_B.H;
				SelectObject( dc, GetStockObject(BLACK_PEN) );
				MoveToEx( dc, _R.left, _R.bottom-1, NULL );
				LineTo( dc, _R.right-1, _R.bottom-1 );
				LineTo( dc, _R.right-1, _R.top );
				LineTo( dc, _R.left, _R.top );
				LineTo( dc, _R.left, _R.bottom-1 );

				SelectObject( dc, oldpen );
				ReleaseDC( ar->RI->wi, dc );
				}
			*/
			return OO_DoChildrenMethod( 0, obj, method, attrlist );

		case OOM_AREA_STOP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_STARTED)) break;

			OO_DoChildrenMethod( 0, obj, method, attrlist );
			OO_DoBaseMethod( cl, obj, method, attrlist );
			break;

		case OOM_AREA_CLEANUP:
			if (! ((_DATA_AREA(obj)->StatusFlags) & OOF_AREA_SETUP)) break;

			OO_DoChildrenMethod( 0, obj, method, attrlist );
			OO_DoBaseMethod( cl, obj, method, attrlist );
			{
			OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
			OOBuf_Free( &inst->Ptr );
			}
			break;

		case OOM_AREA_CHANGED:
			OO_DoChildrenMethod( 0, obj, method, attrlist );
			break;

		//---------------
		default:
			return OO_DoBaseMethod( cl, obj, method, attrlist );
		}
	return OOV_OK;
}

/***************************************************************************
 *
 *		Utilities
 *
 ***************************************************************************/

static void AreaGroup_AreaSetup( OOClass *cl, OObject *obj, OOTagList *attrlist )
{
	OOInst_AreaGroup *inst = OOINST_DATA(cl,obj);
	OOInst_Area	*ar = _DATA_AREA(obj);

	//--- Flags
	ar->Flags |= OOF_ISGROUP;

	//--- Obtention de _SubBorder
	if (inst->Flags & OOF_GRP_MARGINS)
		{
		ar->_SubBorder.Left = OOV_GRP_HOFFS;
		ar->_SubBorder.Right = OOV_GRP_HOFFS;
		ar->_SubBorder.Top = OOV_GRP_VOFFS;
		ar->_SubBorder.Bottom = OOV_GRP_VOFFS;
		}
}

static void correction_VPC( OOInst_AreaGroup *inst, SWORD zeropourcent, SWORD totpourcent, SWORD numchilds, SWORD numext )
{
  SWORD i, perbox, offs, new, diff, spaceleft;
  OOTmp *pc;
  OOInst_Area *ca;

	if (zeropourcent)
		{
		if (totpourcent < 100) // répartit ce qui manque sur ceux qui sont à 0 
			{
			diff = 100 - totpourcent; // toujours positif 
			if (! (perbox = diff / zeropourcent)) perbox = 1;
			for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags
					{
					ca = _DATA_AREA(pc->Obj);
					if (ca->Weight == 0) ca->Weight = (UBYTE)perbox;
					totpourcent += ca->Weight;
					}
				}
			}
		else // if (totpourcent >= 100) 
			{
			perbox = totpourcent / numext;
			new = totpourcent + (perbox * numext);
			for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags
					{
					ca = _DATA_AREA(pc->Obj);
					if (ca->Weight == 0) ca->Weight = (UBYTE)perbox;
					ca->Weight = (ca->Weight * 100) / new; // Il faut ramener ces poucentages à 100 
					totpourcent += ca->Weight;
					}
				}
			}
		}

	if (totpourcent != 100)
		{
		diff = 100 - totpourcent; // positif ou négatif 
		perbox = diff / numext; spaceleft = diff % numext;
		offs = 0; if (spaceleft > 0) { offs = 1; } else if (spaceleft < 0) { offs = -1; spaceleft = -spaceleft; }
		for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
			{
			if (pc->W & OOF_EXPANDH) // Dupli de child ca->Flags
				{
				ca = _DATA_AREA(pc->Obj);
				ca->Weight += perbox; if (spaceleft) { ca->Weight += offs; spaceleft--; }
				totpourcent += ca->Weight;
				}
			}
		}
}

static void correction_HPC( OOInst_AreaGroup *inst, SWORD zeropourcent, SWORD totpourcent, SWORD numchilds, SWORD numext )
{
  SWORD i, perbox, offs, new, diff, spaceleft;
  OOTmp *pc;
  OOInst_Area *ca;

	if (zeropourcent)
		{
		if (totpourcent < 100) // répartit ce qui manque sur ceux qui sont à 0 
			{				   // diff toujours positif 
			diff = 100 - totpourcent; // toujours positif 
			if (! (perbox = diff / zeropourcent)) perbox = 1;
			for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags
					{
					ca = _DATA_AREA(pc->Obj);
					if (ca->Weight == 0) ca->Weight = (UBYTE)perbox;
					totpourcent += ca->Weight;
					}
				}
			}
		else // if (totpourcent >= 100) 
			{
			perbox = totpourcent / numext;
			new = totpourcent + (perbox * numext);
			for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags
					{
					ca = _DATA_AREA(pc->Obj);
					if (ca->Weight == 0) ca->Weight = (UBYTE)perbox;
					ca->Weight = (ca->Weight * 100) / new; // Il faut ramener ces poucentages à 100 
					totpourcent += ca->Weight;
					}
				}
			}
		}

	if (totpourcent != 100)
		{
		diff = 100 - totpourcent; // positif ou négatif 
		perbox = diff / numext; spaceleft = diff % numext;
		offs = 0; if (spaceleft > 0) { offs = 1; } else if (spaceleft < 0) { offs = -1; spaceleft = -spaceleft; }
		for (totpourcent=0, pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
			{
			if (pc->H & OOF_EXPANDW) // Dupli de child ca->Flags
				{
				ca = _DATA_AREA(pc->Obj);
				ca->Weight += perbox; if (spaceleft) { ca->Weight += offs; spaceleft--; }
				totpourcent += ca->Weight;
				}
			}
		}
}

static SWORD VertAjust_SameWH( OOInst_AreaGroup *inst, SWORD numchilds, SWORD totw, SWORD diffh, SWORD samew, SWORD sameh )
{
	if (samew || sameh)
		{
		SWORD i, add;
		OOTmp *pc;
		OOInst_Area *ca;

		if (diffh < 0)
			for (pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				ca = _DATA_AREA(pc->Obj);
				if ((ca->Flags & OOF_SAMEH) && (ca->_B.H > ca->_MM.MinH))
					{
					add = ca->_B.H - ca->_MM.MinH;
					ca->_B.H -= add;
					pc->H = ca->_B.H;
					diffh += add;
					}
				}

		for (pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
			{
			ca = _DATA_AREA(pc->Obj);
			if (diffh && (ca->Flags & OOF_SAMEH) && (ca->_B.H < sameh))
				{
				add = sameh - ca->_B.H;
				if (add > diffh) add = diffh;
				if (ca->_B.H + add > ca->_MM.MaxH) add = ca->_MM.MaxH - ca->_B.H;
				ca->_B.H += add;
				pc->H = ca->_B.H;
				diffh -= add;
				}

			if ((ca->Flags & OOF_SAMEW) && (ca->_B.W < samew))
				{
				add = samew - ca->_B.W;
				if (add > totw) add = totw;
				if (ca->_B.W + add > ca->_MM.MaxW) add = ca->_MM.MaxW - ca->_B.W;
				ca->_B.W += add;
				}
			}
		}
	return( diffh );
}

static SWORD HorizAjust_SameWH( OOInst_AreaGroup *inst, SWORD numchilds, SWORD diffw, SWORD toth, SWORD samew, SWORD sameh )
{
	if (samew || sameh)
		{
		SWORD i, add;
		OOTmp *pc;
		OOInst_Area *ca;

		if (diffw < 0)
			for (pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
				{
				ca = _DATA_AREA(pc->Obj);
				if ((ca->Flags & OOF_SAMEW) && (ca->_B.W > ca->_MM.MinW))
					{
					add = ca->_B.W - ca->_MM.MinW;
					ca->_B.W -= add;
					pc->W = ca->_B.W;
					diffw += add;
					}
				}

		for (pc=inst->Ptr, i=0; i < numchilds; pc++, i++)
			{
			ca = _DATA_AREA(pc->Obj);
			if (diffw && (ca->Flags & OOF_SAMEW) && (ca->_B.W < samew))
				{
				add = samew - ca->_B.W;
				if (add > diffw) add = diffw;
				if (ca->_B.W + add > ca->_MM.MaxW) add = ca->_MM.MaxW - ca->_B.W;
				ca->_B.W += add;
				pc->W = ca->_B.W;
				diffw -= add;
				}

			if ((ca->Flags & OOF_SAMEH) && (ca->_B.H < sameh))
				{
				add = sameh - ca->_B.H;
				if (add > toth) add = toth;
				if (ca->_B.H + add > ca->_MM.MaxH) add = ca->_MM.MaxH - ca->_B.H;
				ca->_B.H += add;
				}
			}
		}
	return( diffw );
}

/***************************************************************************
 *
 *		Attributes setting
 *
 ***************************************************************************/

// If a SetGet function is given to the attribute, the function has perhaps to change
//	the taglist attribute, and must return OOV_CHANGED, OOV_NOCHANGE, or OOV_ERROR.
// When these functions are called the new tag data value can be the same of the 
//	current attr value.

static ULONG SetGet_All( OOSetGetParms *sgp )
{
	OOInst_AreaGroup *inst = (OOInst_AreaGroup*)sgp->Instance;
	ULONG flag = 0, change = OOV_NOCHANGE, data;

	if (sgp->MethodFlag & (OOF_MSET|OOF_MNEW))
		{
		data = sgp->ExtAttr->ti_Data;
		switch (sgp->ExtAttr->ti_Tag)
			{
			case OOA_VertGroup	: flag = OOF_GRP_VERT 	; break;
			case OOA_HorizGroup	: flag = OOF_GRP_HORIZ	; break;
			case OOA_Margins	: flag = OOF_GRP_MARGINS; break;
			case OOA_Spacing	: flag = OOF_GRP_SPACING; break;
			}

		if (flag)
			{
			if (sgp->ExtAttr->ti_Data == TRUE)
				 { change = (inst->Flags & flag) ? OOV_NOCHANGE : OOV_CHANGED ; inst->Flags |= flag; }
			else { change = (inst->Flags & flag) ? OOV_CHANGED	: OOV_NOCHANGE; inst->Flags &= ~flag; }
			sgp->Attr->ti_Data = sgp->ExtAttr->ti_Data;
			}
		}
	else{
		*(APTR*)sgp->ExtAttr->ti_Ptr = sgp->Attr->ti_Ptr;
		}
	return change;
}

/***************************************************************************
 *
 *		Class definitions
 *
 ***************************************************************************/

static OOAttrDesc attrs[] = {
	{ OOA_VertGroup		, FALSE	, "VertGroup"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | 			  OOF_MAREA },
	{ OOA_HorizGroup	, FALSE	, "HorizGroup"	, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | 			  OOF_MAREA },
	{ OOA_Margins		, FALSE	, "Margins"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
	{ OOA_Spacing		, TRUE	, "Spacing"		, SetGet_All, OOF_MNEW | OOF_MSET | OOF_MGET | OOF_MNOT | OOF_MAREA },
    { TAG_END }
	};

OOClass Class_AreaGroup = { 
	Dispatcher_AreaGroup, 
	&Class_Area,			// Base class
	"Class_AreaGroup",		// Class Name
	0,						// InstOffset
	sizeof(OOInst_AreaGroup),// InstSize  : Taille de l'instance
	attrs,					// AttrsDesc : Table de description des attributs
	sizeof(attrs)/sizeof(OOAttrDesc)*sizeof(OOTagItem)	// AttrsSize : size of instance own taglist
	};
