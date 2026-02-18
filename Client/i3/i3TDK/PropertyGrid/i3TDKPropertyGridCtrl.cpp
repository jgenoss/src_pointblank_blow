#include "stdafx.h"
#include "i3TDKPropertyGridCtrl.h"
#include "i3TDKPropertyGridProperty_Bool.h"
#include "i3TDKPropertyGridProperty_UINT32_BitFlag.h"
#include "i3TDKPropertyGridProperty_Int32.h"
#include "i3TDKPropertyGridProperty_Real32.h"
#include "i3TDKPropertyGridProperty_Vec3D.h"
#include "i3TDKPropertyGridProperty_Vec4D.h"
#include "i3TDKPropertyGridProperty_Matrix.h"
#include "i3TDKPropertyGridProperty_String.h"
#include "i3TDKPropertyGridProperty_StringW.h"
#include "i3TDKPropertyGridProperty_Group.h"
#include "i3TDKPropertyGridProperty_Image.h"
#include "i3TDKPropertyGridProperty_Combo.h"
#include "i3TDKPropertyGridProperty_File.h"
#include "i3TDKPropertyGridProperty_Button.h"

#include "i3TDKPropertyGridProperty_ImageGroup.h"
#include "i3TDKPropertyGridProperty_Empty.h"

#include "i3TDKPropertyGridProperty_Color.h"
#include "i3TDKPropertyGridProperty_ColorPicker.h"
#include "i3TDKPropertyGridProperty_Font.h"

#include "../../include/i3Framework/i3UI/i3UICaptionControl.h"

#include "../i3TDK.h"

#include "i3Base/i3AnyData.h"

#include "i3Base/string/compare/generic_is_iequal.h"

#if defined( I3_WINDOWS)

//notify를 받을 object의 info
struct OBJ_INFO
{
	i3ElementBase*	m_pObj = nullptr;
	i3::vector<PROPERTYINFO*>	m_Props;
};


IMPLEMENT_DYNAMIC( i3TDKPropertyGridCtrl, CMFCPropertyGridCtrl)

i3TDKPropertyGridCtrl::~i3TDKPropertyGridCtrl()
{
	// m_pBase로 설정되는 것들 중에는 메모리 할당된 것이 아니라, Class Member로 정의되어
	// 초기 RefCount가 0인 것들이 있다. 이런 종류의 객체들은 Ref Counting을 하면 문제가 발생한다.
	//I3_SAFE_RELEASE( m_pBase);
}

BEGIN_MESSAGE_MAP(i3TDKPropertyGridCtrl, CMFCPropertyGridCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL i3TDKPropertyGridCtrl::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	m_bCreated = CMFCPropertyGridCtrl::Create( dwStyle, rect, pParentWnd, nID);

	return m_bCreated;
}


