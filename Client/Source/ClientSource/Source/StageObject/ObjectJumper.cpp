#include "pch.h"
#include "ObjectJumper.h"

I3_CLASS_INSTANCE( CGameObjectJumper);//, i3Object);

CGameObjectJumper::CGameObjectJumper()
{
	i3Vector::Zero( &m_vJumpPos);
}

CGameObjectJumper::~CGameObjectJumper()
{
}

namespace pack
{
	struct OBJJUMPER
	{
		UINT32			nVer = 0;
		i3::pack::VEC3D	vecPos;
		UINT32			pad[4] = { 0 };
	};
}

UINT32 CGameObjectJumper::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{	
	(*ppData) = i3MemAlloc( sizeof( pack::OBJJUMPER));

	i3mem::FillZero( *ppData, sizeof(pack::OBJJUMPER));

	pack::OBJJUMPER * pData = (pack::OBJJUMPER*) (*ppData);

	pData->nVer		= 101;

	pData->vecPos.x = getX( &m_vJumpPos);
	pData->vecPos.y = getY( &m_vJumpPos);
	pData->vecPos.z = getZ( &m_vJumpPos);

	return sizeof( pack::OBJJUMPER);
}

UINT32 CGameObjectJumper::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc;
	i3Stream * pStream = pInfo->getStream();

	pack::OBJJUMPER info;
	
	Rc = pStream->Read( &info, sizeof( pack::OBJJUMPER));
	I3_CHKIO( Rc);

	setX( &m_vJumpPos, info.vecPos.x);
	setY( &m_vJumpPos, info.vecPos.y + 0.3f);	// 2009.06.15 jump Position을 30cm 올립니다. dfly79
	setZ( &m_vJumpPos, info.vecPos.z);

	return Rc;
}

struct ALIGN4 I3_PERSIST_JUMPER
{
	VEC3D		pos;
	UINT32		pad[4];
} ;

UINT32 CGameObjectJumper::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = i3Object::OnSave(pResFile);
	I3_CHKIO(result);

	I3_PERSIST_JUMPER data;
	data.pos = m_vJumpPos;

	rc = pResFile->Write(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectJumper::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_JUMPER data;

	result = i3Object::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&data, sizeof(data));
	I3_CHKIO(rc);
	result += rc;

	m_vJumpPos = data.pos;

	return result;
}

#if defined(I3_DEBUG)//XML
bool	CGameObjectJumper::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3Vector::SetToXML( pXML, "ObjEventType", &m_vJumpPos);

	return true;
}

bool	CGameObjectJumper::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3Vector::GetFromXML( pXML, "ObjEventType", &m_vJumpPos);

	return true;
}
#endif
