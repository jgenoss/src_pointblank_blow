#include "stdafx.h"
#include "UIGlobalRes.h"
#include "../i3UIAnimation.h"
using namespace UIGlobalRes;

static HPEN		s_PenTable[ COLORSET_MAX];
static HBRUSH	s_BrushTable[ COLORSET_MAX];
static HFONT	s_DefFont;
static CImageList *	s_pDefImageList_Small;
static CImageList * s_pDefImageList_Big;

static i3Texture  * s_TextureTable[UITEXTURE_COUNT];

static COLORREF		s_ColorTable[ COLORSET_MAX] =
{
	RGB(	95,		154,	214),		// COLORSET_GRIP_NORMAL_BACK = 0,		// Grip의 일반 상태 Background Color
	RGB(	19,		48,		77),		// COLORSET_GRIP_NORMAL_BORDER,			// Grip의 일반 상태 Border Color

	RGB(	214,	95,		154	),		//COLORSET_GRIP_MULTIPLE_BACK,								
	RGB(	77,		19,		48	),		//COLORSET_GRIP_MULTIPLE_BORDER,

	RGB(	218,	54,		58),		// COLORSET_GRIP_SELECT_BACK,			// Grip의 선택 상태 Background Color
	RGB(	0,		0,		0)			// COLORSET_GRIP_SELECT_BORDER,			// Grip의 선택 상태 Border Color
};

static const char * s_strTextureTable[UITEXTURE_COUNT] = 
{
	"res/SelectImage.i3i",
};

static INT32 addIcon( CImageList * pList, INT32 id)
{
	HICON hIcon	= ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE( id));

	INT32 nIndex = pList->Add( hIcon);

	::DestroyIcon( hIcon);

	return nIndex;
}

enum ICON_INDEX
{
	ICON_UISCREEN,
	ICON_UISCREEN_SEL,
	ICON_UILOCK,
	ICON_UIUNLOCK,
	ICON_UISHOW,
	ICON_UIHIDE,
	ICON_UIFILTER,
	ICON_UIFILTER_SEL,
	ICON_FRAMEWND,
	ICON_FRAMEWND_SEL,
	ICON_BTN,
	ICON_BTN_SEL,
	ICON_CHECK,
	ICON_CHECK_SEL,
	ICON_COMBO,
	ICON_COMBO_SEL,
	ICON_EDIT,
	ICON_EDIT_SEL,
	ICON_GROUPCTRL,
	ICON_GROUPCTRL_SEL,
	ICON_HSCROLLBAR,
	ICON_HSCROLLBAR_SEL,

	ICON_LISTBOX,
	ICON_LISTBOX_SEL,

	ICON_PROGRESS,
	ICON_PROGRESS_SEL,


	ICON_RADIO,
	ICON_RADIO_SEL,

	ICON_SLIDER,
	ICON_SLIDER_SEL,

	ICON_SPIN,
	ICON_SPIN_SEL,

	ICON_STATICIMAGE,
	ICON_STATICIMAGE_SEL,

	ICON_STATICTEXT,
	ICON_STATICTEXT_SEL,

	ICON_TABCTRL,
	ICON_TABCTRL_SEL,

	ICON_TREECTRL,
	ICON_TREECTRL_SEL,

	ICON_VSCROLLBAR,
	ICON_VSCROLLBAR_SEL,

	ICON_QUESTION,

	ICON_PROGRESS_IMAGE,
	ICON_PROGRESS_IMAGE_SEL,

	ICON_PROGRESS_SPLIT,
	ICON_PROGRESS_SPLIT_SEL,

	ICON_FILTER,

	ICON_COUNT,
};

