#include "stdafx.h"
#include "i3treeProperty.h"
#include "i3treePropertyBase.h"
#include "propertyeditbox.h"
//#include "i3treeDlg.h"
#include "resource.h"

#define LMARGIN		0
#define RMARGIN		0
#define TMARGIN		0
#define BMARGIN		0

PropertyBoard::PropertyBoard()
{
	m_WhitePen = CreatePen( PS_SOLID, 1, RGB( 255, 255, 255));
	m_GrayPen  = CreatePen( PS_SOLID, 1, RGB( 128, 128, 128));
	m_BlackPen = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0));

	//For Use Font
	LOGFONT logFont;
	BOOL	Fontrv;

	memset( &logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight			= 12;
	logFont.lfWeight			= FW_DONTCARE;
	logFont.lfItalic			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy( logFont.lfFaceName, "굴림");

	//For Creaet Font 
	m_pFont = new CFont();
	ASSERT(m_pFont != NULL);
	Fontrv = m_pFont->CreateFontIndirect(&logFont);
	ASSERT( Fontrv != NULL);
}

PropertyBoard::~PropertyBoard()
{	
	DeleteObject( m_WhitePen);
	DeleteObject( m_GrayPen);
	DeleteObject( m_BlackPen);

	if(m_pFont != NULL)
	{
		delete m_pFont; 
		m_pFont = NULL; 
	}

	DeleteAllPropertys(); 
}

void PropertyBoard::CreateControl(CWnd * Dlg)
{
	BOOL Rv; 
	RECT rect; 

	//For Edit Box
	rect.bottom = rect.left = rect.right = rect.top = 0;
	Rv = m_EditBox.Create( ES_AUTOHSCROLL | ES_NOHIDESEL | WS_CHILD, rect, Dlg, IDC_EDIT);	
	ASSERT( Rv != FALSE);

	//For List Box 	
	Rv = m_ListBox.Create( LBS_NOTIFY | WS_BORDER | WS_OVERLAPPED, rect, Dlg, IDC_LISTBOX);
	ASSERT( Rv != FALSE);

	//For Dialog Box 

	//Set Control Font
	m_EditBox.SetFont( m_pFont, TRUE);
	m_ListBox.SetFont( m_pFont, TRUE); 

	//List Box Show Buttom 
	Rv = m_DropDownButton.Create( "", BS_OWNERDRAW | WS_CHILD, rect, Dlg, IDC_DORPDOWNBUTTON);
	ASSERT( Rv != FALSE);

	Rv = m_DropDownButton.LoadBitmap( IDB_LISTBOXB, 8, 8);
	ASSERT( Rv != FALSE);

	//Dialog Box Show Buttom 
	Rv = m_DialogButton.Create( "", BS_OWNERDRAW | WS_CHILD, rect, Dlg, IDC_DIALOGBUTTON);
	ASSERT( Rv != FALSE);

	Rv = m_DialogButton.LoadBitmap( IDB_USERDIALOG, 8, 8);
	ASSERT( Rv != FALSE);


}

void PropertyBoard::SetBoard(i3ElementBase * pObject)
{
	i3ElementBase * m_pAA = m_pSelObj; 
	m_pSelObj = pObject; 
	
    //For Control
	HideControls(); 

	//Delete All Property
	DeleteAllPropertys(); 

	//For Select Line 
	m_SelectLine = -1;

	if(pObject == NULL)return; 

	//Set Propertys
	SetListPropertys(); 

}

void PropertyBoard::Draw(CPaintDC *pDc, INT32 StartX,INT32 cx, INT32 cy, INT32 center)
{
	//For Init Value 
	m_pDc		= pDc; 	
	m_cx		= cx; 
	m_cy		= cy; 
	m_Startx	= StartX; 
	m_center	= center; 

	/////////////////////////////////////////////////////////////////////
	//Start Draw
	HFONT	OldFont;
	HPEN	OldPen; 
	OldFont = (HFONT) pDc->SelectObject(m_pFont->m_hObject);
	OldPen	= (HPEN)  pDc->SelectObject(m_GrayPen);
	pDc->SetBkColor(GetSysColor(COLOR_BTNFACE)); 

	//Draw Boarder
	DrawBoarder();

	//Draw Property 
	if (m_pSelObj != NULL)
		DrawProperty(); 

	//Center
	m_pDc->SelectObject(m_BlackPen);	
	m_pDc->MoveTo(m_center, TMARGIN);
	m_pDc->LineTo(m_center, m_cy - BMARGIN); 
	m_pDc->SelectObject(m_WhitePen);
	m_pDc->MoveTo(m_center+1, TMARGIN+1);
	m_pDc->LineTo(m_center+1, m_cy - BMARGIN-1); 

	////////////////////////////////////////////////////////////////////
	//End Draw
	pDc->SelectObject(OldFont);
	pDc->SelectObject(OldPen);
	return; 
}


