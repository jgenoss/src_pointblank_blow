#include "stdafx.h"
#include "MainFrm.h"
#include "UIGlobalRes.h"
#include "resource.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CMainFrame::CreateRibbon_Template(void)
{
	CMFCRibbonPanel * pPanel;
	CMFCRibbonButton * pBtn;

	// Add "Project" category
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory( LPCTSTR("Template"), IDB_TB_SM, IDB_TB_LA);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Template Library Panel
	{
		pPanel = pCategoryHome->AddPanel( "Tools");

		pBtn = new CMFCRibbonButton( ID_PROJECT_SAVETEMPLATELIBRARY, "Save Templates (OLD)", -1, 1, TRUE);
		pPanel->Add( pBtn);

		pBtn = new CMFCRibbonButton(ID_PROJECT_SAVETEMPLATELIBRARY_NEW, "Save Templates (NEW)", -1, 1, TRUE);
		pPanel->Add(pBtn);

		pBtn = new CMFCRibbonButton( ID_PROJECT_EXPORTTEMPLATELIBRARY, "Create New Templates", 4, 4, TRUE);
		pPanel->Add( pBtn);

		pBtn = new CMFCRibbonButton( ID_PROJECT_IMPORTTEMPLATELIBRARY, "Import Templates", 5, 5, TRUE);
		pPanel->Add( pBtn);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UI Templates
	{
		pPanel = pCategoryHome->AddPanel( "Add a new template");

		CSize sz;

		// Template Icon의 크기 설정.
		sz.cx = TEMPLATE_GALLERY_ICON_CX;
		sz.cy = TEMPLATE_GALLERY_ICON_CY;
		m_pGallery_Template = new CMFCRibbonGallery( ID_TEMPLATE_ADDANEWTEMPLATE, "Template Palette", 0, 0);

		m_pGallery_Template->SetButtonMode();
		m_pGallery_Template->SetIconsInRow(4);
		m_pGallery_Template->EnableMenuResize(TRUE);
		//m_pGallery_Template->EnableMenuSideBar( TRUE);

		pPanel->Add( m_pGallery_Template);

		_DrawTemplateGallery();
	}
}

void CMainFrame::_ResetTemplateGallery(void)
{
	m_pGallery_Template->Clear();

	for(size_t i = 0; i < m_listBitmap.size(); i++)
	{
		CMFCToolBarImages * pImages = m_listBitmap[i];

		delete pImages;
	}
}

void CMainFrame::_DrawTemplateGallery(void)
{
	i3::vector<i3ClassMeta*> list;
	i3::vector<i3UITemplate*>	templateList;
	INT32 cateCount[ I3UI_CATEGORY_MAX];

	_ResetTemplateGallery();

	i3mem::FillZero( cateCount, sizeof(cateCount));

	// UI Template Class에 속하는 모든 Class 구해 오기
	{
		i3UITemplate::static_meta()->get_all_derived_metas( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			i3ClassMeta * pMeta = list[i];

			if(	pMeta->get_class_meta_style() == i3::cms_abstract ) //	pMeta->IsAbstractClass())
				continue;

			i3UITemplate * pTemplate = (i3UITemplate *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			pTemplate->AddRef();

			templateList.push_back( pTemplate);

			cateCount[ pTemplate->getTemplateCategory()] ++;
		}
	}

	{
		// Icon을 그리기 위한 준비.
		HDC hDC, dc;
		HBITMAP hBitmap, hOldBitmap;
		HFONT hFont, hOldFont;
		HBRUSH hClearBrush;
		COLORREF TransColor = RGB( 255, 255, 255);
		i3UITemplate * pTemplate;

		{
			// DC
			dc = ::GetDC( NULL);
			hDC = ::CreateCompatibleDC( dc);
			::ReleaseDC( NULL, dc);

			// Bitmap
			hBitmap = CreateBitmap( TEMPLATE_GALLERY_ICON_CX, TEMPLATE_GALLERY_ICON_CY, 1, 32, NULL);

			hOldBitmap = (HBITMAP) ::SelectObject( hDC, hBitmap);

			// Font
			hFont = i3TDK::CreateDefFont( 11, "Tahoma", 400, CLEARTYPE_QUALITY);
			hOldFont = (HFONT) ::SelectObject( hDC, hFont);

			// Brush
			hClearBrush = ::CreateSolidBrush( TransColor);
		}

		for(INT32 i = 0; i < I3UI_CATEGORY_MAX; i++)
		{
			if( cateCount[ i] <= 0)
				continue;

			// Category별로 CMFCToolbarImages Class를 생성한다.
			// Bitmap을 추가할 ToolBarImages를 생성.
			CMFCToolBarImages * pImages = new CMFCToolBarImages;

			m_listBitmap.push_back( pImages);

			pImages->SetImageSize( CSize( TEMPLATE_GALLERY_ICON_CX, TEMPLATE_GALLERY_ICON_CY));
			//pImages->SetTransparentColor( TransColor);

			// 검색된 Template들을 돌아보며, 해당 Category의 Control이라면 Icon 이미지를 그린다.
			for(size_t j = 0; j < templateList.size(); j++)
			{
				pTemplate =templateList[j];

				if( pTemplate->getTemplateCategory() != i)
					continue;

				_DrawTemplate( hDC, hClearBrush, pTemplate);

				::SelectObject( hDC, hOldBitmap);
				pImages->AddImage( hBitmap);
				::SelectObject( hDC, hBitmap);
			}

			// Template Gallery에 추가한다.
			{
				LPCTSTR pszName = i3UITemplate::getTemplateCategoryName( (I3UI_CATEGORY)i);

				m_pGallery_Template->AddGroup( pszName, *pImages);
			}
		}

		{
			::DeleteObject( hClearBrush);
			::SelectObject( hDC, hOldFont);
			::SelectObject( hDC, hOldBitmap);

			::DeleteObject( hFont);
			::DeleteObject( hBitmap);
			::DeleteDC( hDC);
		}
	}

	for(size_t i = 0; i < templateList.size(); i++)
	{
		i3UITemplate * pTemplate = templateList[i];

		pTemplate->Release();
	}
}

void CMainFrame::_DrawTemplate( HDC dc, HBRUSH hClearBrush, i3UITemplate * pTemplate)
{
	CRect rc;
	CImageList * pImageList;

	rc.SetRect( 0, 0, TEMPLATE_GALLERY_ICON_CX, TEMPLATE_GALLERY_ICON_CX);

	pImageList = UIGlobalRes::getDefBigImageList();

	{
		::FillRect( dc, &rc, hClearBrush);
	}

	{
		INT32 idxImage = UIGlobalRes::getCategoryImageIndex( pTemplate->getTemplateCategory());
		POINT pt;
		CDC * pDC;
		
		pt.x = (TEMPLATE_GALLERY_ICON_CX >> 1) - (16 >> 1);
		pt.y = (32 >> 1) - (16 >> 1);

		pDC = CDC::FromHandle( dc);
		pImageList->Draw( pDC, idxImage, pt, ILD_NORMAL);
	}

	{
		rc.SetRect( 0, 32, TEMPLATE_GALLERY_ICON_CX, TEMPLATE_GALLERY_ICON_CY);

		SetBkMode( dc, TRANSPARENT);

		::DrawText( dc, pTemplate->getTemplateName(), -1, &rc, DT_VCENTER | DT_CENTER);
	}
}

void CMainFrame::OnTemplateAddanewtemplate()
{
	//INT32 idx = m_pGallery_Template->GetLastSelectedItem( ID_TEMPLATE_ADDANEWTEMPLATE);
}

void CMainFrame::checkWorkingFolder(void)
{
	const char* szWorkingDir = i3ResourceFile::GetWorkingFolder();

	if (szWorkingDir[0] != 0)
		return;
	
	MessageBox( "Working Directory 가 설정 되어 있지 않습니다.", "알림", MB_OK);

	OnProjectWorkingfolder();
	
	checkWorkingFolder();		// 다시 체크 루틴으로.. 설정 안하면 진행 못함!!!
}

void CMainFrame::checkVirtualTexture(void)
{
	i3VirtualTexture * pVirtualTexture = i3UI::getVirtualTexture();
	if( pVirtualTexture != NULL)
		return;

	if( m_strVTexPath.empty() )
	{
		MessageBox( "Virtual Texture가 설정 되어 있지 않습니다.", "알림", MB_OK);

		INT32 ret = MessageBox( "이미 만들어진 파일을 찾아 보시겠습니까?", "알림", MB_YESNO|MB_ICONINFORMATION );
		if( ret == IDYES)
		{
			OpenVirtualTexture();
		}
		else
		{
			CreateVirtualTexture(true);
		}
	}
	else
	{
		if( i3UI::OpenVirtualTexture(m_strVTexPath.c_str(), 2048, 2048) == false)
		{
			m_strVTexPath.clear();
		}
	}

	checkVirtualTexture();
}

void CMainFrame::OpenVirtualTexture()
{
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex != NULL && pVTex->hasName())
	{
		INT32 ret = MessageBox( "이미 설정된 Virtual Texture가 있습니다. \n Virtual Texture는 동시에 하나만 열 수 있습니다. \n 기존의 것을 닫고 새로 여시겠습니까?", "알림", MB_YESNO|MB_ICONINFORMATION );
		if( ret != IDYES)
			return;
	}

//	TCHAR szFilters[] = "i3VTex 파일(*.i3VTex)|*.i3VTex|";
	TCHAR szFilters[] = "i3VTex 파일(*.i3VTex)|*.i3VTex|신규 i3VTexList 파일(*.i3VTexList)|*.i3VTexList|";

	CFileDialog fileDlg(TRUE, _T("i3VTex"), _T(""), OFN_FILEMUSTEXIST, szFilters);

	if( fileDlg.DoModal() == IDOK)
	{
		i3UI::CloseVirtualTexture();

		char conv[MAX_PATH];
		i3::make_relative_path( CMainFrame::getWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv );

		if( i3UI::OpenVirtualTexture( conv, 2048, 2048) == true)
		{
			CMainFrame::SetVirtualTexturePath( conv);

			// 국가정보를 얻는다.
			this->UpdateNationFromVTex();
		}
	}
}

void CMainFrame::CreateVirtualTexture(bool bNewFormat)
{
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex != NULL && pVTex->hasName())
	{
		INT32 ret = MessageBox( "이미 설정된 Virtual Texture가 있습니다. \n 새로 만드시렵니까?", "알림", MB_YESNO|MB_ICONINFORMATION );
		if( ret != IDYES)
			return;
	}

	TCHAR szFilters[] = "i3VTex 파일(*.i3VTex)|*.i3VTex|";

	CFileDialog fileDlg(FALSE, _T("i3VTex"), _T(""), OFN_HIDEREADONLY, szFilters);

	if( fileDlg.DoModal() == IDOK)
	{
		i3UI::CloseVirtualTexture();

		i3VirtualTexture * pVTex2 = i3VirtualTexture::new_object_ref();

			
		if( pVTex2->GetFileFormat()->Create((LPCTSTR)fileDlg.GetPathName(), I3G_IMAGE_FORMAT_BGRA_8888, bNewFormat))
		{
			i3UI::setVirtualTexture( pVTex2, 2048, 2048);

			char conv[MAX_PATH];

			const char* szWorkingDir = CMainFrame::getWorkingFolder();

			i3::make_relative_path(szWorkingDir, (LPCTSTR)fileDlg.GetPathName(), conv );

			SetVirtualTexturePath( conv);
		}
	}
}