void UIGlobalRes::Init(void)
{
		
 	for(INT32 i = 0; i < COLORSET_MAX; i++)
 	{
 		s_PenTable[i] = CreatePen( PS_SOLID, 1, s_ColorTable[ i]);
 		s_BrushTable[i] = CreateSolidBrush( s_ColorTable[i]);
 	}
 
 	s_DefFont = i3TDK::CreateDefFont( -11, "Tahoma", 400, CLEARTYPE_QUALITY);
 
 	//2번 루프를 돌면서 한번은 16x16, 한번은 32x32로 만든다. 
 	for( INT32 i=0; i<2; i++)
 	{
 		CImageList* pImgList = new CImageList;
 		
 		if( i == 0)
 		{
 			pImgList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);
 		}
 		else
 		{
 			pImgList->Create( 32, 32, ILC_COLOR32 | ILC_MASK, 32, 0);
 		}
 		
 		//See ICON_INDEX
 		addIcon( pImgList, IDI_ICON_UISCREEN);			// 0 ( ICON_UISCREEN)
 		addIcon( pImgList, IDI_ICON_UISCREEN_SEL);		// 1 ( ICON_UISCREEN_SEL)
 		addIcon( pImgList, IDI_ICON_UILOCK);			// 2 ( ...)
 		addIcon( pImgList, IDI_ICON_UIUNLOCK);			// 3
 		addIcon( pImgList, IDI_ICON_UISHOW);			// 4
 		addIcon( pImgList, IDI_ICON_UIHIDE);			// 5
 		addIcon( pImgList, IDI_ICON_UIFILTER);			// 6
 		addIcon( pImgList, IDI_ICON_UIFILTER_SEL);		// 7
 		addIcon( pImgList, IDI_ICON_FRAMEWND);			// 8
 		addIcon( pImgList, IDI_ICON_FRAMEWND_SEL);		// 9
 		addIcon( pImgList, IDI_ICON_BTN);				// 10
 		addIcon( pImgList, IDI_ICON_BTN_SEL);			// 11
 		addIcon( pImgList, IDI_ICON_CHECK);				// 12
 		addIcon( pImgList, IDI_ICON_CHECK_SEL);			// 13
 		addIcon( pImgList, IDI_ICON_COMBO);				// 14
 		addIcon( pImgList, IDI_ICON_COMBO_SEL);			// 15
 		addIcon( pImgList, IDI_ICON_EDIT);				// 16
 		addIcon( pImgList, IDI_ICON_EDIT_SEL);			// 17
 		addIcon( pImgList, IDI_ICON_GROUPCTRL);			// 18
 		addIcon( pImgList, IDI_ICON_GROUPCTRL_SEL);		// 19
 		addIcon( pImgList, IDI_ICON_HSCROLLBAR);		// 20
 		addIcon( pImgList, IDI_ICON_HSCROLLBAR_SEL);	// 21
 		addIcon( pImgList, IDI_ICON_LISTBOX);			// 22
 		addIcon( pImgList, IDI_ICON_LISTBOX_SEL);		// 23
 
 		addIcon( pImgList, IDI_ICON_PROGRESS);			// 24
 		addIcon( pImgList, IDI_ICON_PROGRESS_SEL);		// 25
 
 		addIcon( pImgList, IDI_ICON_RADIO);				// 26
 		addIcon( pImgList, IDI_ICON_RADIO_SEL);			// 27
 
 		addIcon( pImgList, IDI_ICON_SLIDER);			// 28
 		addIcon( pImgList, IDI_ICON_SLIDER_SEL);		// 29
 
 		addIcon( pImgList, IDI_ICON_SPIN);				// 30
 		addIcon( pImgList, IDI_ICON_SPIN_SEL);			// 31
 
 		addIcon( pImgList, IDI_ICON_STATICIMAGE);		// 32
 		addIcon( pImgList, IDI_ICON_STATICIMAGE_SEL);	// 33
 
 		addIcon( pImgList, IDI_ICON_STATICTEXT);		// 34
 		addIcon( pImgList, IDI_ICON_STATICTEXT_SEL);	// 35
 
 		addIcon( pImgList, IDI_ICON_TABCTRL);			// 36
 		addIcon( pImgList, IDI_ICON_TABCTRL_SEL);		// 37
 
 		addIcon( pImgList, IDI_ICON_TREECTRL);			// 38
 		addIcon( pImgList, IDI_ICON_TREECTRL_SEL);		// 39
 
 		addIcon( pImgList, IDI_ICON_VSCROLLBAR);		// 40
 		addIcon( pImgList, IDI_ICON_VSCROLLBAR_SEL);	// 41
 
 		HICON hIcon	=  AfxGetApp()->LoadStandardIcon( IDI_QUESTION);
 		INT32 nIndex = pImgList->Add( hIcon);			// 42
 
 		addIcon( pImgList, IDI_ICON_PROGRESS_IMAGE);			// 43
 		addIcon( pImgList, IDI_ICON_PROGRESS_IMAGE_SEL);		// 44
 
 		addIcon( pImgList, IDI_ICON_PROGRESS_SPLIT);			// 45
 		addIcon( pImgList, IDI_ICON_PROGRESS_SPLIT_SEL);		// 46
 
 
 		if( nIndex != -1)
 		{
 			pImgList->SetOverlayImage( nIndex, 1);
 		}
 
 		if( i == 0)
 		{
 			CBitmap bitmap;
 			bitmap.LoadBitmapA( IDB_TEMPLATE_EXPLORER_REPORT);
 			pImgList->Add( &bitmap, RGB(255, 255, 255));
 
 			s_pDefImageList_Small	= pImgList;
 		}
 		else
 		{
 			CBitmap bitmap;
 			bitmap.LoadBitmapA( IDB_TEMPLATE_EXPLORER_ICON);
 			pImgList->Add( &bitmap, RGB(255, 255, 255));
 
 			s_pDefImageList_Big	= pImgList;
 		}
 	}
}