void PropertyBoard::DrawProperty(void)
{
	INT32 i; 
	INT32 count; 
	INT32 Style; 
	RECT  BoxRect, LineRect; 
	char  s_temp[256]; 
	CpropertyBase * pPropertyBase; 
	
	LineRect.left	= m_Startx + LMARGIN + 2; 
	LineRect.right	= m_cx - RMARGIN - 1; 
	LineRect.top	= TMARGIN + 1; 
	LineRect.bottom = TMARGIN + 1 + PROPERTY_LINE_HSIZE; 

	count = m_PropertyList.GetCount(); 

	if(count ==0)
		return; 

	for(i = 0; i < count ; i++)
	{
		//Get Property 
		pPropertyBase = (CpropertyBase *)m_PropertyList.GetItem(i); 	

		//Get Type 
		Style = pPropertyBase->GetStyle(); 

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
		//For Subject
		BoxRect = LineRect; 
		BoxRect.top = BoxRect.top + 3; 
		BoxRect.left = BoxRect.left + 2; 

		m_pDc->SetTextColor(RGB( 0, 0, 0)); 
		m_pDc->DrawText((LPCSTR)pPropertyBase->GetName(), (int)strlen((const char *)pPropertyBase->GetName()), &BoxRect, DT_NOPREFIX|DT_SINGLELINE); 

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//For Value
		if((Style & I3_READONLY) == I3_READONLY)
		{
			///////////////////////////////////////////////////
			//Read only
			BoxRect = LineRect; 
			BoxRect.top = BoxRect.top + 3; 
			BoxRect.left = m_center + 4; 

			//Get Value
			pPropertyBase->Get(s_temp); 

			m_pDc->SetTextColor(RGB( 100, 100, 100)); 
			m_pDc->DrawText((LPCSTR)s_temp, (int)strlen(s_temp), &BoxRect, DT_NOPREFIX|DT_SINGLELINE); 

			//For Line 
			BoxRect = LineRect; 
			m_pDc->SetTextColor(RGB( 0, 0, 0));
			m_pDc->DrawFocusRect(&BoxRect); 
		}else
		{			
			if(m_SelectLine == i)
			{				
				///////////////////////////////////////////////////
				//Selected

				//For Edit Box
				if((Style & I3_EDIT) == I3_EDIT)
				{
					BoxRect = LineRect; 
					BoxRect.top		= BoxRect.top + 1; 
					BoxRect.left	= m_center +1 ;// - 2; 
					BoxRect.bottom	= BoxRect.bottom - 1; 
				
					strcpy(s_temp,"");
					pPropertyBase->Get(s_temp);
					m_EditBox.SetProperty((INT8 *)s_temp);
					m_EditBox.MoveWindow( &BoxRect, FALSE);
					m_EditBox.ShowWindow( SW_SHOW);
					m_EditBox.SetFocus();
				}else
				{
					if((Style & I3_USER_DRAW) == I3_USER_DRAW)
					{
						BoxRect = LineRect; 
						BoxRect.left = m_center + 4; 
						pPropertyBase->UserDraw(m_pDc, &BoxRect); 
					}else
					{
						//For Value
						BoxRect = LineRect; 
						BoxRect.top = BoxRect.top + 3; 
						BoxRect.left = m_center + 4; 

						//Get Value
						pPropertyBase->Get(s_temp); 

						//Draw Value
						m_pDc->SetTextColor(RGB( 0, 0, 196)); 																		
						m_pDc->DrawText((LPCSTR)s_temp, (int)strlen(s_temp), &BoxRect, DT_NOPREFIX|DT_SINGLELINE); 
					}
				}

				//For Select Box 
				if((Style & I3_LIST) == I3_LIST)
				{
					//IDC_DORPDOWNBUTTON
					BoxRect = LineRect; 
					BoxRect.left	= BoxRect.right  - 16; 
					BoxRect.top		= BoxRect.top	 + 2;
					BoxRect.bottom	= BoxRect.bottom - 2;

					m_DropDownButton.MoveWindow(&BoxRect, FALSE);
					m_DropDownButton.ShowWindow(SW_SHOW);
				}

				//For Dialog Box Button
				if((Style & I3_DIALOG) == I3_DIALOG)
				{
					//IDC_DIALOGBUTTON
					BoxRect = LineRect; 
					BoxRect.left	= BoxRect.right  - 16; 
					BoxRect.top		= BoxRect.top	 + 2;
					BoxRect.bottom	= BoxRect.bottom - 2;

					m_DialogButton.MoveWindow(&BoxRect, FALSE);
					m_DialogButton.ShowWindow(SW_SHOW);
				}

				if((Style & I3_BIG_DIALOG) == I3_BIG_DIALOG)
				{
					//IDC_DIALOGBUTTON
					BoxRect = LineRect; 
					
					BoxRect.top		= BoxRect.top	 + 2;
					BoxRect.bottom	= BoxRect.bottom - 2;
					BoxRect.left	= m_center	 + 2; 
					BoxRect.right	= BoxRect.right	 - 2; 

					m_DialogButton.MoveWindow(&BoxRect, FALSE);
					m_DialogButton.ShowWindow(SW_SHOW);
				}

				//Draw Select Line 
				//Top
				BoxRect = LineRect; 
				m_pDc->SelectObject(m_GrayPen); 
				m_pDc->MoveTo(BoxRect.left,  BoxRect.top ); 
				m_pDc->LineTo(BoxRect.right, BoxRect.top ); 

				m_pDc->SelectObject(m_BlackPen); 
				m_pDc->MoveTo(BoxRect.left,  BoxRect.top + 1); 
				m_pDc->LineTo(BoxRect.right, BoxRect.top + 1); 

				//Bottom 
				m_pDc->SelectObject(m_WhitePen); 
				m_pDc->MoveTo(BoxRect.left,  BoxRect.bottom - 1); 
				m_pDc->LineTo(BoxRect.right, BoxRect.bottom - 1); 

			}else
			{
				///////////////////////////////////////////////////
				//Not Selected
				//For Value
				if((Style & I3_USER_DRAW) == I3_USER_DRAW)
				{
						BoxRect = LineRect; 
						BoxRect.left = m_center + 4; 
						pPropertyBase->UserDraw(m_pDc, &BoxRect); 
				}else
				{
					BoxRect = LineRect; 
					BoxRect.top = BoxRect.top + 3; 
					BoxRect.left = m_center + 4; 

					//Get Value
					pPropertyBase->Get(s_temp); 

					//Set Draw Color
					if((Style & I3_BIG_DIALOG) == I3_BIG_DIALOG)	m_pDc->SetTextColor(RGB( 196, 0, 0));
					else											m_pDc->SetTextColor(RGB( 0, 0, 196));
                    
					//Draw Value
					m_pDc->DrawText((LPCSTR)s_temp, (int)strlen(s_temp), &BoxRect, DT_NOPREFIX|DT_SINGLELINE); 
				}
				//For Line 
				m_pDc->SetTextColor(RGB( 0, 0, 0));
				BoxRect = LineRect; 
				m_pDc->DrawFocusRect(&BoxRect); 
				
			}

		}


		//For Next Box
		LineRect.top	= LineRect.bottom; 
		LineRect.bottom = LineRect.top + PROPERTY_LINE_HSIZE; 
	}
}

