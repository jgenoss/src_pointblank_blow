#include "i3FrameworkPCH.h"
#include "i3UIComboBase.h"

#include "i3UIListBox.h"

#include "i3UIManager.h"
#include "../i3Framework.h"

#include "i3Base/string/ext/utf8_to_utf16.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"


I3_CLASS_INSTANCE( i3UIComboBase);

#define EMPTY_LISTBOX_SCROLLBAR_WIDTH	15.f	// 20.f

i3UIComboBase::i3UIComboBase()
{
	m_pListBox = i3UIListBox::new_object();
	m_pListBox->SetName("Combo_LB");
}

i3UIComboBase::~i3UIComboBase()
{
	I3_SAFE_RELEASE( m_pListBox);
}

void i3UIComboBase::_UpdateShapeState(void)
{
	setShapeVisible( I3UI_COMBOBOX_SHAPE_BKGND, true);

	i3UICaptionControl::_UpdateShapeState();
}

void	i3UIComboBase::_UpdateComponentSize(REAL32 cx, REAL32 cy)
{
	
	this->_UpdateComponentSize_DerivedCombo(cx, cy);
	
	REAL32 offsetY = cy - m_fDefHeightButton;
	
	if( isComboStyle(I3UI_COMBO_STYLE_LISTBOXDIRUP) )
	{
		m_pListBox->setPos( 0.0f, -offsetY);
	}
	else
	{
		m_pListBox->setPos( 0.0f, m_fDefHeightButton);
	}

	if( i3UI::getToolDevMode() == true)
	{
		m_pListBox->setSize( cx, offsetY);
		m_fListBoxHeight = offsetY;			// 멤버변수의 의미와는 달리, 이 변수는 로딩된 높이값을 유지해주어야한다.(11.10.05.수빈)
	}

	
}

void	i3UIComboBase::_AddListBox( void)
{

//	m_pListBox->SetDefaultItems( m_strListBoxItems);
	m_pListBox->Init();

	m_pListBox->CalculateAutoSizeByItem( getWidth(), m_fListBoxHeight);

	setSize( getWidth(), m_DerivedTextCtrl->getHeight() + m_pListBox->getHeight());

	m_pListBox->EnableCtrl( true);
	//m_pListBox->ShowTop();
}

void	i3UIComboBase::_RemoveListBox( void)
{
	this->_RemoveListBox_DerivedCombo();

	i3UIManager* pManager = i3UI::getManager();

	if( pManager != nullptr)
	{
		i3UIControl* target = pManager->getCurrMouseTarget();

		if ( target == nullptr || target == m_pListBox )
			pManager->resetOnMouseTarget();
	}

	//	m_pListBox->setSize( m_pListBox->getWidth(), 0.0f);			// 다시 켤때 문제되므로 사이즈를 미리 줄입니다.(11.06.03.수빈)	
	m_pListBox->EnableCtrl( false, true);

}

void i3UIComboBase::SetTextColor( COLOR * pCol)
{
	i3UICaptionControl::SetTextColor( pCol);

}


void i3UIComboBase::SetCaption( const WCHAR16* pszCaption)
{
	i3UICaptionControl::SetCaption( pszCaption);
	m_DerivedTextCtrl->SetCaption(pszCaption);		// 에딧 박스에 캡션 전달을 해야한다. (내부에서 SetText())
}

void i3UIComboBase::SetTextAlign( I3UI_TEXT_ALIGN_X AlignX, I3UI_TEXT_ALIGN_Y AlignY)
{
	i3UICaptionControl::SetTextAlign(AlignX, AlignY);

	m_DerivedTextCtrl->SetTextAlign(AlignX, AlignY);
	m_DerivedTextCtrl->OnUpdateTextPos();
}

void i3UIComboBase::SetTextSpaceofLetters( INT32 nX, INT32 nY)
{
	i3UICaptionControl::SetTextSpaceofLetters( nX, nY);

	m_DerivedTextCtrl->SetTextSpaceofLetters( nX, nY);
}

void i3UIComboBase::SetTextMargin( INT32 iMarginX, INT32 iMarginY)
{
	i3UICaptionControl::SetTextMargin( iMarginX, iMarginY);

	m_DerivedTextCtrl->SetTextMargin( iMarginX, iMarginY);
}


void i3UIComboBase::SetComboBoxTextColor( COLOR * pCol )
{
	m_DerivedTextCtrl->SetTextColor( pCol);
}

void i3UIComboBase::BackupTextColor()
{
	m_BackupTextColor = m_DerivedTextCtrl->getTextColor();
}

void i3UIComboBase::RestoreTextColor()
{
	m_DerivedTextCtrl->SetTextColor(m_BackupTextColor);
}

void i3UIComboBase::Clear( void)
{
	//Combo를 초기상태로 돌린다.
	//Clear Editbox
	SetCurSel( -1);

	//Clear ListBox
	m_pListBox->Clear();
	m_pListBox->ClearDefaultItemString();			// ListBox클리어 함수와는 별도로 보존되던 디폴트스트링은 별도 함수로 제거..
	_RemoveListBox();
}

void i3UIComboBase::SetItems( const char* pszItems)
{
	if( pszItems == nullptr)
	{
		m_pListBox->ClearDefaultItemString();			// ListBox클리어 함수와는 별도로 보존되던 디폴트스트링은 별도 함수로 제거..
		m_pListBox->Init();
	}
	else
	{
		INT32 len = i3String::MultiByteToWideChar( CP_UTF8, 0, pszItems, i3::generic_string_size( pszItems), nullptr, 0);

		i3::wstring	wstrRes(len, 0);

		i3String::MultiByteToWideChar( CP_UTF8, 0, pszItems, i3::generic_string_size( pszItems), &wstrRes[0], len);

		m_pListBox->SetDefaultItems( wstrRes );
		m_pListBox->Init();
	}
}