void UIGlobalRes::InitAfter(void)
{
	INT32 i;
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == NULL)
		return;

	i3ResourceManager * pResMng = pFramework->GetResourceManager();

	for( i = 0; i < UITEXTURE_COUNT; ++i)
	{
#if defined( I3_ENGINE3)
		i3ResourceObject* pResObj = pResMng->LoadResource( s_strTextureTable[i]);
		
		I3ASSERT( i3::kind_of<i3Texture*>(pResObj)); //->IsTypeOf( i3Texture::static_meta()));

		s_TextureTable[i] = (i3Texture*)pResObj;

		pResObj->AddRef();
#else
		s_TextureTable[i] = pResMng->LoadTexture( s_strTextureTable[i]);	//2.0

		//일단 예외처리.. 발생 안하게 할려면..SelectImage.i3i를 UI Image폴더에 같이 넣어서 묶기..
		if (i3::generic_is_iequal(s_strTextureTable[i],"res/SelectImage.i3i") == false)
			I3ASSERT( s_TextureTable[i] != NULL);
#endif	
	}
}

void UIGlobalRes::Close(void)
{
	INT32 i;

	for( i = 0; i < COLORSET_MAX; i++)
	{
		::DeleteObject( s_PenTable[i]);
		::DeleteObject( s_BrushTable[i]);
	}

	::DeleteObject( s_DefFont);

	if( s_pDefImageList_Small != NULL)
	{
		delete s_pDefImageList_Small;
		s_pDefImageList_Small = NULL;
	}

	if( s_pDefImageList_Big != NULL)
	{
		delete s_pDefImageList_Big;
		s_pDefImageList_Big = NULL;
	}

	for( i = 0; i < UITEXTURE_COUNT; ++i)
	{
		I3_SAFE_RELEASE( s_TextureTable[i]);
	}
}

HPEN UIGlobalRes::getPen( COLORSET cs)
{
	I3_BOUNDCHK( cs, COLORSET_MAX);

	return s_PenTable[ cs];
}

HBRUSH UIGlobalRes::getBrush( COLORSET cs)
{
	I3_BOUNDCHK( cs, COLORSET_MAX);

	return s_BrushTable[ cs];
}

HFONT UIGlobalRes::getDefFont(void)
{
	return s_DefFont;
}

CImageList *	UIGlobalRes::getDefSmallImageList(void)
{
	return s_pDefImageList_Small;
}

CImageList *	UIGlobalRes::getDefBigImageList(void)
{
	return s_pDefImageList_Big;
}

i3Texture	*	UIGlobalRes::getTexture( UITEXTURE idx)
{
	return s_TextureTable[idx];
}

void UIGlobalRes::DeleteAllTexture()
{
	for(INT32 i = 0; i < UITEXTURE_COUNT; ++i)
	{
		I3_SAFE_RELEASE( s_TextureTable[i]);
	}
}

