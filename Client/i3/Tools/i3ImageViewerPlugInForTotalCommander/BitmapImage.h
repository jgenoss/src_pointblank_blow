#pragma once

//#include <afxstr.h>
//#include <atlstr.h>
#include <atlimage.h>
#include <comdef.h>

class CBitmapImage
{
protected:
	INT32	m_Width, m_Height;
	CImage	m_ImageRGB;		// RGB 채널
	CImage	m_ImageAlpha;	// Alpha 채널
	CImage	m_ImageAll;		// 그리드 이미지와 알파 블랜딩한 이미지

	COLORREF	*m_pBuffer;

	CImage		m_GridImage;
	COLORREF	*m_pGridBuffer;

public:
	enum DRAW_MODE
	{
		DRAW_MODE_RGB	= 0,
		DRAW_MODE_ALPHA,
		DRAW_MODE_ALL,
		MAX_DRAW_MODE,
	};

	CBitmapImage(void);
	virtual ~CBitmapImage(void);

	void DestroyGrid();
	BOOL CreateGrid( INT32 nWidth, INT32 nHeight, INT32 nGridSize, COLORREF FrontColor, COLORREF BackColor );

	void Destroy();
	BOOL Create( i3Texture *pTex, INT32 nLevel );
	
	void Draw( HDC hDC, DRAW_MODE nDrawMode, BOOL bFlipX );
};
