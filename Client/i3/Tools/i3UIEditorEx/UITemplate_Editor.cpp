// UITemplate_Editor.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "UITemplate_Editor.h"
#include "GlobalVariables.h"

#include "Control/UIGripCtrlRect.h"
#include "MainFrm.h"
#include "UIUVSelectorDlg.h"

#include "Dlg_SelectTexture.h"
#include "UIGlobalRes.h"

#include "LeftAlignValueDlg.h"
#include "Dlg_SplitValue.h"

#include "i3Base/string/compare/generic_is_iequal.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"

// CUITemplate_Editor

IMPLEMENT_DYNAMIC(CUITemplate_Editor, CWnd)

CUITemplate_Editor::CUITemplate_Editor()
: m_nLRCnt(0)
, m_nUDCnt(0)
, m_pfClickEvent(NULL)
{
	m_pCurrentItem = NULL;
	m_pCurrentCtrl = NULL;

	m_bLBDragging  = false;
	m_ptDragStart.SetPoint( 0,0);

	m_pFocusedPoint	= NULL;
	m_pFocusedShape	= NULL;
	m_mode			= GCM_NONE;

	m_bSelLock		= false;

	m_pTestScene	= NULL;
	m_pDummyFrameWnd= NULL;

	m_bSendToFramework = false;

	m_bSpreaded			= false;
	m_bSelectTemplate		= false;
	m_bNotify			= true;

	m_ShowX			= 0;
	m_ShowY			= 0;

	m_ScrollX = 0;
	m_ScrollY = 0;

	m_OffsetX		= 0;
	m_OffsetY		= 0;
	
	m_WndSz.cx = 0;
	m_WndSz.cy = 0;

	i3Vector::Set( &m_vSizeCurCtrl, 0.0f, 0.0f);

	m_MouseMode = MOUSEMODE_NONE;

	m_pGripManager.Create( CUIGripObjHandler_Shape::static_meta());

	m_bBanding = false;

}

CUITemplate_Editor::~CUITemplate_Editor()
{
	I3_SAFE_RELEASE( m_pDummyFrameWnd);
	I3_SAFE_RELEASE( m_pTestScene);
	I3_SAFE_RELEASE( m_pCurrentItem);
}

void CUITemplate_Editor::setCurrentTemplate( i3UITemplate * pTemplate)
{
	if( pTemplate == m_pCurrentItem)
		return;

	_CleanItem();

	
	I3_REF_CHANGE( m_pCurrentItem, pTemplate);

	if( _BuildItem() == false)
	{
		I3_SAFE_RELEASE( m_pCurrentItem);

		::InvalidateRect( m_hWnd, NULL, false);

		return;
	}
	
	if( m_pCurrentItem != NULL)
	{
		m_pCurrentItem->GetControlRect(&m_Rect);

		m_pCurrentCtrl->setSize( m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top);

		m_pCurrentCtrl->EnableCtrl( true, true);
		m_pCurrentCtrl->removeState( I3UI_STATE_WAITLOAD);

		i3Vector::Set( &m_vSizeCurCtrl, m_pCurrentCtrl->getWidth(), m_pCurrentCtrl->getHeight());

		::InvalidateRect( m_hWnd, NULL, false);
	}

	ResetMoveCellValue();
}

void CUITemplate_Editor::_CleanItem()
{
	if( m_pCurrentItem == NULL)
		return;

	if( m_bSpreaded)
	{
		AdjustLayout();
	}

	m_pDummyFrameWnd->SetFocusCtrl( NULL);

	SendInputToFramework( false);

	i3Framework::ReleaseCapture();

	i3UIManager * pUIManager = i3UI::getManager();
	pUIManager->resetOnMouseTarget();
	
	m_pCurrentCtrl->EnableCtrl( false, true);
	if( m_pCurrentCtrl->getParent() != NULL)
		m_pCurrentCtrl->getParent()->RemoveChild( m_pCurrentCtrl);

	I3_SAFE_RELEASE( m_pCurrentCtrl);
	I3_SAFE_RELEASE( m_pCurrentItem);
}

// 아무것도 설정되어 있지 않다면 기본 초기화를 시도 한다.
bool CUITemplate_Editor::_ValidateTemplate( i3UITemplate * pTemplate)
{
	m_pGripManager.Reset();

	i3Texture * pEmptyImg = UIGlobalRes::getTexture( UITEXTURE_EMPTY);
	if( !pEmptyImg )
	{
		UIGlobalRes::InitAfter();
		pEmptyImg = UIGlobalRes::getTexture( UITEXTURE_EMPTY);
	}
	I3ASSERT( pEmptyImg != NULL);

	i3VirtualTexture * pVTex = _checkVirtualTexture();
	if( pVTex == NULL)
	{
		I3TRACE("failed virtual texture check\n");
		return false;
	}

	INT32 offX = 0, offY = 0;

	i3VTFileFormat* pFF = pVTex->GetFileFormat();

	pFF->PlaceTexture( pEmptyImg, &offX, &offY);
	pFF->Flush();
	pFF->SaveRefTextureInfo();

	bool bNeedInitLayout = false;
	INT32 i;//, col = 0, row = 0;
	INT32 shapeCnt = pTemplate->getShapeCount();
	for( i = 0;i < shapeCnt; ++i)
	{
		i3UIShape * pShape = pTemplate->getShape( i);
		if( pShape->hasTexture() == false ||
			i3::generic_is_iequal( pEmptyImg->getResourceLoadPath(), pShape->getTextureResPath() ) )
		{
			i3::pack::RECT rt;
			rt.left = rt.top = 0.00001f;
			rt.right = rt.bottom = 127.0f;

			pShape->setTextureResPath( pEmptyImg->getResourceLoadPath());
			pShape->SetTextureCoord( &rt);
			pShape->setTexcoordOffset( offX, offY);

			bNeedInitLayout = true;
		}
	}

	if( bNeedInitLayout)
	{
		pTemplate->InitializeLayout();
	}

	return true;
}