void i3UIComboBase::GetItems( char* pBuf, INT32 nBufSize)
{
	const i3::rc_wstring& wstrString = m_pListBox->GetDefaultItemString();

	INT32 len = i3String::WideCharToMultiByte( CP_UTF8, 0, wstrString.c_str(), wstrString.length(), pBuf, nBufSize, nullptr, nullptr);	
	pBuf[len] = 0;
}

//ListBox의 nIdx번째 아이템을 EditBox에 표시합니다.
void i3UIComboBase::SetCurSel( INT32 nIdx)
{

	if( nIdx >= 0 && nIdx < m_pListBox->GetItemCount())
	{
		WCHAR16 pOut[MAX_PATH];

		m_pListBox->GetItem( nIdx, pOut, MAX_PATH);

		m_DerivedTextCtrl->SetText( pOut);
	}
	else
	{
		m_DerivedTextCtrl->SetText( L"");
	}

	m_nCurSel = nIdx;

}

INT32 i3UIComboBase::GetCurSel( void)
{
	return m_nCurSel;
}

void i3UIComboBase::GetCurSelString( WCHAR16 * pOut)
{
	m_pListBox->GetItem( m_nCurSel, pOut, MAX_PATH);
}

bool	 i3UIComboBase::FoldListBox(void)				// 실제로 액션이 일어나면 true, 이미 처리됬거나 처리불가능할때 false로 가정했다. (11.05.26.수빈)
{
	if (!m_pListBox->isEnable())				
		return false;

	_RemoveListBox();

	this->_SetFocusEditBoxIfExists_DerivedCombo();

	Notify( I3UI_EVT_COMBOBOX_FOLDED, 0, 0);

	return true;
}

bool	 i3UIComboBase::UnfoldListBox(void)
{
	if (m_pListBox->isEnable()) 
		return false;

	_AddListBox();

	i3Framework::SetCurrentFocus( m_pListBox);

	Notify( I3UI_EVT_COMBOBOX_UNFOLDED, 0, 0);

	return true;
}

bool	i3UIComboBase::isListBoxUnfolded(void)
{ 
	return m_pListBox->isEnable() != 0; 
}

void i3UIComboBase::OnInitAfterLoad()
{
	i3UICaptionControl::OnInitAfterLoad();

	m_fListBoxHeight = getY( getSize());

	m_DerivedTextCtrl->SetFont( &m_lf);

	this->_OnInitAfterLoad_DerivedCombo();

	m_fListBoxHeight -= m_DerivedTextCtrl->getHeight();



	_RemoveListBox();
}

void i3UIComboBase::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);

	this->_OnEnabled_DerivedCombo(bEnable);


	if( i3UI::getToolDevMode() == true)
	{
		m_pListBox->EnableCtrl( bEnable);
	}
	else
	{
		if( !bEnable) 
			m_pListBox->EnableCtrl( bEnable);
	}

}


void i3UIComboBase::OnBindTemplate(void)
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;


	//ListBox
	//나중에 버튼 누르면 생성하는 방법으로도 Z-order(?)문제가 해결되지 않는다..
	//게다가 ListBox의 설정값들도 모두 초기화되므로 문제가 있음..
	//그냥 여기서 만들자..
	if (m_pListBox->getParent() == nullptr)
	{
		AddControl( m_pListBox);
		m_pListBox->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pListBox->addStyle( I3UI_STYLE_SLAVE);
	}

	this->_OnBindTemplate_DerivedCombo();		// 이 함수호출뒤에 파생클래스의 모든 콘트롤의 셋팅이 정리되어야함..

	//Set Template

	//Update
	_UpdateShapeState();
	_UpdateComponentSize(m_Size.x, m_Size.y);

	if( i3UI::getToolDevMode() == true)
		m_pListBox->Init();		// 툴때문에 일단 사용해봄..

//		_AddListBox();

}

void i3UIComboBase::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_CLICKED :		//
		//	case I3UI_EVT_BTN_PUSHED:	// Button이 눌려졌다.(버튼의 경우, CLICKED와 BTN_PUSHED양쪽에서 모두 오기 때문에 둘중 하나만 써야합니다.
		{
			// 에딧박스는 CLICKED만 옵니다..) 11.07.03.수빈.
		
			if( !isActivated() )
				return;
			
			bool bComboList = this->_IsListBoxActionNeededWhenClick_DerivedCombo(pCtrl);

			if( bComboList)
			{
				// Disable 시키는 방법
				if( m_pListBox->isEnable())
				{
					_RemoveListBox();
					this->_SetFocusEditBoxIfExists_DerivedCombo();
					Notify( I3UI_EVT_COMBOBOX_FOLDED, 0, 0);
				}
				else
				{
					_AddListBox();
					i3Framework::SetCurrentFocus( m_pListBox);
					Notify( I3UI_EVT_COMBOBOX_UNFOLDED, 0, 0);
				}

				//				I3TRACE(" Combo Button!\n");
			}
		}
		break;
	case I3UI_EVT_SELITEM:		// Item이 선택되었다.
		{
			I3ASSERT( pCtrl == m_pListBox);

			if( !isActivated() )
				return;

			SetCurSel( param1);

			if(i3::same_of<i3UIListView_Cell* >(getParent()))
			{
				// 리스트 뷰 내 콤보박스 아이템이 설정됬을 경우,I3UI_EVT_COMBOBOX_SELITEM 이벤트를 상위로 올립니다. 
				// ( param1 = 콤보박스에서 선택된 아이템 인덱스) (2012.07.25 - 양승천) 
				ParentNotify(I3UI_EVT_COMBOBOX_SELITEM, 0, param1);		
			}
			else
			{
				// 리스트박스의 페어런트 노티파이는 있었지만, 여기서는 아예 노티파이가 없었음..(추가..11.06.23.수빈)
				Notify(I3UI_EVT_SELITEM, param1,0);		
			}

			_RemoveListBox();

			I3TRACE( "i3UIComboBox::OnNotify -- I3UI_EVT_SELITEM: %d\n", param1);

			if( param1 != -1)
			{
				this->_SetFocusEditBoxIfExists_DerivedCombo();
			}
			Notify( I3UI_EVT_COMBOBOX_FOLDED, 0, 0);
		}
		break;

	case I3UI_EVT_KILLFOCUS :

		//		if ( pCtrl == m_pEditBox )
		//		{
		//			if( !m_pEditBox->isReadOnly() )	// 쓰기가능 에딧박스에 포커스가 죽으면 안에 있는 입력에 대해 바깥에 Notify를 주는게 좋겠다..(루아용?) 11.07.22.수빈.
		//			{
		//				// 귀찮게 이벤트 아이디를 만들어줘야될것 같다..당분간 홀드시키고 주석칩니다...11.07.22.수빈.
		//
		//			}
		//		}

		if( m_pListBox->isEnable() )
		{
			bool bCloseListBox = true;

			i3UIControl * pByForce = (i3UIControl*) param1;
			if( pByForce != nullptr )
			{
				if( i3::same_of<i3UIFrameWnd*>(pByForce))
				{
					pByForce = ((i3UIFrameWnd*)pByForce)->GetPreFocusCtrl();
				}

				// Child에 대한 Kill이다 ListBox는 닫지 않습니다.
				bCloseListBox = IsChildControl( pByForce)?false:true;
			}

			if( bCloseListBox)
			{	
				_RemoveListBox();

				Notify( I3UI_EVT_COMBOBOX_FOLDED, 0, 0);
			}
		}
		break;
	default:
		break;
	}

	i3UICaptionControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIComboBase::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	_UpdateComponentSize( cx, cy);
}

