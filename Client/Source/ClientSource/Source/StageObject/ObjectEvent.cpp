#include "pch.h"
#include "ObjectEvent.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CGameObjectEvent);//, i3Object);

CGameObjectEvent::CGameObjectEvent()
{
	m_iObjEventType = 0;
	m_iObjUseCount = 0;
}

CGameObjectEvent::~CGameObjectEvent()
{
}

struct PERSIST_OBJEvent
{
	UINT32			nVer;
	UINT32			nEventType;
	UINT32			pad[32];
} ;

UINT32 CGameObjectEvent::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{	
	(*ppData) = i3MemAlloc( sizeof( PERSIST_OBJEvent));

	i3mem::FillZero( *ppData, sizeof(PERSIST_OBJEvent));

	PERSIST_OBJEvent * pData = (PERSIST_OBJEvent*) (*ppData);

	pData->nVer		= 101;

	pData->nEventType = m_iObjEventType;

	return sizeof( PERSIST_OBJEvent);
}

UINT32 CGameObjectEvent::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc;
	i3Stream * pStream = pInfo->getStream();

	PERSIST_OBJEvent info;
	
	Rc = pStream->Read( &info, sizeof( PERSIST_OBJEvent));
	I3_CHKIO( Rc);

	m_iObjEventType = info.nEventType;

	return Rc;
}

struct ALIGN4 I3_PERSIST_EVENT
{
	INT32		iObjEventType;
	UINT32		pad[4];
} ;

UINT32 CGameObjectEvent::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = CGameControledObject::OnSave(pResFile);
	I3_CHKIO(result);

	I3_PERSIST_EVENT data;
	data.iObjEventType = m_iObjEventType;

	rc = pResFile->Write(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectEvent::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_EVENT data;

	result = CGameControledObject::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	m_iObjEventType = data.iObjEventType;

	return result;
}

#if defined(I3_DEBUG)//XML
bool	CGameObjectEvent::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "TargetType",	m_iObjEventType);

	return true;
}

bool	CGameObjectEvent::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->getAttr( "TargetType",  &m_iObjEventType);

	return true;
}
#endif

void CGameObjectEvent::ResetObject()
{
	m_iObjUseCount = 1;
}

void CGameObjectEvent::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
}

void CGameObjectEvent::InitObject( void)
{
	ResetObject();
}