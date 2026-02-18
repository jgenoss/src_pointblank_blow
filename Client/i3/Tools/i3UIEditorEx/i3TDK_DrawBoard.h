#pragma once

class i3TDK_DrawBoard
{
public:
	HBITMAP		m_hBitmap;
	HBITMAP		m_hOldBitmap;
	HDC			m_hDC;

	INT32		m_Width;
	INT32		m_Height;

protected:
	void		_CreateDC( HWND hwnd);

public:
	i3TDK_DrawBoard(void);
	~i3TDK_DrawBoard(void);

	bool		Create( HWND hwnd, INT32 cx, INT32 cy);
	bool		LoadRes( HWND hwnd, HINSTANCE hInst, UINT32 idRes);

	HBITMAP		Extract( INT32 x, INT32 y, INT32 cx, INT32 cy);
};
