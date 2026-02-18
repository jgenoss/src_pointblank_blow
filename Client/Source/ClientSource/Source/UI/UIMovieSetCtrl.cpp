#include "pch.h"
#include "UIMainFrame.h"
#include "UIMovieSetCtrl.h"
#include "../VideoPlay.h"


UIMovieSetGroup::UIMovieSetGroup()
{	
}

UIMovieSetGroup::~UIMovieSetGroup()
{

}

UIMovieSetCtrl*	 UIMovieSetGroup::CreateMovieSetCtrl(i3UIImageBoxEx* bound_rsc) const
{
#if defined(ENABLE_MOVIEPLAYER)
	UIMovieSetCtrl* res = UIMovieSetCtrl::new_object();
	res->Initialize(this, bound_rsc);
	return res;
#else
	return nullptr;
#endif
}

UIMovieSetCtrl*	 UIMovieSetGroup::CreateMovieSetCtrl( i3UIScene* scn, const char* bound_rsc ) const
{
	i3UIImageBoxEx* bound_ctrl = static_cast<i3UIImageBoxEx*>(scn->FindChildByName(bound_rsc));
	I3ASSERT(bound_ctrl);
	return UIMovieSetGroup::CreateMovieSetCtrl(bound_ctrl);
}

I3_CLASS_INSTANCE( UIMovieSetCtrl); //, i3GameNode);

UIMovieSetCtrl::UIMovieSetCtrl() : m_CurrentPlayIdx(-1)
{
	m_pBoundImage	= nullptr;
	m_pMovieGroup	= nullptr;
	m_pTex			= nullptr;
	m_pMovieFileList.clear();
}

UIMovieSetCtrl::~UIMovieSetCtrl()
{
	if(m_pTex)		I3_SAFE_RELEASE(m_pTex);
}

bool UIMovieSetCtrl::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	CVideoPlay * pPlayer = g_pFramework->GetVideoPlayer();
	if( pPlayer != nullptr)
		pPlayer->StopVideo();

	return true;
}

bool UIMovieSetCtrl::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	if( m_CurrentPlayIdx != -1)
		PlayVideo( m_CurrentPlayIdx);

	return true;
}

void UIMovieSetCtrl::Initialize(const UIMovieSetGroup* tpl_grp, i3UIImageBoxEx* bound_rsc)
{
	m_pMovieGroup = tpl_grp;
	m_pBoundImage = bound_rsc;
}

void UIMovieSetCtrl::InitMovie()
{
	CVideoPlay * pPlayer = g_pFramework->GetVideoPlayer();
	if( pPlayer == nullptr)
		return;

	if(m_pMovieFileList.size() <= 0)
	{
		I3PRINTLOG(I3LOG_NOTICE,"not registered movie file.");
	}

	pPlayer->OpenFile(getMoviePath(0));

	m_pTex = i3TextureDX::new_object();
	m_pTex->Create(1024, 768, 1, I3G_IMAGE_FORMAT_BGRX_8888, I3G_USAGE_NONE);
	m_pTex->SetDXTexture(g_pFramework->GetVideoPlayer()->getTexture());

	i3::pack::RECT pPos, pCoord;
	pPos.left = 0.0f;
	pPos.top = 0.0f;
	pPos.right = m_pBoundImage->getSize()->x;
	pPos.bottom = m_pBoundImage->getSize()->y;
	pCoord.left = 0.0f;
	pCoord.top = 0.0;
	pCoord.right = (REAL32)m_pTex->GetWidth();
	pCoord.bottom = (REAL32)m_pTex->GetHeight();
	m_pBoundImage->AddTextureMovie( m_pTex, pPos, pCoord);

	m_pBoundImage->CreateBaseSceneObjects();
	m_pBoundImage->SetCurrentImage(0);
}

bool UIMovieSetCtrl::PlayVideo(UINT32 idx)
{
	CVideoPlay * pPlayer = g_pFramework->GetVideoPlayer();
	if( pPlayer == nullptr)
		return false;

	pPlayer->StopVideo();
	if(idx >= m_pMovieFileList.size())
		return false;

	pPlayer->OpenFile(getMoviePath(idx));
	pPlayer->PlayVideo();
	m_pTex->SetDXTexture( pPlayer->getTexture());

	m_CurrentPlayIdx = idx;

	return true;
}

void UIMovieSetCtrl::StopVideo( void)
{
	CVideoPlay * pPlayer = g_pFramework->GetVideoPlayer();
	if( pPlayer != nullptr)
		pPlayer->StopVideo();
}

void UIMovieSetCtrl::CloseVideo( void)
{
	CVideoPlay * pPlayer = g_pFramework->GetVideoPlayer();
	if( pPlayer != nullptr)
		pPlayer->CloseVideo();
}
