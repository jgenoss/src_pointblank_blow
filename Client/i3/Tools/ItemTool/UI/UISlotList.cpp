#include "StdAfx.h"
#include "UISlotList.h"
#include "..\MainFrm.h"
#include "..\DlgFileLoader.h"
#include "..\FileData.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/utf16_to_mb.h"

const UINT32	UIWidthWeapon			= 204;
const UINT32	UIHeightWeapon			= 57;
const INT32		UIWidthItem				= 90;
const INT32		UIHeightItem			= 65;
const INT32		UIWidthChara			= 135;
const INT32		UIHeightChara			= 78;

const UINT32	UICountPerPageWeapon	= 88;	// 한페이지 당 갯수

const UINT32	UICountPerLineWeapon	= 5;	// 한줄 당 갯수
const INT32		UICountPerLineItem		= 5;

const UINT32	UICountPerLineChara		= 3;
const UINT32	UICountLineChara		= 6;
const UINT32	UICountPerPageChara		= UICountPerLineChara * UICountLineChara;

const REAL32	SlotInterval			= 5.0f;
const REAL32	RateSlotImage			= 0.8f;


I3_CLASS_INSTANCE(cUISlotList);//, i3GameNode)

cUISlotList::cUISlotList(void)
: m_PosX(0.f), m_PosY(0.f)
, m_Width(100.f), m_Height(100.f)
, m_SlotWidth(100.f), m_SlotHeight(100.f)
, m_RateX(1.f), m_RateY(1.f)
, m_Row(8), m_ColPage(4)
, m_StartIndex(0)
, m_SelectIndex(-1)
, m_pBgTexture(NULL)
{
}

cUISlotList::~cUISlotList(void)
{
	_Destroy();
}


bool cUISlotList::Create(i3Node* pSceneNode)
{
	if( pSceneNode == NULL )
		return false;


	for( INT32 i = 0; i < SLOT_COUNT_MAX; i++ )
	{
		m_Slot[i].Create(pSceneNode);
	}

	i3ResourceManager * pResMng = g_pMainSCFramework->GetResourceManager();

	CString strBackImgPath = g_strCurrentDir + "\\res\\white.bmp";
	m_pBgTexture = pResMng->LoadTexture( strBackImgPath );

	return true;
}

void cUISlotList::_Destroy(void)
{
	for(INT32 i = 0; i < SLOT_COUNT_MAX; i++ )
	{
		m_Slot[i].Destroy();
	}

	for(UINT32 i = 0; i < UI_TEX_MAX; i++)
	{
		for(UINT32 j = 0; j < m_vTexture[i].size(); j++)
		{
			I3_SAFE_RELEASE( m_vTexture[i][j] );
		}
	}

	I3_SAFE_RELEASE(m_pBgTexture);
}


void cUISlotList::SetSize(INT32 nX, INT32 nY, UINT32 nWidth, UINT32 nHeight)
{
	m_PosX		= static_cast<REAL32>(nX);
	m_PosY		= static_cast<REAL32>(nY);
	m_Width		= static_cast<REAL32>(nWidth);
	m_Height	= static_cast<REAL32>(nHeight);


 //	RECT	rcSrc	= g_pViewer->GetRenderContext()->GetSourcePresentRect();
 //	RECT	rcDest	= g_pViewer->GetRenderContext()->GetDestPresentRect();

	//float fCheckSlotW = m_Width / m_Row;
	//float fCheckSlotH = m_Height / m_ColPage;

	m_Row = (INT32)(m_Width / 150.0f);
	m_ColPage = (INT32)(m_Height / 140.0f);
// 
 	//ResetItemList(0);

	
	//m_RateX	= static_cast<REAL32>( rcSrc.right - rcSrc.left ) / ( rcDest.right - rcDest.left );
	//m_RateY	= static_cast<REAL32>( rcSrc.bottom - rcSrc.top ) / ( rcDest.bottom - rcDest.top );
	
	for (int i = 0; i < SLOT_COUNT_MAX; ++i)
	{
		m_Slot[i].SetVisible(FALSE);
	}
	

	REAL32	SlotWidth; 
	REAL32	SlotHeight;
	REAL32	ImageHeight;
	REAL32	SpriteWidth;
	REAL32	SpriteHeight;

	_CalcUISize(SlotWidth, SlotHeight, SpriteWidth, SpriteHeight, ImageHeight);
	

	REAL32	PosY	= m_PosY;
	for(INT32 i = 0; i < m_ColPage; i++)
	{
		REAL32	PosX	= m_PosX; 


		for(INT32 j = 0; j < m_Row; j++)
		{
			m_Slot[(m_Row * i) + j].SetRect(PosX, PosY, SlotWidth, SlotHeight, 
				ImageHeight, SpriteWidth, SpriteHeight);

			PosX	+= SlotWidth + SlotInterval;
		}
		PosY	+= SlotHeight + SlotInterval;
	}
	ResetItemList(m_StartIndex);
}