CMFCPropertyGridProperty* i3TDKPropertyGridCtrl::_GetProperty( PROPERTYINFO* pInfo)
{
	CMFCPropertyGridProperty* pProp = nullptr;

	// 실제 Property가 만들어지는 부분. 
	// Property들은 생성자에서 많은 일을 하므로 생성자가 가장 중요.
	switch( pInfo->m_eType)
	{
	case EPT_VOID:		break;
	case EPT_BOOLEAN:	pProp = new i3TDKPropertyGridProperty_Bool( pInfo);				break;
	case EPT_UINT32_BITFLAG: pProp = new i3TDKPropertyGridProperty_UINT32_BitFlag(pInfo);	break;
	case EPT_INT32:		pProp = new i3TDKPropertyGridProperty_Int32( pInfo);				break;
	case EPT_REAL32:	pProp = new i3TDKPropertyGridProperty_Real32( pInfo);				break;
	case EPT_VEC3D:		pProp = new i3TDKPropertyGridProperty_Vec3D( pInfo);				break;
	case EPT_VEC4D:		pProp = new i3TDKPropertyGridProperty_Vec4D( pInfo);				break;
	case EPT_MATRIX:	pProp = new i3TDKPropertyGridProperty_Matrix( pInfo);				break;
	case EPT_STRING:	pProp = new i3TDKPropertyGridProperty_String( pInfo);				break;
	case EPT_STRINGW:	pProp = new i3TDKPropertyGridProperty_StringW( pInfo);				break;
	case EPT_GROUP:		pProp = new	i3TDKPropertyGridProperty_Group( pInfo);				break;
	case EPT_FILE:		pProp = new i3TDKPropertyGridProperty_File( pInfo);					break;
	case EPT_COMBO:		pProp = new i3TDKPropertyGridProperty_Combo( pInfo);				break;
	case EPT_BUTTON:	pProp = new i3TDKPropertyGridProperty_Button( pInfo);				break;
	case EPT_IMAGE:		pProp = new i3TDKPropertyGridProperty_ImageGroup( pInfo, this);		break;
	case EPT_COLOR:		pProp = new i3TDKPropertyGridProperty_ColorPicker( pInfo);				break;
	case EPT_FONT:		pProp = new i3TDKPropertyGridProperty_Font( pInfo);					break;
	case EPT_RCSTRING:	pProp = new i3TDKPropertyGridProperty_RcString(pInfo);				break;
	case EPT_RCSTRINGW:	pProp = new i3TDKPropertyGridProperty_RcStringW(pInfo);				break;
	case EPT_FILE_RCSTRING:	pProp = new i3TDKPropertyGridProperty_File_RcString(pInfo);		break;
	default:			ASSERT(0);															break;
	}

	return pProp;
}

void i3TDKPropertyGridCtrl::ClearProps( void)
{
	RemoveAll();
	AdjustLayout();

	// m_pBase로 설정되는 것들 중에는 메모리 할당된 것이 아니라, Class Member로 정의되어
	// 초기 RefCount가 0인 것들이 있다. 이런 종류의 객체들은 Ref Counting을 하면 문제가 발생한다.
	m_pBase = nullptr;
	//I3_SAFE_RELEASE( m_pBase);
}

// 어떤 노드를 클릭하였을 때 프로퍼티들을 추가하는 부분
void i3TDKPropertyGridCtrl::AddProp( i3ElementBase* pBase)
{
	if( pBase == nullptr)
		return;

	i3::vector<PROPERTYINFO*> List;

	// 중요!!
	
	// m_pBase로 설정되는 것들 중에는 메모리 할당된 것이 아니라, Class Member로 정의되어
	// 초기 RefCount가 0인 것들이 있다. 이런 종류의 객체들은 Ref Counting을 하면 문제가 발생한다.
	m_pBase = pBase;
	// I3_REF_CHANGE( m_pBase, pBase);

	m_pBase->GetTDKGridProperties( List);

	// AddProperty를 먼저 하고 하위 항목들을 AddSubItem으로 추가하면 안됨.
	// Group의 하위 항목들을 모두 AddSubItem으로 추가하고 나서 
	// 그 그룹을 AddProperty로 추가해야한다. 
	for( size_t i=0; i< List.size(); i++)
	{
		PROPERTYINFO* pInfo = List[i]; 

		EPROPERTYTYPE eType = pInfo->m_eType;
		
		CMFCPropertyGridProperty* pProp = _GetProperty( pInfo);

		//if(i3String::Compare(pProp->GetName(), "Template") == 0)
		//{
		//	char strTemp[128];
		//	sprintf_s(strTemp, "%s", (const char *)(pProp->GetData()));
		//	// 없는 Template 찾는 코드
		//	if(i3String::Compare(strTemp, "(null)") == 0)
		//		I3PRINTLOG(I3LOG_NOTICE, "템플릿 라이브러리 삭제됨(텍스쳐 or 라이브러리) : %s", ((i3UIControl*)m_pBase)->getTemplate()->GetName());
		//}
		

		//Begin Group
		if( pProp != nullptr && eType == EPT_GROUP)		
		{
			if( m_pCurProp != nullptr)
				m_pCurProp->AddSubItem( pProp);

			m_pCurProp = pProp;
						
			I3MEM_SAFE_FREE( pInfo);
			continue;
		}

		//End Group
		if( eType == EPT_VOID && m_pCurProp != nullptr)
		{
			CMFCPropertyGridProperty* pParent = m_pCurProp->GetParent();

			if( pParent == nullptr)
				AddProperty( m_pCurProp);
			
			m_pCurProp = m_pCurProp->GetParent();
			
			I3MEM_SAFE_FREE( pInfo);
			continue;
		}

		// Otherwise..
		if( pProp != nullptr)
		{
			if( m_pCurProp == nullptr)
				AddProperty( pProp);
			else
				m_pCurProp->AddSubItem( pProp);		
		}

		I3MEM_SAFE_FREE( pInfo);
	}

	AdjustLayout();

}

