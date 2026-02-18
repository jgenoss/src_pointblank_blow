#include "i3FrameworkPCH.h"
#include "i3GameResBinary.h"

I3_CLASS_INSTANCE( i3GameResBinary);

i3GameResBinary::i3GameResBinary(void)
{
	m_Type		 = I3_GAMERES_BINARY;
}

i3GameResBinary::~i3GameResBinary(void)
{
	I3_SAFE_RELEASE( m_pKeyObject);
}

void i3GameResBinary::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GameRes::OnBuildObjectList( List);
}

UINT32 i3GameResBinary::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result;//, Rc;	// C4189
	//i3Stream * pStream = pResFile->GetStream();	// C4189

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);

	return Result;
}

UINT32 i3GameResBinary::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result;//, Rc;	// C4189
	//i3Stream * pStream = pResFile->GetStream();	// C4189

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	return Result;
}