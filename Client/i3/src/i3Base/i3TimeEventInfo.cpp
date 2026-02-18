#include "i3CommonType.h"
#include "i3TimeEventInfo.h"


I3_CLASS_INSTANCE( i3TimeEventInfo);


i3TimeEventInfo::~i3TimeEventInfo(void)
{
	I3_SAFE_RELEASE( m_pAction);
}

void i3TimeEventInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3TimeEventInfo * pInfo = (i3TimeEventInfo *) pDest;

	pInfo->setState( getState());
	pInfo->setStartTime( getStartTime());

	i3Action * pAction = getAction();
	if( pAction->getStyle() & I3_ACTION_STYLE_NEEDCOPYINST)
	{
		i3Action * pDestAction = pAction->create_instance();	// cloneÀº ¾Æ´Ô..
			// (i3Action*)CREATEINSTANCE( pAction->GetMeta());

		pAction->CopyTo( pDestAction, I3_COPY_INSTANCE);

		pInfo->setAction( pDestAction);
	}
	else
	{
		pInfo->setAction( getAction());
	}

	pInfo->setParam( getParam());
	pInfo->setDuration( getDuration());
}

void i3TimeEventInfo::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pAction != nullptr)
	{
		m_pAction->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct TIME_EVENT_INFO
	{
		UINT8		m_ID[4] = { 'T', 'E', 'I', '1' };
		UINT32		m_State = 0;
		REAL32		m_Time = 0.0f;
		OBJREF		m_pAction = 0;
		UINT32		m_Param = 0;
		REAL32		m_Duration = 0.0f;
		UINT32		pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TimeEventInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::TIME_EVENT_INFO data;

	result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_State		=	getState();
	data.m_Time			=	getStartTime();
	data.m_pAction		=	pResFile->GetObjectPersistID( getAction());
	data.m_Param		=	getParam();
	data.m_Duration		=	getDuration();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3TimeEventInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::TIME_EVENT_INFO data;

	result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result +=rc;

	setState( data.m_State);
	setStartTime( data.m_Time);
	setDuration( data.m_Duration);

	if( data.m_pAction != 0)
	{
		i3Action * pAction = (i3Action *) pResFile->FindObjectByID( data.m_pAction);
		I3ASSERT( pAction != nullptr);

		setAction( pAction);
	}

	setParam( data.m_Param);

	return result;
}