INT32 UIGlobalRes::getCategoryImageIndex( I3UI_CATEGORY c)
{
	switch(c)
	{
		case I3UI_CATEGORY_BUTTON :				return 11;
		case I3UI_CATEGORY_CHECKBOX :			return 13;
		case i3UI_CATEGORY_RADIO :				return 27;
		case I3UI_CATEGORY_EDITBOX :			return 17;
		case I3UI_CATEGORY_LISTBOX :			return 23;
		case I3UI_CATEGORY_COMBO :				return 15;
		case I3UI_CATEGORY_BUTTONCOMBO :		return 15;
		case I3UI_CATEGORY_COMBO_NOEDITBOX:		return 15;
		case I3UI_CATEGORY_SCROLL :				return 21;
		case I3UI_CATEGORY_LISTVIEWBOX :		return 23;
		case I3UI_CATEGORY_LISTVIEWITEM :		return 23;
		case I3UI_CATEGORY_TREEVIEW :			return 39;
		case I3UI_CATEGORY_STATIC_IMAGE :		return 33;
		case I3UI_CATEGORY_SLIDER :				return 29;
		case I3UI_CATEGORY_SOCKET :				return 19;
		case I3UI_CATEGORY_PROGRESS :			return 25;
		case I3UI_CATEGORY_PROGRESS_IMAGE :		return 44;
		case I3UI_CATEGORY_PROGRESS_SPLIT:		return 46;
		case I3UI_CATEGORY_FRAMEWND :			return 9;
		case I3UI_CATEGORY_SPIN :				return 31;
		case I3UI_CATEGORY_TAB :				return 37;
		case I3UI_CATEGORY_TOOLTIP:				return 33;
		case I3UI_CATEGORY_HBAR:				return 28;
		case I3UI_CATEGORY_STATIC_TEXT:			return 34;
		case I3UI_CATEGORY_CUSTOM:				return 9;
		case I3UI_CATEGORY_DOUBLETEXT:			return 34;
	}

	I3TRACE( "Invalid Category : %d\n", c);

	return 11;
}

