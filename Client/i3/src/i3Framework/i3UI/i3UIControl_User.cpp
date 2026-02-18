#include "i3FrameworkPCH.h"
#include "i3Framework.h"
#include "i3UIControl.h"
#include "i3UIManager.h"
#include "i3UITemplate.h"
#include "i3UIRenderer.h"
#include "i3UIControl_User.h"
#include "i3UIListView_Cell.h"


I3_CLASS_INSTANCE(i3UIUserControl);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3UIUserControl::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	INT32 i;
	INT32 nSubCount;

	// I3_GAMENODE_STYLE_VOLATILE 가 있는 애들만 지운다.		RemoveAllControl(); X
	i3::vector<i3UIControl*> list;
	GetChildControls( list, false);

	for( size_t i=0; i< list.size(); i++)
	{
		i3UIControl * pChild = list[i];

		if( pChild->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE)
		{
			RemoveControl( pChild);
		}
	}
	
	
	if( m_pTemplate != nullptr)
	{
		i3::pack::RECT rect;
		nSubCount = m_pTemplate->getSubTemplateCount();

		for( i = 0;i < nSubCount; ++i)
		{
			i3UITemplate * pTemplate = (i3UITemplate*)m_pTemplate->getSubTemplate( i);

			i3UIControl * pCtrl = (i3UIControl*)pTemplate->GetInstanceMeta()->create_instance();
			I3ASSERT( pCtrl != nullptr && !i3::same_of<i3UIControl*>(pCtrl));

			pCtrl->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
			pCtrl->addStyle( I3UI_STYLE_SLAVE);

			pTemplate->GetControlRect(&rect);
			pCtrl->setSize( rect.right - rect.left, rect.bottom - rect.top);

			pCtrl->SetTemplate( pTemplate);

			AddControl( pCtrl);
		}
	}
}

bool i3UIUserControl::CreateBaseSceneObjects()
{
	i3UIControl::CreateBaseSceneObjects();

	return true;
}

void i3UIUserControl::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDblClk( nFlag, point);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Cell* >(pParent))
	{
		i3UIListView_Cell* pCell = (i3UIListView_Cell*)pParent;
		pCell->OnLButtonDblClk( nFlag, point);
	}
}

void i3UIUserControl::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDown( nFlag, point);

	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);

	addState( I3UI_STATE_NEED_UPDATE);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Cell*>( pParent) )
	{
		ParentNotify( I3UI_EVT_SELITEM, point.x, point.y);
	}
}

void i3UIUserControl::_UpdateShapeState(void)	// 현재 state에 따른 visible state를 update한다.
{
	i3UIControl::_UpdateShapeState();

	if( m_pAncestor != nullptr && m_pAncestor->getLuaContext() != nullptr)
	{
		i3UI::Notify(this, I3UI_EVT_UPDATE_STATE, 0, 0);
	}
	
	INT32 i;

	if( i3UI::getToolDevMode())
	{
		for( i = 0;i < m_nShapeCount; ++i)
		{
			setShapeVisible( i, true);
		}
	}
	else
	{
		if( m_CurShape == -1 )
		{
			for( i = 0;i < m_nShapeCount; ++i)
			{
				setShapeVisible( i, false);
			}
		}
		else
		{
			for( i = 0;i < m_nShapeCount; ++i)
			{
				if( isValidShape( i))
					setShapeVisible( i, (i == (INT32)m_CurShape));	//C4389
			}
		}
	}
}

void i3UIUserControl::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3UIControl::OnBuildObjectList( List);
}

UINT32 i3UIUserControl::OnSave( i3ResourceFile * pResFile)
{
	//UINT32 Rc;	// unreferenced local variable
	UINT32 Result = 0;
	
	Result = i3UIControl::OnSave( pResFile);
	I3_CHKIO( Result);

	return Result;
}

UINT32 i3UIUserControl::OnLoad( i3ResourceFile * pResFile)
{
	//UINT32 Rc;	// unreferenced local variable
	UINT32 Result = 0;

	Result = i3UIControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	return Result;
}

bool i3UIUserControl::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3UIUserControl::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	return bResult;
}

void i3UIUserControl::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	//I3ASSERT( i3::kind_of<i3UIUserControl*>(pDest));
}

void i3UIUserControl::OnInitAfterLoad()
{
	i3UIControl::OnInitAfterLoad();
}

void i3UIUserControl::SetCurrentShape( INT32 idx)
{
	if( m_CurShape != idx)
	{
		setShapeVisible( idx, true);

		m_CurShape = idx;
		addState( I3UI_STATE_NEED_UPDATE);
	}
}

extern "C" {
	int i3UIUserControl_SetCurrentShape( LuaState * L)
	{
		i3UIUserControl * pCtrl = (i3UIUserControl*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			INT32 idx = i3Lua::GetIntegerArg( L, 2);
			pCtrl->SetCurrentShape( idx);
		}
		return 0;
	}
}

LuaFuncReg i3UIUserControl_Glue[] = {
	{"uiUser_SetCurrentShape",		i3UIUserControl_SetCurrentShape},
	{nullptr,							nullptr}
};

void i3UIUserControl::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIUserControl_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIUserControl_Glue[i].name, i3UIUserControl_Glue[i].func);
	}
}