void i3UIComboBase::OnMove( REAL32 x, REAL32 y)
{
	i3UICaptionControl::OnMove( x, y);

	_UpdateComponentSize( getWidth(), getHeight());
}


bool i3UIComboBase::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{

	if( m_pListBox->isEnable())
	{
		_RemoveListBox();
		Notify( I3UI_EVT_COMBOBOX_FOLDED, 0, 0);
	}

	return i3UICaptionControl::OnKillFocus( pByCtrl);
}


void i3UIComboBase::OnDraw( i3UIRenderer * pRenderer)
{
	if( isEnable() == false)
		return;

	//pRenderer->changeDepth();

	if( isAllocedSprite() )
	{
		INT32 i;

		for( i = 0; i < getShapeCount(); i++)
		{
			INT32 hSprite = getSpriteHandle( i);
			if( hSprite != -1)
				pRenderer->Draw_Kick( hSprite);
		}
	}

	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild == m_pListBox)
		{
			if( m_pListBox->isEnable())
			{
				pRenderer->Draw_SetTopMost( true);
			}
		}

		pChild->OnDraw( pRenderer);

		pChild = (i3UIControl *) pChild->getNext();


		pRenderer->Draw_SetTopMost( false);
	}
}


namespace
{
	UINT32& FileIDToUINT(UINT8 (&strID)[4] ) {  return reinterpret_cast<UINT32&>(strID); }
	const UINT32& FileIDToUINT(const char (&strID)[5] ) {  return reinterpret_cast<const UINT32&>(strID); }