INT32 UIGlobalRes::getImageIndex( i3ClassMeta* pMeta)
{
	INT32 idx = ICON_QUESTION;

	if( pMeta == NULL)			idx = ICON_QUESTION;

	else if( i3::kind_of<i3UIScene*>(pMeta)) //->IsTypeOf( i3UIScene::static_meta()))			
		idx = ICON_UIFILTER;
	else if( i3::same_of<i3UILTreeFilter*>(pMeta)) //->IsExactTypeOf( i3UILTreeFilter::static_meta()))	
		idx = ICON_UIFILTER;
	else if( i3::same_of<i3UITemplate_FrameWnd*>(pMeta)) //->IsExactTypeOf( i3UITemplate_FrameWnd::static_meta()))	
		idx = ICON_FRAMEWND;
	else if( i3::same_of<i3UIFrameWnd*>(pMeta)) //->IsExactTypeOf( i3UIFrameWnd::static_meta()))		
		idx = ICON_FRAMEWND;
	else if( i3::same_of<i3UITemplate_MessageBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_MessageBox::static_meta()))	
		idx = ICON_FRAMEWND;
	else if( i3::same_of<i3UIMessageBox*>(pMeta)) //->IsExactTypeOf( i3UIMessageBox::static_meta()))				
		idx = ICON_FRAMEWND;
	else if( i3::same_of<i3UITemplate_Button*>(pMeta)) //->IsExactTypeOf( i3UITemplate_Button::static_meta()))	
		idx = ICON_BTN;
	else if( i3::same_of<i3UIButton*>(pMeta)) //->IsExactTypeOf( i3UIButton::static_meta()))				
		idx = ICON_BTN;
	else if( i3::same_of<i3UITemplate_ButtonComposed3*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ButtonComposed3::static_meta()))	
		idx = ICON_BTN;
	else if( i3::same_of<i3UIButtonComposed3*>(pMeta)) //->IsExactTypeOf( i3UIButtonComposed3::static_meta()))			
		idx = ICON_BTN;
	else if( i3::same_of<i3UITemplate_ButtonImageSet*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ButtonImageSet::static_meta()))	
		idx = ICON_BTN;
	else if( i3::same_of<i3UIButtonImageSet*>(pMeta)) //->IsExactTypeOf( i3UIButtonImageSet::static_meta()))				
		idx = ICON_BTN;	
	else if( i3::same_of<i3UITemplate_EditBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_EditBox::static_meta()))	
		idx = ICON_EDIT;
	else if( i3::same_of<i3UIEditBox*>(pMeta)) //->IsExactTypeOf( i3UIEditBox::static_meta()))			
		idx = ICON_EDIT;
	else if( i3::same_of<i3UITemplate_HScrollBar*>(pMeta)) //->IsExactTypeOf( i3UITemplate_HScrollBar::static_meta()))	
		idx = ICON_HSCROLLBAR;
	else if( i3::same_of<i3UITemplate_VScrollBar*>(pMeta)) //->IsExactTypeOf( i3UITemplate_VScrollBar::static_meta()))	
		idx = ICON_VSCROLLBAR;
	else if( i3::same_of<i3UIScrollBar*>(pMeta)) //->IsExactTypeOf( i3UIScrollBar::static_meta()))				
		idx = ICON_HSCROLLBAR;
	else if( i3::same_of<i3UITemplate_CheckBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_CheckBox::static_meta()))	
		idx = ICON_CHECK;
	else if( i3::same_of<i3UICheckBox*>(pMeta)) //->IsExactTypeOf( i3UICheckBox::static_meta()))			
		idx = ICON_CHECK;
	else if( i3::same_of<i3UITemplate_ProgressBar*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ProgressBar::static_meta()))	
		idx = ICON_PROGRESS;
	else if( i3::same_of<i3UIProgressBar*>(pMeta)) //->IsExactTypeOf( i3UIProgressBar::static_meta()))			
		idx = ICON_PROGRESS;
	else if( i3::same_of<i3UITemplate_ProgressBarImage*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ProgressBarImage::static_meta()))	
		idx = ICON_PROGRESS_IMAGE;
	else if( i3::same_of<i3UIProgressBarImage*>(pMeta)) //->IsExactTypeOf( i3UIProgressBarImage::static_meta()))			
		idx = ICON_PROGRESS_IMAGE;
	else if( i3::same_of<i3UITemplate_ProgressBarSplit*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ProgressBarSplit::static_meta()))	
		idx = ICON_PROGRESS_SPLIT;
	else if( i3::same_of<i3UIProgressBarSplit*>(pMeta)) //->IsExactTypeOf( i3UIProgressBarSplit::static_meta()))			
		idx = ICON_PROGRESS_SPLIT;
	else if( i3::same_of<i3UITemplate_Radio*>(pMeta)) //->IsExactTypeOf( i3UITemplate_Radio::static_meta()))		
		idx = ICON_RADIO;
	else if( i3::same_of<i3UIRadio*>(pMeta)) //->IsExactTypeOf( i3UIRadio::static_meta()))				
		idx = ICON_RADIO;
	else if( i3::same_of<i3UITemplate_Tab*>(pMeta)) //->IsExactTypeOf( i3UITemplate_Tab::static_meta()))	
		idx = ICON_TABCTRL;
	else if( i3::same_of<i3UITab*>(pMeta)) //->IsExactTypeOf( i3UITab::static_meta()))			
		idx = ICON_TABCTRL;
	else if( i3::same_of<i3UITemplate_ToolTip*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ToolTip::static_meta()))	
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UIToolTip*>(pMeta)) //->IsExactTypeOf( i3UIToolTip::static_meta()))			
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UITemplate_SocketBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_SocketBox::static_meta()))	
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UISocketBox*>(pMeta)) //->IsExactTypeOf( i3UISocketBox::static_meta()))			
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UITemplate_ListBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ListBox::static_meta()))	
		idx = ICON_LISTBOX;
	else if( i3::same_of<i3UIListBox*>(pMeta)) //->IsExactTypeOf( i3UIListBox::static_meta()))			
		idx = ICON_LISTBOX;
	else if( i3::same_of<i3UITemplate_ButtonComboBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ComboBox::static_meta()))	
		idx = ICON_COMBO;
	else if( i3::same_of<i3UIButtonComboBox*>(pMeta)) //->IsExactTypeOf( i3UIComboBox::static_meta()))			
		idx = ICON_COMBO;
	else if( i3::same_of<i3UITemplate_ComboBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ComboBox::static_meta()))	
		idx = ICON_COMBO;
	else if( i3::same_of<i3UIComboBox*>(pMeta)) //->IsExactTypeOf( i3UIComboBox::static_meta()))			
		idx = ICON_COMBO;
	else if( i3::same_of<i3UITemplate_ComboBox_NoEditBox*>(pMeta)) 
		idx = ICON_COMBO;
	else if( i3::same_of<i3UIComboBox_NoEditBox*>(pMeta)) 
		idx = ICON_COMBO;
	else if( i3::same_of<i3UITemplate_ImageBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ImageBox::static_meta()))		
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UIImageBox*>(pMeta)) //->IsExactTypeOf( i3UIImageBox::static_meta()))				
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UITemplate_ImageBoxEx*>(pMeta)) //->IsExactTypeOf( i3UITemplate_ImageBoxEx::static_meta()))	
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UIImageBoxEx*>(pMeta)) //->IsExactTypeOf( i3UIImageBoxEx::static_meta()))				
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UITemplate_StaticText*>(pMeta)) //->IsExactTypeOf( i3UITemplate_StaticText::static_meta()))	
		idx = ICON_STATICTEXT;
	else if( i3::same_of<i3UIStaticText*>(pMeta)) //->IsExactTypeOf( i3UIStaticText::static_meta()))				
		idx = ICON_STATICTEXT;
	else if( i3::same_of<i3UITemplate_HBar*>(pMeta)) //->IsExactTypeOf( i3UITemplate_HBar::static_meta()))		
		idx = ICON_HSCROLLBAR;
	else if( i3::same_of<i3UIHBar*>(pMeta)) //->IsExactTypeOf( i3UIHBar::static_meta()))				
		idx = ICON_HSCROLLBAR;
	// ListView
	else if( i3::same_of<i3UITemplate_LVBox*>(pMeta)) //->IsExactTypeOf( i3UITemplate_LVBox::static_meta()))				
		idx = ICON_LISTBOX;
	else if( i3::same_of<i3UIListView_Box*>(pMeta)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))				
		idx = ICON_LISTBOX;
	
	else if( i3::same_of<i3UITemplate_LVItem*>(pMeta)) //->IsExactTypeOf( i3UITemplate_LVItem::static_meta()))		
		idx = ICON_BTN;
	else if( i3::same_of<i3UIListView_Item*>(pMeta)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))			
		idx = ICON_BTN;
	
	else if( i3::same_of<i3UITemplate_LVCell*>(pMeta)) //->IsExactTypeOf( i3UITemplate_LVCell::static_meta()))		
		idx = ICON_BTN;
	else if( i3::same_of<i3UIListView_Cell*>(pMeta)) //->IsExactTypeOf( i3UIListView_Cell::static_meta()))		
		idx = ICON_BTN;
	
	else if( i3::same_of<i3UITemplate_LVSep*>(pMeta)) //->IsExactTypeOf( i3UITemplate_LVSep::static_meta()))		
		idx = ICON_BTN;
	else if( i3::same_of<i3UIListView_Sep*>(pMeta)) //->IsExactTypeOf( i3UIListView_Sep::static_meta()))		
		idx = ICON_BTN;
	
	else if( i3::same_of<i3UITemplate_LVHeader*>(pMeta)) //->IsExactTypeOf( i3UITemplate_LVHeader::static_meta()))		
		idx = ICON_BTN;
	else if( i3::same_of<i3UIListView_Header*>(pMeta)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))		
		idx = ICON_BTN;
	// List View

	else if( i3::same_of<i3UITemplate_User*>(pMeta)) //->IsExactTypeOf( i3UITemplate_User::static_meta()))		
		idx = ICON_FRAMEWND;
	else if( i3::same_of<i3UIUserControl*>(pMeta)) //->IsExactTypeOf( i3UIUserControl::static_meta()))		
		idx = ICON_FRAMEWND;

	else if( i3::same_of<i3UISpin*>(pMeta)) //->IsExactTypeOf( i3UISpin::static_meta()))			
		idx = ICON_SPIN;
	else if( i3::same_of<i3UITemplate_Spin*>(pMeta)) //->IsExactTypeOf( i3UITemplate_Spin::static_meta()))	
		idx = ICON_SPIN;

	else if( i3::same_of<i3UILTreeNode_Filter*>(pMeta)) //->IsExactTypeOf( i3UILTreeNode_Filter::static_meta()))	
		idx = ICON_FILTER;

	else if( i3::same_of<i3UITemplate_3DView*>(pMeta)) //->IsExactTypeOf( i3UITemplate_3DView::static_meta()))		
		idx = ICON_STATICIMAGE;
	else if( i3::same_of<i3UI3DViewBox*>(pMeta)) //->IsExactTypeOf( i3UI3DViewBox::static_meta()))				
		idx = ICON_STATICIMAGE;

	else if( i3::same_of<i3UITemplate_DoubleText*>(pMeta)) //->IsExactTypeOf( i3UITemplate_DoubleText::static_meta()))	
		idx = ICON_STATICTEXT;
	else if( i3::same_of<i3UIDoubleText*>(pMeta)) //->IsExactTypeOf( i3UIDoubleText::static_meta()))				
		idx = ICON_STATICTEXT;
	
	return idx;
}

