#if !defined( __I3G_WIPI2D_H )
#define __I3G_WIPI2D_H

#include "i3Base.h"

#if !defined( I3G_WIPI2D )
#error "This header file is only for WIPI."
#endif

/************************************************************************/
/*					   WIPI-Graphic context								*/
/*	WIPIgrp 이름 규칙 : I3WIPI2D::function()							*/
/*	작성자 : 정순구	dfly79@zepetto.com									*/
/*	작성일 : 2005. 11. 07												*/
/*	SKT의 GIGA 및 KTF의 기본 Graphic 통합								*/
/************************************************************************/

namespace I3WIPI2D
{
	inline INT32 GetImageProperty(MC_GrpImage img, int index)
	{	
		return MC_grpGetImageProperty( img, index);
	}

	inline MC_GrpFrameBuffer GetImageFrameBuffer(MC_GrpImage img)
	{
		return MC_grpGetImageFrameBuffer( img);
	}

	inline MC_GrpFrameBuffer GetScreenFrameBuffer(int i)
	{
		return MC_grpGetScreenFrameBuffer( i);
	}

	inline void DestroyOffScreenFrameBuffer(MC_GrpFrameBuffer fb)
	{
		MC_grpDestroyOffScreenFrameBuffer( fb);
	}

	inline MC_GrpFrameBuffer CreateOffScreenFrameBuffer(M_Int32 w, M_Int32 h)
	{
		return MC_grpCreateOffScreenFrameBuffer( w, h);
	}

	inline void InitContext(MC_GrpContext *pgc)
	{
		MC_grpInitContext( pgc);
	}

	inline void SetContext(MC_GrpContext *pgc, M_Int32 index, void *pv)
	{
		MC_grpSetContext( pgc, index, pv);
	}

	inline void GetContext(MC_GrpContext *pgc, M_Int32 index, void *pv)
	{
		MC_grpGetContext( pgc, index, pv);
	}

	inline void PutPixel(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, MC_GrpContext *pgc)
	{
		MC_grpPutPixel( dst, x, y, pgc);
	}

	inline void DrawLine(MC_GrpFrameBuffer dst, M_Int32 x1, M_Int32 y1, M_Int32 x2, M_Int32 y2, MC_GrpContext *pgc)
	{
		MC_grpDrawLine( dst, x1, y1, x2, y2, pgc);
	}

	inline void DrawRect(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h, MC_GrpContext *pgc)
	{
		MC_grpDrawRect( dst, x, y, w, h, pgc);
	}

	inline void FillRect(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h, MC_GrpContext *pgc)
	{
		MC_grpFillRect( dst, x, y, w, h, pgc);
	}

	inline void CopyFrameBuffer(MC_GrpFrameBuffer dst, M_Int32 dx, M_Int32 dy, M_Int32 w, M_Int32 h,
		MC_GrpFrameBuffer src, M_Int32 sx, M_Int32 sy, MC_GrpContext *pgc)
	{
		MC_grpCopyFrameBuffer( dst, dx, dy, w, h, src, sx, sy, pgc);
	}

	inline void DrawImage( MC_GrpFrameBuffer dst, M_Int32 dx, M_Int32 dy, M_Int32 w, M_Int32 h,
		MC_GrpImage src, M_Int32 sx, M_Int32 sy, MC_GrpContext *pgc)
	{
		MC_grpDrawImage( dst, dx, dy, w, h, src, sx, sy, pgc);
	}

	inline void CopyArea(MC_GrpFrameBuffer dst, M_Int32 dx, M_Int32 dy, M_Int32 w, M_Int32 h, M_Int32 x, M_Int32 y, MC_GrpContext *pgc)
	{
		MC_grpCopyArea( dst, dx, dy, w, h, x, y, pgc);
	}

	inline void DrawArc(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h, M_Int32 s, M_Int32 e, MC_GrpContext *pgc)
	{
		MC_grpDrawArc( dst, x, y, w, h, s, e, pgc);
	}

	inline void FillArc(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h, M_Int32 s, M_Int32 e, MC_GrpContext *pgc)
	{
		MC_grpFillArc(dst, x, y, w, h, s, e, pgc);
	}