void cUISlotList::ResizeUI( void )
{
	REAL32	SlotWidth;
	REAL32	SlotHeight;
	REAL32	ImageHeight;
	REAL32	SpriteWidth;
	REAL32	SpriteHeight;

	_CalcUISize(SlotWidth, SlotHeight, SpriteWidth, SpriteHeight, ImageHeight);


	REAL32	PosY	= m_PosY;
	for(INT32 i = 0; i < m_ColPage; i++)
	{
		REAL32	PosX	= m_PosX; 

		for(INT32 j = 0; j < m_Row; j++)
		{
			m_Slot[(m_Row * i) + j].SetRectImage(PosX, PosY, SlotWidth, ImageHeight, 
				SpriteWidth, SpriteHeight);

			PosX	+= SlotWidth + SlotInterval;

			//I3TRACE("COL:%d ROW:%d\n", i, j);
		}
		PosY	+= SlotHeight + SlotInterval;
	}
}

void cUISlotList::_CalcUISize( REAL32& SlotWidth, REAL32& SlotHeight, REAL32& SpriteWidth, REAL32& SpriteHeight, REAL32& ImageHeight )
{
	m_SlotWidth		= (REAL32)(g_pViewer->GetMaxW() / m_Row);
	m_SlotHeight	= (REAL32)(g_pViewer->GetMaxH() / m_ColPage);

	SlotWidth		= m_SlotWidth - SlotInterval;
	SlotHeight		= m_SlotHeight - SlotInterval;
	ImageHeight		= SlotHeight * RateSlotImage;

	REAL32	UIRateBasic		= SlotWidth > 0.0f ? ImageHeight / SlotWidth : 1.0f;
	REAL32	UIRateWidth		= 1.0f;
	REAL32	UIRateHeight	= 1.0f;

	if( g_pPefList )
	{
		REAL32	AdjustWidth		= SlotWidth;
		REAL32	AdjustHeight	= ImageHeight;

		switch( g_pPefList->GetCurrentType() )
		{
		case E_PEF_WEAPON:		
			AdjustWidth		= UIWidthWeapon;
			AdjustHeight	= UIHeightWeapon;
			break;
		case E_PEF_GOODS:
			AdjustWidth		= UIWidthItem;
			AdjustHeight	= UIHeightItem;
			break;
		case E_PEF_EQUIPMENT:
			AdjustWidth		= UIWidthChara;
			AdjustHeight	= UIHeightChara;
			break;
		case E_PEF_CHARA:
			AdjustWidth		= UIWidthChara;
			AdjustHeight	= UIHeightChara;
			break;
		}

		REAL32	UIRate		= AdjustHeight / AdjustWidth;

		if( UIRate < UIRateBasic )
			UIRateHeight	= UIRate / UIRateBasic;
		else
			UIRateWidth		= UIRateBasic / UIRate;
	}

	UIRateHeight	*= RateSlotImage;

	SpriteWidth		= SlotWidth * UIRateWidth;
	SpriteHeight	= SlotHeight * UIRateHeight;

	m_SlotWidth = m_Width / m_Row;
	m_SlotHeight= m_Height / m_ColPage;
}