bool CUITemplate_Editor::_BuildItem()
{
	if( m_pCurrentItem == NULL)
		return false;

	i3ClassMeta * pMeta = m_pCurrentItem->GetInstanceMeta();
	I3ASSERT( pMeta != NULL && i3::kind_of<i3UIControl*>(pMeta)); //->IsTypeOf( i3UIControl::static_meta()));

 	if( _ValidateTemplate( m_pCurrentItem) == false)
 	{
 		return false;
 	}

	i3UIControl * pCtrl = (i3UIControl*)pMeta->create_instance(); // CREATEINSTANCE( pMeta);
	pCtrl->SetTemplate( m_pCurrentItem);

	//editbox의 동작을 살펴보기 위해 ime를 enable시킨 상태입니다. 필요없으면 삭제해도 됩니다.
	if( i3::kind_of<i3UIEditBox*>(pCtrl)) //->IsTypeOf( i3UIEditBox::static_meta()))
	{
		i3UIEditBox * pBox = (i3UIEditBox*)pCtrl;

		pBox->SetMaxTextCount( 128);
		pBox->SetReadOnly( false);
	}

	I3_REF_CHANGE( m_pCurrentCtrl , pCtrl);

	return true;
}

void CUITemplate_Editor::AttachScene()
{
	SetTimer( 1, 16, NULL);

	/*
	if( m_pCurrentCtrl != NULL)
	{
		m_pCurrentCtrl->EnableCtrl( true, true);
		if( m_bSpreaded)
		{
			m_pCurrentCtrl->setAllShapeVisible(true);
		}
	}
	*/
}

void CUITemplate_Editor::DetachScene()
{
	/*
	UnSelectAll();

	if( m_pCurrentCtrl != NULL)	m_pCurrentCtrl->EnableCtrl( false, true);
	*/

	KillTimer( 1);
}

void CUITemplate_Editor::SelectTexture(void)
{
	I3VT_TEXINFO * pSelTex = NULL;

	const i3::vector<CUIGripCtrl*>& SelectedItem = m_pGripManager.getSelectedItemList();

	i3VirtualTexture * pVTex = _checkVirtualTexture();
	if( pVTex == NULL)
		return;

	{
		CDlg_SelectTexture dlg;

		if( dlg.Execute( pVTex) == false)
			return;

		pSelTex = dlg.getSelectedRefTexture();

		if( pSelTex == NULL)
			return;
	}

	m_bSelLock = true;
	{
		i3::vector<i3UIShape*> shapeList;

		for( size_t i=0; i< SelectedItem.size(); i++)
		{
			CUIGripCtrl * pCtrl = SelectedItem[i];
			i3UIShape * pShape = (i3UIShape*)pCtrl->getObject();

			shapeList.push_back( pShape);
		}

		if( 0 < shapeList.size())
		{
			INT32 curIdx = _UVSelectOneTouch( shapeList, pSelTex, 0);		//_UVSelectOneTouch !!
			
			while( curIdx > 0 && curIdx < (INT32)shapeList.size())
			{
				curIdx = _UVSelectOneTouch( shapeList, pSelTex, curIdx);
			}

			CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
			Ci3UIEditorView_Template* pView = (Ci3UIEditorView_Template*)(frame->GetActiveView());

			if (pView)
				pView->UpdateShapeListCtrl();
			else
				assert(0);
		}
	}
	m_bSelLock = false;
}

void CUITemplate_Editor::SelectUV(void)
{
	const i3::vector<CUIGripCtrl*>& SelectedItem = m_pGripManager.getSelectedItemList();

	i3VirtualTexture * pVTex = _checkVirtualTexture();
	if( pVTex == NULL)
		return;

	m_bSelLock = true;
	{
		
		for(size_t i = 0;i < SelectedItem.size();++i)
		{
			CUIGripCtrl * pCtrl = SelectedItem[i];

			i3UIShape * pShape	= (i3UIShape*)pCtrl->getObject();
			if( pShape->hasTexture())
			{
				I3VT_TEXINFO * pTexInfo = pVTex->FindRefTexture( pShape->getTextureResPath());

				if(pTexInfo != NULL)
				{
					_UVSelect( pShape, pTexInfo, true);
				}
			}
		}
	}
	m_bSelLock = false;
}

void CUITemplate_Editor::AlignLeftTop()
{
	const i3::vector<CUIGripCtrl*>& SelectedItem = m_pGripManager.getSelectedItemList();


	CLeftAlignValueDlg Dlg;
	if( Dlg.DoModal() == IDOK)
	{
		REAL32 X = Dlg.m_X;
		REAL32 Y = Dlg.m_Y;

		for(size_t i = 0;i < SelectedItem.size();++i)
		{
			CUIGripCtrl * pCtrl = SelectedItem[i];

			i3UIShape * pShape	= (i3UIShape*)pCtrl->getObject();

			pShape->setPosX( X);
			pShape->setPosY( Y);
		}

		AdjustLayout();
	}
}

void CUITemplate_Editor::Spread()
{
	const i3::vector<CUIGripCtrl*>& SelectedItem = m_pGripManager.getSelectedItemList();
	

	CDlg_SplitValue Dlg;
	if( Dlg.DoModal() == IDOK)
	{
		INT32 col	= Dlg.m_Col;
		INT32 Margin = Dlg.m_Margin;

		{
			REAL32 left = 0.0f;
			REAL32 top = 0.0f;

			for(size_t i = 0;i < SelectedItem.size();++i)
			{
				CUIGripCtrl * pCtrl = SelectedItem[i];

				i3UIShape * pShape	= (i3UIShape*)pCtrl->getObject();

				pShape->setPosX( left);
				pShape->setPosY( top);
				
				left += pShape->getWidth() + Margin;

				if((INT32(i)%col) == (col-1))
				{
					left = 0.0f;
					top += pShape->getHeight() + Margin;
				}

				I3TRACE("x: %d, y: %d\n", left, top);
			}
		}

		AdjustLayout();
	}
}

void CUITemplate_Editor::SpreadLayout()
{
	if( m_pCurrentItem != NULL)
	{
		REAL32 x, y;
		m_pCurrentItem->SpreadLayout(&x, &y);

		if( m_pCurrentCtrl->isEnable() == false)
			m_pCurrentCtrl->EnableCtrl( true, true);

		m_pCurrentCtrl->setSpreadShape( true);
		m_pCurrentCtrl->setAllShapeVisible(true);

		if( m_bNotify)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIShape::static_meta());

		m_pGripManager.Reset();

		Invalidate(FALSE);

		m_bSpreaded = true;
		
		ResetMoveCellValue();
	}
}

void CUITemplate_Editor::AdjustLayout()
{
	if( m_pCurrentItem != NULL)
	{
		if( m_pCurrentCtrl->isEnable() == false)
			m_pCurrentCtrl->EnableCtrl( true, true);

		m_pCurrentCtrl->setSpreadShape( false);

		m_pCurrentItem->AdjustLayout( (REAL32)(m_Rect.right - m_Rect.left), (REAL32)(m_Rect.bottom - m_Rect.top));
		m_pCurrentItem->GetControlRect(&m_Rect);

		m_pCurrentCtrl->setSize( m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top);

		m_pGripManager.Reset();

		if( m_bNotify)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIShape::static_meta());

		Invalidate(FALSE);

		m_bSpreaded = false;

		ResetMoveCellValue();
	}
}