	const UINT32& UCM1 = FileIDToUINT("UCM1");
	const UINT32& UCM2 = FileIDToUINT("UCM2");
	const UINT32& UCM3 = FileIDToUINT("UCM3");
///
	const UINT32& UCM4 = FileIDToUINT("UCM4");	//	--> ComboBox_NoEditBox 의 별도 읽기/저장 및 리스트 폰트 처리..
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UICOMBOBOX
	{
		UINT8		m_ID[4] = { 'U', 'C', 'M', '4' };
		UINT32		m_ComboStyle = 0;		// I3UI_COMBO_STYLE
		WCHAR16		m_ListBoxItems[I3UI_ITEMLIST_SIZE] = { 0 };
		REAL32		m_fListBoxScrollBarWidth = 0.0f;
		REAL32		m_fListMarginUpScrollBar = 0.0f;
		REAL32		m_fListMarginDownScrollBar = 0.0f;
		REAL32		m_fListMarginRightScrollBar = 0.0f;
		REAL32		m_fDefHeightButton = 0.0f;

		UINT32		m_clrLBText_MouseOver = 0;
		UINT32		m_clrLBText_Selected = 0;
		UINT32		m_clrLBText_Normal = 0;
		INT32		m_ListBoxLeftMargin = 0;

		INT32		m_idxListFontName = 0;
		UINT32		m_ListFontWeight = 0;
		INT32		m_ListFontSizePt = 0;

		UINT8		m_ListItalic = 0;
		UINT8		m_ListUnderline = 0;
		UINT8		m_ListStrikeOut = 0;
		UINT8		m_pad_1byte = 0;

		UINT32		pad[19] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

namespace
{
	static void Process_Save_I3_PERSIST_UICOMBOBOX(i3ResourceFile * pResFile, UINT32 comboStyle, REAL32 defaultHeightButton, 
			i3UIListBox* listBox, UINT32 saveID, pack::UICOMBOBOX& inout)
	{
		
		//i3mem::Copy( inout.m_ID, &saveID, 4);		// UCM4 /초기화에서 설정합니다. 여기는 주석처리 2017.2.14. soon9
		inout.m_ComboStyle = comboStyle;

		i3mem::Copy( inout.m_ListBoxItems, listBox->GetDefaultItemString().c_str(), _countof(inout.m_ListBoxItems));

		inout.m_fDefHeightButton = defaultHeightButton;
		inout.m_fListBoxScrollBarWidth = listBox->GetScrollBarWidth();
		inout.m_fListMarginUpScrollBar =	listBox->GetScrollBarMarginUp();		
		inout.m_fListMarginDownScrollBar = listBox->GetScrollBarMarginDown(); 
		inout.m_fListMarginRightScrollBar = listBox->GetScrollBarMarginRight();
		I3COLOR dwClr = listBox->GetTextColor_MouseOver();
		inout.m_clrLBText_MouseOver  = reinterpret_cast<UINT32&>(dwClr);
		dwClr = listBox->GetTextColor_Selected();
		inout.m_clrLBText_Selected = reinterpret_cast<UINT32&>(dwClr);
		inout.m_clrLBText_Normal = listBox->getTextColor();
		inout.m_ListBoxLeftMargin = listBox->getTextSpaceX();
		
		const LOGFONTA* lf = listBox->GetLogFont();
		
		if (lf->lfFaceName[0] != 0)
		{
			inout.m_idxListFontName	= i3::vu::int_index_of(pResFile->GetStringTable(),lf->lfFaceName);
		}
		else
			inout.m_idxListFontName = -1;
		
		inout.m_ListFontWeight = lf->lfWeight;
		inout.m_ListFontSizePt = _HeightToPoint(lf->lfHeight);

		inout.m_ListItalic = lf->lfItalic;
		inout.m_ListUnderline = lf->lfUnderline;
		inout.m_ListStrikeOut = lf->lfStrikeOut;
		

	}

	static void Process_Load_I3_PERSIST_UICOMBOBOX(i3ResourceFile * pResFile, pack::UICOMBOBOX& base_info, UINT32& comboStyle, REAL32& defaultHeightButton, 
		i3UIListBox* listBox, UINT32 defaultClr, INT32 baseVer)		// baseVer = 0,1,2 ...
	{
		
		comboStyle = base_info.m_ComboStyle;
		listBox->SetDefaultItems( base_info.m_ListBoxItems );
		defaultHeightButton = base_info.m_fDefHeightButton;

		if( base_info.m_fListBoxScrollBarWidth == 0.f)
			base_info.m_fListBoxScrollBarWidth = 15.f;					// 0이면 강제로 15로 넣는것 같다..

		listBox->SetScrollBarWidth( base_info.m_fListBoxScrollBarWidth);
		listBox->SetScrollBarMarginUp(base_info.m_fListMarginUpScrollBar);
		listBox->SetScrollBarMarginDown(base_info.m_fListMarginDownScrollBar);
		listBox->SetScrollBarMarginRight(base_info.m_fListMarginRightScrollBar);

		switch (baseVer)
		{
		case 0:
			{
				listBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(defaultClr));
				listBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(defaultClr));
			}
		break;

		case 1:
			{
				listBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_MouseOver));
				listBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_Selected));
			}
		break; 

		case 2:
			{
				listBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_MouseOver));
				listBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_Selected));
				listBox->SetTextColor(base_info.m_clrLBText_Normal);
				listBox->SetTextSpace(base_info.m_ListBoxLeftMargin , listBox->getTextSpaceY());
			}
		break;

		case 3:
			{
				listBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_MouseOver));
				listBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(base_info.m_clrLBText_Selected));
				listBox->SetTextColor(base_info.m_clrLBText_Normal);
				listBox->SetTextSpace(base_info.m_ListBoxLeftMargin , listBox->getTextSpaceY());
				
				if (base_info.m_ListFontSizePt > 0)		// 0폰트사이즈인경우 포맷이 다른 것으로 취급..
				{
					LOGFONTA lf;
					::memcpy(&lf, listBox->GetLogFont(), sizeof(LOGFONTA));

					if (base_info.m_idxListFontName != -1)
					{
						const i3::rc_string& font_name = pResFile->GetStringTable()[base_info.m_idxListFontName];
						i3::safe_string_copy(lf.lfFaceName, font_name, LF_FACESIZE);
					}
					else
						i3::safe_string_copy(lf.lfFaceName, "굴림", LF_FACESIZE);

					lf.lfHeight = _PointToHeight(base_info.m_ListFontSizePt);
					lf.lfWeight = base_info.m_ListFontWeight;
					lf.lfItalic = base_info.m_ListItalic;
					lf.lfUnderline = base_info.m_ListUnderline;
					lf.lfStrikeOut = base_info.m_ListStrikeOut;

					listBox->SetFont(&lf);
				}
			}
		break;
		default:
			I3PRINTLOG(I3LOG_FATAL, "Not Found ComboBox Persist ID.");
		break;
		}
		
	}

}



UINT32 i3UIComboBase::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);

	pack::UICOMBOBOX data;
	
	Process_Save_I3_PERSIST_UICOMBOBOX(pResFile, m_ComboStyle, m_fDefHeightButton, m_pListBox, UCM4, data);
	Rc = pResFile->Write( &data, sizeof( data));

	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIComboBase::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Rc, Result = 0;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);
	
	pack::UICOMBOBOX data;

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;
	
	UINT32 fileID = FileIDToUINT(data.m_ID);

	INT32 baseVer = -1;
	if (fileID == UCM1) baseVer = 0; 
	else
	if (fileID == UCM2) baseVer = 1;
	else
	if (fileID == UCM3 ) baseVer = 2;
	else
	if ( fileID == UCM4 ) baseVer = 3;

	Process_Load_I3_PERSIST_UICOMBOBOX(pResFile, data, m_ComboStyle, 
		m_fDefHeightButton, m_pListBox, this->getTextColor(), baseVer);
	
	Rc = this->_OnLoadMore(pResFile, fileID);

	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

bool i3UIComboBase::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	char buf[MAX_PATH*2];
	const i3::rc_wstring& wstrDefault = m_pListBox->GetDefaultItemString();

	INT32 len = i3String::WideCharToMultiByte( CP_UTF8, 0, wstrDefault.c_str(), wstrDefault.length(), buf, sizeof( buf), nullptr, nullptr);
	buf[len] = 0;
	pXML->addAttr( "ListBox_Items", buf);
	pXML->addAttr( "ComboStyle", m_ComboStyle);
	pXML->addAttr( "Combobox_BtnHeight", m_fDefHeightButton );

	pXML->addAttr( "ScrollBarWidth",	m_pListBox->GetScrollBarWidth() );
	pXML->addAttr( "ScrollBarMarginUp", m_pListBox->GetScrollBarMarginUp());
	pXML->addAttr( "ScrollBarMarginDown", m_pListBox->GetScrollBarMarginDown() );
	pXML->addAttr( "ScrollBarMarginRgiht", m_pListBox->GetScrollBarMarginRight() );

	I3COLOR clr = m_pListBox->GetTextColor_MouseOver();