bool cUISlotList::SetUITexture( UI_TEXTURE_TYPE textype, UINT32 nIndex, i3Texture* pTex )
{
	if( pTex == NULL || textype < 0 || textype >= UI_TEX_MAX )
		return false;

	vTexture&	vTex	= m_vTexture[textype];

	if( vTex.size() <= nIndex )
		vTex.resize(nIndex+1);

	if (vTex[nIndex] != NULL)
		I3_SAFE_RELEASE(vTex[nIndex]);
	
	vTex[nIndex]  = pTex;

	return true;
}


void cUISlotList::ResetItemList( INT32 nStartIndex )
{
	if (!g_pPefList) 
		return;

	ePefType	type	= g_pPefList->GetCurrentType();

	if ( E_PEF_NULL == type) 
		return;

	_SetSelectedSlot(false);

	m_StartIndex	= nStartIndex;

	_SetSelectedSlot(true);


	

	int nTypeCnt = (int)UI_TEX_TYPE_BERET;
	for(INT32 i = 0; i < m_ColPage; i++)
	{
		for(INT32 j = 0; j < m_Row; j++)
		{
			UINT32	nItemIndex	= (m_Row * (i + nStartIndex)) + j;
			UINT32	nSlotIndex	= (m_Row * i) + j;

			const char*	pName	= g_pPefList->GetItemNodeName(nItemIndex);
			
			cUIItemSlot& kSlot = m_Slot[nSlotIndex];
			kSlot.SetText(pName);


			i3Texture*	pTexture	= NULL;
			CRect		rcTex;
			bool		rv			= false;

			switch( type )
			{
				case E_PEF_WEAPON:
					rv	= _UITexWeapon( nItemIndex, pTexture, rcTex );
					break;
				case E_PEF_GOODS:
					rv	= _UITexGoods( nItemIndex, pTexture, rcTex );
					break;
				case E_PEF_EQUIPMENT:
					rv	= _UITexEquip( nItemIndex, pTexture, rcTex, nTypeCnt );
					break;
				case E_PEF_CHARA:
					rv	= _UITexChara( nItemIndex, pTexture, rcTex );
					break;
			}

			if( rv )
				kSlot.SetTexture(pTexture, rcTex);
			else
				kSlot.SetDisableUI();

			CRect rt;
			rt.left = 0;
			rt.top = 0;
			rt.right = m_pBgTexture->GetWidth();
			rt.bottom = m_pBgTexture->GetHeight();
			kSlot.SetBgTexture(m_pBgTexture,rt);
		}
	}
}


