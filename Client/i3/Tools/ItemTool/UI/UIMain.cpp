#include "StdAfx.h"
#include "UIMain.h"
#include "UI3DView.h"
#include "UISlotList.h"


I3_CLASS_INSTANCE(cUIMain);//, i3GameNode);

cUIMain::cUIMain(void)
: m_3DView()
, m_SlotList()
, m_pSceneNode(NULL)
{
}

cUIMain::~cUIMain(void)
{
	_Destroy();
}


bool cUIMain::Create( i3Node* pRenderNode )
{
	if( pRenderNode == NULL )
		return false;

	if( m_pSceneNode != NULL )
		return false;

	m_pSceneNode	= i3Node::new_object();
	pRenderNode->AddChild(m_pSceneNode);

	m_SlotList.Create(m_pSceneNode);

	return true;
}

void cUIMain::_Destroy( void )
{
	I3_SAFE_NODE_RELEASE(m_pSceneNode);
}



bool cUIMain::LoadFile(const char* pszPath)
{
	if( pszPath == NULL || m_pSceneNode == NULL )
		return false;

	//return m_SlotList.Create(m_pSceneNode, pszPath);

	return false;
}


void cUIMain::Set3DView( i3SceneGraphInfo * pSgInfo )
{

	m_3DView.Set3DView(m_pSceneNode, pSgInfo);
}


const REAL32	RateRenderView	= 0.3f;
const REAL32	RateRenderUI	= 1.0f - RateRenderView;

void cUIMain::SetSize(POINT2D& Size)
{
	INT32	nX		= 0;//static_cast<INT32>( Size.x * RateRenderView );
	INT32	nY		= 0;
	INT32	nWidth	= Size.x;// - nX;
	INT32	nHeight	= Size.y;

	m_SlotList.SetSize(nX, nY, nWidth, nHeight);
	//m_3DView.SetSize(0,0, Size.x - nWidth, nHeight);
}


void cUIMain::OnLButtonDown( const POINT& point )
{
	m_SlotList.SelectSlot(point);
}


bool cUIMain::SetUITexture( UI_TEXTURE_TYPE textype, INT32 nIndex, i3Texture* pTex )
{
	return m_SlotList.SetUITexture(textype, nIndex, pTex);
}

void cUIMain::ResetItemList( INT32 nStartIndex )
{
	m_SlotList.ResetItemList( nStartIndex );
}

void cUIMain::ResizeUI( void )
{
	m_SlotList.ResizeUI();
}

INT32 cUIMain::GetScrollCount( void ) const
{
	return m_SlotList.GetScrollCount();
}

INT32 cUIMain::GetColPage( void ) const
{
	return m_SlotList.GetColPage();
}

void cUIMain::OnUpdate( RT_REAL32 rDeltaSeconds )
{
	//m_3DView.OnUpdate(rDeltaSeconds);
}
