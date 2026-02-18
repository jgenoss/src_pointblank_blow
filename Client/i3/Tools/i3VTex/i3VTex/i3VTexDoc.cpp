// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3VTexDoc.cpp : implementation of the Ci3VTexDoc class
//

#include "stdafx.h"
#include "i3VTex.h"

#include "i3VTexDoc.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3VTexDoc

IMPLEMENT_DYNCREATE(Ci3VTexDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3VTexDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &Ci3VTexDoc::OnFileOpen)
	ON_COMMAND(ID_TOOL_CREATENEWVIRTUALTEXTURE, &Ci3VTexDoc::OnToolCreatenewvirtualtexture)
	ON_COMMAND(ID_TOOL_DUMPVIRTUALTEXTURE, &Ci3VTexDoc::OnToolDumpvirtualtexture)
	ON_COMMAND(ID_TOOL_VERIFYVIRTUALTEXTURE, &Ci3VTexDoc::OnToolVerifyvirtualtexture)
	ON_COMMAND(ID_TOOL_DUMPFONTTEXTURE, &Ci3VTexDoc::OnToolDumpfonttexture)
END_MESSAGE_MAP()


// Ci3VTexDoc construction/destruction

Ci3VTexDoc::Ci3VTexDoc()
{
	m_pTex = NULL;

	m_szLoadPath[0] = 0;
}

Ci3VTexDoc::~Ci3VTexDoc()
{
	I3_SAFE_RELEASE( m_pTex);
}

BOOL Ci3VTexDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	I3_SAFE_RELEASE( m_pTex);

	return TRUE;
}




// Ci3VTexDoc serialization

void Ci3VTexDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// Ci3VTexDoc diagnostics

#ifdef _DEBUG
void Ci3VTexDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3VTexDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3VTexDoc commands

void Ci3VTexDoc::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3VTex", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Virtual Texture File (*.i3VTex)|*.i3VTex||", AfxGetMainWnd());

	if( Dlg.DoModal() != IDOK) return;

	I3_SAFE_RELEASE( m_pTex);		

	m_pTex = i3VirtualTexture::NewObject();

	if( m_pTex->Open( (const char *) LPCTSTR( Dlg.GetPathName())) == false)
	{
		NotifyBox( AfxGetMainWnd()->m_hWnd, "Could not load %s file.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	m_pTex->PrepareForRendering( 2048, 2048);

	UpdateAllViews( NULL);
}

bool Ci3VTexDoc::CreateNewVirualTexture(void)
{
	HBITMAP hBmp, hOldBmp;
	HFONT	hFont, hOldFont;
	HDC dc, scdc;
	char * pPixelData;
	INT32 i, x, y, sz, LineSize;

	{
		CFileDialog	Dlg( FALSE, "i3VTex", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3Engine Virtual Texture File (*.i3VTex)|*.i3VTex||", AfxGetMainWnd());

		if( Dlg.DoModal() != IDOK) return false;

		I3_SAFE_RELEASE( m_pTex);

		m_pTex = i3VirtualTexture::NewObject();
		m_pTex->Create( LPCTSTR( Dlg.GetPathName()), I3G_IMAGE_FORMAT_BGRA_8888);
	}

	{
		scdc = ::GetDC( NULL);
		dc = ::CreateCompatibleDC( scdc);

		::ReleaseDC( NULL, scdc);
	}

	LineSize = i3Gfx::CalcRasterBufferSize( 256, 1, 4, I3G_IMAGE_FORMAT_BGRA_8888);
	sz = LineSize * 256;

	{
		BITMAPINFO info;

		i3mem::FillZero( &info, sizeof(info));

		info.bmiHeader.biSize		= sizeof( info.bmiHeader);
		info.bmiHeader.biBitCount	= 32;
		info.bmiHeader.biWidth		= 256;
		info.bmiHeader.biHeight		= 256;
		info.bmiHeader.biPlanes		= 1;
		info.bmiHeader.biCompression = BI_RGB;
		info.bmiHeader.biSizeImage	= sz;
		info.bmiHeader.biXPelsPerMeter	= 0;
		info.bmiHeader.biYPelsPerMeter	= 0;
		info.bmiHeader.biClrUsed		= 0;
		info.bmiHeader.biClrImportant	= 0;

		hBmp = ::CreateDIBSection( dc, &info, DIB_RGB_COLORS, (void **) &pPixelData, NULL, 0);

		hOldBmp = (HBITMAP) ::SelectObject( dc, hBmp);
	}

	{
		hFont = i3TDK::CreateDefFont( 128, "Arial", 700);

		hOldFont = (HFONT) ::SelectObject( dc, hFont);
	}

	{
		CRect rt;
		char szTemp[128];
		HBRUSH hbr;
		i3Texture * pSrcTex;
		INT32 ln;

		{
			pSrcTex = i3Texture::NewObject();

			pSrcTex->Create( 256, 256, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
		}

		rt.SetRect( 0, 0, 256, 256);

		SetTextColor( dc, RGB( 0, 0, 0));
		SetBkMode( dc, TRANSPARENT);

		i = 0;

		for( y = 0; y < 100; y++)
		{
			for( x = 0; x < 100; x++)
			{
				UINT8 r, g, b;

				r = (rand() % 128) + 64;
				g = (rand() % 128) + 64;
				b = (rand() % 128) + 64;

				hbr = ::CreateSolidBrush( RGB( r, g, b));

				::FillRect( dc, &rt, hbr);

				::DeleteObject( hbr);

				i3String::Format( szTemp, sizeof(szTemp), "%d", i);		i++;

				::DrawText( dc, szTemp, i3String::Length( szTemp), &rt, DT_CENTER | DT_VCENTER);

				pSrcTex->Lock( 0);

				// Bitmap의 Pixel Data는 세로 방향으로 뒤집혀 있다.
				{
					char * pDestLine, * pSrcLine;
					
					for( ln = 0; ln < 256; ln++)
					{
						// Line 단위로 복사
						pDestLine	= pSrcTex->GetLockedBuffer() + (ln * LineSize);
						pSrcLine	= pPixelData + ((255 - ln) * LineSize);

						i3mem::Copy( pDestLine, pSrcLine, LineSize);
					}
				}

				pSrcTex->Unlock();

				m_pTex->PlaceTexture( x * 256, y * 256, pSrcTex);
			}
		}

		I3_SAFE_RELEASE( pSrcTex);
	}

	::SelectObject( dc, hOldBmp);
	::SelectObject( dc, hOldFont);

	::DeleteObject( hBmp);
	::DeleteObject( hFont);

	m_pTex->Flush();

	return true;
}

void Ci3VTexDoc::OnToolCreatenewvirtualtexture()
{
	CreateNewVirualTexture();
}

void Ci3VTexDoc::OnToolDumpvirtualtexture()
{
	if( m_pTex != NULL)
	{
		m_pTex->Dump();
	}
}

void Ci3VTexDoc::OnToolVerifyvirtualtexture()
{
	if( m_pTex != NULL)
	{
		m_pTex->Verify();
	}
}

void Ci3VTexDoc::OnToolDumpfonttexture()
{
	i3UITextManager * pTextSys = g_pFramework->getUIRenderer()->getTextManager();
	
	i3UIFontManager * pFontSys = pTextSys->getFontManager();

	pFontSys->Dump();
}