// Property 값이 변경되었을 때 처리하는 부분 !!
void i3TDKPropertyGridCtrl::OnPropertyChanged(  CMFCPropertyGridProperty* pProp ) const
{
	if( pProp == nullptr)
		return;

	char szName[MAX_PATH] = {0,};

//  
#ifdef _UNICODE
	i3::utf16_to_mb(pProp->GetName(), szName, MAX_PATH);	
#else
	i3::safe_string_copy( szName, pProp->GetName(), MAX_PATH);
#endif
	
	// RemoveAll이 호출되고 나서 OnPropertyChanged가 호출되면 Crash 
	// : Property를 수정한 뒤 Scene-Graph의 다른 노드를 클릭하는 경우
	// 이런 경우 그냥 return
	if( GetPropertyCount() == 0)	
		return;						
	
	// File, Color Property는 i3TDKPropertyGridProperty에서 파생되지 않으므로 따로 처리
	// (i3TDKPropertyGridProperty_File::)OnClickButton 에서 처리해 주고 있음.
	
	//Runtime type check
	CRuntimeClass* prt = pProp->GetRuntimeClass();
	const char* pszClassname = prt->m_lpszClassName;

	const size_t nNotifyCount = m_NotifyList.size();
	if( nNotifyCount == 0)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_BEFORE_PROP_CHANGE, m_pBase);
	}
	else if( nNotifyCount > 0)
	{

		i3AnyData	a =  i3::vector<i3ElementBase*>() ;

		i3::vector<i3ElementBase*>& list = AnyDataCast<i3::vector<i3ElementBase*>&>(a);

		list.push_back( m_pBase);
		
		for( size_t i=0; i< nNotifyCount; i++)
		{
			OBJ_INFO* pObjInfo = m_NotifyList[i];
			i3ElementBase* pBase = pObjInfo->m_pObj;

			list.push_back( pBase);
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_BEFORE_PROP_CHANGE, &a);
	}
		
	if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_File") )
	{
		i3TDKPropertyGridProperty_File* pFileProp = (i3TDKPropertyGridProperty_File*)pProp;
		pFileProp->UpdateFromProperty();


		// Notify to other objects
		for( size_t i=0; i< m_NotifyList.size(); i++)
		{
			OBJ_INFO* pObjInfo = m_NotifyList[i];

			i3ElementBase* pBase = pObjInfo->m_pObj;

			if( pBase->same_of( m_pBase->meta()))
			{
				i3::vector<PROPERTYINFO*>& List = pObjInfo->m_Props;

				for( size_t j=0; j< List.size() ; j++)
				{
					PROPERTYINFO* pPropInfo = List[j];

					if( pPropInfo->m_pszName != nullptr && 
						i3::generic_is_iequal( pPropInfo->m_pszName, szName) )
					{
						pFileProp->UpdateFromProperty( pPropInfo->m_pData);
						pBase->OnTDKPropertyChanged( szName);

						break;
					}
				}
			}
		}
	}
	else if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Color"))
	{
		i3TDKPropertyGridProperty_Color* pColorProp = (i3TDKPropertyGridProperty_Color*)pProp;
		pColorProp->UpdateFromProperty();


		// Notify to other objects
		for( size_t i=0; i< m_NotifyList.size(); i++)
		{
			OBJ_INFO* pObjInfo = m_NotifyList[i];

			i3ElementBase* pBase = pObjInfo->m_pObj;

			if( pBase->same_of( m_pBase->meta()))
			{
				i3::vector<PROPERTYINFO*>& List = pObjInfo->m_Props;

				for( size_t j=0; j< List.size(); j++)
				{
					PROPERTYINFO* pPropInfo = List[j];

					if( pPropInfo->m_pszName != nullptr && 
						i3::generic_is_iequal( pPropInfo->m_pszName, szName) )
					{
						pColorProp->UpdateFromProperty( pPropInfo->m_pData);
						pBase->OnTDKPropertyChanged( szName);

						break;
					}
				}
			}
		}
	}
	else if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Font") )
	{
		i3TDKPropertyGridProperty_Font* pFontProp = (i3TDKPropertyGridProperty_Font*)pProp;
		pFontProp->UpdateFromProperty();


		// Notify to other objects
		for( size_t i=0; i< m_NotifyList.size(); i++)
		{
			OBJ_INFO* pObjInfo = m_NotifyList[i];

			i3ElementBase* pBase = pObjInfo->m_pObj;

			if( pBase->same_of( m_pBase->meta()))
			{
				i3::vector<PROPERTYINFO*>& List = pObjInfo->m_Props;

				for( size_t j=0; j< List.size(); j++)
				{
					PROPERTYINFO* pPropInfo = List[j];

					if( pPropInfo->m_pszName != nullptr && i3::generic_is_iequal( pPropInfo->m_pszName, szName) )
					{
						pFontProp->UpdateFromProperty( pPropInfo->m_pData, pPropInfo->m_pUserData);
						pBase->OnTDKPropertyChanged( szName);

						break;
					}
				}
			}
		}
	}
	else
	{
		i3TDKPropertyGridProperty* pProperty = (i3TDKPropertyGridProperty*)pProp;	//dynamic_cast<i3TDKPropertyGridProperty*>(pProp);
		pProperty->UpdateFromProperty();		// Data
		
		// Notify to other objects
		for( size_t i=0; i< m_NotifyList.size(); i++)
		{
			OBJ_INFO* pObjInfo = m_NotifyList[i];

			i3ElementBase* pBase = pObjInfo->m_pObj;

			if( pBase->same_of( m_pBase->meta()))
			{
				i3::vector<PROPERTYINFO*>& List = pObjInfo->m_Props;

				for( size_t j=0; j< List.size(); j++)
				{
					PROPERTYINFO* pPropInfo = List[j];

					if( pPropInfo->m_pszName != nullptr && 
						i3::generic_is_iequal( pPropInfo->m_pszName, szName) )
					{
						pProperty->UpdateFromProperty( pPropInfo->m_pData);
						pBase->OnTDKPropertyChanged( szName);

						break;
					}
				}
			}
		}

		m_pBase->OnTDKPropertyChanged( szName);	// Notify
	
		//i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, m_pBase);	// Select again

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBase);

		return;
	}

	m_pBase->OnTDKPropertyChanged( szName);	// Notify
	
	//i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, m_pBase);	// Select again

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pBase);
}

