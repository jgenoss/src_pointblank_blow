#include "pch.h"
#include "UIPopupClanHonor.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"

#include "../TextSet.h"

#include "ClanGameContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( UIPopupClanHonor);//, UIPopupBase);

namespace
{
	const REAL32 HONOR_EVENT_CLOSE_TIME = 3.33f;
	const INT32	 CLAN_MARK_TEX_UV		= 6;
}


UIPopupClanHonor::UIPopupClanHonor()
{
	m_p3DView = nullptr;

	m_nIndex = 0;

	i3mem::FillZero(m_pClanMarkTexture, sizeof(m_pClanMarkTexture) );

	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx )
	{
		m_pClanMarkTexture[idx] = i3Texture::new_object();
		I3ASSERT( m_pClanMarkTexture[idx]);

#if defined( I3_DEBUG)
		i3::rc_string str_name = "";
		i3::sprintf(str_name, "ClanMark_%d", idx + 1);
		m_pClanMarkTexture[idx]->SetName(str_name);
#endif
		m_pClanMarkTexture[idx]->Create(64, 64, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
	}

	m_rPopupElapsedTime = 0.0f;

	m_pEffectParent = nullptr;
}

UIPopupClanHonor::~UIPopupClanHonor()
{
	for(INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx )
		I3_SAFE_RELEASE( m_pClanMarkTexture[idx]);
}

/*virtual*/ void UIPopupClanHonor::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_ClanEvent.i3UIs");

	m_rPopupElapsedTime = 0.0f;

}

/*virtual*/ void UIPopupClanHonor::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	_UpdatePopup(rDeltaSeconds);
}

/*virtual*/ void UIPopupClanHonor::OnLoadAllScenes()
{
	if (GetScene(0) != nullptr)
	{
		i3UI::setDisableUIInput(true);

		i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "Effect");
		if( pCharaWnd != nullptr)
			Create3DViewBox( pCharaWnd);
	}
}

/*virtual*/ void UIPopupClanHonor::OnUnloadAllScenes()
{
	Delete3DViewBox();
	i3UI::setDisableUIInput(false);
}

/*virtual*/ bool UIPopupClanHonor::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	CreateViewEffect();
	SetUiText();

	return true;
}

void UIPopupClanHonor::Create3DViewBox( i3UIControl * pParent )
{
	I3ASSERT( pParent != nullptr);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIPopupClanHonor::Delete3DViewBox( void)
{
	if( m_p3DView != nullptr)
	{
		// Effect Manager Parent Change
		g_pEffectManager->GetNode()->ChangeParent( m_pEffectParent);
		m_pEffectParent = nullptr;

		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);

		I3_SAFE_RELEASE( m_p3DView);
	}
}
void UIPopupClanHonor::CreateViewEffect()
{
	m_rPopupElapsedTime = 0.0f;

	i3Node * p3DViewNode = i3Node::new_object_ref();

	// Effect Manager Parent Change
	{
		i3LayerRootNode * pNewRoot = i3LayerRootNode::new_object_ref();
		i3LayerRootNode * pRoot = g_pFramework->GetLayerRoot( "EffectRoot");
		pRoot->CopyTo( pNewRoot, I3_COPY_INSTANCE);

		m_pEffectParent = g_pEffectManager->GetNode()->GetParent();
		g_pEffectManager->GetNode()->ChangeParent( pNewRoot);
		p3DViewNode->AddChild(pNewRoot);
	}	

	// Explosion Particle
	{
		i3GameRes * pRes = g_pFramework->LoadResource( "Effect/H/Hit_Honor.i3s", 0);
		i3GameResSceneGraph * pSgRes = (i3GameResSceneGraph *) pRes;

		i3Node * pFindNode = i3Scene::FindNodeByName(pSgRes->getScene(), "i3TimeSequence");
		if (pFindNode !=nullptr)
			pFindNode->OnChangeTime(0.0f);

		p3DViewNode->AddChild( pSgRes->getScene() );
	}

	// Particle Render Node
	{
		i3ParticleRenderNode * pParticleNode = i3ParticleRenderNode::new_object_ref();
		p3DViewNode->AddChild( pParticleNode);
	}

	// Main Effect
	{
		i3GameRes * pRes = g_pFramework->LoadResource( "Effect/H/Honor.i3s", 0);
		i3GameResSceneGraph * pSgRes = (i3GameResSceneGraph *) pRes;

		i3Node * pFindNode = i3Scene::FindNodeByName(pSgRes->getScene(), "Scene Root");
		if (pFindNode !=nullptr)
			pFindNode->OnChangeTime(0.0f);

		// Set Clan Mark Texture
		for(INT32 idx = 0; idx < TEAM_COUNT ; ++idx)
		{
			i3::rc_string str_attr_name;
			i3::sprintf(str_attr_name, "Model_Clan%02d_Material", idx + 1);
			i3TextureBindAttr * pTexture = (i3TextureBindAttr*)FindTextureBindInAttrSet(pSgRes->getScene(), str_attr_name.c_str(), i3TextureBindAttr::static_meta(), false);

			const CLAN_MATCH_BATTLE_TEAM * pTeamInfo = ClanGameContext::i()->getClanBattleTeamInfo( static_cast<TEAM_TYPE>(idx) );
			if( MakeClanMarkTex(m_pClanMarkTexture[idx], CLAN_MARK_MEDIUM, pTeamInfo->getClanMark() , CLAN_MARK_TEX_UV, CLAN_MARK_TEX_UV) )
				pTexture->SetTexture(m_pClanMarkTexture[idx]);
		}

		p3DViewNode->AddChild( pSgRes->getScene());
	}

	// 3D View Create & Setting
	{
		m_p3DView->Create3DView();
		m_p3DView->Set3DView( p3DViewNode);

		i3Sprite2D* pSprite = m_p3DView->GetSprite();
		pSprite->SetSrcBlend(I3G_BLEND_SRCALPHA);
		pSprite->SetDestBlend(I3G_BLEND_ONE);

		m_p3DView->OnVisibleScene();
		m_p3DView->get3DCamera()->addStyle(I3_CAM_STYLE_FULLFRAME);
		MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
		i3Matrix::Identity( pCamMtx);
		i3Matrix::SetPos( pCamMtx, 0.17f, -0.50f, 27.0f, 1.0f);
		m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
	}
}