// 현재 보이는 화면은 렌더 타겟에 출력한후 윈도우 크기에 맞춰 축소된 값이므로
// 그 비율을 계산한 뒤 좌표를 보정해서 인덱스를 계산한다.
void cUISlotList::SelectSlot( const POINT& point )
{
	if( g_pPefList == NULL || NULL == g_pPefList->GetCurrKeyList())
		return;

	REAL32	rPointX	= point.x * m_RateX;
	REAL32	rPointY	= point.y * m_RateY;

	if( rPointX < m_PosX || rPointY < m_PosY || rPointX > m_PosX + m_Width || rPointY > m_PosY + m_Height )
		return;

	rPointX		-= m_PosX;
	rPointY		-= m_PosY;

	INT32	IndexX	= static_cast<INT32>(rPointX / m_SlotWidth);
	INT32	IndexY	= static_cast<INT32>(rPointY / m_SlotHeight);
	REAL32	Left	= m_SlotWidth * IndexX;
	REAL32	Top		= m_SlotHeight * IndexY;
	REAL32	Right	= Left + m_SlotWidth - SlotInterval;
	REAL32	Bottom	= Top + m_SlotHeight - SlotInterval;

	if( rPointX < Left || rPointY < Top || rPointX > Right || rPointY > Bottom )
		return;


	INT32	nItemIndex	= ( m_StartIndex + IndexY ) * m_Row + IndexX;
	if( m_SelectIndex == nItemIndex )
		return;

	_SetSelectedSlot(false);

	m_SelectIndex	= nItemIndex;

	_SetSelectedSlot(true);

	g_pPefList->SetNodePropertyEditor(nItemIndex);

	if (nItemIndex >= g_pPefList->GetItemCount())
		return;

	const char* szName = g_pPefList->GetItemKey(nItemIndex)->GetName();

	I3_TDK_UPDATE_INFO Data;
	memset(&Data,0,sizeof(I3_TDK_UPDATE_INFO));
	Data.m_Event = I3_TDK_UPDATE_SELECT;

	::SendMessage(g_HwndDockPropertyWnd, WM_TDK_UPDATE, (WPARAM)&Data, (LPARAM)szName);

	i3::rc_wstring wstr;
	i3::string	str;

	if( g_pPefList->GetCurrentType() == E_PEF_WEAPON )
	{
		Data.m_Event = I3_TDK_UPDATE_SELECT;

		char szFolderName[MAX_PATH];
		g_pPefList->GetItemResName(nItemIndex,szFolderName,MAX_PATH);


		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		cDlgFileLoader* pLoader = pFrame->GetFileLoader();
		if (pLoader->IsLoaded())
		{
			i3::to_lower(szFolderName);

			//weapon 다이얼로그 로딩
			std::string strCharaEditorName = pLoader->GetFileDir(szFolderName);
			if (strCharaEditorName.empty())
				return;

			::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)strCharaEditorName.c_str());

			//메인 렌더타겟 뷰어
			int nPos = strCharaEditorName.rfind(".");
			strCharaEditorName = strCharaEditorName.substr(0, nPos);
			CString szI3sPath = g_strRootPath + "\\" + strCharaEditorName.c_str() + ".i3s";

			g_pMainSCFramework->LoadFile(szI3sPath,"i3s");
		}
	}
	else if (g_pPefList->GetCurrentType() == E_PEF_EQUIPMENT)
	{
		char szResLocalPath[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "ResLocalPath", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szResLocalPath, str);

		char szResPostName[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "ResPostName", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szResPostName, str);


		char szDiffuse[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "DiffuseTexture", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szDiffuse, str);

		char szNormal[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "NormalTexture", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szNormal, str);

		char szSpecular[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "SpecularTexture", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szSpecular, str);

		if( i3::generic_string_size( szResLocalPath) > 0)
		{
			CString stri3Path = g_strRootPath + "\\Equip\\" + szResLocalPath;

			if (g_pPefList->GetCategory(nItemIndex) == "Beret")
			{
				stri3Path += "\\Bella_Beret.I3S";
				g_pMainSCFramework->LoadFile(stri3Path, "i3s");
				g_pMainSCFramework->SetBeretTexture(	szDiffuse, szNormal, szSpecular, false,
												"beret.i3i","beret_NM.i3i", "beret_SP.i3i");
			}
			else if (g_pPefList->GetCategory(nItemIndex) == "Event")
			{
				stri3Path = stri3Path + "\\Bella_" + szResPostName + ".i3s";
				g_pMainSCFramework->LoadFile(stri3Path, "i3s");
				g_pMainSCFramework->SetBeretTexture(szDiffuse, szNormal, szSpecular, true	);
			}	
			else if (g_pPefList->GetCategory(nItemIndex) == "Head")
			{
				stri3Path = stri3Path + "\\Bella_" + szResPostName + ".i3s";
				g_pMainSCFramework->LoadFile(stri3Path, "i3s");
				g_pMainSCFramework->SetBeretTexture(szDiffuse, szNormal, szSpecular, true	);
			}
			else if (g_pPefList->GetCategory(nItemIndex) == "Mask")
			{
				stri3Path = stri3Path + "\\Bella_Mask_" + szResPostName + ".i3s";
				g_pMainSCFramework->LoadFile(stri3Path, "i3s");
				g_pMainSCFramework->SetBeretTexture(szDiffuse, szNormal, szSpecular, true	);
			}
		}
		else
		{	// v2 버전용
			i3RegKey * pParent = (i3RegKey*)g_pPefList->GetCurrentKey(nItemIndex)->getParent();
			if( pParent != NULL)
			{
				CString stri3Path = g_strRootPath + "\\Avatar\\" + pParent->GetName();
				stri3Path = stri3Path + "\\" + szResPostName + "\\" + szResPostName + ".i3s";
					g_pMainSCFramework->LoadFile(stri3Path, "i3s");
			}
		}
	}
	else if (g_pPefList->GetCurrentType() == E_PEF_CHARA)
	{
		char szResLocalPath[256] = "";
		FIND_REG_DATA(g_pPefList->GetCurrentKey(nItemIndex), "_ResPath", wstr);
		i3::utf16_to_mb( wstr, str);
		i3::generic_string_copy( szResLocalPath, str);
		CString stri3sPath = g_strRootPath + "\\" + szResLocalPath;
		int nPos = stri3sPath.ReverseFind('.');
		stri3sPath = stri3sPath.Mid(0, nPos+1);
		stri3sPath += "i3s";

		g_pMainSCFramework->LoadFile(stri3sPath, "i3s");

	}

	//클릭시엔 카메라 리셋을 하지 않는다.
	g_pItemSCFramework->SetResetCamera(false);