//	pXML->addAttr( "ListBox_TextColor_MouseOver", reinterpret_cast<UINT32&>(clr));
	pXML->addAttr( "ListBoxText_MouseOver_ColorR", (INT32) i3Color::GetR( &clr));
	pXML->addAttr( "ListBoxText_MouseOver_ColorG", (INT32) i3Color::GetG( &clr));
	pXML->addAttr( "ListBoxText_MouseOver_ColorB", (INT32) i3Color::GetB( &clr));
	pXML->addAttr( "ListBoxText_MouseOver_ColorA", (INT32) i3Color::GetA( &clr));

	clr = m_pListBox->GetTextColor_Selected();
//	pXML->addAttr( "ListBox_TextColor_Selected", reinterpret_cast<UINT32&>(clr));		
	pXML->addAttr( "ListBoxText_Selected_ColorR", (INT32) i3Color::GetR( &clr));
	pXML->addAttr( "ListBoxText_Selected_ColorG", (INT32) i3Color::GetG( &clr));
	pXML->addAttr( "ListBoxText_Selected_ColorB", (INT32) i3Color::GetB( &clr));
	pXML->addAttr( "ListBoxText_Selected_ColorA", (INT32) i3Color::GetA( &clr));

	UINT32 ui32Clr = m_pListBox->getTextColor();
	clr = reinterpret_cast<I3COLOR&>( ui32Clr);
		
	pXML->addAttr( "ListBoxText_Normal_ColorR", (INT32) i3Color::GetR( &clr));
	pXML->addAttr( "ListBoxText_Normal_ColorG", (INT32) i3Color::GetG( &clr));
	pXML->addAttr( "ListBoxText_Normal_ColorB", (INT32) i3Color::GetB( &clr));
	pXML->addAttr( "ListBoxText_Normal_ColorA", (INT32) i3Color::GetA( &clr));

	pXML->addAttr( "ListBoxText_LeftMargin", m_pListBox->getTextSpaceX());

	const LOGFONTA* lf = m_pListBox->GetLogFont();

	pXML->addAttr( "ListFontName", lf->lfFaceName);
	pXML->addAttr( "ListFontSize", _HeightToPoint(lf->lfHeight));
	pXML->addAttr( "ListFontWeight", lf->lfWeight);
	pXML->addAttr( "ListItalic", lf->lfItalic);
	pXML->addAttr( "ListUnderline", lf->lfUnderline);
	pXML->addAttr( "ListStrikeOut", lf->lfStrikeOut);

	return bResult;
}

bool i3UIComboBase::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	const char* pszText = pXML->getAttr( "ListBox_Items");
	INT32 len = i3::generic_string_size(pszText);

	i3::stack_wstring wstrRes( len, 0);		// 이 경우의 가정은 멀티바이트 글자 1개당 1~n바이트를 차지하고, wchar_t로 변환시 element갯수는 늘지 않고
	// 같거나 줄어든다는 것이다.
	i3String::MultiByteToWideChar( CP_UTF8, 0, pszText, len, &wstrRes[0], wstrRes.length() );

	wstrRes.erase( i3::char_traits<wchar_t>::length(&wstrRes[0]) );		//널값을 끝에 n개 포함하므로, 그냥 넣으면 안된다.

	m_pListBox->SetDefaultItems(wstrRes);

	pXML->getAttr( "ComboStyle", &m_ComboStyle);

	pXML->getAttr( "Combobox_BtnHeight", &m_fDefHeightButton );


	if( m_fDefHeightButton == 0.f )
		m_fDefHeightButton = 22.0f;

	REAL32 fTemp;

	pXML->getAttr( "ScrollBarWidth", &fTemp);
	if( fTemp == 0.f)
		fTemp = 15.f;
	m_pListBox->SetScrollBarWidth(fTemp);

	pXML->getAttr( "ScrollBarMarginUp", &fTemp);
	m_pListBox->SetScrollBarMarginUp(fTemp);
	pXML->getAttr( "ScrollBarMarginDown", &fTemp);
	m_pListBox->SetScrollBarMarginDown(fTemp);
	pXML->getAttr( "ScrollBarMarginRgiht", &fTemp);			// 문자열 철자가 틀려있는데 고치기가 난감하다..
	m_pListBox->SetScrollBarMarginRight(fTemp);