////////////////////////////////////////////////////////////////////
//Draw Splite Line			
void PropertyBoard::DrawBoarder(void)
{
	//Left	
	m_pDc->SelectObject(m_BlackPen);	
	m_pDc->MoveTo(m_Startx + LMARGIN, TMARGIN);
	m_pDc->LineTo(m_Startx + LMARGIN, m_cy - BMARGIN); 
	m_pDc->SelectObject(m_WhitePen);
	m_pDc->MoveTo(m_Startx + LMARGIN +1, TMARGIN);
	m_pDc->LineTo(m_Startx + LMARGIN +1, m_cy - BMARGIN); 

	//Right 
	m_pDc->SelectObject(m_BlackPen);	
	m_pDc->MoveTo(m_cx - RMARGIN, TMARGIN);
	m_pDc->LineTo(m_cx - RMARGIN, m_cy - BMARGIN); 
	m_pDc->SelectObject(m_WhitePen);
	m_pDc->MoveTo(m_cx - RMARGIN +1, TMARGIN);
	m_pDc->LineTo(m_cx - RMARGIN +1, m_cy - BMARGIN); 

	//Up
	m_pDc->SelectObject(m_BlackPen);	
	m_pDc->MoveTo(m_Startx + LMARGIN, TMARGIN);
	m_pDc->LineTo(m_cx - RMARGIN, TMARGIN); 

	//Down 
	m_pDc->SelectObject(m_BlackPen);	
	m_pDc->MoveTo(m_Startx + LMARGIN, m_cy - BMARGIN);
	m_pDc->LineTo(m_cx - RMARGIN, m_cy - BMARGIN); 

	return; 
}

