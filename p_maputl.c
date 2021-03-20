// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	Movement/collision utility functions,
//	as used by function in p_map.c. 
//	BLOCKMAP Iterator functions,
//	and some PIT_* functions to use for iteration.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: p_maputl.c,v 1.5 1997/02/03 22:45:11 b1 Exp $";


#include <stdlib.h>


#include "m_bbox.h"

#include "doomdef.h"
#include "p_local.h"


// State.
#include "r_state.h"

//
// P_AproxDistance
// Gives an estimation of distance (not exact)
//

fixed_t
P_AproxDistance
( fixed_t	dx,
  fixed_t	dy )
{
    dx = abs(dx);
    dy = abs(dy);
    if (dx < dy)
	return dx+dy-(dx>>1);
    return dx+dy-(dy>>1);
}


//
// P_PointOnLineSide
// Returns 0 or 1
//
int
P_PointOnLineSide
( fixed_t	x,
  fixed_t	y,
  line_t*	line )
{
    fixed_t	dx;
    fixed_t	dy;
    fixed_t	left;
    fixed_t	right;
	
    if (!line->dx)
    {
	if (x <= line->v1->x)
	    return line->dy > 0;
	
	return line->dy < 0;
    }
    if (!line->dy)
    {
	if (y <= line->v1->y)
	    return line->dx < 0;
	
	return line->dx > 0;
    }
	
    dx = (x - line->v1->x);
    dy = (y - line->v1->y);
	
    left = FixedMul ( line->dy>>FRACBITS , dx );
    right = FixedMul ( dy , line->dx>>FRACBITS );
	
    if (right < left)
	return 0;		// front side
    return 1;			// back side
}



//
// P_BoxOnLineSide
// Considers the line to be infinite
// Returns side 0 or 1, -1 if box crosses the line.
//
int
P_BoxOnLineSide
( fixed_t*	tmbox,
  line_t*	ld )
{
    int		p1;
    int		p2;
	
    switch (ld->slopetype)
    {
      case ST_HORIZONTAL:
	p1 = tmbox[BOXTOP] > ld->v1->y;
	p2 = tmbox[BOXBOTTOM] > ld->v1->y;
	if (ld->dx < 0)
	{
	    p1 ^= 1;
	    p2 ^= 1;
	}
	break;
	
      case ST_VERTICAL:
	p1 = tmbox[BOXRIGHT] < ld->v1->x;
	p2 = tmbox[BOXLEFT] < ld->v1->x;
	if (ld->dy < 0)
	{
	    p1 ^= 1;
	    p2 ^= 1;
	}
	break;
	
      case ST_POSITIVE:
	p1 = P_PointOnLineSide (tmbox[BOXLEFT], tmbox[BOXTOP], ld);
	p2 = P_PointOnLineSide (tmbox[BOXRIGHT], tmbox[BOXBOTTOM], ld);
	break;
	
      case ST_NEGATIVE:
	p1 = P_PointOnLineSide (tmbox[BOXRIGHT], tmbox[BOXTOP], ld);
	p2 = P_PointOnLineSide (tmbox[BOXLEFT], tmbox[BOXBOTTOM], ld);
	break;
    }

    if (p1 == p2)
	return p1;
    return -1;
}


//
// P_PointOnDivlineSide
// Returns 0 or 1.
//
int
P_PointOnDivlineSide
( fixed_t	x,
  fixed_t	y,
  divline_t*	line )
{
    fixed_t	dx;
    fixed_t	dy;
    fixed_t	left;
    fixed_t	right;
	
    if (!line->dx)
    {
	if (x <= line->x)
	    return line->dy > 0;
	
	return line->dy < 0;
    }
    if (!line->dy)
    {
	if (y <= line->y)
	    return line->dx < 0;

	return line->dx > 0;
    }
	
    dx = (x - line->x);
    dy = (y - line->y);
	
    // try to quickly decide by looking at sign bits
    if ( (line->dy ^ line->dx ^ dx ^ dy)&0x80000000 )
    {
	if ( (line->dy ^ dx) & 0x80000000 )
	    return 1;		// (left is negative)
	return 0;
    }
	
    left = FixedMul ( line->dy>>8, dx>>8 );
    right = FixedMul ( dy>>8 , line->dx>>8 );
	
    if (right < left)
	return 0;		// front side
    return 1;			// back side
}