// ListBoxText_MouseOver_ColorR, ListBoxText_Selected_ColorR

	INT32 r = 255, g = 255, b = 255, a = 255;

	if (pXML->getAttr("ListBoxText_MouseOver_ColorR", &r) )
	{
		pXML->getAttr("ListBoxText_MouseOver_ColorG", &g);
		pXML->getAttr("ListBoxText_MouseOver_ColorB", &b);
		pXML->getAttr("ListBoxText_MouseOver_ColorA", &a);
		
		I3COLOR color;
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pListBox->SetTextColor_MouseOver(color);

		pXML->getAttr("ListBoxText_Selected_ColorR", &r);
		pXML->getAttr("ListBoxText_Selected_ColorG", &g);
		pXML->getAttr("ListBoxText_Selected_ColorB", &b);
		pXML->getAttr("ListBoxText_Selected_ColorA", &a);
		
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pListBox->SetTextColor_Selected(color);
		
		if ( pXML->getAttr("ListBoxText_Normal_ColorR", &r) )
		{
			pXML->getAttr("ListBoxText_Normal_ColorG", &g);
			pXML->getAttr("ListBoxText_Normal_ColorB", &b);
			pXML->getAttr("ListBoxText_Normal_ColorA", &a);
		}
		else 
		{
			r = g = b = a = 255;
		}

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_pListBox->SetTextColor(&color);

		INT32 left_margin = 0;
		pXML->getAttr("ListBoxText_LeftMargin", &left_margin);
		m_pListBox->SetTextSpace(left_margin, m_pListBox->getTextSpaceY());
	}
	else
	{
		UINT32 dwClr = this->getTextColor();

		if (pXML->getAttr("ListBox_TextColor_MouseOver", &dwClr) )
		{
			m_pListBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(dwClr));
		}
		else
		{
			dwClr = this->getTextColor();
			m_pListBox->SetTextColor_MouseOver(reinterpret_cast<I3COLOR&>(dwClr));
		}

		if (pXML->getAttr("ListBox_TextColor_Selected", &dwClr) )
		{
			m_pListBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(dwClr));
		}
		else
		{
			dwClr = this->getTextColor();
			m_pListBox->SetTextColor_Selected(reinterpret_cast<I3COLOR&>(dwClr));
		}

		dwClr = this->getTextColor();
		m_pListBox->SetTextColor(dwClr);
	}


	const char* szFontName;

	if ( nullptr != ( szFontName = pXML->getAttr("ListFontName") ) )
	{
		LOGFONTA lf;
		::memcpy(&lf, m_pListBox->GetLogFont(), sizeof(LOGFONTA));

		i3::safe_string_copy(lf.lfFaceName, szFontName, _countof(lf.lfFaceName) - 1);
		INT32 val = 0;
		pXML->getAttr( "ListFontSize", &val);
		lf.lfHeight = _PointToHeight(val);
				
		pXML->getAttr( "ListFontWeight", &val);
		lf.lfWeight = (LONG) val;

		pXML->getAttr( "ListItalic", &val);
		lf.lfItalic = (BYTE) val;

		pXML->getAttr( "ListUnderline", &val); 
		lf.lfUnderline = (BYTE)val;

		pXML->getAttr( "ListStrikeOut", &val);
		lf.lfStrikeOut = (BYTE)val;

		m_pListBox->SetFont(&lf);
	}

	return bResult;
}


void i3UIComboBase::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::kind_of< i3UIComboBase*>(pDest)); 

	i3UIComboBase* pCombo = (i3UIComboBase*)pDest;
	pCombo->addComboStyle( m_ComboStyle);
	pCombo->m_fDefHeightButton = m_fDefHeightButton;

	this->m_pListBox->CopyTo( pCombo->m_pListBox, method);
}


#if defined( I3_DEBUG)
void i3UIComboBase::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIComboBox");

	AddMFCProperty_Real32( List, &m_fDefHeightButton, "Button Height", "Button의 높이값 속성" );

	AddMFCProperty_UINT32BitFlag( List, &m_ComboStyle, I3UI_COMBO_STYLE_LISTBOXDIRUP,"ListBox Dir Up",	"ListBox를 위에 붙이는 경우 TRUE 설정");

	this->_GetTDKGridProperties_DerivedCombo(List);


	{
		//리스트박스,에딧박스 등  자식콘트롤쪽 속성은 안쓰는게 훨씬 더 많으므로 지저분하게 끌고 가지 말고, 저장/로딩되는 놈들만 프로퍼티에 배치한다...
		// 2014.12.19.수빈 //
		AddMFCProperty_BeginGroup( List, "Child ListBox");
	//	m_pListBox->GetTDKGridProperties( List);

		AddMFCProperty_RcStringW( List, m_pListBox->m_strDefaultItems, "Initial Items", "자동으로 ListBox에 추가될 스트링을 ';'로 구분하여 입력하십시오");

		AddMFCProperty_Real32( List, &m_pListBox->m_fWidthScrollBar, "Scrollbar Width", "Scrollbar의 Width값");
		AddMFCProperty_Real32( List, &m_pListBox->m_fMarginUpScrollBar, "Scrollbar MarginUp", "Scrollbar의 MarginUp값");
		AddMFCProperty_Real32( List, &m_pListBox->m_fMarginDownScrollBar, "Scrollbar MarginDown", "Scrollbar의 MarginDown값");
		AddMFCProperty_Real32( List, &m_pListBox->m_fMarginRightScrollBar, "Scrollbar MarginRight", "Scrollbar의 MarginRight값");

		AddMFCProperty_Int32( List, &m_pListBox->m_SpaceX, "ListBox Left Margin", "리스트박스의 Left 마진값");

		AddMFCProperty_Font(List, &m_pListBox->m_lf, &m_pListBox->m_TextColor, "List Font", "리스트 텍스트 폰트");

		AddMFCProperty_Color( List, &m_pListBox->m_TextColor, "TextColor Normal", "리스트의 색상");
		AddMFCProperty_Color(List, &m_pListBox->m_TextColor_MouseOver, "TextColor MouseOver", "마우스오버된 텍스트의 색상");
		AddMFCProperty_Color(List, &m_pListBox->m_TextColor_Selected, "TextColor Selected", "셀렉션된 텍스트의 색상");

		AddMFCProperty_EndGroup( List);
	}

	AddMFCProperty_EndGroup( List);
}