void PropertyBoard::DeleteAllPropertys(void)
{
	INT32 i;
	INT32 Counter; 

	//Get Counter
	Counter = m_PropertyList.GetCount(); 
	
	if (Counter == 0) return; 
	
	//Delete All Property 
	CpropertyBase * pPropertyBase; 
	for(i = 0; i < Counter; i++)
	{
		pPropertyBase = (CpropertyBase *)m_PropertyList.GetItem(i); 	
		delete pPropertyBase; 
	}

	//Delete All Property List
	m_PropertyList.Clear(); 
	return; 
}

void PropertyBoard::MouseDown(CPoint point)
{
	INT32 count = m_PropertyList.GetCount(); 
	INT8 BSelectLine; 
	RECT rect; 

	//Nothing to do 	
	if( (count == NULL) || ((count * PROPERTY_LINE_HSIZE) + TMARGIN < point.y))
	{
		if (m_SelectLine >= 0)
		{
			HideControls();
			rect.top = TMARGIN + (PROPERTY_LINE_HSIZE * m_SelectLine); 
			rect.bottom = rect.top + PROPERTY_LINE_HSIZE + 2; 
			rect.top = rect.top - 2; 
			rect.left = m_Startx; 
			rect.right = m_cx - RMARGIN; 

			RedrawWindow(m_Wnd, &rect, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
		}
		m_SelectLine = -1; 
		return; 	
	}


	//Get Property 
	BSelectLine = m_SelectLine; 
	m_SelectLine = (INT8) ((point.y - TMARGIN - 1) / PROPERTY_LINE_HSIZE); 

	//ReDraw Select Line 
	if (BSelectLine != m_SelectLine)
	{
		// 전 라인과 
		HideControls();
		if (BSelectLine >= 0)
		{
			//m_center, m_cy - BMARGIN
			//BoxRect.top	= TMARGIN + 1; 
			rect.top = TMARGIN + (PROPERTY_LINE_HSIZE * BSelectLine); 
			rect.bottom = rect.top + PROPERTY_LINE_HSIZE + 2; 
			rect.top = rect.top - 2; 
			rect.left = m_Startx; 
			rect.right = m_cx - RMARGIN; 
			RedrawWindow(m_Wnd, &rect, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);			
		}
		if (m_SelectLine >= 0)
		{
			rect.top = TMARGIN + (PROPERTY_LINE_HSIZE * m_SelectLine); 
			rect.bottom = rect.top + PROPERTY_LINE_HSIZE + 2; 
			rect.top = rect.top - 2; 
			rect.left = m_Startx; 
			rect.right = m_cx - RMARGIN; 

			RedrawWindow(m_Wnd, &rect, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
		}
	}
	return; 
}


//필요한가? 쩝.
CpropertyBase * PropertyBoard::GetProperty()
{	
	CpropertyBase * pPropertyBase = NULL; 
	pPropertyBase = (CpropertyBase *) m_PropertyList.GetItem(m_SelectLine); 
	if(pPropertyBase == NULL)ASSERT(0); 
	return pPropertyBase;
}

void PropertyBoard::HideControls(void)
{
	if( ::IsWindow( m_ListBox.m_hWnd) && (m_ListBox.GetStyle() & WS_VISIBLE))
		m_ListBox.ShowWindow( SW_HIDE);

	if( ::IsWindow( m_EditBox.m_hWnd) && (m_EditBox.GetStyle() & WS_VISIBLE))
		m_EditBox.ShowWindow( SW_HIDE);

	if( ::IsWindow( m_DropDownButton.m_hWnd) && (m_DropDownButton.GetStyle() & WS_VISIBLE))
		m_DropDownButton.ShowWindow( SW_HIDE);

	if( ::IsWindow( m_DialogButton.m_hWnd) && (m_DialogButton.GetStyle() & WS_VISIBLE))
		m_DialogButton.ShowWindow( SW_HIDE);
}

void PropertyBoard::ChangeProperty(void)
{
	BOOL rv; 

	if( m_pSelObj->IsTypeOf( i3Node::GetClassMeta()))
	{
		//////////////////////////////////////////////////////////////
		//For Node 
		rv = ChangeNode(); 		
		if (rv == TRUE)
		{
//			g_pDlg->ChangeTree((i3Node *)m_pSelObj); 
			return; 
		}

		if( m_pSelObj->IsTypeOf( i3Geometry::GetClassMeta()))
			ChangeGeometryList();
		else if( m_pSelObj->IsTypeOf( i3AttrSet::GetClassMeta()))
			ChangeAttrSetList();
		else if( m_pSelObj->IsTypeOf( i3Camera::GetClassMeta()))
			ChangeCameraList();
		else if( m_pSelObj->IsTypeOf( i3SceneObject::GetClassMeta()))
			ChangeSceneObjectList();
		else if( m_pSelObj->IsTypeOf( i3Transform::GetClassMeta()))
			ChangeTransformList();
		else if( m_pSelObj->IsTypeOf( i3Particle::GetClassMeta()))
			ChangeParticleList();
		else if( m_pSelObj->IsTypeOf( i3ColliderSet::GetClassMeta()))
			ChangeColliderSetList();
		else if( m_pSelObj->IsTypeOf( i3LightSet::GetClassMeta()))
			ChangeLightSetList();
		else if( m_pSelObj->IsTypeOf( i3Skin::GetClassMeta()))
			ChangeSkinList();
		else if( m_pSelObj->IsTypeOf( i3Body::GetClassMeta()))
		{
			ChangeBodyList();
		}
		else if( m_pSelObj->IsTypeOf( i3BoneRef::GetClassMeta()))
		{
			ChangeBoneRefList();
		}
		else if( m_pSelObj->IsTypeOf( i3TimeSequence::GetClassMeta()))
		{

		}
		else 
		{
			ChangeNodeList();
		}
	}
	else if( m_pSelObj->IsTypeOf( i3Collidee::GetClassMeta()))
	{
		if( m_pSelObj->IsExactTypeOf( i3CollideeSphere::GetClassMeta()))
			ChangeCollideeSphere();
		else if( m_pSelObj->IsExactTypeOf( i3CollideeMesh::GetClassMeta()))
			ChangeCollideeMesh();
		else
			ChangeCollider();
	}
	else if( m_pSelObj->IsTypeOf( i3RenderAttr::GetClassMeta()))
	{
		//////////////////////////////////////////////////////////////
		//For Attr
		rv = ChangeAttr(); 
		if (rv == TRUE)		return; 

		if( m_pSelObj->IsExactTypeOf( i3MaterialAttr::GetClassMeta()))
			ChangeMaterialAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3ColorAttr::GetClassMeta()))
			ChangeColorAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3AlphaFuncAttr::GetClassMeta()))
			ChangeAlphaFuncAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3BlendModeAttr::GetClassMeta()))
			ChangeBlendModeAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3FaceCullModeAttr::GetClassMeta()))
			ChangeFaceCullModeAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3FogColorAttr::GetClassMeta()))
			ChangeFogColorAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3FogDensityAttr::GetClassMeta()))
			ChangeFogDensityAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3FogModeAttr::GetClassMeta()))
			ChangeFogModeAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3FogNearFarAttr::GetClassMeta()))
			ChangeFogNearFarAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3GeometryAttr::GetClassMeta()))
			ChangeGeometryAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3LightAttr::GetClassMeta()))
			ChangeLightAttrList();
		else if( m_pSelObj->IsTypeOf( i3TextureFilterAttr::GetClassMeta()))
			ChangeTextureFilterAttrList();
		else if( m_pSelObj->IsTypeOf( i3TextureWrapAttr::GetClassMeta()))
			ChangeTextureWrapAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3ZFuncAttr::GetClassMeta()))
			ChangeZfuncAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3ShadeModeAttr::GetClassMeta()))
			ChangeShadeModeList();
		else if( m_pSelObj->IsExactTypeOf( i3RenderModeAttr::GetClassMeta()))
			ChangeRenderModeList();
		else if( m_pSelObj->IsTypeOf( i3TextureBindAttr::GetClassMeta()))
			ChangeTextureBindList();
		else if( m_pSelObj->IsExactTypeOf( i3TextureMatrixAttr::GetClassMeta()))
			ChangeTextureMatrixAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3TextureCoordSourceAttr::GetClassMeta()))
			ChangeTextureCoordsourceAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3TextureFunctionAttr::GetClassMeta()))
			ChangeTextureFuctionAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3TextureEnableAttr::GetClassMeta()))
			ChangeTextureEnableAttr();
		else if( m_pSelObj->IsExactTypeOf( i3ProjectMatrixAttr::GetClassMeta()))
			ChangeProjectMatrixAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3ViewMatrixAttr::GetClassMeta()))
			ChangeViewMatrixAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3ModelViewMatrixAttr::GetClassMeta()))
			ChangeModelViewMatrixAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3StencilFuncAttr::GetClassMeta()))
			ChangeStencilFuncAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3StencilOperationAttr::GetClassMeta()))
			ChangeStencilOPAttrList();
		else if( m_pSelObj->IsExactTypeOf( i3NormalMapBindAttr::GetClassMeta()))
			ChangeNormalMapBindAttr();
		else if( m_pSelObj->IsExactTypeOf( i3SpecularMapBindAttr::GetClassMeta()))
			ChangeSpecularMapBindAttr();
		else if( m_pSelObj->IsExactTypeOf( i3LuxMapBindAttr::GetClassMeta()))
			ChangeLuxMapBindAttr();
		else if( m_pSelObj->IsExactTypeOf( i3FresnelAttr::GetClassMeta()))
			ChangeFresnelAttr();
		else
		{
			if( m_pSelObj->IsTypeOf( i3AbstractBoolAttr::GetClassMeta()))
				ChangeEnableAttr();
		}
	}

	return; 
}

