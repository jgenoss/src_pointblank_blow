#include "i3FrameworkPCH.h"
#include "i3Framework.h"
#include "i3UIControl.h"
#include "i3UIManager.h"

extern "C" {
	int i3UIC_getName(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushString(L, pCtrl->GetName());
		}
		else
		{
			i3Lua::PushString( L, "");	// 상관없나?
		}

		return 1;
	}

	int i3UIC_setName(lua_State *L)
	{
		char szName[ 260];

		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			if( i3Lua::GetStringArg( L, 2, szName, sizeof( szName)) != nullptr)
			{
				pCtrl->SetName( szName);
				return 1;
			}
		}

		return 0;
	}

	int i3UIC_getClass(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3ClassMeta * pMeta = pCtrl->meta();
			I3ASSERT( pMeta != nullptr);

			i3Lua::PushString( L, pMeta->class_name().c_str());
		}

		return 1;
	}

	int i3UIC_do(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			pCtrl->OnCommand( L);
		}

		return 1;
	}

	int i3UIC_getPosition(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			REAL32 fX = pCtrl->getPos()->x;
			REAL32 fY = pCtrl->getPos()->y;
			REAL32 fZ = pCtrl->getPos()->z;
			
			fX /= i3UI::getUIResolutionWidth();
			fY /= i3UI::getUIResolutionHeight();
			
			i3Lua::PushNumber( L, fX);
			i3Lua::PushNumber( L, fY);
			i3Lua::PushNumber( L, fZ);

			return 3;
		}

		return 0;
	}

	int i3UIC_getPositionAcc(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			REAL32 fX = pCtrl->getAccumulatedPos()->x;
			REAL32 fY = pCtrl->getAccumulatedPos()->y;
			REAL32 fZ = pCtrl->getAccumulatedPos()->z;

			fX /= i3UI::getUIResolutionWidth();
			fY /= i3UI::getUIResolutionHeight();

			i3Lua::PushNumber( L, fX);
			i3Lua::PushNumber( L, fY);
			i3Lua::PushNumber( L, fZ);

			return 3;
		}

		return 0;
	}

	int i3UIC_setPosition(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			REAL32 fX = (REAL32)i3Lua::GetNumberArg(L, 2, 0.0f);
			REAL32 fY = (REAL32)i3Lua::GetNumberArg(L, 3, 0.0f);
			REAL32 fZ = (REAL32)i3Lua::GetNumberArg(L, 4, 0.0f);

			fX *= i3UI::getUIResolutionWidth();
			fY *= i3UI::getUIResolutionHeight();

			pCtrl->setPos(fX, fY, fZ);
		}

		return 0;
	}

	int i3UIC_getSize(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			REAL32 width = (REAL32)pCtrl->getWidth();
			REAL32 height = (REAL32)pCtrl->getHeight();

			width /= i3UI::getUIResolutionWidth();
			height /= i3UI::getUIResolutionHeight();

			i3Lua::PushNumber( L, width);
			i3Lua::PushNumber( L, height);

			return 2;
		}

		return 1;
	}

	int i3UIC_setSize(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			REAL32 width = (REAL32)i3Lua::GetNumberArg(L, 2, 0.0f);
			REAL32 height = (REAL32)i3Lua::GetNumberArg(L, 3, 0.0f);

			width *= i3UI::getUIResolutionWidth();
			height *= i3UI::getUIResolutionHeight();

			pCtrl->setSize( width, height);
		}
		return 0;
	}

	int i3UIC_getTemplate(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3UITemplate * pTemplate = pCtrl->getTemplate();
			if( pTemplate != nullptr)
			{
				i3Lua::PushString( L, pTemplate->GetName());
				return 1;
			}
			else
			{
				return 0;
			}
		}

		return 0;
	}

	int i3UIC_setTemplate( lua_State * L)
	{
		i3UIControl * pCtrl = (i3UIControl*) i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			char szTemplateName[ 260];
			const char * pszTemplateName = i3Lua::GetStringArg( L, 2, szTemplateName, sizeof( szTemplateName));
			if( pszTemplateName != nullptr)
			{
				if( i3::generic_string_size( pszTemplateName) > 0)
					pCtrl->SetTemplateByName( pszTemplateName);
			}
		}

		return 0;
	}

	int i3UIC_getEnable(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, (pCtrl->isEnable())? true : false);
			return 1;
		}

		return 0;
	}

	int i3UIC_setEnable(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bEnable = i3Lua::GetBooleanArg( L, 2, 0);
			if( bEnable != pCtrl->isEnable())
				pCtrl->EnableCtrl( bEnable, true);
		}

		return 0;
	}

	int i3UIC_isActivate(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, (pCtrl->isActivated())? true : false);
			return 1;
		}

		return 0;
	}

	int i3UIC_setActivate(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bActivate = i3Lua::GetBooleanArg( L, 2, 0);

			if( bActivate)
				pCtrl->removeState( I3UI_STATE_DEACTIVATED);
			else
				pCtrl->addState( I3UI_STATE_DEACTIVATED);

			pCtrl->setModifyState();
		}

		return 0;
	}

	int i3UIC_setSelected( LuaState * L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bSelected = i3Lua::GetBooleanArg( L, 2, 0);

			if( bSelected)
				pCtrl->addState( I3UI_STATE_SELECTED);
			else
				pCtrl->removeState( I3UI_STATE_SELECTED);

			pCtrl->setModifyState();			// 강제셀렉션이 즉각 효과를 발휘하도록 추가. (11.07.05.수빈)

			return 1;
		}

		return 0;
	}

	int i3UIC_isSelected(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, (pCtrl->isSelected())? true : false);

			return 1;
		}
		return 1;
	}

	int i3UIC_isVisible(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, (pCtrl->isVisible())? true : false);

			return 1;
		}

		return 1;
	}

	int i3UIC_setInputDisable( LuaState * L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bDisable = i3Lua::GetBooleanArg( L, 2, 0);

			pCtrl->setInputDisable( bDisable);

			return 1;
		}

		return 0;
	}

	int i3UIC_getShapeCount(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg(L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushInteger(L, pCtrl->getShapeCount());
			return 1;
		}

		return 0;
	}

	int i3UIC_setVisibleShape(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg(L, 1);
		if( pCtrl != nullptr)
		{
			bool bEnable	= i3Lua::GetBooleanArg(L, 2, 0);
			INT32 index		= i3Lua::GetIntegerArg(L, 3, 0);

			if( index < 0)
			{
				pCtrl->setAllShapeVisible( bEnable);
			}
			else
			{
				pCtrl->setShapeVisible( index, bEnable);
			}

			pCtrl->setModifyState();
		}

		return 0;
	}
	/*
	extern "C" int i3UIC_isFixedRatio(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
		}
		return 1;
	}

	extern "C" int i3UIC_setFixedRatio(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			pCtrl->removeStyle( I3UI_STYLE_SIZE_RATIO);
		}
		return 0;
	}
	*/
	 int i3UIC_regHandler(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			INT32 evt = i3Lua::GetIntegerArg( L, 2, 0);
			I3_BOUNDCHK(evt, I3UI_EVT_MAXCOUNT);

			INT32 enable = i3Lua::GetIntegerArg(L, 3, 0);

			pCtrl->setEvtEnable((I3UI_EVT)evt, (enable >0)?true:false);
		}

		return 0;
	}

	int i3UIC_regNotifyHandler(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			INT32 evt = i3Lua::GetIntegerArg( L, 2, 0);
			I3_BOUNDCHK(evt, I3UI_EVT_MAXCOUNT);

			INT32 enable = i3Lua::GetIntegerArg(L, 3, 0);

			pCtrl->setNotifyEnable((I3UI_EVT)evt, (enable >0)?true:false);
		}

		return 0;
	}

	int i3UIC_setToolTipMsg(lua_State *L)
	{
		char szMsg[ 512];

		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			if( i3Lua::GetStringArg( L, 2, szMsg, sizeof( szMsg), "i3UIC_setToolTipMsg") != nullptr)
			{
				INT32 nLen = i3::generic_string_size( szMsg);

				INT32 nwLen = i3String::MultiByteToWideChar( CP_UTF8, 0, szMsg, nLen, nullptr, 0);
				WCHAR16* pMsg = (WCHAR16*) i3MemAlloc( sizeof(WCHAR16) * (nwLen + 1));

				i3String::MultiByteToWideChar( CP_UTF8, 0, szMsg, nLen, pMsg, nwLen);
				pMsg[nwLen] = 0;

				pCtrl->SetToolTipMsg( pMsg);

				I3MEM_SAFE_FREE( pMsg);
			}
			else
			{
				pCtrl->SetToolTipMsg( nullptr);
			}
		}

		return 0;
	}

	int i3UIC_getToolTipMsg(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			const WCHAR16* pszMsg = pCtrl->GetToolTipMsg();

			if( pszMsg == nullptr)
				return 0;

			INT32 nwLen = i3::generic_string_size( pszMsg);

			INT32 nLen = i3String::WideCharToMultiByte( CP_UTF8, 0, pszMsg, nwLen, nullptr, 0, nullptr, nullptr);
			char* pBuf = (char*) i3MemAlloc( sizeof(char) * (nLen + 1));

			i3String::WideCharToMultiByte( CP_UTF8, 0, pszMsg, nwLen, pBuf, nLen, nullptr, nullptr);
			pBuf[nLen] = 0;

			i3Lua::PushString( L, pBuf);

			I3MEM_SAFE_FREE( pBuf);
		}

		return 1;
	}

	int i3UIC_setColor(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		
		if( pCtrl != nullptr)
		{
			if( g_pUIRenderer != nullptr)
			{
				UINT8 r = (UINT8)i3Lua::GetNumberArg( L, 2, 0);
				UINT8 g = (UINT8)i3Lua::GetNumberArg( L, 3, 0);
				UINT8 b = (UINT8)i3Lua::GetNumberArg( L, 4, 0);
				UINT8 a = (UINT8)i3Lua::GetNumberArg( L, 5, 0);

				COLOR color;
		
				i3Color::Set( &color, r, g, b, a);
				
				pCtrl->setColor( &color);
			}
		}

		return 0;
	}

	int i3UIC_getColor(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		
		if( pCtrl != nullptr)
		{
			if( g_pUIRenderer != nullptr)
			{
				COLOR* pColor = pCtrl->getColor();
				i3Lua::PushNumber( L, i3Color::GetR(pColor));
				i3Lua::PushNumber( L, i3Color::GetG(pColor));
				i3Lua::PushNumber( L, i3Color::GetB(pColor));
				i3Lua::PushNumber( L, i3Color::GetA(pColor));

				return 4;
			}
		}

		return 0;
	}


	int i3UIC_getMousePosition(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			i3UIManager* pMgr = i3Framework::getCurrentFramework()->getUIManager();
			if( pMgr != nullptr) {
				POINT point = pMgr->getCurrentMousePosition();
				REAL32 fX = point.x - pCtrl->getAccumulatedPos()->x;
				REAL32 fY = point.y - pCtrl->getAccumulatedPos()->y;

				fX /= i3UI::getUIResolutionWidth();
				fY /= i3UI::getUIResolutionHeight();

				i3Lua::PushNumber( L, fX);
				i3Lua::PushNumber( L, fY);
				return 2;
			}
		}

		return 0;
	}

	int i3UIC_getMousePositionAcc(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			i3UIManager* pMgr = i3Framework::getCurrentFramework()->getUIManager();
			if( pMgr != nullptr) {
				POINT point = pMgr->getCurrentMousePosition();
				REAL32 fX = (REAL32)point.x;
				REAL32 fY = (REAL32)point.y;

				fX /= i3UI::getUIResolutionWidth();
				fY /= i3UI::getUIResolutionHeight();

				i3Lua::PushNumber( L, fX);
				i3Lua::PushNumber( L, fY);
				return 2;
			}
		}

		return 0;
	}

	int i3UIC_setMousePosition(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			i3UIManager* pMgr = i3Framework::getCurrentFramework()->getUIManager();
			if( pMgr != nullptr) {
	//			POINT point = pMgr->getCurrentMousePosition();
				REAL32 fX = (REAL32)i3Lua::GetNumberArg( L, 2, 0);
				REAL32 fY = (REAL32)i3Lua::GetNumberArg( L, 3, 0);
		
				// 받아온 좌표가 Control의 상대좌표니까 절대좌표로 바꾸기위해 Control의 누적좌표를 더함
				fX += pCtrl->getAccumulatedPos()->x;
				fY += pCtrl->getAccumulatedPos()->y;

				fX *= i3UI::getUIResolutionWidth();
				fY *= i3UI::getUIResolutionHeight();

				i3InputDeviceManager* pManager = i3Framework::getCurrentFramework()->GetViewer()->GetInputDeviceManager();
				i3InputMouse* pMouse = pManager->GetMouse();
				pMouse->SetMousePos( fX, fY, 0.0f);
				pManager->Update();
			}
		}

		return 0;
	}

	int i3UIC_setMousePositionAcc(lua_State *L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			i3UIManager* pMgr = i3Framework::getCurrentFramework()->getUIManager();
			if( pMgr != nullptr) {
	//			POINT point = pMgr->getCurrentMousePosition();
				REAL32 fX = (REAL32)i3Lua::GetNumberArg( L, 2, 0);
				REAL32 fY = (REAL32)i3Lua::GetNumberArg( L, 3, 0);

				fX *= i3UI::getUIResolutionWidth();
				fY *= i3UI::getUIResolutionHeight();

				i3InputDeviceManager* pManager = i3Framework::getCurrentFramework()->GetViewer()->GetInputDeviceManager();
				i3InputMouse* pMouse = pManager->GetMouse();
				pMouse->SetMousePos( fX, fY, 0.0f);
				pManager->Update();
			}
		}

		return 0;
	}

	int i3UIC_setFocus( lua_State* L)
	{
		i3UIControl* pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			pCtrl->OnSetFocus();
		}
		return 0;
	}

	int i3UIC_killFocus( lua_State* L)
	{
		i3UIControl* pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			pCtrl->OnKillFocus();
		}
		return 0;
	}

	int i3UIC_getChild( LuaState * L)
	{
		i3UIControl * pCtrl = (i3UIControl*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			INT32 idx = i3Lua::GetIntegerArg( L, 2);

			if( idx < pCtrl->GetChildCount())
			{
				i3Lua::PushUserData( L, pCtrl->GetChildByIndex( idx));
				return 1;
			}
		}

		return 0;
	}

	int i3UIC_setVisible( LuaState * L)
	{
		i3UIControl * pCtrl = (i3UIControl*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			pCtrl->SetVisible( i3Lua::GetBooleanArg( L, 2) ? true : false);
		}

		return 0;
	}

	// i3UIButton 용.. TFT4차에 자주 사용되는 기능이 없어서 추가합니다.
	// 버튼이 눌려진 상태에 사용됩니다. gyungju.kim 14.11.18
	int i3UIC_setPushFlag(LuaState* L)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bSelected = i3Lua::GetBooleanArg( L, 2, 0);

			if( bSelected)
				pCtrl->addState( I3UI_STATE_PUSHED);
			else
				pCtrl->removeState( I3UI_STATE_PUSHED);

			pCtrl->setModifyState();			

			return 1;
		}

		return 0;
	}
}

