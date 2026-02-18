#include "StdAfx.h"
#include "i3GuiEditorFramework.h"
#include "GlobalVariable.h"

I3_CLASS_INSTANCE( i3GuiEditorFramework); //, i3Framework );

i3GuiEditorFramework::i3GuiEditorFramework(void)
{
	g_pFramework = this;
}

i3GuiEditorFramework::~i3GuiEditorFramework(void)
{
}

void i3GuiEditorFramework::Reset(void)
{
}

void i3GuiEditorFramework::SetSceneGraph( i3Node * pSG )
{
	i3Node * pParent = OnQueryAttachNode( NULL);

	if( m_pCurNode != NULL)
	{
		pParent->RemoveChild( m_pCurNode);
		m_pCurNode->Release();
	}

	m_pCurNode = pSG;

	if( pSG != NULL)
	{
		pParent->AddChild( pSG);
		m_pCurNode->AddRef();
	}

	i3NamedResourceManager::Reset();

	OnChangeScene();
	OnResetCameraPosition();
	OnUpdateCameraTransform();
}

void i3GuiEditorFramework::OnUpdate( REAL32 tm)
{
}