bool CUITemplate_Editor::_RecalculateTexcoordByVTex( i3UIShape * pShape, I3VT_TEXINFO * pTexInfo)
{
	pShape->setTexcoordOffset( pTexInfo->m_X, pTexInfo->m_Y);

	return true;
}

i3VirtualTexture * CUITemplate_Editor::_checkVirtualTexture()
{
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex == NULL)
	{
		CMainFrame * pMainWnd = (CMainFrame *) ::AfxGetMainWnd();

		pMainWnd->checkVirtualTexture();
	}

	return i3UI::getVirtualTexture();
}

//_UVSelect 변형: 한번에 여러 Shape의 TexCoord를 설정할 수 있습니다.
INT32 CUITemplate_Editor::_UVSelectOneTouch( const i3::vector<i3UIShape*>& ShapeList, I3VT_TEXINFO* pTexInfo, INT32 curIdx)
{
	INT32 nRet = curIdx;
	i3VirtualTexture * pVTex = _checkVirtualTexture();

	if( pVTex == NULL)
		return 0;

	// 임시로 사용할 Texture를 생성한다.
	i3Texture * pTex = i3Texture::new_object();
	pTex->Create( pTexInfo->m_Width, pTexInfo->m_Height, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_FREQUENT_UPDATE);

	char szFull[ MAX_PATH];
	
	if( i3ResourceFile::GetWorkingFolder()[0] != 0)
		i3::snprintf( szFull, _countof( szFull), "%s/%s", i3ResourceFile::GetWorkingFolder(), pTexInfo->m_strName);
	else
		i3::snprintf( szFull, _countof( szFull), "%s", pTexInfo->m_strName);

	if( i3System::IsFile( szFull))
	{
		IDirect3DSurface9 * pSurf = pTex->getSurface(0)->getSurface();

		I3PRINTLOG(I3LOG_NOTICE, "원본 파일(%s)에서 읽어 옵니다.", szFull);

		D3DXLoadSurfaceFromFile( pSurf, NULL, NULL, szFull, NULL, D3DX_FILTER_POINT, 0, NULL);
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "원본 파일(%s)가 존재하지 않아, Virtual Texture의 이미지를 직접 사용합니다.", pTexInfo->m_strName.c_str());

		pVTex->ExtractTexture( pTex, pTexInfo->m_X, pTexInfo->m_Y, pTexInfo->m_Width, pTexInfo->m_Height);
	}

	CUIUVSelectorDlg Dlg;
	Dlg.SetSourceTexture( pTex);
	
	if( m_pCurrentItem != NULL)
	{
		if( curIdx < (INT32)ShapeList.size())
		{
			i3UIShape* pShape = ShapeList[curIdx];

			for( INT32 i=0; i<m_pCurrentItem->getShapeCount(); i++)
			{
				i3UIShape * pUIShape = m_pCurrentItem->getShape( i);
				if( pShape == pUIShape)
				{
					const char * pszInfoString = m_pCurrentItem->getShapeInfoString(i);
					Dlg.SetWindowCaption( pszInfoString);
					break;
				}
			}
		}
	}

	if( Dlg.DoModal() == IDOK)
	{
		while( Dlg.getSelectedUVCount() > 0 && (INT32)ShapeList.size() > nRet)
		{
			i3UIShape* pShape = ShapeList[nRet++];

			Dlg.SetTextureCoordinateFromFirstUV( pShape);	// First UV로 설정
			
			_RecalculateTexcoordByVTex( pShape, pTexInfo);
			pShape->setTextureResPath( pTexInfo->m_strName.c_str());

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pShape);
	
			Dlg.UnSelectFirstUV();	// First UV를 제거: getSelectedUVCount를 1만큼 감소시킨다. 
		}
	}
	else
	{
		nRet++;
	}

	pTex->Release();

	return nRet;
}

bool CUITemplate_Editor::_UVSelect( i3UIShape * pShape, I3VT_TEXINFO * pTexInfo, bool bHasUV)
{
	CUIUVSelectorDlg Dlg;
	i3Texture * pTex;
	bool bResult = false;

	i3VirtualTexture * pVTex = _checkVirtualTexture();
	if( pVTex == NULL)
		return false;

	// 임시로 사용할 Texture를 생성한다.
	pTex = i3Texture::new_object();
	pTex->Create( pTexInfo->m_Width, pTexInfo->m_Height, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_FREQUENT_UPDATE);

	char szFull[ MAX_PATH];
	
	if( i3ResourceFile::GetWorkingFolder()[0] != 0)
		i3::snprintf( szFull, _countof( szFull), "%s/%s", i3ResourceFile::GetWorkingFolder(), pTexInfo->m_strName);
	else
		i3::snprintf( szFull, _countof( szFull), "%s", pTexInfo->m_strName);

	if( i3System::IsFile( szFull))
	{
		IDirect3DSurface9 * pSurf = pTex->getSurface(0)->getSurface();

		I3PRINTLOG(I3LOG_NOTICE, "원본 파일(%s)에서 읽어 옵니다.", szFull);

		D3DXLoadSurfaceFromFile( pSurf, NULL, NULL, szFull, NULL, D3DX_FILTER_POINT, 0, NULL);
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "원본 파일(%s)가 존재하지 않아, Virtual Texture의 이미지를 직접 사용합니다.", pTexInfo->m_strName.c_str());

		pVTex->ExtractTexture( pTex, pTexInfo->m_X, pTexInfo->m_Y, pTexInfo->m_Width, pTexInfo->m_Height);
	}

	Dlg.SetSourceTexture( pTex);
	if( bHasUV)
	{
		Dlg.setInitialTexCoord( pShape->getCoordU(), pShape->getCoordV(), pShape->getCoordW(), pShape->getCoordH());
	}

	if( m_pCurrentItem != NULL)
	{
		INT32 i;
		for( i = 0;i < m_pCurrentItem->getShapeCount(); ++i)
		{
			i3UIShape * pUIShape = m_pCurrentItem->getShape( i);
			if( pShape == pUIShape)
			{
				const char * pszInfoString = m_pCurrentItem->getShapeInfoString(i);

				Dlg.SetWindowCaption( pszInfoString);

				break;
			}
		}
	}

	if( Dlg.DoModal() == IDOK)
	{
		Dlg.SetTextureCoordinate( pShape);

		_RecalculateTexcoordByVTex( pShape, pTexInfo);

		pShape->setTextureResPath( pTexInfo->m_strName.c_str());

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pShape);

		bResult = true;
	}

	pTex->Release();

	return false;
}