//
// P_MakeDivline
//
void
P_MakeDivline
( line_t*	li,
  divline_t*	dl )
{
    dl->x = li->v1->x;
    dl->y = li->v1->y;
    dl->dx = li->dx;
    dl->dy = li->dy;
}



//
// P_InterceptVector
// Returns the fractional intercept point
// along the first divline.
// This is only called by the addthings
// and addlines traversers.
//
fixed_t
P_InterceptVector
( divline_t*	v2,
  divline_t*	v1 )
{
#if 1
    fixed_t	frac;
    fixed_t	num;
    fixed_t	den;
	
    den = FixedMul (v1->dy>>8,v2->dx) - FixedMul(v1->dx>>8,v2->dy);

    if (den == 0)
	return 0;
    //	I_Error ("P_InterceptVector: parallel");
    
    num =
	FixedMul ( (v1->x - v2->x)>>8 ,v1->dy )
	+FixedMul ( (v2->y - v1->y)>>8, v1->dx );

    frac = FixedDiv (num , den);

    return frac;
#else	// UNUSED, float debug.
    float	frac;
    float	num;
    float	den;
    float	v1x;
    float	v1y;
    float	v1dx;
    float	v1dy;
    float	v2x;
    float	v2y;
    float	v2dx;
    float	v2dy;

    v1x = (float)v1->x/FRACUNIT;
    v1y = (float)v1->y/FRACUNIT;
    v1dx = (float)v1->dx/FRACUNIT;
    v1dy = (float)v1->dy/FRACUNIT;
    v2x = (float)v2->x/FRACUNIT;
    v2y = (float)v2->y/FRACUNIT;
    v2dx = (float)v2->dx/FRACUNIT;
    v2dy = (float)v2->dy/FRACUNIT;
	
    den = v1dy*v2dx - v1dx*v2dy;

    if (den == 0)
	return 0;	// parallel
    
    num = (v1x - v2x)*v1dy + (v2y - v1y)*v1dx;
    frac = num / den;

    return frac*FRACUNIT;
#endif
}


//
// P_LineOpening
// Sets opentop and openbottom to the window
// through a two sided line.
// OPTIMIZE: keep this precalculated
//
fixed_t opentop;
fixed_t openbottom;
fixed_t openrange;
fixed_t	lowfloor;


void P_LineOpening (line_t* linedef)
{
    sector_t*	front;
    sector_t*	back;
	
    if (linedef->sidenum[1] == -1)
    {
	// single sided line
	openrange = 0;
	return;
    }
	 
    front = linedef->frontsector;
    back = linedef->backsector;
	
    if (front->ceilingheight < back->ceilingheight)
	opentop = front->ceilingheight;
    else
	opentop = back->ceilingheight;

    if (front->floorheight > back->floorheight)
    {
	openbottom = front->floorheight;
	lowfloor = back->floorheight;
    }
    else
    {
	openbottom = back->floorheight;
	lowfloor = front->floorheight;
    }
	
    openrange = opentop - openbottom;
}


//
// THING POSITION SETTING
//


