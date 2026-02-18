#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Image.h"
#include "i3TDKPropertyGridProperty_ImageGroup.h"

#include "i3Base/string/ext/string_ncopy_nullpad.h"

#include "../resource.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Image, i3TDKPropertyGridProperty_String)

i3TDKPropertyGridProperty_Image::i3TDKPropertyGridProperty_Image( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty_String( pInfo)
{
	I3ASSERT( MAX_PATH <= GetBufSize());

	AllowEdit( FALSE);	

	SetName( _T("Path") );
	
	CreateViewer();
}
	
void i3TDKPropertyGridProperty_Image::AdjustButtonRect()
{
	i3TDKPropertyGridProperty::AdjustButtonRect();

	m_rectButton.left -= m_rectButton.Width();
}

void i3TDKPropertyGridProperty_Image::OnClickButton(CPoint point)
{
	bool bLeft = point.x < m_rectButton.CenterPoint().x;

	if( bLeft)
		OnButtonLeft();
	else
		OnButtonRight();
}

void i3TDKPropertyGridProperty_Image::OnButtonLeft( void)
{
	TCHAR szFilters[] = _T("i3i 파일(*.i3i)|*.i3i|TGA파일(*.tga)|*.tga|비트맵파일(*.bmp)|*.bmp|");

	CFileDialog fileDlg(TRUE, _T("i3i"), _T("*.i3i"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if( fileDlg.DoModal() == IDOK)
	{
		CString strName = fileDlg.GetPathName();
		TCHAR pszName[MAX_PATH];
		i3::safe_string_copy( pszName, strName, _countof( pszName) );

		if( m_ImageViewer.CheckFileName( pszName) )
		{
			SetValue( pszName);

			m_ImageViewer.LoadImageFile( pszName);
			*m_ppTexture = m_ImageViewer.GetImage();

			i3TDKPropertyGridProperty_String::UpdateFromProperty();

			i3TDKPropertyGridProperty_ImageGroup* pParent = (i3TDKPropertyGridProperty_ImageGroup*)GetParent();
			if( pParent != nullptr)
				pParent->UpdateFromProperty();
		}
		else
		{
			AfxMessageBox( _T("Not an image file") );
		}
	}
}

void i3TDKPropertyGridProperty_Image::OnButtonRight( void)
{
	if( *m_ppTexture != nullptr)
	{
		m_ImageViewer.SetWindowPos(  nullptr, 0, 0, 500, 500, SWP_SHOWWINDOW);
	}
	else
	{
		const char* pszImageFile = GetStringData();
#ifdef _UNICODE
		i3::stack_wstring wstrImageFile;	i3::mb_to_utf16(pszImageFile, wstrImageFile);
		const wchar_t* pszImageFilename = wstrImageFile.c_str();
#else
		const char* pszImageFilename = pszImageFile;
#endif
		m_ImageViewer.LoadImageFile( pszImageFilename);
		m_ImageViewer.SetWindowPos(  nullptr, 0, 0, 500, 500, SWP_SHOWWINDOW);

		*m_ppTexture = m_ImageViewer.GetImage();
	}
}

void i3TDKPropertyGridProperty_Image::OnDrawButton(CDC* pDC, CRect rectButton)
{
	//http://www.aspfree.com/c/a/.NET/List-Control-and-Property-Grid-with-the-MFC-Feature-Pack/3/
	
	// Button Image
	CString strImagePath = _T("res/button.jpg");
	CImage Image;
	HRESULT hResult = Image.Load(strImagePath);

	if( FAILED( hResult))
	{
		TRACE( "Failed to load\n");
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		CMFCToolBarButton button;
		CRect rect = rectButton;

		if (i == 0)
		{
			rect.right = rect.left + rect.Width() / 2;
		}
		else
		{
			rect.left = rect.right - rect.Width() / 2;
		}

		// Acquire some visual studio style
		CMFCVisualManager::AFX_BUTTON_STATE state = CMFCVisualManager::ButtonsIsHighlighted;
		CMFCVisualManager::GetInstance()-> OnFillButtonInterior(pDC, &button, rect, state);
		
		Image.BitBlt( pDC->m_hDC, rect.left, rect.top, 14, 14, 0, 0);

		CMFCVisualManager::GetInstance()-> OnDrawButtonBorder(pDC, &button, rect, state);
	}
}

void i3TDKPropertyGridProperty_Image::UpdateFromProperty( void* pData)
{
	
}

void i3TDKPropertyGridProperty_Image::CreateViewer( void)
{
	if( m_ImageViewer.IsCreated())
		return;

	m_ImageViewer.Create( WS_POPUPWINDOW | WS_CAPTION | WS_SIZEBOX , nullptr, 0);

	if((m_ppTexture == nullptr) || (*m_ppTexture == nullptr))
	{

		const char* pszImageFile = GetStringData();
#ifdef _UNICODE
		i3::stack_wstring wstrImageFile;	i3::mb_to_utf16(pszImageFile, wstrImageFile);
		const wchar_t* pszImageFilename = wstrImageFile.c_str();
#else
		const char* pszImageFilename = pszImageFile;
#endif

		if( m_ImageViewer.CheckFileName( pszImageFilename) )
			m_ImageViewer.LoadImageFile( pszImageFilename );
	}
	else
	{
		m_ImageViewer.SetImage( *m_ppTexture); 
	}
}


// Description창에 그림 띄우기.. 사용하지 않음.
//void i3TDKPropertyGridProperty_Image::OnDrawDescription( CDC* pDC, CRect rect)
//{
//	// Test1
//	/*CString strImagePath = _T("c:/test.jpg");
//	CImage Image;
//	HRESULT hResult = Image.Load(strImagePath);
//
//	if( FAILED( hResult))
//	{
//		TRACE( "Failed to load\n");
//		return;
//	}
//	
//	Image.BitBlt( pDC->m_hDC, rect.left, rect.top, Image.GetWidth(), Image.GetHeight(), 0, 0);*/
//
//	
//	// Test 2
//	static bool bFirst = true;
//
//	if( bFirst)
//	{
//		if( !m_ImageViewer.IsCreated())
//		{
//			//CWnd* pParent = GetParentWindow();
//			//I3ASSERT( pParent != nullptr);
//			
//			m_ImageViewer.Create( WS_POPUPWINDOW | WS_HSCROLL | WS_VSCROLL /*| WS_CAPTION | WS_SIZEBOX*/ , nullptr, 0);	
//		}
//
//		char* pszImageFile = GetStringData();
//
//		if( m_ImageViewer.CheckFileName( pszImageFile) )
//		{
//			m_ImageViewer.LoadImageFile( pszImageFile);
//			
//		}
//		bFirst = false;
//	}
//
//	m_ImageViewer.SetWindowPos(  nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
//
//	I3TRACE("OnDrawDescription\n");
//}

//BOOL i3TDKPropertyGridProperty_Image::OnSetCursor() const
//{
//	I3TRACE("OnSetCursor\n");
//	return TRUE;
//}

#endif