BEGIN_MESSAGE_MAP(CUITemplate_Editor, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()



// CUITemplate_Editor message handlers

BOOL CUITemplate_Editor::Create( DWORD dwStyle, const RECT &rt, CWnd * pWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, g_hbrBackground, NULL);

	return CWnd::Create( pszClass, "", dwStyle, rt, pWnd, nID);
}
void CUITemplate_Editor::SetMouseClickCallBackFunc(pfCallback cfunc)
{
	m_pfClickEvent = cfunc;
}
						

int CUITemplate_Editor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	i3TDK::RegisterUpdate( m_hWnd, i3VirtualTexture::static_meta(), I3_TDK_UPDATE_SET);
	i3TDK::RegisterUpdate( m_hWnd, i3UIShape::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3UIControl::static_meta(), I3_TDK_UPDATE_EDIT);
	
	{
		CRect rc;
		GetClientRect( &rc);

		m_pDummyFrameWnd = i3UIFrameWnd::new_object();
		m_pDummyFrameWnd->setPos( 0.0f,0.0f, 0.0f);
		m_pDummyFrameWnd->setSize((REAL32)g_pViewer->GetViewWidth(),(REAL32)g_pViewer->GetViewWidth());

		m_pTestScene = i3UIScene::new_object();
		m_pTestScene->AddChild( m_pDummyFrameWnd);
	}

	return 0;
}

void CUITemplate_Editor::OnDestroy()
{
	DetachScene();

	I3_SAFE_RELEASE( m_pCurrentItem);
	I3_SAFE_RELEASE( m_pCurrentCtrl);

	CWnd::OnDestroy();
}


void CUITemplate_Editor::OnPaint()
{
	CPaintDC PaintDC(this); // device context for painting

	if( (g_pViewer == NULL) || !i3::kind_of<i3UIControl*>(m_pCurrentCtrl))
		return;
	
	if( g_pInterface != NULL && m_bSendToFramework == true && theApp.getRunModuleEnable())
	{
		g_pInterface->OnDrive( g_pViewer);
	}
	else
	{
		i3UIRenderer * pUIRenderer = i3UIRenderer::getInstance();

		if( m_pCurrentCtrl != NULL)
		{
			if( m_bSendToFramework == false)
			{
				if( m_bLBDragging == false)
				{
					m_OffsetX = MAX( 0, (g_pViewer->GetViewWidth() >> 1) - (INT32)(m_pCurrentCtrl->getWidth() * 0.5f));
					m_OffsetY = MAX( 0, (g_pViewer->GetViewHeight() >> 1) - (INT32)(m_pCurrentCtrl->getHeight() * 0.5f));

					if (m_bSpreaded == false)
						m_pCurrentCtrl->setPos((REAL32)m_OffsetX, (REAL32)m_OffsetY, 0.0f);
				}

				POINT pt;
				pt.x = m_OffsetX;
				pt.y = m_OffsetY;

				m_pGripManager.SetOffsetPos(pt);

				if( m_bSpreaded)
				{
					m_pCurrentCtrl->setAllShapeVisible(true);
					m_pCurrentCtrl->UpdateRenderObjects();
				}
				else
				{
					m_pCurrentCtrl->setModifyState(true);
					m_pCurrentCtrl->OnUpdate( 0.016f);
				}

				m_pCurrentCtrl->OnDraw( pUIRenderer);
			}				
		}

		pUIRenderer->Draw_Finish();

		g_pViewer->Render();
	}

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

	HDC dc = pFrame->GetUIDC();
	if( dc != NULL)
	{
		if( m_bSendToFramework == false)
			m_pGripManager.Draw( dc);
		
		//source's left and top depend on the scroll position
		INT32 xSrc = m_ShowX;
		INT32 ySrc = m_ShowY;

		//source's width and height depend on the zoom rate
		INT32 wSrc = (INT32)(m_WndSz.cx / g_fZoom);
		INT32 hSrc = (INT32)(m_WndSz.cy / g_fZoom);
		
		//Always same
		INT32 xDest = 0;	
		INT32 yDest = 0;
		INT32 wDest = m_WndSz.cx;
		INT32 hDest = m_WndSz.cy;

		::StretchBlt( PaintDC.m_hDC, xDest, yDest, wDest, hDest, dc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);

		/*if( m_bSendToFramework == false)
			m_pGripManager.Draw( PaintDC.m_hDC);*/

		pFrame->ReleaseUIDC( dc);
	}

	i3Vector::Set( &m_vSizeCurCtrl, m_pCurrentCtrl->getWidth(), m_pCurrentCtrl->getHeight());
}

void CUITemplate_Editor::_AdjustRect( CPoint point1, CPoint point2, CRect * prtDrag)
{
	prtDrag->SetRect( MIN( point1.x , point2.x), MIN( point1.y, point2.y),
					MAX( point1.x , point2.x), MAX( point1.y, point2.y));
}


bool CUITemplate_Editor::_IsptInImage( CPoint point, i3UIShape * pShape)
{
	CRect rt;
	rt.SetRect( (INT32)pShape->getPosX(), (INT32)pShape->getPosY(), (INT32)(pShape->getPosX() + pShape->getWidth()), (INT32)(pShape->getPosY() + pShape->getHeight()));

	REAL32 tmpX = m_pCurrentCtrl->getPos()->x - m_OffsetX;
	REAL32 tmpY = m_pCurrentCtrl->getPos()->y - m_OffsetY;
	
	tmpX = pShape->getPosX() + tmpX;
	tmpY = pShape->getPosY() + tmpY;

	CRect rtTmp;
	rtTmp.SetRect( (INT32)tmpX, (INT32)tmpY, (INT32)(tmpX + pShape->getWidth()), (INT32)(tmpY + pShape->getHeight()));

	rtTmp.NormalizeRect();

	if( rtTmp.PtInRect( point))
		return true;
	else
		return false;
}