// 	mAIDriveCon::iterator iter = g_mAIDrive.begin();
// 
// 	for (; iter != g_mAIDrive.end(); ++iter)
// 	{
// 		I3TRACE(iter->first);
// 		for (unsigned int k = 0; k < iter->second.size(); ++k)
// 		{
// 			I3TRACE(iter->second[k]);
// 			I3TRACE("\n");
// 		}
// 		I3TRACE("");
// 	}
}

void cUISlotList::_SetSelectedSlot(bool bSelected)
{
	INT32	Index		= m_SelectIndex - (m_StartIndex * m_Row);
	if( Index < 0 )
		return;

	INT32	SlotCount	= m_ColPage * m_Row;
	if( Index > SlotCount )
		return;

	m_Slot[Index].SetBgColor(bSelected);
}



INT32 cUISlotList::GetScrollCount( void ) const
{
	INT32	nLine	= ( g_pPefList->GetItemCount() + m_Row - 1 ) / m_Row;

	if( nLine > m_ColPage )
		return nLine - m_ColPage;

	return 0;
}

INT32 cUISlotList::GetColPage( void ) const
{
	return m_ColPage;
}


// BOOL CEquipShapeManager::SetWeaponSelectMark(i3GuiStatic* pStatic,UINT8 nWeaponClass, UINT8 nWeaponNum) 참조
bool cUISlotList::_UITexWeapon( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex )
{
	if( m_vTexture[UI_TEX_TYPE_WEAPON].size() <= 0 )
		return false;

	INT32	nUIIndex	= g_pPefList->GetItemUIIndex( nItemIndex );
	if( nUIIndex < 0 )
		return false;

	UINT32	nTexIndex	= nUIIndex / UICountPerPageWeapon;
	if( nTexIndex >= m_vTexture[UI_TEX_TYPE_WEAPON].size() )
		return false;

	pTexture			= m_vTexture[UI_TEX_TYPE_WEAPON][nTexIndex];

	INT32	nTexOrd		= nUIIndex % UICountPerPageWeapon;

	INT32	nU			= UIWidthWeapon * (nTexOrd % UICountPerLineWeapon);
	INT32	nV			= UIHeightWeapon * (nTexOrd / UICountPerLineWeapon);

	rcTex.SetRect( nU, nV, nU + UIWidthWeapon, nV + UIHeightWeapon );

	return true;
}


