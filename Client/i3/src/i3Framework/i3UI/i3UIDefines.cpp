#include "i3FrameworkPCH.h"
#include "i3UIDefines.h"
#include "i3UITemplate.h"
#include "i3UIManager.h"

#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Framework.h"
#include "i3UIFilePathMgr.h"
#include "i3UIAnimation2DSprite.h"

I3_CLASS_INSTANCE(i3UIShape);


///////////////////////////////////////////////////////////////////////////////////////////////////////

void i3UIShape::enableStyle( bool bEnable, UINT32 nStyle)
{
	if( bEnable)
		addStyle( nStyle);
	else
		removeStyle( nStyle);
}

void i3UIShape::setTextureResPath(const char * pszPath) 
{ 
	i3::stack_string strUniqueName;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszPath, strUniqueName);
	m_strTexFile = strUniqueName;
}


#if defined( I3_DEBUG)
void i3UIShape::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	// 먼저 Base Class의 GetTDKGridProperties를 호출
	i3NamedElement::GetTDKGridProperties( List);

	// BeginGroup은 EndGroup과 짝을 맞추어야 합니다.
	AddMFCProperty_BeginGroup( List, "i3UIShape");

	AddMFCProperty_File_RcString( List, m_strTexFile, "Texture");
	AddMFCProperty_Color( List, &m_ColorKey, "ColorKey", "Color Key for UV Detection");

	AddMFCProperty_Real32( List, &m_Rect.left, "Left", nullptr, 0.0f, 0.0f, false);
	AddMFCProperty_Real32( List, &m_Rect.top, "Top", nullptr, 0.0f, 0.0f, false);
	AddMFCProperty_Real32( List, &m_Rect.right, "Width", nullptr, 0.0f, 0.0f, false);
	AddMFCProperty_Real32( List, &m_Rect.bottom, "Height", nullptr, 0.0f, 0.0f, false);
	
	AddMFCProperty_Real32( List, &m_TexCoord.left, "Texcoord U");
	AddMFCProperty_Real32( List, &m_TexCoord.top, "Texcoord V");
	AddMFCProperty_Real32( List, &m_TexCoord.right, "Texcoord Width");
	AddMFCProperty_Real32( List, &m_TexCoord.bottom, "Texcoord Height");

	AddMFCProperty_Int32(List, &m_TextAlignOffsetX, "TextAlignOffsetX");
	AddMFCProperty_Int32(List, &m_TextAlignOffsetY, "TextAlignOffsetY");


	m_AnimationList.clear();
	m_nAnimationIndex = -1;

	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if (pFramework)
	{
		m_AnimationList.push_back("None");
		i3UIAnimationMgr* pAnimation = pFramework->getUIAnimationMgr();
		if (pAnimation)
		{
			for (INT32 i = 0; i < pAnimation->GetAnimationCount(); i++)
			{
				m_AnimationList.push_back(pAnimation->GetAnimation(i)->GetName());
				if (i3::generic_compare(pAnimation->GetAnimation(i)->GetName(), m_strAnimationName) == 0)
				{
					m_nAnimationIndex = i+1;
				}
			}
			AddMFCProperty_Combo(List, m_AnimationList, (INT32*)&m_nAnimationIndex, "Sprite Animation");
		}
	}

	AddMFCProperty_Bool(List, &m_bAnimationRoof, "Sprite Roof");

	AddMFCProperty_EndGroup( List);
}

void i3UIShape::OnTDKPropertyChanged( const char* szName)
{
	i3NamedElement::OnTDKPropertyChanged( szName);

	if (i3::generic_is_iequal(szName, "Sprite Animation"))
	{
		if (m_nAnimationIndex > -1)
		{
			const char* pszTemplateName = m_AnimationList[m_nAnimationIndex].c_str();
			SetAnimationName(pszTemplateName);
		}
	}
}

#endif	// I3_DEBUG

void i3UIShape::OnBuildObjectList(i3::vector<i3PersistantElement*>& List )
{
	//Texture는 TextureList에서 저장하므로 OnSave()리스트에 넣지 않습니다.
	i3NamedElement::OnBuildObjectList( List );
}