bool CUITemplate_Editor::_IsincludeRect( CRect *prtDrag, i3UIShape * pShape)
{
	CRect rt;
	rt.SetRect( (INT32)pShape->getPosX(), (INT32)pShape->getPosY(), (INT32)(pShape->getPosX() + pShape->getWidth()), (INT32)(pShape->getPosY() + pShape->getHeight()));

	rt.NormalizeRect();

	INT32 includecnt = 0;

	if( prtDrag->PtInRect( rt.TopLeft())) includecnt++;	
	if( prtDrag->PtInRect( rt.BottomRight())) includecnt++;
	if( prtDrag->PtInRect( CPoint( rt.right, rt.top))) includecnt++;
	if( prtDrag->PtInRect( CPoint( rt.left, rt.bottom))) includecnt++;

	if( includecnt == 0)
	{
		if( rt.PtInRect( prtDrag->TopLeft())) includecnt++;	
		if( rt.PtInRect( prtDrag->BottomRight())) includecnt++;
		if( rt.PtInRect( CPoint( prtDrag->right, prtDrag->top))) includecnt++;
		if( rt.PtInRect( CPoint( prtDrag->left, prtDrag->bottom))) includecnt++;
	}

	if( includecnt > 0)
		return true;
	else 
		return false;
}


void CUITemplate_Editor::_SelectItemInRect( CRect *prtDrag, bool bReverse)
{
	if( m_pCurrentItem == NULL)	return;

	INT32 i;
	INT32 shapeCnt = m_pCurrentItem->getShapeCount();

	for( i = 0;i < shapeCnt; ++i)
	{
		i3UIShape * pShape = m_pCurrentItem->getShape( i);

		if( _IsincludeRect( prtDrag, pShape))
		{
			if( bReverse)
			{
				if( m_pGripManager.isSelected( pShape))
					UnSelect( pShape);
				else
					Select( pShape);
			}
			else
			{
				Select( pShape);
			}

			if( m_bSpreaded == false)
			{
				// Shape가 펼쳐진 상태가 아니라면 첫번째 1개만 선택하고 바로 종료.
				break;
			}
		}
	}
}

INT32 CUITemplate_Editor::Select( i3UIShape * pShape)
{
	if( m_bSelLock == false)
	{
		INT32 rv = m_pGripManager.AddSelect( pShape, NULL, false);

		if( m_bNotify)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pShape, i3UIShape::static_meta());
		}

		return rv;
	}
	else
		return -1;
}

INT32 CUITemplate_Editor::SelectTemplate(bool bSelect)
{
	if( m_pCurrentItem == NULL)
	{
		m_bSelectTemplate = false;
		return 0;
	}

	if( bSelect && (m_bSpreaded == false))
	{
		UnSelectAll();
		m_pCurrentItem->GetControlRect( &m_Rect);
		m_pGripManager.AddSelect( &m_Rect, CUIGripObjHandler_RECT::static_meta(), false);

		if( m_bNotify)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, m_pCurrentCtrl);
		}
	}
	else
	{
		m_pGripManager.RemoveSelect( &m_Rect);

		if( m_bNotify)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, m_pCurrentCtrl);
		}
	}

	m_bSelectTemplate = bSelect;

	return 1;
}

void CUITemplate_Editor::SelectAll(void)
{
	if( m_pCurrentItem == NULL)	return;

	// 기존에 선택된 것들을 모두 제거한후 순서대로 추가한다.
	UnSelectAll();

	INT32 i;
	INT32 shapeCnt = m_pCurrentItem->getShapeCount();
	for( i = 0;i < shapeCnt; ++i)
	{
		i3UIShape * pShape = m_pCurrentItem->getShape( i);

		Select( pShape);
	}
}

void CUITemplate_Editor::UnSelect( i3UIShape * pShape)
{
	if( m_bSelLock == false)
	{	
		m_pGripManager.RemoveSelect( pShape);

		if( m_bNotify)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pShape);
	}
}

void CUITemplate_Editor::UnSelectAll(void)
{
	if( m_bSelLock == false)
	{		
		m_pGripManager.Reset();

		if( m_bNotify)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIShape::static_meta());
	}
}

void CUITemplate_Editor::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint ptRel = _AbsToRel( point);

	m_ptDragStart = ptRel;

	if( m_pGripManager.OnLButtonDown( ptRel))
	{
		::InvalidateRect( g_hActiveView, &g_rtTarget, false);

		m_bLBDragging = true;
		return;
	}
	else
	{
		ptRel = _AbsToRel( point, false);

		m_bBanding = true;
		m_pGripManager.StartBanding( ptRel);
	}

	m_bLBDragging = false;
}

void CUITemplate_Editor::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->SetFocus();

	if( m_bBanding == true)
	{
		m_bBanding = false;
		m_pGripManager.EndBanding();
	}

	if (m_pfClickEvent)
		m_pfClickEvent();

	CPoint ptRel = _AbsToRel( point);
	
	CRect rtDrag;
	_AdjustRect( ptRel, m_ptDragStart, &rtDrag);	// 주어지는 두 위치를 이용하여 뒤집혀 지지 않은 rect를 만들어 낸다.

	m_pGripManager.OnLButtonUp( ptRel);

	if( m_bLBDragging == false)
	{
		SelectTemplate( false);

		bool bAddMode = (::GetKeyState( VK_LCONTROL) & 0x80)? true : false;

		if( bAddMode == false)
			UnSelectAll();

		if( m_bSpreaded)
			_SelectItemInRect( &rtDrag, bAddMode);		// Add Mode인 경우, Reverse 형태로 동작.

		if( m_pGripManager.getSelectedItemCount() <= 0)
		{
			SelectTemplate( true);
		}
	}

	m_bLBDragging = false;

	::InvalidateRect( m_hWnd, &g_rtTarget, false);
}