void i3TDKPropertyGridCtrl::AddDrawProp( i3TDKPropertyGridProperty* pProp)
{

	m_DrawProps.push_back( pProp);

}

void i3TDKPropertyGridCtrl::RemoveDrawProp( i3TDKPropertyGridProperty* pProp)
{

	i3::vu::remove(m_DrawProps, pProp);

}

void i3TDKPropertyGridCtrl::AddRect( CRect* pRect)
{
	m_Rects.push_back( pRect);
}

void i3TDKPropertyGridCtrl::RemoveRect( CRect* pRect)
{

	i3::vu::remove(m_Rects,  pRect);

}

void i3TDKPropertyGridCtrl::AddImageViewer( i3TDKPropertyGridImageViewer* pImageViewer)
{

	m_ImageViewers.push_back( pImageViewer);

}

void i3TDKPropertyGridCtrl::RemoveImageViewer( i3TDKPropertyGridImageViewer* pImageViewer)
{

	i3::vu::remove(m_ImageViewers, pImageViewer);

}

void i3TDKPropertyGridCtrl::AddImageProp( i3TDKPropertyGridProperty_Image*	pImageProp)
{

	m_ImageProps.push_back( pImageProp);

}

void i3TDKPropertyGridCtrl::RemoveImageProp( i3TDKPropertyGridProperty_Image*	pImageProp)
{

	i3::vu::remove(m_ImageProps, pImageProp);

}