void i3UIComboBase::OnTDKPropertyChanged( const char* szName)
{
	i3UICaptionControl::OnTDKPropertyChanged( szName);

	//Font color가 이상하게 업데이트되서 ㄷㄷㄷ
	if (i3::generic_is_iequal(szName, "Font Color"))
	{
		this->_OnTDKPropertyChanged_DerivedColor(m_TextColor);
	}

	if( i3::generic_is_equal( szName, "ListBox Dir Up") )
	{
		// 변수는 자동갱신..  렌더링 반영이 안된게 있다면 여기서 재구현..
	}
	else if( i3::generic_is_equal( szName, "Scrollbar Width") || i3::generic_is_equal( szName, "Scrollbar MarginUp") ||
		     i3::generic_is_equal( szName, "Scrollbar MarginDown") || i3::generic_is_equal( szName, "Scrollbar MarginRight") ||
			 i3::generic_is_equal(szName, "TextColor MouseOver") || i3::generic_is_equal(szName, "TextColor Selected") )
	{
		m_pListBox->OnTDKPropertyChanged(szName);
	}
	else if ( i3::generic_is_equal( szName, "TextColor Normal"))
	{
		// m_pListBox->getTextColor() 내부 구현문제로 그 함수를 쓰지 말아야함..
		m_pListBox->SetTextColor(&m_pListBox->m_TextColor);			// m_pListBox->getTextColor());
		m_pListBox->addState( I3UI_STATE_NEED_UPDATE);
	}
	else if ( i3::generic_is_equal(szName, "ListBox Left Margin") )
	{
		m_pListBox->OnTDKPropertyChanged("Left Margin");
	}
	else if ( i3::generic_is_equal(szName, "List Font") )
	{
		m_pListBox->OnTDKPropertyChanged("Font");
	}
	else if (i3::generic_is_iequal( szName, "Initial Items") )
	{
		m_pListBox->OnTDKPropertyChanged(szName);		// 반영 후에는 텍스트 적재도 수행하는게 좋겠다.(툴에서 디폴트문자열들이 이제 저장되도록 처리됨)
	}
	else if( i3::generic_is_iequal( szName, "Template") )
	{
		// Template에 대한 변경은 List Box 등에 전달되어선 안된다.
		// 이 Template는 ComboBox를 위한 것이기 때문.
		return;
	}
	else if( i3::generic_is_iequal( szName, "Font")  ||
		i3::generic_is_iequal( szName, "Font Quality")  ||
		i3::generic_is_iequal( szName, "Font Color")  )
	{
		m_DerivedTextCtrl->SetFont( m_lf.lfFaceName, _HeightToPoint(m_lf.lfHeight), m_lf.lfWeight);
		m_DerivedTextCtrl->SetTextColor( &m_TextColor);
		m_DerivedTextCtrl->addState( I3UI_STATE_NEED_UPDATE);
	}
	else if( i3::generic_is_iequal( szName, "Left Margin")  || 
		i3::generic_is_iequal( szName, "Top Margin") )
	{
		m_DerivedTextCtrl->SetTextMargin(m_SpaceX, m_SpaceY);
	}
	else if( i3::generic_is_iequal( szName, "Text Align X")  ||
		i3::generic_is_iequal( szName, "Text Align Y") )
	{
		m_DerivedTextCtrl->SetTextAlign( (I3UI_TEXT_ALIGN_X) m_AlignX, (I3UI_TEXT_ALIGN_Y) m_AlignY);
		m_DerivedTextCtrl->OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Space of Letters X")  ||
		i3::generic_is_iequal( szName, "Space of Letters Y") )
	{
		m_DerivedTextCtrl->SetTextSpaceofLetters( m_SpaceofLettersX, m_SpaceofLettersY);
	}
	else if( i3::generic_is_iequal( szName, "Line Feed") )
	{
		m_DerivedTextCtrl->setTextAutoLineFeed( m_bLineFeed);
		m_DerivedTextCtrl->OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Line Pitch") )
	{
		m_DerivedTextCtrl->setLinePitch( m_nLinePitch);
		m_DerivedTextCtrl->OnUpdateTextPos();
	}
	else if( i3::generic_is_iequal( szName, "Line Feed With Word") )
	{
		m_DerivedTextCtrl->setTextLineFeedSplitWord( m_bLineFeedSplitWord);
		m_DerivedTextCtrl->OnUpdateTextPos();
	}
	else if(i3::generic_is_iequal( szName, "Shadow Type") )
	{
		m_DerivedTextCtrl->SetTextShadowType( m_TextShadowType);
	}
	else
	{
		this->_OnTDKPropertyChanged_DerivedCombo(szName);
	}

}

#endif


REAL32 i3UIComboBase::getMinimumHeight( void)
{
	return m_DerivedTextCtrl->getHeight();
}

bool i3UIComboBase::isOnMouse(  I3UI_OVERLAP_INFO * pInfo)
{
	if( m_pListBox->isEnable())
	{	// ListBox가 Enable 상태라면
		// ListBox가 FrameWnd 밖에 잇을 수도 있다.. 이것도 검사
		REAL32 right;

		if( m_Size.x < m_pListBox->getWidth())
			right = m_AccPos.x + m_pListBox->getWidth();
		else
			right = m_AccPos.x + m_Size.x;

		if( pInfo->x >= m_AccPos.x && pInfo->x <= right )
		{
			REAL32 top;
			REAL32 bottom;

			if( isComboStyle(I3UI_COMBO_STYLE_LISTBOXDIRUP))
			{
				top	= m_AccPos.y - (m_Size.y - m_fDefHeightButton);
				bottom = m_AccPos.y + m_fDefHeightButton;
			}
			else
			{
				top = m_AccPos.y;
				bottom	= m_AccPos.y + m_Size.y;
			}

			if( pInfo->y >= top && pInfo->y <= bottom )
			{
				return true;
			}
		}

		return false;
	}
	else
	{
		REAL32 right	= m_AccPos.x + m_Size.x;
		REAL32 bottom	= m_AccPos.y + m_DerivedTextCtrl->getHeight();

		if( pInfo->x >= m_AccPos.x && pInfo->x <= right )
		{
			if( pInfo->y >= m_AccPos.y && pInfo->y <= bottom )
			{
				return true;
			}
		}

		return false;
	}
}

// ===========================================================================
// Lua Glue
// ===========================================================================
extern "C" int i3UICB_SetItems( lua_State* L)
{
	char szItems[ 2048];	// 이 사이즈 더 클수도 있다? 네 커졌네요 그래서 2배로 늘립니다.

	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);

	if( pBase != nullptr)
	{
		if( i3Lua::GetStringArg( L, 2, szItems, sizeof( szItems), "") != nullptr)
		{
			pBase->SetItems(szItems);
		}
	}

	return 0;
}

