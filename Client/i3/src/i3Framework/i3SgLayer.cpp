#include "i3FrameworkPCH.h"
#include "i3SgLayer.h"

I3_CLASS_INSTANCE( i3SgLayer);


i3SgLayer::~i3SgLayer(void)
{
	if( m_pRoot != nullptr)
	{
		m_pRoot->RemoveFromParent();
		I3_SAFE_RELEASE( m_pRoot);
	}
}

void	i3SgLayer::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3SgLayer * pLayer = (i3SgLayer *) pDest;

	pLayer->setStyle( getStyle());

	if( method == I3_COPY_REF)
	{
		pLayer->setLayerRoot( getLayerRoot());
	}
	else
	{
		i3LayerRootNode * pLayerRoot;

		pLayerRoot = (i3LayerRootNode *) i3Scene::CreateClone( getLayerRoot(), method);
		I3ASSERT( pLayerRoot != nullptr);

		pLayer->setLayerRoot( pLayerRoot);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 SG_LAYER
	{
		UINT32		m_Style = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SgLayer::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream  = pResFile->GetStream();
	
	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	pack::SG_LAYER data;
	data.m_Style = getStyle();

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3SgLayer::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SG_LAYER data;

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	setStyle( data.m_Style);

	if( hasName())
	{
		m_pRoot->SetName( GetNameString());
	}

	return Result;
}