void i3TDKPropertyGridCtrl::OnPaint()
{
	CMFCPropertyGridCtrl::OnPaint();

	// StretchBlt로 그려야하는 Property들
	for( size_t i=0; i< m_DrawProps.size(); i++)
	{
		i3TDKPropertyGridProperty* pProp = m_DrawProps[i];
		pProp->DrawImage();
	}
}

void i3TDKPropertyGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//I3TRACE("%d, %d\n", point.x, point.y);


	//for( INT32 i=0; i< m_pImageProps->GetCount(); i++)
	//{
	//	//CRect* pRt = (CRect*)m_pRects->GetItem(i);
	//	i3TDKPropertyGridProperty_Image* pImageProp = (i3TDKPropertyGridProperty_Image*)m_pImageProps->GetItem( i);
	//	CRect rt = pImageProp->GetRect();
	//	
	//	i3TDKPropertyGridImageViewer* pImageViewer = pImageProp->GetImageViewer();
	//	I3ASSERT( pImageViewer != nullptr);

	//	INT32 x = point.x;
	//	INT32 y = point.y;

	//	//I3TRACE( "%d, %d, %d, %d\n", pRt->left, pRt->right, pRt->top, pRt->bottom);
	//	//I3TRACE( "%d, %d\n",x, y);

	//	if( x > rt.left && x < rt.right && y > rt.top && y < rt.bottom)
	//	{
	//		m_bDraw = true;
	//		SetTimer( 0, 300, nullptr);
	//		//OnPaint();
	//		//pImageViewer->SetShowWindow( true);
	//		//I3TRACE("Show\n");
	//	}
	//	else
	//	{
	//		
	//		if( m_bDraw == true)
	//		{
	//			KillTimer( 0);
	//			m_bDraw = false;
	//			m_bShow = false;
	//			AdjustLayout();
	//		}

	//		//OnPaint();
	//		//pImageViewer->SetShowWindow( false);
	//		//I3TRACE("Hide\n");
	//	}
	//}

	CMFCPropertyGridCtrl::OnMouseMove(nFlags, point);
}

void i3TDKPropertyGridCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	
	/*if( !m_bShow)
	{
		OnPaint();
		I3TRACE("OnTimer\n");
		m_bShow = true;
	}*/

	CMFCPropertyGridCtrl::OnTimer(nIDEvent);
}