//
// P_UnsetThingPosition
// Unlinks a thing from block map and sectors.
// On each position change, BLOCKMAP and other
// lookups maintaining lists ot things inside
// these structures need to be updated.
//
void P_UnsetThingPosition (mobj_t* thing)
{
    int		blockx;
    int		blocky;

    if ( ! (thing->flags & MF_NOSECTOR) )
    {
	// inert things don't need to be in blockmap?
	// unlink from subsector
	if (thing->snext)
	    thing->snext->sprev = thing->sprev;

	if (thing->sprev)
	    thing->sprev->snext = thing->snext;
	else
	    thing->subsector->sector->thinglist = thing->snext;
    }
	
    if ( ! (thing->flags & MF_NOBLOCKMAP) )
    {
	// inert things don't need to be in blockmap
	// unlink from block map
	if (thing->bnext)
	    thing->bnext->bprev = thing->bprev;
	
	if (thing->bprev)
	    thing->bprev->bnext = thing->bnext;
	else
	{
	    blockx = (thing->x - bmaporgx)>>MAPBLOCKSHIFT;
	    blocky = (thing->y - bmaporgy)>>MAPBLOCKSHIFT;

	    if (blockx>=0 && blockx < bmapwidth
		&& blocky>=0 && blocky <bmapheight)
	    {
		blocklinks[blocky*bmapwidth+blockx] = thing->bnext;
	    }
	}
    }
}


