#include "pch.h"
#include "UIFirstLoad.h"
#include "resource.h"
#include "i3Base/string/ext/format_string.h"

I3_CLASS_INSTANCE( UIFirstLoad);//, UIBase);

#define LOADING_PROGRESS_POSX		20.0f
#define LOADING_PROGRESS_POSY		745.0f
#define LOADING_PROGRESS_WIDTH		984.0f
#define LOADING_PROGRESS_HEIGHT	8.0f

UIFirstLoad::UIFirstLoad()
{
	m_ImageCount = 0;
	m_ImageIdx = 0;
	m_ChangeImageTime = 0.f;
	m_pLoadingBackGround = nullptr;
	m_pLoadingRotate = nullptr;

	m_rLoadingPercent = 0.f;
	m_pLoadingProgressbar = nullptr;
	m_pAttrSetLoading = nullptr;
}

UIFirstLoad::~UIFirstLoad()
{
	I3_SAFE_RELEASE(m_pLoadingProgressbar);
	I3_SAFE_RELEASE(m_pAttrSetLoading);
}
bool UIFirstLoad :: AddTradeIcon(char* Path)
{
	i3ResourceManager * pResMng = g_pFramework->GetResourceManager();

	i3Texture * pTexture = nullptr;

	pTexture = pResMng->LoadTexture(Path);
	I3ASSERT(pTexture);

	if (!pTexture)
	{
		return false;
	}
	m_pIconArray.push_back(i3UIImageBoxEx::new_object_ref());
	int Index = m_pIconArray.size() - 1;
	if (Index >= 0)
	{
		m_pIconArray[Index]->AddTexture(pTexture, 53.f, 53.f);
		m_pIconArray[Index]->CreateBaseSceneObjects();
		m_pIconArray[Index]->setPos(6.f + 59 * Index, 6.f);
		m_pLoadingBackGround->AddChild(m_pIconArray[Index]);
		m_pIconArray[Index]->SetCurrentImage(0);
	}

	I3_MUST_RELEASE(pTexture);
	return true;
}
/*virtual*/ void UIFirstLoad::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);
	i3ResourceManager * pResMng = g_pFramework->GetResourceManager();

	m_pLoadingBackGround = (i3UIFrameWnd *) pScene->FindChildByName( "i3UIFrameWnd_FirstLoading");
	I3ASSERT(m_pLoadingBackGround != nullptr);

	m_pLoadingRotate = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSet_FirstLoading");
	I3ASSERT( m_pLoadingRotate != nullptr);
	m_pLoadingRotate->SetVisible(false);

	i3Texture * pTexture = pResMng->LoadTexture("Gui/Loading/FirstLoading.i3i");
	I3ASSERT(pTexture);

	

	i3::pack::POINT2D size;

	size.x = i3UI::getManager()->getWidth();
	size.y = i3UI::getManager()->getHeight();

	m_pBG = i3UIImageBoxEx::new_object_ref();
	
	m_pBG->AddTexture( pTexture, 1024.f, 576.f);
	
	I3_MUST_RELEASE(pTexture);
	

	m_pBG->CreateBaseSceneObjects();
	m_pLoadingBackGround->AddChildToHead( m_pBG);
	m_pBG->SetCurrentImage(0);

	//LS CHECK : 
#ifdef USE_LEUCO_SHELL
		AddTradeIcon("Gui/Loading/ulsan_icon.i3i");
#endif


	// Create Progress Bar 
	if(m_pLoadingProgressbar == nullptr)
	{
		m_pLoadingProgressbar = i3Sprite2D::new_object();
		m_pLoadingProgressbar->Create(1, false, true, false);
	}

	m_rLoadingPercent = -1.0f;

	m_fScreenWidthRate = g_pEnvSet->m_nScreenWidth / 1024.0f;
	m_fScreenHeightRate =  g_pEnvSet->m_nScreenHeight / 768.0f;

	m_rProg_PosX = LOADING_PROGRESS_POSX * m_fScreenWidthRate; 
	m_rProg_PosY = LOADING_PROGRESS_POSY * m_fScreenHeightRate;
	m_rProg_Height = LOADING_PROGRESS_HEIGHT * m_fScreenHeightRate;

	m_pLoadingProgressbar->SetEnable(0 , true);
	m_pLoadingProgressbar->SetRect(0, m_rProg_PosX, m_rProg_PosY, 0.0f, m_rProg_Height);
	m_pLoadingProgressbar->SetColor(0, 255, 255, 255, 255); // test

	// 프로그레스바 attr 추가
	if(m_pAttrSetLoading == nullptr)
	{
		m_pAttrSetLoading = i3AttrSet::new_object();
		I3ASSERT( m_pAttrSetLoading);

		m_pAttrSetLoading->AddChild(m_pLoadingProgressbar);
		g_pFramework->getGuiLayer()->AddChild(m_pAttrSetLoading);
	}
	m_pAttrSetLoading->RemoveFlag( I3_NODEFLAG_DISABLE);

	_Resize();
}

void UIFirstLoad::_ClearAtUnload( i3UIScene * pScene)
{
	//부모가 먼저 제거 되기 때문에 제거 예외 처리합니다. 
	m_pBG = nullptr;
	UIBase::_ClearAtUnload( pScene);

	if(m_pAttrSetLoading != nullptr)
		m_pAttrSetLoading->AddFlag(I3_NODEFLAG_DISABLE  );
}

/*virtual*/ void UIFirstLoad::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Main/PointBlankRe_FirstLoading.i3UIs");
}
 
/*virtual*/ void UIFirstLoad::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);

	i3ResourceManager * pResMng = g_pFramework->GetResourceManager();

	// Wide screen, Normal Screen 예외처리
	REAL32 rRate = (REAL32)g_pEnvSet->m_nScreenWidth / (REAL32)g_pEnvSet->m_nScreenHeight; 
	
	// Normal Screen일 경우 화면 가장 끝 부터 출력합니다
	if( rRate  <= 1.4f)		m_rProg_PosX = 0.f;
	
	m_rLoadingPercent = (REAL32)pResMng->GetLoadingProgress() * 0.8f;
	m_rLoadingPercent += (REAL32)g_pCharaManager->GetDummyCharaLoadProg() * 0.2f;

	REAL32 rWidth = 0.f;
	if(rRate <= 1.4f)
		rWidth = g_pEnvSet->m_nScreenWidth *( 0.01f * m_rLoadingPercent);
	else
		rWidth = (LOADING_PROGRESS_WIDTH * m_fScreenWidthRate)  * ( 0.01f * m_rLoadingPercent); 
	
	m_pLoadingProgressbar->SetRect(0, m_rProg_PosX, m_rProg_PosY, rWidth, m_rProg_Height );
}

bool UIFirstLoad::OnRevive( i3RenderContext * pCtx)
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	_Resize();
	return true;
}
