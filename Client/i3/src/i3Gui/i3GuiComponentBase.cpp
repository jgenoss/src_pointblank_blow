// ! praptor - 미완성입니다. 작동하지 않습니다.

#include "i3GuiPCH.h"
#include "i3GuiComponentBase.h"

I3_CLASS_INSTANCE( i3GuiComponentBase);


void i3GuiComponentBase::SetContainer(i3GuiObjBase* pGuiContainer)
{
	m_pGuiContainer = pGuiContainer;
}

UINT32 i3GuiComponentBase::OnSave(i3ResourceFile* pResFile)
{
	i3Stream* pStream = pResFile->GetStream();

	OBJREF orGuiContainer = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pGuiContainer);
	UINT32 length = pStream->Write( &orGuiContainer, sizeof(orGuiContainer));

	if( length == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}
	
	return length;
}

UINT32 i3GuiComponentBase::OnLoad(i3ResourceFile* pResFile)
{
	OBJREF orGuiContainer;
	UINT32 length =	pResFile->Read( &orGuiContainer, sizeof(orGuiContainer));
	if( length == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load data.");
		return STREAM_ERR;
	}

	if( orGuiContainer != 0)
	{
		m_pGuiContainer = (i3GuiObjBase*)pResFile->FindObjectByID( orGuiContainer);
		I3ASSERT(m_pGuiContainer);
	}

	return length;
}