void i3UIShape::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3UIShape * pShape = (i3UIShape*)pDest;

	pShape->setStyle( getStyle());
	pShape->setRect( &m_Rect);
	pShape->SetTextureCoord( &m_TexCoord);
	pShape->setTexcoordOffset( &m_VTexOffset);

	pShape->setTextureResPath( getTextureResPath());
	pShape->setColorKey( getColorKey());
	pShape->setRotation( getRotation());
	pShape->SetAnimationName(GetAnimationName());
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UI_SHAPE
	{
		UINT8		m_ID[4] = { 'U', 'I', 'S', '1' };
		UINT32		m_nStyle = 0;

		UINT8		m_szTexFile[MAX_PATH] = { 0 };		// 저장 용도이므로 문자열컨테이너로 변경하면 안된다. (2012.11.20.수빈)		
		UINT8		m_strAnimationName[MAX_PATH] = { 0 };
		COLOR		m_ColorKey;

		i3::pack::RECT		m_Rect;
		i3::pack::RECT		m_TexCoord;
		i3::pack::POINT2D	m_VTexOffset;
		REAL32		m_RotationRad = 0.0f;

		INT32		m_TextAlignOffsetX = 0;
		INT32		m_TextAlignOffsetY = 0;

		INT8		Reserved[24] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UIShape::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::UI_SHAPE data;

	Result = i3NamedElement::OnSave( pResFile );
	I3_CHKIO( Result);
	
	if( i3UI::getToolDevMode() )
	{
		if (!m_strTexFile.empty())
		{
			i3::stack_string strOldStyleFormatName;
			i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(m_strTexFile, strOldStyleFormatName);
			i3::safe_string_copy((char*)data.m_szTexFile, strOldStyleFormatName, MAX_PATH);		// Texture의 원본 파일명 입니다.
		}
	}

	i3::safe_string_copy((char*)data.m_strAnimationName, m_strAnimationName, MAX_PATH);

	i3Color::Copy( &data.m_ColorKey, &m_ColorKey);							// 원본 파일에서 UV검출을 위한 Color Key

	data.m_Rect				= m_Rect;
	data.m_TexCoord			= m_TexCoord;

	i3UILibrary* pUILib = i3UI::getUILibrary();

	if (pUILib && pUILib->IsNewTemplateLibraryFormat())
	{
		data.m_VTexOffset.x = 0;	data.m_VTexOffset.y = 0;
	}
	else
	{
		data.m_VTexOffset = m_VTexOffset;
	}

	data.m_RotationRad		= m_RotationRad;
	data.m_TextAlignOffsetX	= m_TextAlignOffsetX;
	data.m_TextAlignOffsetY	= m_TextAlignOffsetY;

	Rc = pStream->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIShape::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	pack::UI_SHAPE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data ));
	I3_CHKIO( Result);
	Result += Rc;

	if( i3::generic_is_niequal( (const char*)data.m_ID, "UIS1", 4) == false) 
	{
		I3PRINTLOG(I3LOG_WARN,  "Not a valid I3UIShape");
		return STREAM_ERR;
	}

	m_Rect				= data.m_Rect;
	m_TexCoord			= data.m_TexCoord;

	i3UILibrary* pUILib = i3UI::getUILibrary();

	if (pUILib && pUILib->IsNewTemplateLibraryFormat())
	{
		m_VTexOffset.x = 0;	m_VTexOffset.y = 0;
	}
	else
	{
		m_VTexOffset = data.m_VTexOffset;
	}
	
	m_RotationRad		= data.m_RotationRad;
	m_TextAlignOffsetX	= data.m_TextAlignOffsetX;
	m_TextAlignOffsetY	= data.m_TextAlignOffsetY;

	i3::stack_string strNewFormatFileName;
//	i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(reinterpret_cast<const char*>(data.m_szTexFile), strNewFormatFileName);
	// 아예 UI버전 이전 경로를 전체 다 자르는 식으로 수정..
	i3UIFilePathMgr::i()->ObtainUniquePath(reinterpret_cast<const char*>(data.m_szTexFile), strNewFormatFileName);

	i3::make_unix_path(strNewFormatFileName);

	setTextureResPath(strNewFormatFileName.c_str());
	
	I3ASSERT( m_strTexFile.size() > 0);

	i3Color::Copy( &m_ColorKey, &data.m_ColorKey);													// 원본 파일에서 UV검출을 위한 Color Key

	//SetAnimationName(data.m_strAnimationName);

	return Result;
}

