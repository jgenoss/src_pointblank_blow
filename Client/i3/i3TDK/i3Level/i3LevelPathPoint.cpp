#include "stdafx.h"
#include "i3LevelPathPoint.h"

I3_CLASS_INSTANCE( i3LevelPathPoint);

i3LevelPathPoint::i3LevelPathPoint(void)
{
	RemoveStyle( I3_LEVEL_STYLE_VOLATILE);
}

void i3LevelPathPoint::Create(void)
{
	{
		i3Node * pNode = i3SceneUtil::BuildSphereMesh( 1.0f, TRUE, FALSE, 10, 10);

		i3Transform * pTrans = i3Transform::new_object_ref();
		pTrans->AddChild( pNode);
		I3_MUST_RELEASE(pNode);			// Build계열함수는 이제 레퍼런스카운팅을 갖는다..(2012.09.13.수빈)

		i3Matrix::SetScale( pTrans->GetMatrix(), 0.5f, 0.5f, 0.5f);
		//i3Matrix::SetTranslate( pTrans->GetMatrix(), 0.0f, 0.25f, 0.0f);

		SetSymbol( pTrans);

		SetMaterialDiffuse( 1.0f, 0.4f, 0.0f, 1.0f);
	}
}

void i3LevelPathPoint::OnTargeted( BOOL bTargeted)
{
	i3LevelControl::OnTargeted( bTargeted);

	if( bTargeted)
		SetMaterialDiffuse( 1.0f, 0.0f, 0.0f, 1.0f);
	else
		SetMaterialDiffuse( 1.0f, 0.4f, 0.0f, 1.0f);
}

void i3LevelPathPoint::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelControl::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelPathPoint*>(pDest));

	i3LevelPathPoint * pPoint = (i3LevelPathPoint *) pDest;

	pPoint->setPriority( getPriority());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 PATHPOINT
	{
		UINT8		m_ID[4] = { 'P', 'H', 'P', '1' };
		INT32		m_Priority = 0;
		UINT8		m_Reserved[64] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelPathPoint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PATHPOINT Info;

	Result = i3LevelControl::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	Info.m_Priority = getPriority();

	Rc  = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelPathPoint::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3LevelPathPoint::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PATHPOINT Info;

	Result = i3LevelControl::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelPathPoint::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	setPriority( Info.m_Priority);

	return Result;
}

bool i3LevelPathPoint::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3LevelControl::OnSaveXML( pFile, pXML);

	pXML->addAttr( "Priority", m_Priority);

	return true;
}

bool i3LevelPathPoint::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3LevelControl::OnLoadXML( pFile, pXML);

	pXML->getAttr( "Priority", &m_Priority);

	return true;
}
