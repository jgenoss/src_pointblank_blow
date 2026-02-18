#include "i3SceneDef.h"
#include "i3GPUCommandListAttr.h"

I3_CLASS_INSTANCE( i3GPUCommandListAttr);

i3GPUCommandListAttr::i3GPUCommandListAttr(void)
{
	SetID( I3_ATTRID_GPUCOMMANDLIST);
}

i3GPUCommandListAttr::~i3GPUCommandListAttr(void)
{
	I3_SAFE_RELEASE( m_pCmd);
}

void i3GPUCommandListAttr::Set( i3GPUCommandList * pCmdList)
{
	I3_REF_CHANGE( m_pCmd, pCmdList);
}

void i3GPUCommandListAttr::Apply( i3RenderContext * pContext)
{
	pContext->DrawCommandList( m_pCmd);
}

void i3GPUCommandListAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	if( i3::same_of<i3GPUCommandListAttr*>(pObj))
	{
		i3GPUCommandListAttr * pCmd = (i3GPUCommandListAttr *) pObj;

		// Instance 방식으로 복사하는 경우, Command List를 새롭게
		// Build해야 하기 때문에 현재로는 Reference 방식만 지원한다.
		pCmd->m_pCmd = m_pCmd;
	}
}


void i3GPUCommandListAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pCmd != nullptr)
		m_pCmd->OnBuildObjectList( List);

	i3RenderAttr::OnBuildObjectList( List);
}

UINT32 i3GPUCommandListAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	ref = (OBJREF) pResFile->GetObjectPersistID( m_pCmd);

	Rc = pStream->Write( &ref, sizeof(OBJREF));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GPUCommadListAttr::OnSave()", "Could not write GPU command list object reference.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GPUCommandListAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	Rc = pStream->Read( &ref, sizeof(OBJREF));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GPUCommandListAttr::OnLoad()", "Could not write GPU Command list object reference.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( ref != 0)
	{
		i3GPUCommandList * pCmd = (i3GPUCommandList *) pResFile->FindObjectByID( ref);

		I3ASSERT( pCmd != nullptr);
		Set( pCmd);
	}

	return Result;
}