// BOOL CEquipShapeManager::SetCashItemShape(i3GuiStatic* pStatic, INT32 ItemID) 참조
bool cUISlotList::_UITexGoods( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex )
{
	INT32	ItemType	= -1;
	INT32	TexIndex	= -1;
	INT32	ShapeIndex	= -1;

	if( g_pPefList->GetItemUIIndex( nItemIndex, ItemType, TexIndex, ShapeIndex ) == false )
		return false;

	if( ItemType < 0 || TexIndex < 0 || ShapeIndex < 0 )
		return false;


	enum CASHITEM_USAGE_TYPE
	{
		CASHITEM_USAGE_UNKNOWN		= -1,
		CASHITEM_USAGE_ALL			= CASHITEM_USAGE_UNKNOWN,
		CASHITEM_USAGE_MAINTENANCE	= 0,	//	지속성 아이템
		CASHITEM_USAGE_EXPENDABLES	= 1,	//	소모성 아이템

		CASHITEM_USAGE_COUNT
	};

	UI_TEXTURE_TYPE		TexType;

	switch(ItemType)
	{
	case CASHITEM_USAGE_MAINTENANCE:	TexType	= UI_TEX_TYPE_ITEM_PERIOD;	break;
	case CASHITEM_USAGE_EXPENDABLES:	TexType	= UI_TEX_TYPE_ITEM_COUNT;	break;
	default:
		return false;
	}
	if( m_vTexture[TexType].size() <= (UINT32)TexIndex )
		return false;

	pTexture	= m_vTexture[TexType][TexIndex];

	INT32	u	= (1 + (ShapeIndex % UICountPerLineItem) * (UIWidthItem + 1));
	INT32	v	= (1 + (ShapeIndex / UICountPerLineItem) * (UIHeightItem + 1));

	rcTex.SetRect( u, v, u + UIWidthItem, v + UIHeightItem);

	return true;
}

bool cUISlotList::_UITexEquip( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex, int nType )
{
	UI_TEXTURE_TYPE eTextureType;

	std::string strCategory = g_pPefList->GetCategory(nItemIndex);
	if (strCategory == "Beret")
	{
		eTextureType = UI_TEX_TYPE_BERET;
	}
	else if (strCategory == "Event")
	{
		eTextureType = UI_TEX_TYPE_HEAD;
	}
	else if (strCategory == "Head")
	{
		eTextureType = UI_TEX_TYPE_HEAD;
	}
	else if (strCategory == "Mask")
	{
		eTextureType = UI_TEX_TYPE_MASK;
	}
	else
	{
		return false;
	}

	
	if (eTextureType > UI_TEX_TYPE_MASK)
		return true;

	if( m_vTexture[eTextureType].size() <= 0 )
		return false;

	INT32	ShapeIndex	= -1;

	g_pPefList->GetItemIndex( nItemIndex, "UIShapeIndex", ShapeIndex );
	if( ShapeIndex < 0 )
		return false;

	UINT32	TexIndex = ShapeIndex / UICountPerPageChara;
	
	//I3TRACE("%d %d\n", eTextureType, TexIndex);

	if( m_vTexture[eTextureType].size() < TexIndex )
		return false;

	pTexture	= m_vTexture[eTextureType][TexIndex];

	INT32	TexOrd	= ShapeIndex % UICountPerPageChara;

	INT32	u	= (1 + (TexOrd % UICountPerLineChara) * (UIWidthChara + 1));
	INT32	v	= (1 + (TexOrd / UICountPerLineChara) * (UIHeightChara + 1));

	rcTex.SetRect( u, v, u + UIWidthChara, v + UIHeightChara );
	
	return true;
}

// BOOL CEquipShapeManager::SetCharacterShape(i3GuiStatic* pStatic, INT32 ItemID)
bool cUISlotList::_UITexChara( INT32 nItemIndex, i3Texture*& pTexture, CRect& rcTex )
{
	if( m_vTexture[UI_TEX_TYPE_CHARA].size() <= 0 )
		return false;

	INT32	ShapeIndex	= -1;

	g_pPefList->GetItemIndex( nItemIndex, "UIShapeIndex", ShapeIndex );
	if( ShapeIndex < 0 )
		return false;

	UINT32	TexIndex = ShapeIndex / UICountPerPageChara;

	if( m_vTexture[UI_TEX_TYPE_CHARA].size() < TexIndex )
		return false;

	pTexture	= m_vTexture[UI_TEX_TYPE_CHARA][TexIndex];

	INT32	TexOrd	= ShapeIndex % UICountPerPageChara;

	INT32	u	= (1 + (TexOrd % UICountPerLineChara) * (UIWidthChara + 1));
	INT32	v	= (1 + (TexOrd / UICountPerLineChara) * (UIHeightChara + 1));

	rcTex.SetRect( u, v, u + UIWidthChara, v + UIHeightChara );

	return true;
}