BOOL PropertyBoard::Ok(void)
{	

	if( m_EditBox.GetStyle() & WS_VISIBLE)
	{
		//Set Save Change Property
		CpropertyBase * pPropertyBase; 	
		char temp[64];
		
		m_EditBox.GetProperty((INT8*)temp);
		pPropertyBase = GetProperty(); 
		pPropertyBase->Set((const char*)temp);
        ChangeProperty(); 		
		return TRUE; 
	}

	return FALSE; 
}

BOOL PropertyBoard::Cancel(void)
{
	if( m_EditBox.GetStyle() & WS_VISIBLE)
	{
		m_EditBox.Undo(); 
		return TRUE; 
	}

	return FALSE; 
}

BOOL PropertyBoard::ActControl(WPARAM wParam, LPARAM lParam)
{
	WORD Code, Id;
	BOOL rv = FALSE; 

	//Get Messages And Action
	Code = HIWORD( wParam);
	Id = LOWORD( wParam);

	switch(Code)
	{
		case BN_CLICKED :
			{
				switch (Id)
				{
				case IDC_DORPDOWNBUTTON	:  
					OpenListBox();		//Draw List Box; 
					break; 
				case IDC_DIALOGBUTTON	:  
					//For Dialog Box 
					CpropertyBase * pPropertyBase; 
					pPropertyBase = GetProperty(); 					
					pPropertyBase->PopUpDialog(); 
					//Change Property 
					ChangeProperty();
					rv = TRUE; 

					break; 
				}
			}
			break;
		case LBN_SELCHANGE : 
			if (Id == IDC_LISTBOX)
			{
				//Property에 변경된 값을 넣어 준다. 
				char Buf[256];
				CpropertyBase * pPropertyBase; 
				pPropertyBase = GetProperty(); 

				m_ListBox.GetText( m_ListBox.GetCurSel(), Buf);
				pPropertyBase->Set((const char *)Buf); 
				m_ListBox.ShowWindow( SW_HIDE);				
				ChangeProperty();
				rv = TRUE; 
			}
			break; 
		case LBN_KILLFOCUS : 
			break;
 
	}
	return rv; 
}