LuaFuncReg i3UIControlGlue[] = 
{
	{"uiCtrl_RegEventHandler",	i3UIC_regHandler		},
	{"uiCtrl_RegNotifyHandler",	i3UIC_regNotifyHandler	},
	{"uiCtrl_getName",			i3UIC_getName			},
	{"uiCtrl_setName",			i3UIC_setName			},	// 이름은 tool에서만 변경하도록 해야 할듯.
	{"uiCtrl_getClass",			i3UIC_getClass			},
	{"uiCtrl_OnCommand",		i3UIC_do				},
	
	{"uiCtrl_getPosition",		i3UIC_getPosition		},
	{"uiCtrl_getPositionAcc",	i3UIC_getPositionAcc	},
	{"uiCtrl_setPosition",		i3UIC_setPosition		},
	{"uiCtrl_getSize",			i3UIC_getSize			},
	{"uiCtrl_setSize",			i3UIC_setSize			},
	{"uiCtrl_getTemplate",		i3UIC_getTemplate		},
	{"uiCtrl_setTemplate",		i3UIC_setTemplate		},
	{"uiCtrl_getEnable",		i3UIC_getEnable			},
	{"uiCtrl_setEnable",		i3UIC_setEnable			},
	{"uiCtrl_isActivate",		i3UIC_isActivate		},
	{"uiCtrl_setActivate",		i3UIC_setActivate		},
	{"uiCtrl_setSelected",		i3UIC_setSelected		},
	{"uiCtrl_isSelected",		i3UIC_isSelected		},
	{"uiCtrl_isVisible",		i3UIC_isVisible			},
	{"uiCtrl_setInputDisable",	i3UIC_setInputDisable	},

	{"uiCtrl_getShapeCount",	i3UIC_getShapeCount		},
	{"uiCtrl_setVisibleShape",	i3UIC_setVisibleShape	},

	{"uiCtrl_setToolTipMsg",	i3UIC_setToolTipMsg		},
	{"uiCtrl_getToolTipMsg",	i3UIC_getToolTipMsg		},

	{	"uiCtrl_getColor",		i3UIC_getColor			},
	{	"uiCtrl_setColor",		i3UIC_setColor			},

	{"uiCtrl_getMousePosition",		i3UIC_getMousePosition		},
	{"uiCtrl_getMousePositionAcc",	i3UIC_getMousePositionAcc	},
	{"uiCtrl_setMousePosition",		i3UIC_setMousePosition		},
	{"uiCtrl_setMousePositionAcc",	i3UIC_setMousePositionAcc	},

	{"uiCtrl_setFocus",				i3UIC_setFocus		},
	{"uiCtrl_killFocus",			i3UIC_killFocus		},

	{"uiCtrl_getChild",				i3UIC_getChild		},
	{"uiCtrl_setVisible",			i3UIC_setVisible	},
	{"uiCtrl_setPushFlag",			i3UIC_setPushFlag	},

	{nullptr, nullptr}
};

void i3UIControl::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIControlGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIControlGlue[i].name, i3UIControlGlue[i].func);
	}
}