	inline void DrawString(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, const char *str, M_Int32 len, MC_GrpContext *pgc)
	{
		MC_grpDrawString( dst, x, y, str, len, pgc);
	}

	inline void GetRGBPixels(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h, M_Uint32 *pd, M_Int32 ipl)
	{
		MC_grpGetRGBPixels( dst, x, y, w, h, pd, ipl);
	}

	inline void SetRGBPixels(MC_GrpFrameBuffer dst, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h,
		const M_Uint32 *psrc, M_Int32 ibpl, MC_GrpContext *pgc)
	{
		MC_grpSetRGBPixels( dst, x, y, w, h, psrc, ibpl, pgc);
	}

	inline void FlushLcd(M_Int32 i, MC_GrpFrameBuffer frm, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h)
	{
		MC_grpFlushLcd( i, frm, x, y, w, h);
	}

	inline M_Int32 GetPixelFromRGB(M_Int32 r, M_Int32 g, M_Int32 b)
	{
		return MC_grpGetPixelFromRGB( r, g, b);
	}

	inline M_Int32 GetRGBFromPixel(M_Int32 pixel, M_Int32 *r, M_Int32 *g, M_Int32 *b)
	{
		return MC_grpGetRGBFromPixel( pixel, r, g, b);
	}

	inline M_Int32 GetDisplayInfo(M_Int32 i, MC_GrpDisplayInfo *pi)
	{
		return MC_grpGetDisplayInfo( i, pi);
	}

	inline void Repaint(M_Int32 lcd, M_Int32 x, M_Int32 y, M_Int32 w, M_Int32 h)
	{
		MC_grpRepaint( lcd, x, y, w, h);
	}

	inline M_Int32 GetFont(M_Int32 face, M_Int32 size, M_Int32 style)
	{
		return MC_grpGetFont( face, size, style);
	}

	inline M_Int32 GetFontHeight(M_Int32 font)
	{
		return MC_grpGetFontHeight( font);
	}

	inline M_Int32 GetFontAscent(M_Int32 font)
	{
		return MC_grpGetFontAscent( font);
	}

	inline M_Int32 GetFontDescent(M_Int32 font)
	{
		return MC_grpGetFontDescent( font);
	}

	inline M_Int32 GetStringWidth(M_Int32 font, const M_Uint8 *str, M_Int32 len)
	{
		return MC_grpGetStringWidth( font, str, len);
	}

	inline M_Int32 GetUnicodeStringWidth(M_Int32 font, const M_UCode *str,	M_Int32 len)
	{
		return MC_grpGetUnicodeStringWidth( font, str, len);
	}

	inline M_Int32 CreateImage(MC_GrpImage *newImg, M_Int32 bufID, M_Int32 off, M_Int32 len)
	{
		return MC_grpCreateImage( newImg, bufID, off, len);
	}

	inline void DestroyImage(MC_GrpImage img)
	{
		MC_grpDestroyImage( img);
	}

	inline M_Int32 DecodeNextImage(MC_GrpImage dst)
	{
		return MC_grpDecodeNextImage( dst);
	}

	inline M_Int32 EncodeImage(MC_GrpFrameBuffer src, M_Int32 x, M_Int32 y,	M_Int32 w, M_Int32 h, M_Int32 *len)
	{
		return MC_grpEncodeImage( src, x, y, w, h, len);
	}

	inline M_Int32 PostEvent(M_Int32 id, M_Int32 type, M_Int32 param1, M_Int32 param2)
	{
		return MC_grpPostEvent( id, type, param1, param2);
	}

	inline void DrawPolygon(MC_GrpFrameBuffer dst, M_Int32* xPoints, M_Int32* yPoints, M_Int32 nPoints, MC_GrpContext* pgc)
	{
		MC_grpDrawPolygon( dst, xPoints, yPoints, nPoints, pgc);
	}

	inline void FillPolygon(MC_GrpFrameBuffer dst, M_Int32* xPoints, M_Int32* yPoints, M_Int32 nPoints, MC_GrpContext* pgc)
	{
		MC_grpFillPolygon( dst, xPoints, yPoints, nPoints, pgc);
	}
}

#endif	// I3WIPI2D_H