void CUITemplate_Editor::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_MouseMode == MOUSEMODE_NONE)
	{
		CPoint ptRel = _AbsToRel( point);

		if( !m_bLBDragging)
		{
			HCURSOR hCursor = m_pGripManager.getCursor( ptRel);
			::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
		}
		
		if( m_bBanding)
		{
			CPoint pt2 = _AbsToRel( point, false);
			m_pGripManager.Banding( pt2);

			Invalidate( FALSE);
		}

		{
			CUIGripCtrlPoint * pFocused = m_pGripManager.getFocusedPoint();
			if( pFocused != NULL)
			{
				if( pFocused ->getStyle() & UIGRIP_CORNER_LEFT)	ptRel.x = 0;
				if( pFocused ->getStyle() & UIGRIP_CORNER_TOP)	ptRel.y = 0;
			}
		}

		if( (m_bSpreaded == false) && m_pGripManager.OnMouseMove( ptRel, nFlags))
		{
			if( ptRel != m_ptOldPos)
			{
				REAL32 w = m_Rect.right - m_Rect.left;
				REAL32 h = m_Rect.bottom - m_Rect.top;

				if (m_pCurrentItem)
				{
					m_pCurrentItem->AdjustLayout(w, h);
					m_pCurrentItem->GetControlRect(&m_Rect);

					m_pCurrentCtrl->setSize(w, h);
					//I3TRACE("CUITemplate_Editor::OnMouseMove: %f, %f\n",w, h);

					m_pGripManager.RecalcLayout();
				}

				::InvalidateRect( m_hWnd, &g_rtTarget, false);
			}

			m_ptOldPos = ptRel;

			return;
		}

		m_ptOldPos = ptRel;
	}
	else if( m_MouseMode == MOUSEMODE_SCROLL)
	{
		CPoint subPos;
		
		subPos = m_ScrollStart - point;

		if( subPos.x > 0)
			UpdateScroll( false, SB_LINEDOWN, subPos.x, NULL);
		else if( subPos.x < 0)
			UpdateScroll( false, SB_LINEUP, subPos.x, NULL);

		if( subPos.y > 0)
			UpdateScroll( true, SB_LINEDOWN, subPos.y, NULL);
		else if( subPos.y < 0)
			UpdateScroll( true, SB_LINEUP, subPos.y, NULL);

		m_ScrollStart = point;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CUITemplate_Editor::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
}

BOOL CUITemplate_Editor::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}


void CUITemplate_Editor::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	m_bNotify = false;

	if( i3::kind_of<i3UIShape *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_SELECT :
				if( pInfo->m_pObject != NULL)
				{
					Select( (i3UIShape *) pInfo->m_pObject);
				}
				else
				{
					SelectAll();
				}
				break;

			case I3_TDK_UPDATE_UNSELECT :
				if( pInfo->m_pObject != NULL)
				{
					UnSelect( (i3UIShape *) pInfo->m_pObject);
				}
				else
				{
					UnSelectAll();
				}
				break;

			case I3_TDK_UPDATE_EDIT :
				if( pInfo->m_pObject != NULL)
				{
					i3UIShape * pShape = (i3UIShape*)pInfo->m_pObject;	// * local variable is initialized but not referenced

					INT32 i;
					for( i = 0; i < m_pCurrentCtrl->getShapeCount(); i++)
					{
						if( m_pCurrentCtrl->getShape( i) == pShape)
						{
							//현재 ProgressBar만 적용
							if ( i3::same_of<i3UITemplate_ProgressBarImage*>(m_pCurrentCtrl->getTemplate()) //->IsExactTypeOf(i3UITemplate_ProgressBarImage::static_meta()) 
								|| i3::same_of<i3UITemplate_ProgressBarSplit*>(m_pCurrentCtrl->getTemplate()) //->IsExactTypeOf(i3UITemplate_ProgressBarSplit::static_meta()) 
								|| i3::same_of<i3UITemplate_ProgressBar*>(m_pCurrentCtrl->getTemplate())) //->IsExactTypeOf(i3UITemplate_ProgressBar::static_meta()))
							{
								//배경 높이 변경할시만 적용..
								if (i < 3)
								{
									VEC2D* vec = m_pCurrentCtrl->getSize();
									m_pCurrentCtrl->setSize(vec->x, pShape->getHeight());
									m_Rect.bottom = pShape->getHeight();
								}
							}

							/*
							if ( i3::same_of<i3UITemplate_ButtonImageSet*>(m_pCurrentCtrl->getTemplate()))
							{
								REAL32 w = pShape->getRect()->right - pShape->getRect()->left;
								REAL32 h = pShape->getRect()->bottom - pShape->getRect()->top;

								m_pCurrentItem->AdjustLayout( w, h);
								m_pCurrentItem->GetControlRect(&m_Rect);

								m_pCurrentCtrl->setSize( w, h);
								//I3TRACE("CUITemplate_Editor::OnChangeSize : %f, %f\n",w, h);

								m_pGripManager.RecalcLayout();
							}
							else
							{
							*/
								if( m_bSpreaded == false)
								{
									// Spread가 되어 있을 때엔 펼쳐진 크기가 오기 때문에 올바르지 않다.
									m_pCurrentItem->GetControlRect( &m_Rect);	// 정상적인 이 후 동작을 위해 m_Rect 멤버를 갱신해 두어야 한다.
								}
							//}

							break;
						}
					}
				}

				m_pCurrentCtrl->UpdateRenderObjects();
				m_pGripManager.RecalcLayout();

				Invalidate();
				break;

			case I3_TDK_UPDATE_ADD :
			case I3_TDK_UPDATE_REMOVE :
				// Shape가 새롭게 추가되거나, 생성된 경우, Control에 새럽게 Template 설정을 해주어야 함.
				if( m_pCurrentCtrl != NULL)
				{
					UnSelectAll();
					m_pCurrentCtrl->SetTemplate( m_pCurrentItem);

					Invalidate();
				}
				break;
		}
	}
	else if( i3::kind_of<i3UIControl *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_EDIT :
				if( pInfo->m_pObject == m_pCurrentCtrl)
				{
					// 현재 Control에 대한 수정이 있었다. 크기 변경이 있을 수 있으므로 Grip을 새로 잡는다.
					VEC2D * pSize = m_pCurrentCtrl->getSize();

					m_pCurrentItem->AdjustLayout( pSize->x, pSize->y);
					m_pCurrentItem->GetControlRect(&m_Rect);

					m_pGripManager.RecalcLayout();

					::InvalidateRect( m_hWnd, NULL, false);
				}
				break;
		}
	}

	m_bNotify = true;
}

LRESULT CUITemplate_Editor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		if( IsWindowVisible())
			OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void CUITemplate_Editor::OnTimer(UINT_PTR nIDEvent)
{
	{
		Invalidate( FALSE);
	}

	CWnd::OnTimer(nIDEvent);
}

void CUITemplate_Editor::SetupScroll(void)
{
	SCROLLINFO info;
	CRect rt;

	GetClientRect( &rt);

	// Horizonal Scroll
	info.cbSize = sizeof(info);
	info.fMask	= SIF_PAGE | SIF_RANGE;
	info.nMin	= 0;
	info.nMax	= (INT32)((g_rtTarget.right - g_rtTarget.left) * g_fZoom);	
	info.nPage	= rt.Width();
	
	::SetScrollInfo( m_hWnd, SB_HORZ, &info, TRUE);

	// Vertical Scroll
	info.nMax	= (INT32)((g_rtTarget.bottom - g_rtTarget.top) * g_fZoom);
	info.nPage	= rt.Height();

	::SetScrollInfo( m_hWnd, SB_VERT, &info, TRUE);
}

