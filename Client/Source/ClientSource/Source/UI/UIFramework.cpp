#include "pch.h"
#include "UIFramework.h"
#include "UIMainFrame_V15.h"
#include "UIMainFrame_V11.h"
#include "UIMainFrame_V10.h"

I3_CLASS_INSTANCE( UIFramework);//, i3Stage);

UIFramework::UIFramework()
{
	m_pMainfram = nullptr;
}

UIFramework::~UIFramework()
{

//	I3_SAFE_RELEASE( m_pMainfram);
}

/*virtual*/ void UIFramework::OnCreate( void)
{
	i3Stage::OnCreate();

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		m_pMainfram = UIMainFrame_V15::new_object();
		I3_GAMENODE_ADDCHILD( this, m_pMainfram);
	}
	else if (g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		m_pMainfram = UIMainFrame_V11::new_object();
		I3_GAMENODE_ADDCHILD( this, m_pMainfram);
	}
	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		m_pMainfram = UIMainFrame_V10::new_object();
		I3_GAMENODE_ADDCHILD( this, m_pMainfram);
	}
	m_pMainfram->Create( this);

}

/*virtual*/ void UIFramework::OnLoadEnd(void)
{
	i3Stage::OnLoadEnd();

	m_pMainfram->OnLoadEnd(this);
}

/*virtual*/ bool UIFramework::OnFinish( void)
{
	m_pMainfram->OnFinish();
	I3_SAFE_RELEASE(m_pMainfram);
	i3Framework::FlushDXForThread();	// thread에서 처리하는 것을 삭제

	return i3Stage::OnFinish();
}

/*virtual*/ bool UIFramework::OnQueryLoad(INT32 numStage)
{
	if (false == i3Stage::OnQueryLoad(numStage)) return false;
#if legacy_gui_operator
//	m_gui_operator.LoadOldGUI();
#endif
	return true;
}


/*virtual*/ bool UIFramework::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( m_pMainfram )
		m_pMainfram->OnEvent( event, pObj, param2, code);

	return i3Stage::OnEvent( event, pObj, param2, code);
}

void UIFramework::OnEscape( void)
{
	if( m_pMainfram)
	{
		m_pMainfram->OnEscape();
	}
}

void UIFramework::OnUpdate( REAL32 rDeltaSeconds )
{
	i3Stage::OnUpdate(rDeltaSeconds);
}