//
// P_SetThingPosition
// Links a thing into both a block and a subsector
// based on it's x y.
// Sets thing->subsector properly
//
void
P_SetThingPosition (mobj_t* thing)
{
    subsector_t*	ss;
    sector_t*		sec;
    int			blockx;
    int			blocky;
    mobj_t**		link;

    
    // link into subsector
    ss = R_PointInSubsector (thing->x,thing->y);
    thing->subsector = ss;
    
    if ( ! (thing->flags & MF_NOSECTOR) )
    {
	// invisible things don't go into the sector links
	sec = ss->sector;
	
	thing->sprev = NULL;
	thing->snext = sec->thinglist;

	if (sec->thinglist)
	    sec->thinglist->sprev = thing;

	sec->thinglist = thing;
    }

    
    // link into blockmap
    if ( ! (thing->flags & MF_NOBLOCKMAP) )
    {
	// inert things don't need to be in blockmap		
	blockx = (thing->x - bmaporgx)>>MAPBLOCKSHIFT;
	blocky = (thing->y - bmaporgy)>>MAPBLOCKSHIFT;

	if (blockx>=0
	    && blockx < bmapwidth
	    && blocky>=0
	    && blocky < bmapheight)
	{
	    link = &blocklinks[blocky*bmapwidth+blockx];
	    thing->bprev = NULL;
	    thing->bnext = *link;
	    if (*link)
		(*link)->bprev = t  ºDA.É%W‰Ãø  àì  2 À,4@RPrv	j@‘›ÒPA|h
>üÙ=?í‡HÅ7ü}Äa±"‚ùiÚèä…0«­ãAˆd—×åÃš¤ÌÖ›UÖ  °°  °°  °°   ×ÿû€  µ„OóÃ€  pp  pp  pp   (@a4@š	 ’ Ë€d ü„i4 C	€‹ÿe€è†€ÀIÿÔ•O‹%yV>P\ûŠ(·&41ÀºÀ˜üèRhÀtBrĞbÎM! gA)¿óme"à@È»ş :V`ht˜h 2H" `$ÃN°ápÓùÿX`h€¡0ûC5ƒÿÌÀ¶ŒLp T@;O!£¥[  =Ù›ƒ ©gı‰x¢=Õ˜Ìn`Bh† PÿáI4[‘ À2@BÏ µY:Äólìµ±èzÂ®`!üM=a`vÁÌ¢bràI¢`À4Z6ó ¡ø	‘±)Àa¿¨„‚Of€¨AäëÑ2€b )òÀb½“Ğl ğ0`I =G‰j-@?€©d.ŒïM¡€Yˆ™40 W¶@âj †³€Dm¦9Öiæ€hÀ@ø@4X! Gpï˜xR†g!:¹ğ	ÀË˜™°QÛ‹€Ä Ü,@
´RhÅ'¸ñÖf&d“CP
!“Sø!€)E”àüú²(€TŒè dì€B x¢XàĞ1ìH˜ ~~A0
‚ ‚ëT^AeFPÂËß„6  b * ¨Gp0Xá¡$sà@; {À¡e£€éÉI`	Ï ‚ z .Ä0Ä€Ü`Ğ@ ¼‰e"`€
á„'NJuâJV°@øA€€€; \ Ø˜° `W(o&ï“Ä+À@@-,”`Å°î	€ˆğĞÄ­ÓÀª˜@¿÷Ø_`L?\˜ nä¼C RØ@Npò f  ü€7%°€P¬½¥&’° Ø?@L	0 ´!`‰ü –íÉ,3$8L ”Ø@@Ğ ú¹$Üj’>+'¬!šŒM‚Š& 3`ä í¿oÉJZG@€À˜CP·S¡jŠÅ¿f‹Ë}`»‰€3 ^RP }úC]%´x@ÇId¿±¾ËAe#ñ›°°tHN¶¤L '!§’Ÿ°ò|8 Ò‚h@ÉeXÀÃ”:¼ ğšxœ€Ø R	À b  +OûØ ÀvĞ-Ş±ÖÉa°(7Ö‚ïÚÂO
œ&ô@;ˆ¸M†oL§W\·ÇúZ¤¿%«4H°qĞÂÉö+oÛD=ô><q>ñ0Òay“a¼ïçÓÙø™0“Şâœ}üÀj­µ!§ô¨b_®œù,.ÅâiÍ¸¢4±“.{n6o‚)v¤nAÖŞlª€  ?”’'J» P? ¹˜ ô° 9XC¹ @LÜ`ii¬ğjÓÛØùó"`c¬˜…“3©Í€rL1,É$lwHñ0p¡€ˆhJ@CKÈCrYàXI‰%…£Àg$Ïş	bâÙ˜à×Ğû@¡€]#O€bŸØø¼1gIy»~„ù!˜\¸g]ZtÑ»¬èúäù‘´`0”üPgCº0sD¡˜
¸ÀtŠ&à‚n+-ğU¶%ä¿í¾$m‰.Nh´à“pj1\¥œçÀB÷´<š€ÂŠŞ% ¤’Å %fª&$€PÉÃ pˆ¤ö%%'q13§†= ¼·5‚Yl&Ü’S‚R°>u3C­Ôşˆ5øùˆ¶djRz©oï&Û˜È‡À'-%>6#DÔá¸DCìœ€&¡Ã¤î×£äI7±âdûSG±Õ6‘ÇÜ¯ucÆÁçÁ©$É2JûË»&%fÅ™l>ôÿãcl&‘1áğ ~€ € ¨àúÄĞ[€è€¾ ; Ìµ’İ±XÕÌ9L§ã±¾: HL À @€L Ğ4˜Eä/†ro<¾1_¸Šê2I1ìVL p è0 D 
€ €„L&C/Èiâİää„$á¼Ø›¾ æUaáAô(,[˜B¹†Bá$"CHY<¢“‡vT R „áE ( ¸˜dÉ€3zCZ±,{%{(Â_@À¸ Õ ;“€rÉGù8P	 ü˜3d@t_&¸êô~ …‰dÒœ	ãÅß<ˆR€€ op1ˆ×º ~ Ø
€. b@ Å¢h€ªJâú:¾ï  äš”X·s€6ÎCûİ ¾&k>a}«ä{!Ãì¤!H%!—Î°Aa…”İÔbj8¢óÅ†fN¸
%QŞZ­´-å~„9Î2NùØ  ºDA.Í¶‰Ãø  àì  #o€•†4Y)5Ş>Ò˜
Ad¾Œûº¶€B)<§V‹&ü1\ *î¬t…ªèXhhbĞ†çh”5ù–cÀg%¬GŒ/¤À9 ×ı(úËÆ^a| xyÚ(Ğö¿ü›sÌò
+ÑE†0y.Pâ=)^èª†a•r0=Ä)Jxğ„E#lì±q¢èş?›Ç¡Åcb”PÊ»ó¥ÉZUGñ
Q©°sôQ†‹IÇ&NïšCROwpÇFÃ§ÆRpØ®…7óJ¶Øé|¦Ê’5©Ñ˜Pú@½™£µ®U°I
2Y’å.Ñ{²ä´¦º?‰Òš=Rª‘¦@  Ÿ0 À4 tr, €ÄY€İ'ŒĞ (0@Ò`$
 dH@;I/Î…ç	XÇ@  HÀÒH@`S<äH `€ªÃ	  +¤8fK…é7…Xûf$†Cş@ÆJA¹ c¥ö|IeiĞ*BäÂV&†-(`´‡ óàÎVÎy\³–œQly¢äC2•ßå-bÖ`è|}±G€p‚!Òš)’…ÏÕßºy9PO@Jâxëfd”ò­Ò•›êÙ9ä¬SŠÊş‚®Â%Ôqµ÷<.ÒG¤¤5'?ĞÏ‘ÃJà{,ÿĞ° î-_áü<…ZÇÃz„Ù729¬ÜP0&  h…h)Çø¼víƒÔÛè™|¯ÅE†¬ËpW$}$½°Än€
OœéFİ0Cç¹l¯øøth¸×›´‹°¥Â¥áPN]XW `	ÉH ­ T5à‹d$î_Áp8Š/”1#Rp§#À1!À 	À0&“ tYI‰0šà;+”rƒÎSntÂ‹Ò!<?F°ÒFP
ÅD¯ñ ’Ö…Eä¿rtXt“ŠÃät±üÛe3hş%ĞøM9ìĞ^ƒãYW , r€2 üEìàX˜øĞ—‹HR Ğ–”T òPP| à†† ¼Éû1EbÜòQ¥ÀEÈğ°Ğ*KOw„2Ï3‡À ÑH(T‡À!Øh! :aÎD£»UÁ$0íx7åK‰»ïxü5W¬2Ç†2\9¥yœÛA/ºvÎÜd5–7ÿG|-§¥Şb±Á§àßáÉX(°¨´1İîÉŒ°µÄ®é!±¤Â,„ueŠˆDÅ€ÄPi<ù$—eEb%Èœízë%­óU°aœ¹!Ó·w¶A6«@¬ $.PsÊÎÆê¥XB;c®À¯)s5³1Lu´aqjĞBí—	#Gááwm3Æ\ÄYİíAU¦„I§ùõïu×1Ó@  Œ™¡â3@;&§¤o&7Ç·8Q"É€1+“Frö€’³	?‰G±¶2`!€.î3tù,%q+“`÷ÜÎÛ8qqE,²Wi™[ {xá¡ÎÈ1÷8Á/';ºqIFá;q’êµÀe€¬Ñ/3,Î€€İ’y#‘‰Ö·ø—Ø””Õä¸Ôr] n4Õ‰y¼Ï“yhpÑƒ:rºRGÄèb’5	Å˜ı8y ín?’ÀxEV’¤¨gÊæ´‰wòˆDÀHÕpşæD î€†$3îF%$°&„á>ˆb@¦+`0?¥ÉßDŞÅ–Œ³†:Ä»M–Y™öu¤Â<M, 4‚7ı#n)¼B	A(å¾%¼Å–aa½£üM%¥@]¾ÚMèIg‡ÉÚFNÀp÷Âäà#ªÓe÷’¤Ê·–PûIÃ`w’z ÑCÀUäÖ0À×'[?ÂÎ¼°7»A«jø×â÷ÿÉÜH¦"ÛpéxÃÎe¢ÇÃà,ZÎ’PÎp‡›`%aT‘€0 zL!`* ë&îH,¾p{DĞÂÀ¨a`&&€À¼$¬XiH$g1 PCä0“H`;!÷G( ³d–ŸÀ¾ˆ@x€@”pK°ZöÊRp*çZLĞÄ” 3xvQ, älPÀ;!’ù0Ê+şgq¸‘ f‚jq5–¡ø”@T ä¼RR‘„€0Z,1Lp¢|³…¼Ykë'ZZKYiJ’@³Iğn_Qï„J	@1,˜±ü|PÀÁxš 	òz‡HvG@TCJô~y­m0A
“@RV(05 ²_ËGÆ Ø·Îo€Á °	ƒC@€À°“p	Ë¸ğ’[•ÀÎ%€Ü„‘ àc:‚g
uˆĞ€£ Tóà“]$Ä:3Û(„PÎ­ÓÄd&ØÆÃa0hhoA ”Œ½rjIªs°tYæ¯Åaâ!“;åÁ‰!”‡á^N³`ÍœaíNË~`IHQhÜğ¨@Û(zãÎkP™8V„ÈF*J8Æû„%ÏÀn‘–;+ßaŒ;ö|³öƒï2AD­ÔÃx
îµx0?Üà+‘g öc°áQHY4š=‰&é­f  ºDA.ÒLU‰Ãø  àì  GÿxR@õs!‡¤VÀÂ{HûÛ.ı§œtıøÛa„ªĞje™?º Ç‘GñôÑc¢«Zî€v|i±¶Â qs8éÄ]Úl |Z#Lh^^ÙH	ŒDõÁw˜ò€"g“v‚¥Ê][HNhËSM   ?‘†“I‰ÊF (4™÷ß°ç® ¸`CÛìÿğÄlÇK†PKÚÇÆÌ“‰½8Ävr$ÙiqJ€²(½úùÂ¸UÒ‘¿®YnùDfäwº×\Ä„;Ì>ĞI®Ê‘ŒúœÓGRS²WÛtŸñrÕl"«w´¬Ø|D‹Ìm¥$ÎÃ…ÚX£–â4â”8é­
ƒJHÃP„l„†âËİY¥7; Œæ¿tóòƒ@I‚?Pæ(PïN’¸Ğ3¶á{0s©ÇÉÃ9H!‚rk'“oŒsOZ£Ì{Ba Â`nb‹ °7$–°ääŸ($°!%Œp× Y>&¥Ğ€á ğaEô¨%pDÿ)Ín’Ğøbû$©à' ¼h`®ªñ·@•ßoòŒ	SU±Ş:®ĞaB!«ÃÃ_/A¤>à6û0! 4’—l²åğé¦9âí;Æı–ÂhÆƒzsº<:ĞB)u©ïHÂöÊ?úàYÈ³ñox€ôà“aL‰€'Ø|”À;,7“ ÃVBvK`
A#ÿ?’‚ ",˜à8,ç3–µ‹h ÌÀšz¶nDˆa€)&Ê|ã”3IG‡‚T¿[rXÑLiL,¾7  PÜ?{(¾ ~M&ä¤€í¢×ÙP¹.ü>-,œbìÍ©8Ñ÷˜Mß YÅ˜`À¿Àg) ĞÌ”†!Ç$ö(–zq+³¶ÊÂé|ğ»i ¯$ì‚Û$’Ä¥ ò1Ğ` ~¸ÄÒ ¸P €h ô™²	‰FpÀÔ¬ø4„ŒÄ’ÿİÏT”J/ f‚a1 T0°ÀŞÒ²b=RHE`™<Ë!–ã ûGãK×&Paa¬çnD]i@@vKÄ £e€ã,  øÒXA„·nŒÔ $à'B üdÂÿ?¸‡]æ`*Íz¢Ç	±áƒ
%“¾7­™º‰ÄzAlÖ9àĞÂù7“C9OÍãp±x((”~[„CI,Ü]ñ¬z=æ~×Æ0´ÿø”YÀlËNù¼ãÄÈËI/¥!~XÇh}%g¤*Ğ™—ÖØ'lÀøšœ‡&l#e€²úˆ\ô'şÅCKN8§ÜôJÊão0f&¤
§:@rãs Â¸ÒhìÊ‰DÒÆFœ0g@åÀP˜²ñÊ&INmÔÔùvPî¹–‚G¶]˜ë™vÃíUÜÈñùc=Zg1’¼!4¥)Ò6wa[¡ã±©»I™wŒ¶£K—WCïO~U°ĞI:8Éw”[İÕ£)F  	ˆ˜^ t J ÿ€œ@€èd ¤h(‡†,	Ÿ j Ä8€`0°(M:J/¡{wT œLŞ ¼ÛD†–…”éâe   / *&€˜† P T`P1(G£KÄÀ2àO„0Ì" zY(ô@	dÕÃN'Xø\0•¿ÉÉíD¨¼…¶t@×b"òÿq>C7ñy.J}àW¬¬¿J—hL!¸Ğˆmœ0š0µ`è´–²_ÉÉB•  B ‰¡¬„J<Mr±j^?}4«}…òNi!™\–‘ K'ğjú|w:”«Y<_Bb«§¶%!}Àÿ<sI!ÜÃØUÂ2ş0Ç/‡ÔyÔüÊu›Ø¢Ò~„?,zÉFò4CAEr‹	$'…@%I@=;‡$C9âl0`Ag›ÔKÃ6äœ”¨ P€&‹B±)<÷B–ÎhšÀ`åp	€š´p(ş©cÂ;î0=¨|8 ØdÄ8Î×ğâ¹ß®D²®`øfàL˜XÃ$îÌæö>‡PÓ-A(tnÒmUó’çA¡¤‚fÆ“O¢ÔRXØ4€0p‹B®‰ˆ:Ùá§)k‹;¦ßÏ®Š1®K¾´nòîÉƒì}FøÚº&Ç_œï.İVÊV Ôæ?¨DQ4
 rĞ’ìÀuÂ7àRÆ²¾4¢\ +€v©A 0ü4¼†0Æ!ÜpP¤@–©,¥X²ih$-,_ÅœaĞâ"Æ+ıh0`C9õ şå’{È&€ÜÄÔ£ cå¸ÀOGÁĞØØÆôœe Ñ„ ,£°·’C!€œ
dpd„³ôîyª>!€ZrRœè²Èe–ZÈD ÂJ“”)²ÿy;[(Y{§çà8:(5$„`áp}ÜãHJÀÑƒCı!ó‹W†½ É„0 5&€:&€VbÀÁhÂk)$Ò¿	üY° }ˆÀú€P@:(éI„JIX´ØÁğÀ®)¾3wä‘LIÁôYD´C‘†r€ú;+  ºDA.Öİ‰Ãø  àì  ”ib3À:I3—²{mÏİ¼^BGm»œ­aqó 5AÒ`¨	‹bh@\Æ§’’ÇÌS ¤£fpÀ€ ),ˆÁ  Ù4@:ÈE¸fÃVYKe	€ > !Ğ0€2 Zd“ ¨Ü‚d¤ìüÑ¼ ¬4ÄÀÔnLNvŞ PB	4˜M,0
”Ÿ²Qó!p/Ã?ëCg:ÙAË€5ÀT Ò±ÃiŸ	¤ ãqû“š;¡<áœœ÷dÄlŒ*‰$>ÙFól’¡‡ó ”¾§ÇkA5bXı¾äø
şHi0”NÑ7â@ìLÎ¸5%¤4ğ¨c5Ê˜Äj2\‹V>ØIv¥“zn”W»¤wT¤Ôı©Uş¬U¦O—KfvQZÎ€ = F ğâ@vÀcÖb>åì/NÖ04–öã  œàT]˜ ´ğÅ•¾s` ƒü &I`6à:&†t$`n%%\Zœ>uu[½Ÿ0‡ÉZâgz,äÌRÊ,æûĞHëeÄrÈ45"¦±Ã¡—»e-ë}££­Î¼ôÚô‰Ò2ø  
>€Àb@t’ÿì:ùè ¸@A€ ÅÕ$ä=°uéÁ 
@ € œü ñ…ğÎ?v@@ 3 L Bİ€˜
`Ô~e¡eğÎúç& Í™–ä.…`
S€Eò?† İ9``7á¨@€äñ¸¾„­jáx}p”÷w,›ÆæVâäsïbÀQÓ†eÚ„p«?öÌ!'íëš¾¶& h0¸0Éÿ@2Üá¹*€. l(€ §N …! $	)À!ƒzËÀa88­²ó`‡ÊÀìĞM°ÈP|½åÃíT7¶Ç0ÀB/á= ‰ÿ 6$´—Voû"Y”_JX5±7sûBm‰„Ü–¡¥aÍ3pÂJÉ‰³ÄI-,BP"ÿĞ ÈágGvÓ¹½oXù? ’³´0_,•¿+lÅå¶Y¯KG&ğ2œ†îÉqœókq€;&ÿÜ”¬€ÎN@hù’–Æ–†0VI“Íl0b
ÀeâBÃV`Q} \[@ÁG?›ÿ"%ä%ÄĞ*K@,À'àBÿ’ha_©)"|ª&âƒR_À`¢ÒP)ÂC1C~VsZBÕâ^ì Ã€ìÑ†º0J\²˜VŞ ÛÚ¨Øš€8OöwPG‰¿p
€tKC3xÁPm±ì:OøÚ2ĞÖ… á·†âD•Š±©O«E„lòv†^eVÍªo%´_;‡F Kg•aL¹Q 7 (XpĞ0¶@XT¦À1HhÏ >aŒWHb0 È˜–xi: ÈÀ:Å–Wû•‘Èd"†¶Å–àüëÌ¥Š€@€ÂX «€ZÖ’Ù™ÀrQD„	g¡“%Vƒ&DÀĞÒh0o% äÙOäù(QDÌ…dƒøÒiC7Ûãï´ôE£øÊ¦U |Ñì£ŠĞIÂä–á‰Hf-niÑ)€-Ù KÄĞ+”6(ä®îúÆq¿c,†e ¯Š;àÄ0Ü4ÀŸà(	iİ	dî>I+›a…±ã•hÌ&@ÒnOÉS¨30Afàã @ı ä2å ÄaÍÆY¤Ô÷HÅ©V._Á¡¿§)Cœğ¨5€XMÀT¢À¸aÅu”îx|4™˜%c¤âú@ÂC¯ZK˜€ ô 0&P(‚÷JIŒŒëÄø4``€: L·JÀ€€€Èğ@`°Âø!#F¤Á™…ø@ B”‚‹ù÷Ìk…ÄÀ(0ˆb‹ÃMìlÀÖzÂ@Ñ€U9‹³dşÅŠ€ \Aş €BLÉÙÛŠ^3y%t¤19YCfà7(07|µ‡´1,Ù& ‹@ÀCÿ€* QÀ'S‰€tLà…ÿ>+âtK%`	ŠGOÀ]®Âš¹Xƒ"÷X`­[™"ÚumJLteMvqo}áÔªŸn—N¢Æ @ „y` tR tLàTğÈ`€ ÁéJTà"> -8@€@À brø`1) €?î0²É©	3Ú TCØ& )0ÒL¸`q >R<jÏê€  ÀÀ@4 Ìye” Æ ´3¾r±Âl%{-R ²†·™0d0ÒÀ€^CHˆk$