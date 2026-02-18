#if !defined( __GFX_CMD_CORE_H)
#define __GFX_CMD_CORE_H

enum GFX_CMD
{
	GCMD_START = 0,
	GCMD_END,
	GCMD_DRAWPOINT,
	GCMD_DRAWLINE,
	GCMD_DRAWTRI,
	GCMD_TEXT,
	GCMD_COLOR
};

typedef struct _tagCmd_DrawPoint
{
	UINT32		_count;
	REAL32		_size;
} CMD_DRAWPOINT;

typedef struct _tagCmd_DrawLine
{
	UINT32		_count;
} CMD_DRAWLINE;

#define		DRAWTRI_FLAG_LINE		0x00000001
#define		DRAWTRI_FLAG_NORMAL		0x00000002

typedef struct _tagCmd_DrawTri
{
	UINT32		_count;
	UINT32		_flag;
} CMD_DRAWTRI;

typedef struct _tagCmd_Text
{
	UINT32		_count;
	REAL32		_x, _y, _z;
} CMD_TEXT;

typedef struct _tagCmd_Color
{
	COLOR		_color;
} CMD_COLOR;

class GfxCmdCore
{
protected:
	char *	m_pszCur;
	HFONT	m_hFont;

	void	_getStr( char * pszToken);
	void	_getStrLine( char * pszStr);
	INT32	_getInt(void);
	REAL32	_getReal(void);
	bool	_getBool(void);

public:
	GfxCmdCore(void);
	virtual ~GfxCmdCore(void);

	bool		_ParseLog( i3Stream * pOut, char * pBuff, VEC3D * pMin, VEC3D * pMax);
	void		_Draw( i3RenderContext * pCtx, char * pCmd);
	void		_DrawText( i3RenderContext * pCtx, HWND hwnd, char * pCmd);
};

#endif