void CUITemplate_Editor::UpdateScroll( bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO info;
	info.cbSize	= sizeof(info);
	info.fMask	= SIF_POS | SIF_TRACKPOS | SIF_PAGE | SIF_RANGE;
	INT32 nBar = bVert ? SB_VERT : SB_HORZ;		// bHorz or bVert
	::GetScrollInfo( m_hWnd, nBar, &info);

	INT32 nCurPos = info.nPos;

	switch( nSBCode)
	{
		case SB_TOP :			nCurPos = 0;			break;
		case SB_BOTTOM :		nCurPos = info.nMax;	break;
		case SB_LINEDOWN :		nCurPos ++;				break;
		case SB_LINEUP :		nCurPos --;				break;
		case SB_PAGEDOWN :		nCurPos += info.nPage;	break;
		case SB_PAGEUP :		nCurPos -= info.nPage;	break;
		case SB_THUMBPOSITION :
		case SB_THUMBTRACK :	nCurPos = nPos;			break;
	}

	info.nPos = nCurPos;
	info.nTrackPos = nCurPos;
	::SetScrollInfo( m_hWnd, nBar, &info, TRUE);

	// min, max 가 반영된 값을 다시 얻어옴
	AdjustShowXY();
}

void CUITemplate_Editor::AdjustShowXY( void)
{
	SCROLLINFO info;
	info.cbSize = sizeof( SCROLLINFO);
	info.fMask = SIF_POS;
	::GetScrollInfo( m_hWnd, SB_VERT, &info);
	m_ScrollY = info.nPos;
	m_ShowY = (INT32)(info.nPos / g_fZoom);

	::GetScrollInfo( m_hWnd, SB_HORZ, &info);
	m_ScrollX = info.nPos;
	m_ShowX = (INT32)(info.nPos / g_fZoom);
}

void CUITemplate_Editor::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateScroll( true, nSBCode, nPos, pScrollBar);

	//막지 않으면 OnMouseWheel이 여러번 호출됩니다..
	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CUITemplate_Editor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateScroll( false, nSBCode, nPos, pScrollBar);

	//막지 않으면 OnMouseWheel이 여러번 호출됩니다..
	//CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUITemplate_Editor::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_WndSz.cx = cx;
	m_WndSz.cy = cy;

	if( m_pDummyFrameWnd != NULL)
		m_pDummyFrameWnd->setSize((REAL32)cx, (REAL32)cy);

	SetupScroll();
}

void CUITemplate_Editor::DumpVirtualTexture()
{
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex != NULL)
	{
		pVTex->DumpVirtualTexture();
	}
}

void CUITemplate_Editor::SendInputToFramework(bool bEnable)
{
	if( m_pCurrentItem == NULL)
	{
		m_bSendToFramework = false;
		return;
	}

	if( bEnable == m_bSendToFramework)
		return;

	if( bEnable)
	{
		if( i3::kind_of<i3UIFrameWnd*>(m_pCurrentCtrl)) //->IsTypeOf( i3UIFrameWnd::static_meta()) )
		{
			//pManager->AddFrameWnd((i3UIFrameWnd*)m_pCurrentCtrl); 요거이 무한루프 빠져버림...
		}
		else
		{
			m_pDummyFrameWnd->AddControl( m_pCurrentCtrl);
			m_pDummyFrameWnd->OnCompletedLoading();

			m_pTestScene->Attach(false);
		}

		m_bSendToFramework = true;

		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->setEnableOtherScene( true);
	}
	else
	{
		m_bSendToFramework = false;

		if( i3::kind_of<i3UIFrameWnd*>(m_pCurrentCtrl)) //->IsTypeOf( i3UIFrameWnd::static_meta()) )
		{
			//pManager->RemoveFrameWnd((i3UIFrameWnd*)m_pCurrentCtrl); 요거이 무한루프 빠져버림...
		}
		else
		{
			m_pTestScene->Detach();

			m_pDummyFrameWnd->RemoveControl( m_pCurrentCtrl);

			m_pCurrentCtrl->EnableCtrl( true);	//RemoveControl에서 false로 만든것을 다시 true로 바꿔준다. 
		}
		
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->setEnableOtherScene( false);
	}
}

BOOL CUITemplate_Editor::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( m_bSendToFramework == true)
	{
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

		CWnd * pWnd = pFrame->getDummyWindow();

		if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
			(pMsg->message < WM_MOUSEFIRST || pMsg->message > AFX_WM_MOUSELAST))
		{

		}
		else
		{
			if( ::IsWindow( pWnd->m_hWnd))
			{
				if( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= AFX_WM_MOUSELAST)
				{
					POINT pos;

					pos.x = (INT32)(GET_X_LPARAM( pMsg->lParam) / g_fZoom) + m_ShowX; 
					pos.y = (INT32)(GET_Y_LPARAM( pMsg->lParam) / g_fZoom) + m_ShowY; 
					
					::SendMessage( pWnd->m_hWnd, pMsg->message, pMsg->wParam, MAKELPARAM(pos.x, pos.y));
				}
				else
				{
					::SendMessage( pWnd->m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
				}
				
				return TRUE;
			}
		}
	}


	return CWnd::PreTranslateMessage(pMsg);
}