void PropertyBoard::SetListPropertys(void)
{
	i3ClassMeta * pClassMeta; 

	pClassMeta = m_pSelObj->GetMeta(); 

	////////////////////////////////////////////////////////////
	//For Attr 
	if(m_pSelObj->IsTypeOf(i3RenderAttr::GetClassMeta()))
	{
		SetAttr(); 		

		//For each Attr
		if( m_pSelObj->IsTypeOf( i3TextureEnableAttr::GetClassMeta()))
		{
			SetTextureEnableAttr(); 
		}
		else if (m_pSelObj->IsTypeOf(i3AbstractBoolAttr::GetClassMeta()))
		{
			SetEnableAttr(); 
		}
		else
		{	//other Attr
			if(pClassMeta == i3MaterialAttr::GetClassMeta())
			{
				SetMaterialAttrList(); 
			}else if(pClassMeta == i3ColorAttr::GetClassMeta())
			{
				SetColorAttrList(); 
			}else if(pClassMeta == i3AlphaFuncAttr::GetClassMeta())
			{
				SetAlphaFuncAttrList(); 
			}else if(pClassMeta == i3BlendModeAttr::GetClassMeta())
			{
				SetBlendModeAttrList(); 
			}else if(pClassMeta == i3FaceCullModeAttr::GetClassMeta())
			{
				SetFaceCullModeAttrList(); 
			}else if(pClassMeta == i3FogColorAttr::GetClassMeta())
			{
				SetFogColorAttrList(); 
			}else if(pClassMeta == i3FogDensityAttr::GetClassMeta())
			{
				SetFogDensityAttrList(); 
			}else if(pClassMeta == i3FogModeAttr::GetClassMeta())
			{
				SetFogModeAttrList(); 
			}else if(pClassMeta == i3FogNearFarAttr::GetClassMeta())
			{
				SetFogNearFarAttrList(); 
			}else if(pClassMeta == i3GeometryAttr::GetClassMeta())
			{
				SetGeometryAttrList(); 
			}else if(pClassMeta == i3LightAttr::GetClassMeta())
			{
				SetLightAttrList(); 
			}else if(pClassMeta->IsTypeOf( i3TextureFilterAttr::GetClassMeta()))
			{
				SetTextureFilterAttrList(); 
			}else if(pClassMeta->IsTypeOf( i3TextureWrapAttr::GetClassMeta()))
			{
				SetTextureWrapAttrList(); 
			}else if(pClassMeta == i3ZFuncAttr::GetClassMeta())
			{
				SetZfuncAttrList(); 
			}else if(pClassMeta == i3ShadeModeAttr::GetClassMeta())
			{
                SetShadeModeList(); 
			}else if(pClassMeta == i3RenderModeAttr::GetClassMeta())
			{
				SetRenderModeList(); 
			}else if( pClassMeta->IsTypeOf( i3TextureBindAttr::GetClassMeta()))
			{
				SetTextureBindList(); 
			}
			//New Insert 2004 8 26
			else if(pClassMeta == i3TextureMatrixAttr::GetClassMeta())
			{
                SetTextureMatrixAttrList(); 
			}else if(pClassMeta == i3TextureCoordSourceAttr::GetClassMeta())
			{
                SetTextureCoordsourceAttrList(); 
			}else if(pClassMeta == i3TextureFunctionAttr::GetClassMeta())
			{
                SetTextureFuctionAttrList(); 
			}else if(pClassMeta == i3ProjectMatrixAttr::GetClassMeta())
			{
				SetProjectMatrixAttrList(); 
			}else if(pClassMeta == i3ViewMatrixAttr::GetClassMeta())
			{
				SetViewMatrixAttrList(); 
			}
			else if( pClassMeta->IsExactTypeOf( i3FresnelAttr::GetClassMeta()))
			{
				SetFresnelAttr();
			}
			
			//2005.01.05 raja
			else if(pClassMeta == i3StencilFuncAttr::GetClassMeta())
			{
				SetStencilFuncAttrList();
			}
			else if(pClassMeta == i3StencilOperationAttr::GetClassMeta())
			{
				SetStencilOPAttrList();
			}
			else if(pClassMeta == i3ModelViewMatrixAttr::GetClassMeta())
			{
				SetModelViewMatrixAttrList();
			}
			else if( pClassMeta == i3NormalMapBindAttr::GetClassMeta())
			{
				SetNormalMapBindAttr();
			}
			else if( pClassMeta == i3SpecularMapBindAttr::GetClassMeta())
			{
				SetSpecularMapBindAttr();
			}
			else if( pClassMeta == i3LuxMapBindAttr::GetClassMeta())
			{
				SetLuxMapBindAttr();
			}
			//end raja
		}

	}
	else if( m_pSelObj->IsTypeOf(i3Node::GetClassMeta()))
	{
		////////////////////////////////////////////////////////////
		//For Node 

		SetNode(); 
		//For Each Node 

		if(pClassMeta == i3Geometry::GetClassMeta())		
		{
			SetGeometryList();
			return; 
		}else if(pClassMeta == i3AttrSet::GetClassMeta())	
		{
			SetAttrSetList(); 
			return; 
		}else if(pClassMeta == i3Camera::GetClassMeta())
		{
			SetCameraList();
			return; 
		}else if(pClassMeta == i3Node::GetClassMeta())
		{
			SetNodeList();
			return; 
		}else if(pClassMeta == i3Particle::GetClassMeta())
		{
			SetParticleList();
			return;
		}else if(pClassMeta == i3ColliderSet::GetClassMeta())
		{
			SetColliderSetList();
			return; 
		}else if(pClassMeta == i3SceneObject::GetClassMeta())
		{
			SetSceneObjectList(); 
			return; 
		}else if(pClassMeta == i3LightSet::GetClassMeta())
		{
			SetLightSetList(); 
			return; 
		}
		else if( m_pSelObj->IsTypeOf( i3Skin::GetClassMeta()))
		{
			SetSkinList();
			return; 
		}
		else if( m_pSelObj->IsTypeOf( i3Body::GetClassMeta()))
		{
			SetBodyList();
		}
		else if( m_pSelObj->IsTypeOf( i3BoneRef::GetClassMeta()))
		{
			SetBoneRefList();
		}
		else if( m_pSelObj->IsTypeOf( i3Transform::GetClassMeta()))
		{
			SetTransformList();
			return; 
		}
		else if( m_pSelObj->IsTypeOf( i3OcTreeNode::GetClassMeta()))
		{
			SetOcTreeList();
			return;
		}
		else if( m_pSelObj->IsTypeOf( i3TimeSequence::GetClassMeta()))
		{
			SetTimeSequenceList();
		}
	}else
	{
		if(pClassMeta == i3Collider::GetClassMeta())
		{
			SetCollider();
			return; 
		}
		else if(pClassMeta == i3CollideeSphere::GetClassMeta())
		{
			SetCollideeSphere();
			return;
		}
		else if(pClassMeta == i3CollideeMesh::GetClassMeta())
		{
			SetCollideeMesh();
			return; 
		}
		else if( pClassMeta->IsTypeOf( i3LOD::GetClassMeta()))
		{
			SetLODList();
		}
	}
	return; 
}