extern "C" int i3UICB_SetEditTextColor( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);

	if( pBase != nullptr)
	{
		INT32 r = i3Lua::GetIntegerArg( L, 2, 255);
		INT32 g = i3Lua::GetIntegerArg( L, 3, 255);
		INT32 b = i3Lua::GetIntegerArg( L, 4, 255);
		COLOR col;
		i3Color::Set( &col, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)255);

		pBase->getDerivedTextControl()->SetTextColor( &col);
	}

	return 0;
}

extern "C" int i3UICB_SetListTextColor( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		i3UIListBox* pListBox = static_cast<i3UIComboBox*>(pBase)->getListBox();

		INT32 r = i3Lua::GetIntegerArg( L, 2, 255);
		INT32 g = i3Lua::GetIntegerArg( L, 3, 255);
		INT32 b = i3Lua::GetIntegerArg( L, 4, 255);

		COLOR col;
		i3Color::Set( &col, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)255);
		pListBox->SetTextColor( &col);
	}
	return 0;
}

extern "C" int i3UICB_GetItems( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		char pBuf[MAX_PATH];
		pBase->GetItems( pBuf, sizeof( pBuf));
		i3Lua::PushString( L, pBuf);
	}

	return 1;
}

extern "C" int i3UICB_GetCurSel( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		INT32 nIdx = pBase->GetCurSel();
		i3Lua::PushInteger( L, nIdx);
	}

	return 1;
}

extern "C" int i3UICB_SetCurSel( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);
		pBase->SetCurSel( nIdx);
	}

	return 0;
}

extern "C" int i3UICB_Clear( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		pBase->Clear();
	}

	return 0;
}

extern "C" int i3UICB_GetCurSelString( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		WCHAR16 pBuf[MAX_PATH];
		pBase->GetCurSelString(pBuf);
		i3Lua::PushString( L, pBuf);
	}

	return 1;
}

extern "C" int i3UICB_isListBoxUnfolded( lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		i3Lua::PushBoolean(L, pBase->isListBoxUnfolded());
	}
	return 1;
}

extern "C" int i3UICB_FoldListBox(lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);

	if( pBase != nullptr)
	{
		i3Lua::PushBoolean(L, pBase->FoldListBox());
	}
	return 1;
}

extern "C" int i3UICB_UnfoldListBox(lua_State* L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		i3Lua::PushBoolean(L, pBase->UnfoldListBox());
	}
	return 1;
}

extern "C" int i3UICB_SetEditBoxReadOnly( LuaState * L)
{
	i3UIComboBase* pBase = (i3UIComboBase*)i3Lua::GetUserDataArg( L, 1);

	if( pBase != nullptr)
	{
		bool bReadOnly = i3Lua::GetBooleanArg( L, 2);

		if (i3::same_of<i3UIComboBox*>(pBase))
			static_cast<i3UIComboBox*>(pBase)->SetEditBoxReadOnly( bReadOnly);
	}

	return 0;
}

extern "C" int i3UICB_SetEditBoxText( LuaState * L)
{
	i3UIComboBase* pBase = (i3UIComboBase*) i3Lua::GetUserDataArg( L, 1);
	if( pBase != nullptr)
	{
		i3UICaptionControl* pTextCtrl = pBase->getDerivedTextControl();

		char szText[ 1024];	// 이 사이즈 더 클수도 있다?

		if( i3Lua::GetStringArg( L, 2, szText, sizeof( szText), "") != nullptr)
		{
			i3::rc_wstring wstrText;
			i3::utf8_to_utf16(szText, wstrText);
			pTextCtrl->SetText( wstrText );
		}
	}

	return 0;
}

LuaFuncReg i3UIComboBox_Glue[] = 
{
	{"uiCB_SetItems",			i3UICB_SetItems		},
	{"uiCB_GetItems",			i3UICB_GetItems		},
	{"uiCB_GetCurSel",			i3UICB_GetCurSel	},
	{"uiCB_SetCurSel",			i3UICB_SetCurSel	},
	{"uiCB_SetEditTextColor",	i3UICB_SetEditTextColor	},
	{"uiCB_SetListTextColor",	i3UICB_SetListTextColor	},
	{"uiCB_Clear",				i3UICB_Clear	},
	{"uiCB_GetCurSelString",	i3UICB_GetCurSelString	},
	{"uiCB_SetEditBoxReadOnly",	i3UICB_SetEditBoxReadOnly	},
	{"uiCB_isListBoxUnfolded",	i3UICB_isListBoxUnfolded },	// 11.05.26.수빈.
	{"uiCB_FoldListBox",		i3UICB_FoldListBox },		// 11.05.26.수빈.
	{"uiCB_UnfoldListBox",		i3UICB_UnfoldListBox },		// 11.05.26.수빈.
	{"uiCB_SetEditBoxText",		i3UICB_SetEditBoxText },		// 11.05.26.수빈.
	{nullptr, nullptr}
};

void i3UIComboBase::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIComboBox_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIComboBox_Glue[i].name, i3UIComboBox_Glue[i].func);
	}
}

// Test Automation 관련
i3UIControl * i3UIComboBase::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	if (m_DerivedTextCtrl == nullptr)
		return nullptr;

	const wchar_t * pszCtrl = m_DerivedTextCtrl->getText();

	if (pszCtrl == nullptr)
		return nullptr;

	if (isActivated() && isVisible())
	{
		if (bMustBeSame)
		{
			if (_wcsicmp(pszCtrl, pszText) == 0)
				return this;
		}
		else
		{
			if (wcsstr(pszCtrl, pszText) != nullptr)
				return this;
		}
	}

	return i3UICaptionControl::TA_ScanText( pszText, bMustBeSame, bSubControl);
}