bool i3UIShape::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr( "ID", "UIS1");
	pXML->addAttr( "Style", m_nStyle);

	i3::stack_string strOldFormatName;
	i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(m_strTexFile, strOldFormatName);
	pXML->addAttr( "path", strOldFormatName.c_str());
	
	i3Color::SetToXML( pXML, "ColorKey", &m_ColorKey);

	pXML->addAttr( "nX", m_Rect.left);
	pXML->addAttr( "nY", m_Rect.top);
	pXML->addAttr( "nWidth", m_Rect.right);
	pXML->addAttr( "nHeight", m_Rect.bottom);

	pXML->addAttr( "u", m_TexCoord.left);
	pXML->addAttr( "v", m_TexCoord.top);
	pXML->addAttr( "w", m_TexCoord.right);
	pXML->addAttr( "h", m_TexCoord.bottom);

	i3UILibrary* pUILib = i3UI::getUILibrary();

	if (pUILib == nullptr || pUILib->IsNewTemplateLibraryFormat() == false)
	{
		pXML->addAttr("offX", m_VTexOffset.x);
		pXML->addAttr("offY", m_VTexOffset.y);
	}

	//pXML->addAttr( "FlipX", m_bFlipX);
	//pXML->addAttr( "FlipY", m_bFlipY);

	pXML->addAttr( "Rotation", m_RotationRad);
	pXML->addAttr( "TextAlignOffsetX", m_TextAlignOffsetX);
	pXML->addAttr( "TextAlignOffsetY", m_TextAlignOffsetY);

	pXML->addAttr("AnimationName", m_strAnimationName.c_str());

	INT32 ntemp = m_bAnimationRoof;
	pXML->addAttr("AnimationRoof", ntemp);
	return true;
}

bool i3UIShape::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	const char * pszID = pXML->Attribute( "ID");
	if( i3::generic_is_niequal( "UIS1", pszID, 4) == false )
	{
		I3PRINTLOG(I3LOG_FATAL, "invalid i3UIShape id %s", pszID);
		return false;
	}

	const char * pszPath = pXML->Attribute( "path");

	if (nullptr == pszPath)
	{
		pszPath = "";
		setTextureResPath(pszPath);
	}
	else
	{
		i3::stack_string strNewFormatFileName;
	//	i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(pszPath, strNewFormatName);
		// 아예 UI_XX이전 것은 죄다 자른다..
		i3UIFilePathMgr::i()->ObtainUniquePath(pszPath, strNewFormatFileName);

		setTextureResPath(strNewFormatFileName.c_str());
	}

	if (m_strTexFile.size() == 0)
	{
		//I3PRINTLOG(I3LOG_FATAL,  "Template Shape의 Path가 없습니다.");
		//return false;
	}

	i3Color::GetFromXML( pXML, "ColorKey", &m_ColorKey);

	pXML->getAttr( "Style", &m_nStyle);

	pXML->getAttr( "nX", &m_Rect.left);
	pXML->getAttr( "nY", &m_Rect.top);
	pXML->getAttr( "nWidth", &m_Rect.right);
	pXML->getAttr( "nHeight", &m_Rect.bottom);

	pXML->getAttr( "u", &m_TexCoord.left);
	pXML->getAttr( "v", &m_TexCoord.top);
	pXML->getAttr( "w", &m_TexCoord.right);
	pXML->getAttr( "h", &m_TexCoord.bottom);

	i3UILibrary* pUILib = i3UI::getUILibrary();

	if (pUILib != nullptr && pUILib->IsNewTemplateLibraryFormat())
	{
		m_VTexOffset.x = 0;	m_VTexOffset.y = 0;
	}
	else
	{
		pXML->getAttr("offX", &m_VTexOffset.x);
		pXML->getAttr("offY", &m_VTexOffset.y);
	}
	
	//pXML->getAttr( "FlipX", &m_bFlipX);
	//pXML->getAttr( "FlipY", &m_bFlipY);

	pXML->getAttr( "Rotation", &m_RotationRad);
	pXML->getAttr( "TextAlignOffsetX", &m_TextAlignOffsetX);
	pXML->getAttr( "TextAlignOffsetY", &m_TextAlignOffsetY);

	const char * pszAniName = pXML->Attribute("AnimationName");
	if (pszAniName)
		SetAnimationName(pszAniName);

	INT32 ntemp;
	pXML->getAttr("AnimationRoof", &ntemp);

	m_bAnimationRoof = ntemp ? true : false;
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////
/*
const char * GetEventString( I3UI_EVT evt)
{
	const char * s_EvtString[] = 
	{
		"OnNormal",
		"OnCreate",
		"OnDestroy",
		"OnShow",
		"OnHide",
		"OnActivate",
		"OnDeActivate",
		"OnEnable",
		"OnDisable",
		"OnSetFocus",
		"OnKillFocus",
		"OnEnterMouse",
		"OnLeaveMouse",
		"OnClicked",
		"OnDoubleClicked",
		"OnRButtonClicked",
		"OnRButtonDoubleClicked",
		"OnMButtonClicked",
		"OnMButtonDoubleClicked",
		"OnDragStart",
		"OnDragging",
		"OnDragEnd",
		"OnDragCancel",
		"OnSizing",
		"OnSize",
		"OnMoving",
		"OnMove",
		"OnButtonPushed",
		"OnIme",
		"OnChanged",
		"OnModalResult",
		"OnScroll",
		"OnSelItem",
		
		"OnNotify",
		"OnMouseWheel",
		"OnUpdateState",
		"Reserved",
		"Reserved",
		"Reserved",

		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
	};

	return s_EvtString[evt];
}
*/