void i3TDKPropertyGridCtrl::_UpdateToProperties( CMFCPropertyGridProperty* pProp)
{
	if( pProp == nullptr)
		return;

	INT32 nChildCount = pProp->GetSubItemsCount();

	CRuntimeClass* prt = pProp->GetRuntimeClass();
	const char* pszClassname = prt->m_lpszClassName;
		
	if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_File") )
	{
		i3TDKPropertyGridProperty_File* pFileProp = (i3TDKPropertyGridProperty_File*)pProp;
		
		pFileProp->UpdateToProperty();
	}
	else if(i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Color") )
	{
		i3TDKPropertyGridProperty_Color* pColorProp = (i3TDKPropertyGridProperty_Color*)pProp;
		
		pColorProp->UpdateToProperty();
	}
	else if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Font") )
	{
		i3TDKPropertyGridProperty_Font* pFontProp = (i3TDKPropertyGridProperty_Font*)pProp;
		
		pFontProp->UpdateToProperty();
	}
	else
	{
		if( pProp->IsKindOf( RUNTIME_CLASS( i3TDKPropertyGridProperty)))
		{
			i3TDKPropertyGridProperty* pProperty = (i3TDKPropertyGridProperty*)pProp;

			pProperty->UpdateToProperty();
		}
	}
	

	for( INT32 i=0; i<nChildCount; i++)
	{
		_UpdateToProperties( pProp->GetSubItem(i));
	}
}

void i3TDKPropertyGridCtrl::UpdateToProperties( void)
{
	for( INT32 i=0; i<GetPropertyCount(); i++)
	{
		CMFCPropertyGridProperty* pProp = GetProperty(i);
		_UpdateToProperties( pProp);
	}
}

i3TDKPropertyGridProperty* i3TDKPropertyGridCtrl::_GetPropertyByName( CMFCPropertyGridProperty* pProp, const char* pszName)
{
	if( pProp == nullptr)
		return nullptr;

	if( i3::generic_is_iequal( pProp->GetName(), pszName) )
	{
		CRuntimeClass* prt = pProp->GetRuntimeClass();
		const char* pszClassname = prt->m_lpszClassName;

		if( i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Color") == false && 
			i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_File") == false &&
			i3::generic_is_iequal( pszClassname, "i3TDKPropertyGridProperty_Font") == false)
		{
			i3TDKPropertyGridProperty* pProperty = (i3TDKPropertyGridProperty*)pProp;
			return pProperty;
		}
	}
	else
	{
		INT32 nChildCount = pProp->GetSubItemsCount();

		for( INT32 i=0; i< nChildCount; i++)
		{
			CMFCPropertyGridProperty* pSubProp = pProp->GetSubItem(i);
			i3TDKPropertyGridProperty* pProperty = _GetPropertyByName( pSubProp, pszName);
			if( pProperty != nullptr)
				return pProperty;
		}
	}

	return nullptr;
}

i3TDKPropertyGridProperty* i3TDKPropertyGridCtrl::GetPropertyByName( const char* pszName)
{
	for( INT32 i=0; i<GetPropertyCount(); i++)
	{
		CMFCPropertyGridProperty* pProp = GetProperty(i);
		
		i3TDKPropertyGridProperty* pProperty = _GetPropertyByName( pProp, pszName);
		if( pProperty != nullptr)
			return pProperty;
	}

	return nullptr;
}

void i3TDKPropertyGridCtrl::ProcessMessages( const i3::vector<PROPERTYMSG*>& msgList)
{
	for( size_t i=0; i< msgList.size(); i++)
	{
		PROPERTYMSG* pMsg = msgList[i];
		
		i3TDKPropertyGridProperty* pProperty = GetPropertyByName( pMsg->propertyName);
		if( pProperty != nullptr)
		{
			_ProcessMessage( pProperty, pMsg->msg);
		}

		// delete
		I3MEM_SAFE_FREE( pMsg);
	}	
}

bool i3TDKPropertyGridCtrl::_ProcessMessage( i3TDKPropertyGridProperty* pProperty, EI3TDK_PROPERTY_MSG msg)
{
	I3ASSERT( pProperty != nullptr)
	{
		switch( msg)
		{
		case EI3TDK_PROPERTY_MSG_ENABLE:
			pProperty->Enable(TRUE);
			return true;
		case EI3TDK_PROPERTY_MSG_DISABLE:
			pProperty->Enable(FALSE);
			return true;
		default:
			return false;
		}
	}
}

void i3TDKPropertyGridCtrl::addToNotifyList( i3ElementBase* pObj)
{
	if( pObj == nullptr)
		return;

	//중복해서 넣지 않는다.
	for( size_t i=0; i< m_NotifyList.size(); i++)
	{
		OBJ_INFO* pInfo = m_NotifyList[i];
		
		if( pInfo->m_pObj == pObj)
			return;
	}

	//memory alloc(dealloc은 clearNotifyList에서 책임진다.)
	OBJ_INFO* pInfo = new OBJ_INFO;
		//(OBJ_INFO*)i3MemAlloc( sizeof( OBJ_INFO));			// C++ 클래스나 구조체에 대해서는 특별한 이유가 없으면 malloc을 쓰지 말것...
	pInfo->m_pObj = pObj;

//	List* pList = List::new_object();
	i3::vector<PROPERTYINFO*>& List = pInfo->m_Props;
	pObj->GetTDKGridProperties(List);

	m_NotifyList.push_back( pInfo);
}

void i3TDKPropertyGridCtrl::removeFromNotifyList( i3ElementBase* pObj)
{
	for( size_t i=0; i< m_NotifyList.size(); i++)
	{
		OBJ_INFO* pInfo = m_NotifyList[i];

		if( pInfo->m_pObj == pObj)
		{
			i3::vector<PROPERTYINFO*>& List = pInfo->m_Props;
			for( size_t j=0; j < List.size(); j++)
			{
				PROPERTYINFO* pInfo2 = List[j];
				I3MEM_SAFE_FREE( pInfo2);
			}
			List.clear();
			
			delete pInfo;
//			i3MemFree( pInfo);

			m_NotifyList.erase( m_NotifyList.begin() + i);

			return;
		}
	}

	//I3ASSERT( 0);	
}

void i3TDKPropertyGridCtrl::clearNotifyList( void)
{
	for( size_t i=0; i< m_NotifyList.size(); i++)
	{
		OBJ_INFO* pInfo = m_NotifyList[i];

		{
			i3::vector<PROPERTYINFO*>& List = pInfo->m_Props;

			for( size_t j=0; j < List.size(); j++)
			{
				PROPERTYINFO* pInfo2 = List[j];
				I3MEM_SAFE_FREE( pInfo2);
			}
			List.clear();
		}
		delete pInfo;
//		i3MemFree( pInfo);
	}

	m_NotifyList.clear();
}

void i3TDKPropertyGridCtrl::getNotifyObjList(i3::vector<i3ElementBase*>& Out)
{
	for( size_t i=0; i< m_NotifyList.size(); i++)
	{
		OBJ_INFO* pInfo = m_NotifyList[i];
		Out.push_back( pInfo->m_pObj);	//i3ElementBase*
	}
}

bool i3TDKPropertyGridCtrl::notifyListContainsSameObjTypes( void)
{
	for( size_t i=0; i< m_NotifyList.size(); i++)
	{
		OBJ_INFO* pInfo = m_NotifyList[i];
		if( pInfo->m_pObj != m_pBase)
			return false;
	}

	return true;
}

void i3TDKPropertyGridCtrl::OnClickButton(CPoint point)
{
	CMFCPropertyGridCtrl::OnClickButton( point);

	Invalidate();
}

BOOL i3TDKPropertyGridCtrl::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && m_pSel != nullptr)
	{
		// Tab누르면 NextProperti가 선택되게 변경
		CPoint point(m_pSel->GetRect().right - 1, m_pSel->GetRect().bottom + 2);

		CMFCPropertyGridProperty* pHit = HitTest(point, nullptr, TRUE);
		if (pHit != nullptr)
		{
			SetCurSel(pHit);
			EnsureVisible(pHit);

			if (pHit->IsGroup())
			{
				return true;
			}
		}
		else
		{
			SetCurSel(m_bAlphabeticMode ? m_lstTerminalProps.GetHead() : m_lstProps.GetHead());
			OnVScroll(SB_TOP, 0, nullptr);
			return true;
		}
	}

	return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
}

#endif