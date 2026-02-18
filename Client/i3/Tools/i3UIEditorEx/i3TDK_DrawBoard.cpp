#include "stdafx.h"
#include "i3TDK_DrawBoard.h"

///////////////////////////////////////////////////////////////////////////////////
i3TDK_DrawBoard::i3TDK_DrawBoard(void)
{
	m_hBitmap = NULL;
	m_hOldBitmap = NULL;
	m_hDC = NULL;

	m_Width = m_Height = 0;
}

i3TDK_DrawBoard::~i3TDK_DrawBoard(void)
{
	if( m_hBitmap != NULL)
	{
		if( m_hDC != NULL)
			::SelectObject( m_hDC, m_hOldBitmap);

		::DeleteObject( m_hBitmap);
	}

	if( m_hDC != NULL)
	{
		::DeleteDC( m_hDC);
	}
}

void i3TDK_DrawBoard::_CreateDC( HWND hwnd)
{
	if( m_hDC == NULL)
	{
		HDC hDC = ::GetDC( hwnd);

		m_hDC = ::CreateCompatibleDC( hDC);

		::ReleaseDC( hwnd, hDC);
	}
}

bool i3TDK_DrawBoard::Create( HWND hwnd, INT32 cx, INT32 cy)
{
	_CreateDC( hwnd);

	if( m_hBitmap != NULL)
	{
		::SelectObject( m_hDC, m_hOldBitmap);
		::DeleteObject( m_hBitmap);
	}

	m_hBitmap = ::CreateBitmap( cx, cy, 1, 32, NULL);
	if( m_hBitmap == NULL)
		return false;

	m_hOldBitmap = (HBITMAP) ::SelectObject( m_hDC, m_hBitmap);

	m_Width = cx;
	m_Height = cy;

	return true;
}

bool i3TDK_DrawBoard::LoadRes( HWND hwnd, HINSTANCE hInst, UINT32 idRes)
{
	_CreateDC( hwnd);

	if( m_hBitmap != NULL)
	{
		::SelectObject( m_hDC, m_hOldBitmap);
		::DeleteObject( m_hBitmap);
	}

	m_hBitmap = ::LoadBitmap( hInst, MAKEINTRESOURCE( idRes));
	if( m_hBitmap == NULL)
		return false;

	m_hOldBitmap = (HBITMAP) ::SelectObject( m_hDC, m_hBitmap);

	SIZE size;

	::GetBitmapDimensionEx( m_hBitmap, &size);

	m_Width = size.cx;
	m_Height = size.cy;

	return true;
}

HBITMAP	i3TDK_DrawBoard::Extract( INT32 x, INT32 y, INT32 cx, INT32 cy)
{
	HBITMAP hBmp;
	HDC dcTemp = ::CreateCompatibleDC( m_hDC);

	hBmp = ::CreateCompatibleBitmap( m_hDC, cx, cy);

	HBITMAP hOld = (HBITMAP) ::SelectObject( dcTemp, hBmp);

	::BitBlt( dcTemp, 0, 0, cx, cy, m_hDC, x, y, SRCCOPY);

	::SelectObject( dcTemp, hOld);
	::DeleteDC( dcTemp);

	return hBmp;
}