void PropertyBoard::OpenListBox(void)
{	
	RECT Rect;
	char conv[256];
	INT32 i; 
	INT32 maxcharlen, charlen;
	POINT pt; 
	CpropertyBase * pProperty = GetProperty(); 
	maxcharlen = charlen = 0;

	m_ListBox.ResetContent();

	if(m_ListBox.GetStyle() & WS_VISIBLE)
	{//List Box를 윈도우에서 지운다. 
		m_ListBox.ShowWindow( SW_HIDE);
	}else 
	{//List Box를 윈도우에 그린다. 
		i3StringList ListBoxList;							

		//Get Property List Value 
		pProperty->GetValueList(&ListBoxList);
		pProperty->Get((char *)conv);					

		//Get ListBox ListValue 
		for( i = 0; i < ListBoxList.GetCount(); i++) 
		{
			m_ListBox.AddString( (char *)ListBoxList.Items[i]);
			charlen = (INT32)strlen((const char*)ListBoxList.Items[i]); 
			if (charlen > maxcharlen)maxcharlen = charlen;
		}

		//Selecte Line
		m_ListBox.SelectString( 0, conv);
		
		//Set ListBox Position  
		Rect.left	= m_center + 1;
		Rect.top	= (m_SelectLine + 1) * PROPERTY_LINE_HSIZE + TMARGIN; 
		Rect.bottom	= Rect.top + (13 *  ListBoxList.GetCount());
		if ((maxcharlen * 9) > ((m_cx - RMARGIN) - Rect.left))
		{
			Rect.right = Rect.left + (maxcharlen * 9);
		}else
		{
			Rect.right = m_cx - RMARGIN; 
		}		

		//Change Client To Screen 
		pt.x = Rect.left; 		pt.y = Rect.top; 
		ClientToScreen(m_Wnd, &pt);
		Rect.left = pt.x;		Rect.top = pt.y; 

		pt.x = Rect.right; 		pt.y = Rect.bottom; 
		ClientToScreen(m_Wnd, &pt);
		Rect.right = pt.x;		Rect.bottom = pt.y; 

		//Show List Box
		m_ListBox.MoveWindow( &Rect, FALSE);
		m_ListBox.ShowWindow( SW_SHOW);
	}
}