I3_EXPORT_FRAMEWORK I3UI_EVENT_INFO s_EventInfoTable[I3UI_EVT_MAXCOUNT] = 
{
	{	I3UI_EVT_NORMAL,			"",					"UI_EVT_NORMAL"		},
	{	I3UI_EVT_CREATE,			"OnCreate",			"UI_EVT_CREATE"		},
	{	I3UI_EVT_DESTROY,			"OnDestroy",		"UI_EVT_DESTROY"	},
	{	I3UI_EVT_SHOW,				"OnShow",			"UI_EVT_SHOW"		},
	{	I3UI_EVT_HIDE,				"OnHide",			"UI_EVT_HIDE"		},
	{	I3UI_EVT_ACTIVATE,			"OnActivate",		"UI_EVT_ACTIVATE"	},
	{	I3UI_EVT_INACTIVATE,		"OnInactivate",		"UI_EVT_INACTIVATE"	},
	{	I3UI_EVT_ENABLE,			"OnEnable",			"UI_EVT_ENABLE"		},
	{	I3UI_EVT_DISABLE,			"OnDisable",		"UI_EVT_DISABLE"	},
	{	I3UI_EVT_SETFOCUS,			"OnSetFocus",		"UI_EVT_SETFOCUS"	},
	{	I3UI_EVT_KILLFOCUS,			"OnKillFocus",		"UI_EVT_KILLFOCUS"	},
	{	I3UI_EVT_ENTERMOUSE,		"OnEnterMouse",		"UI_EVT_ENTERMOUSE"	},
	{	I3UI_EVT_LEAVEMOUSE,		"OnLeaveMouse",		"UI_EVT_LEAVEMOUSE"	},

	{	I3UI_EVT_CLICKED,			"OnClicked",		"UI_EVT_CLICKED"		},
	{	I3UI_EVT_DBL_CLICKED,		"OnDblClicked",		"UI_EVT_DBL_CLICKED"	},
	{	I3UI_EVT_R_CLICKED,			"OnRClicked",		"UI_EVT_R_CLICKED"		},
	{	I3UI_EVT_R_DBL_CLICKED,		"OnRDblClicked",	"UI_EVT_R_DBL_CLICKED"	},
	{	I3UI_EVT_M_CLICKED,			"OnMClicked",		"UI_EVT_M_CLICKED"		},
	{	I3UI_EVT_M_DBL_CLICKED,		"OnMDblClicked",	"UI_EVT_M_DBL_CLICKED"	},

	{	I3UI_EVT_DRAGSTART,			"OnDragStart",		"UI_EVT_DRAGSTART"	},
	{	I3UI_EVT_DRAGGING,			"OnDragging",		"UI_EVT_DRAGGING"	},
	{	I3UI_EVT_DRAGEND,			"OnDragEnd",		"UI_EVT_DRAGEND"	},
	{	I3UI_EVT_DRAGCANCEL,		"OnDragCancel",		"UI_EVT_DRAGCANCEL"	},

	{	I3UI_EVT_SIZING,			"OnSizing",			"UI_EVT_SIZING"	},
	{	I3UI_EVT_SIZE,				"OnSize",			"UI_EVT_SIZE"	},

	{	I3UI_EVT_MOVING,			"OnMoving",			"UI_EVT_MOVING"	},
	{	I3UI_EVT_MOVE,				"OnMove",			"UI_EVT_MOVE"	},

	{	I3UI_EVT_BTN_PUSHED,		"OnBtnPushed",		"UI_EVT_BTN_PUSHED"	},
	{	I3UI_EVT_IME,				"OnIME",			"UI_EVT_IME"		},

	{	I3UI_EVT_CHANGED,			"OnChanged",		"UI_EVT_CHANGED"		},
	{	I3UI_EVT_MODALRESULT,		"OnModalResult",	"UI_EVT_MODALRESULT"	},

	{	I3UI_EVT_SCROLL,			"OnScroll",			"UI_EVT_SCROLL"		},
	{	I3UI_EVT_SELITEM,			"OnSelItem",		"UI_EVT_SELITEM"	},

	{	I3UI_EVT_NOTIFY,			"OnNotify",			"UI_EVT_NOTIFY"		},
	
	{	I3UI_EVT_MOUSEWHEEL,		"OnMouseWheel",		"UI_EVT_MOUSEWHEEL"	},
	
	{	I3UI_EVT_UPDATE_STATE,		"OnUpdateState",	"UI_EVT_UPDATE_STATE"	},
	
	{	I3UI_EVT_TOOLTIP,			"OnToolTip",		"UI_EVT_TOOLTIP"	},		
	
	
	{	I3UI_EVT_KEYDOWN,			"OnKeyDown",		"UI_EVT_KEYDOWN"	},

	// 아래와 같은 이벤트 함수를 추가했다.(11.05.26.수빈)
	{	I3UI_EVT_COMBOBOX_UNFOLDED, "OnComboBoxUnfolded", "UI_EVT_COMBOBOX_UNFOLDED" },
	{	I3UI_EVT_COMBOBOX_FOLDED,	"OnComboBoxFolded", "UI_EVT_COMBOBOX_FOLDED" },
	{	I3UI_EVT_COMBOBOX_SELITEM,	"OnComboBoxSelItem", "UI_EVT_COMBOBOX_SELITEM"},
	{	I3UI_EVT_RADIO_CHECKED, "OnRadioChecked",		"UI_EVT_RADIO_CHECKED" },
	{	I3UI_EVT_RADIO_UNCHECKED, "OnRadioUnchecked",	"UI_EVT_RADIO_UNCHECKED" },
	{   I3UI_EVT_LVH_PUSHED,	  "OnLVHPushed",		"UI_EVT_LVH_PUSHED" },
	{	I3UI_EVT_RESERVED9,			"OnReserved9",		"UI_EVT_RESERVED9"	},
	{	I3UI_EVT_RESERVED10,		"OnReserved10",		"UI_EVT_RESERVED10"	},
	{	I3UI_EVT_RESERVED11,		"OnReserved11",		"UI_EVT_RESERVED11"	},
	{	I3UI_EVT_RESERVED12,		"OnReserved12",		"UI_EVT_RESERVED12"	},
	{	I3UI_EVT_RESERVED13,		"OnReserved13",		"UI_EVT_RESERVED13"	},
	{	I3UI_EVT_RESERVED14,		"OnReserved14",		"UI_EVT_RESERVED14"	},
	{	I3UI_EVT_RESERVED15,		"OnReserved15",		"UI_EVT_RESERVED15"	},
	{	I3UI_EVT_RESERVED16,		"OnReserved16",		"UI_EVT_RESERVED16"	},
	{	I3UI_EVT_RESERVED17,		"OnReserved17",		"UI_EVT_RESERVED17"	},
	{	I3UI_EVT_RESERVED18,		"OnReserved18",		"UI_EVT_RESERVED18"	},
	{	I3UI_EVT_RESERVED19,		"OnReserved19",		"UI_EVT_RESERVED19"	},
	{	I3UI_EVT_COMPLETE_LOAD,		"OnCompleteLoad",	"UI_EVT_COMPLETELOAD" }
};