i3RenderAttr * UIPopupClanHonor::FindTextureBindInAttrSet( i3Node * pNode, const char * pszAttrName, i3ClassMeta * pMeta, bool bSevereCheck)
{	
	i3Node * pFindNode = i3Scene::FindNodeByName( pNode, pszAttrName);
	//I3ASSERT( pFindNode != nullptr);

	i3AttrSet * pAttrSet = nullptr;

	//	AttrSet 검사
	if( pFindNode != nullptr)
	{
		if( i3::same_of<i3AttrSet*>(pFindNode))
		{				
			pAttrSet = (i3AttrSet *) pFindNode;
		}
		else if( bSevereCheck)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Not i3AttrSet Class (name: %s)", pszAttrName);		
			return nullptr;
		}
	}
	else if( bSevereCheck)
	{	// bSevereCheck 인자 true이면 지정된 AttrSet이름을 찾지 못하면 다운된다.
		I3PRINTLOG(I3LOG_FATAL,  "Not find Node (name: %s)", pszAttrName);
	}	
	else
	{
		I3TRACE( "[%s] warning! Cannot find %s (%s)\n", __FUNCTION__, pMeta->class_name(), pszAttrName);
		return nullptr;
	}

	i3RenderAttr * pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( pAttrSet, pMeta);

	//	Attribute가 GatherUp 되어 있을 경우를 대비
	if( pRenderAttr == nullptr)
	{
		I3ASSERT_RETURN(pAttrSet, nullptr);

		i3Node * pParent = pAttrSet->GetParent();
		while( pParent != nullptr)
		{
			//	그 부모가 AttrSet이면 nullptr이 될때까지 해당 Attribute를 찾는다.
			if( i3::kind_of<i3AttrSet*>(pParent))
			{
				pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( (i3AttrSet *) pParent, pMeta);

				//	찾았다.
				if( pRenderAttr != nullptr)		break;
			}

			pParent = pParent->GetParent();
		}
	}

	return pRenderAttr;
};

void UIPopupClanHonor::_UpdatePopup(REAL32 rDeltaSeconds)
{
	if (GetMainFrame()->IsOpenedPopup(UPW_CLANHONOR))
	{
		m_rPopupElapsedTime += rDeltaSeconds;

		if (m_rPopupElapsedTime > HONOR_EVENT_CLOSE_TIME )
		{
			GetMainFrame()->ClosePopup(UPW_CLANHONOR);
		}
	}
}

void UIPopupClanHonor::SetUiText()
{
	LuaState* L = UILua::CallLuaFunction( GetScene(0), "setClanTitleText");

	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, ClanGameContext::i()->getClanBattleTeamInfo(TEAM_RED)->getClanName() );	// RED Team Clan Name
		i3Lua::PushStringUTF16To8( L, ClanGameContext::i()->getClanBattleTeamInfo(TEAM_BLUE)->getClanName() );	// Blue Team Clan Name
		_EndLuaFunction(L, 2);
	}
}