void CUITemplate_Editor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( GetKeyState( VK_CONTROL) & 0x8000)
	{
		switch( nChar)
		{
		case 'A':	
			SelectAll();		
			break;
		case 'S':	
			#ifdef _DEBUG
			{
				CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
				Ci3UIEditorDoc* pDoc = ( Ci3UIEditorDoc*)pMain->GetActiveDocument();

				bool isNewFormat = false;

				i3UILibrary* pUILib = i3UI::getUILibrary();

				if (pUILib)
					isNewFormat = pUILib->IsNewTemplateLibraryFormat();
				
				pDoc->SaveTemplateLibrary(isNewFormat);
			}
			#endif
			break;
		default:
			break;
		}
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		m_nLRCnt--;
		MoveShapeCell();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_nLRCnt++;
		MoveShapeCell();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		m_nUDCnt--;
		MoveShapeCell();
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		m_nUDCnt++;
		MoveShapeCell();
	}
	else
	{
		switch( nChar)
		{
		case VK_ESCAPE:	
			UnSelectAll();			
			break;	
		default:
			break;
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

INT32 CUITemplate_Editor::getSelectedObjCount( void)
{
	return m_pGripManager.getSelectedItemCount();
}

void CUITemplate_Editor::getSelectedObjList( i3::vector<i3UIControl*>& Out)
{
	m_pGripManager.getSelectedObjList( Out);
}


void CUITemplate_Editor::OnRButtonDown(UINT nFlags, CPoint point)
{
	INT32 selCount = m_pGripManager.getSelectedItemCount();

	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;

	GetCursorPos( &pt);

	{
		Menu.LoadMenu( IDR_MENU_TEMPLATE);
	}

	pPopup = Menu.GetSubMenu( 1);

	if((m_bSpreaded == false) || ( selCount <= 0))
	{
		// Spread가 안된 상태에서는 Shape 선택이 불가능하기 때문에
		// Select Texture 및 Select UV를 할 수 없도록...
		pPopup->EnableMenuItem( ID_VIEW_SELECTTEXTURE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_VIEW_SELECTUV, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}

	if( m_bSpreaded)
		pPopup->EnableMenuItem( ID_VIEW_SPREADLAYOUT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	else
		pPopup->EnableMenuItem( ID_VIEW_ADJUSTLAYOUT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_VIEW_SELECTTEXTURE:			SelectTexture();	break;
		case ID_VIEW_SELECTUV:				SelectUV();			break;
		case ID_VIEW_SPREADLAYOUT:			SpreadLayout();		break;
		case ID_VIEW_ADJUSTLAYOUT:			AdjustLayout();		break;
		case ID_VIEW_SELECTALLSHAPES:		SelectAll();		break;

		case ID_VIEW_ALIGNLEFTTOP:			AlignLeftTop();		break;
		case ID_VIEW_SPREAD:				Spread();			break;
	}

	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CUITemplate_Editor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if( (GetKeyState( VK_CONTROL) & 0x8000) != 0)
	{
		//Ctrl을 누른 상태에선 확대/축소
		REAL32 mag = zDelta > 0 ? 0.25f : -0.25f;
			
		g_fZoom = MINMAX( 0.5f, g_fZoom + mag, 2.0f);

		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
		pMainFrm->UpdateStatusBar( false);

		SetupScroll();

		AdjustShowXY();
		AdjustShowXY();

		::InvalidateRect( m_hWnd, NULL, true);
	}
	else 
	{
		INT32 nScrollAmount = zDelta > 0 ? -5 : 5;
		nScrollAmount = (INT32)(nScrollAmount * g_fZoom);

		if( (GetKeyState( VK_SHIFT) & 0x8000) != 0)
		{
			//Shift를 누른 상태에선 수평스크롤
			OnHScroll( SB_THUMBTRACK, m_ScrollX + nScrollAmount, NULL);
		}
		else	
		{
			//수직스크롤
			OnVScroll( SB_THUMBTRACK, m_ScrollY + nScrollAmount, NULL);
		}
	}

	//막지 않으면 OnMouseWheel이 여러번 호출됩니다..
	//return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

REAL32 CUITemplate_Editor::getZoomRatio( void)
{
	return g_fZoom;
}

//마우스 커서의 좌표를 "Window에 대한 상대좌표"에서 "컨트롤에 대한 상대좌표"로 변환
CPoint CUITemplate_Editor::_AbsToRel( CPoint ptAbs, bool bOffset)
{
	CPoint ptRel;
	
	ptRel.x = m_ShowX + (INT32)(ptAbs.x / g_fZoom);
	ptRel.y = m_ShowY + (INT32)(ptAbs.y / g_fZoom);

	if( bOffset)
	{
		ptRel.x -= m_OffsetX;
		ptRel.y -= m_OffsetY;
	}

	return ptRel;
}

void CUITemplate_Editor::OnMButtonDown( UINT nFlags, CPoint point)
{
	SetFocus();

	SetCapture();

	m_MouseMode = MOUSEMODE_SCROLL;

	m_ScrollStart = point;

	HCURSOR hCursor = ::LoadCursor( NULL,IDC_HAND);

	::SetCursor( hCursor);
	::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

	CWnd::OnMButtonDown( nFlags, point);
}

void CUITemplate_Editor::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_MouseMode = MOUSEMODE_NONE;

	ReleaseCapture();

	HCURSOR hCursor = ::LoadCursor( NULL,IDC_ARROW);

	::SetCursor( hCursor);
	::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);

	CWnd::OnMButtonUp( nFlags, point);
}

void CUITemplate_Editor::MoveShapeCell()
{
	int nCnt = m_pCurrentItem->getShapeCount();
	if (nCnt == 0)
		return;

	i3UIShape* pShapeFirst = m_pCurrentItem->getShape(0);
	i3UIShape* pShapeSecond = NULL;
	if (nCnt > 1)
		pShapeSecond = m_pCurrentItem->getShape(1);
		
	//디폴트 10 간혹간격이 틀릴때도 있던데..
	int nDelta = 10;
	if (pShapeFirst && pShapeSecond)
	{
		//간격
		nDelta = static_cast<INT32>(pShapeSecond->getPosX()) - static_cast<INT32>(pShapeFirst->getPosX()) - static_cast<INT32>(pShapeFirst->getWidth());
		if (nDelta < 0)
			nDelta = 10;
	}
	
	REAL32 nMoveCellX = pShapeFirst->getWidth() + nDelta;
	REAL32 nMoveCellY = pShapeFirst->getHeight() + nDelta;

	REAL32 nCurrX = m_nLRCnt * nMoveCellX;
	REAL32 nCurrY = m_nUDCnt * nMoveCellY;

	m_pCurrentCtrl->setPos(nCurrX+m_OffsetX, nCurrY+m_OffsetY);

	CPoint gripDelta;
	gripDelta.SetPoint((int)nCurrX, (int)nCurrY);
	m_pGripManager.SetDelta(gripDelta);
}


void CUITemplate_Editor::ResetMoveCellValue()
{
	//셀이동 관련 초기화..
	m_nLRCnt = 0;
	m_nUDCnt = 0;
	MoveShapeCell();

	m_OffsetX = MAX( 0, (g_pViewer->GetViewWidth() >> 1) - (INT32)(m_pCurrentCtrl->getWidth() * 0.5f));
	m_OffsetY = MAX( 0, (g_pViewer->GetViewHeight() >> 1) - (INT32)(m_pCurrentCtrl->getHeight() * 0.5f));

	m_pCurrentCtrl->setPos(static_cast<REAL32>(m_OffsetX), static_cast<REAL32>(m_OffsetY), 0.0f);
}