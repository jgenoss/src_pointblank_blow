#include "stdafx.h"
#include "PackNode.h"

I3_CLASS_INSTANCE( PackNode, i3PackNode);

PackNode::PackNode(void)
{
	m_szPath[0] = 0;
}

PackNode::~PackNode(void)
{
}

UINT32 PackNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3PackNode::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Write( &m_szPath, sizeof(m_szPath));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3PackNode::OnSave()", "Could not write path.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 PackNode::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3PackNode::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &m_szPath, sizeof(m_szPath));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3PackNode::OnLoad()", "Could not read path.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}
