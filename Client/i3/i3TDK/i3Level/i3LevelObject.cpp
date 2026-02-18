#include "stdafx.h"
#include "i3LevelObject.h"
#include "i3LevelResObject.h"

I3_CLASS_INSTANCE( i3LevelObject);

i3LevelObject::i3LevelObject(void)
{
	i3::safe_string_copy( m_szInstanceClass, "i3Object", 128);
}

i3LevelObject::~i3LevelObject(void)
{
	I3_SAFE_RELEASE( m_pObject );
}

void i3LevelObject::BindRes( i3LevelRes * pRes)
{
	I3ASSERT( i3::kind_of< i3LevelResObject* >(pRes));

	i3LevelElement3D::BindRes( pRes);
	i3LevelResObject * pObjRes = (i3LevelResObject *) pRes;

	i3Object * pObject = pObjRes->getObject();
	if( pObject == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "리소스에 Object가 존재하지 않습니다.");
		return;
	}

	if( getInstanceClassName()[0] != 0)
	{
		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta( getInstanceClassName());

		I3_SAFE_RELEASE( m_pObject);

		if( pMeta != nullptr)
		{
            m_pObject = (i3Object *)pMeta->create_instance();
			I3_MUST_ADDREF(m_pObject);
		}
		else
		{
			NotifyBox( nullptr, "%s Class가 존재하지 않습니다. i3Object Class로 대체합니다.", getInstanceClassName());

			m_pObject = (i3Object *) i3Object::new_object();
		}
	}
	else
	{
		m_pObject = (i3Object *) i3Object::new_object();
	}
	
	pObject->CopyTo( m_pObject, I3_COPY_INSTANCE);

	setRefObject( m_pObject);

	if( hasName())
	{
		m_pObject->SetName( GetName());
	}

	SetSymbol( m_pObject->getSceneObject());
}

i3GameObj * i3LevelObject::CreateGameInstance(void)
{
	I3_SAFE_ADDREF( m_pObject);

	return m_pObject;
}

void i3LevelObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelElement3D::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelObject* >(pDest));

	i3LevelObject * pObject = (i3LevelObject*)pDest;

	pObject->setObjType( getObjType());
	pObject->setInvisible( m_bInvisible);
	pObject->setObjTeamType( getObjTeamType());

	pObject->setJumpPos( getJumpPos());
}

void	i3LevelObject::OnChangeInstanceClassName(void)
{
	if((m_pObject != nullptr) && ( i3::generic_is_iequal( m_pObject->meta()->class_name(), getInstanceClassName()) == false))
	{
		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(getInstanceClassName());

		if( pMeta != nullptr)
		{
			i3Object * pNewObj = (i3Object *) pMeta->create_instance();	// CREATEINSTANCE( pMeta);

			if( pNewObj != nullptr)
			{
				m_pObject->CopyTo( pNewObj, I3_COPY_INSTANCE);

				// 새 Object로 교체
				I3_REF_CHANGE( m_pObject, pNewObj);
			}
			else
			{
				NotifyBox( nullptr, "%s Class 이름으로 Instance를 생성할 수 없습니다.", getInstanceClassName());
			}
		}
		else
		{
			NotifyBox( nullptr, "%s는 올바른 Class 이름이 아닙니다.", getInstanceClassName());
		}
	}
}

void	i3LevelObject::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	i3LevelElement3D::OnSetInstanceInfo( pInfo);

	pInfo->nParam1 = (UINT32)m_nObjType;
	pInfo->nParam2 = (UINT32)m_nObjTeamType;
	pInfo->nParam3 = (m_bInvisible)? 1 : 0;

	//pInfo->vJumpPos.x = getX( &m_vecJumpPos);
	//pInfo->vJumpPos.y = getY( &m_vecJumpPos);
	//pInfo->vJumpPos.z = getZ( &m_vecJumpPos);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 OBJECT
	{
		UINT8		m_ID[4] = { 'T', 'O', 'B', '1' };
		UINT32		m_nObjType = 0;
		UINT32		m_nObjTeamType = 0;
		UINT32		m_bInvisible = 0;
		i3::pack::VEC3D		m_JumpPos;		// 추가
		UINT32		m_Reserved[10] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelObject::OnSave( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	pack::OBJECT Info;

	Result = i3LevelElement3D::OnSave( pFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Info.m_nObjType		= (UINT32)m_nObjType;
	Info.m_nObjTeamType = (UINT32)m_nObjTeamType;
	Info.m_bInvisible	= (m_bInvisible)? 1 : 0;
	Info.m_JumpPos.x	= getX( &m_vecJumpPos);
	Info.m_JumpPos.y	= getY( &m_vecJumpPos);
	Info.m_JumpPos.z	= getZ( &m_vecJumpPos);

	Rc  = pFile->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelObject::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3LevelObject::OnLoad( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pFile->GetStream();
	pack::OBJECT Info;

	Result = i3LevelElement3D::OnLoad( pFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelObject::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_nObjType		= (I3LV_OBJECT_TYPE)Info.m_nObjType;
	m_nObjTeamType	= (I3LV_OBJECT_TEAM_TYPE)Info.m_nObjTeamType;
	m_bInvisible	= (Info.m_bInvisible)? true : false;

	setX( &m_vecJumpPos, Info.m_JumpPos.x);
	setY( &m_vecJumpPos, Info.m_JumpPos.y);
	setZ( &m_vecJumpPos, Info.m_JumpPos.z);

	return Result;
}

bool i3LevelObject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelElement3D::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->addAttr( "ObjType",	m_nObjType);
	pXML->addAttr( "TeamType",	m_nObjTeamType);
	pXML->addAttr( "Invisible",	m_bInvisible);
	
	i3Vector::SetToXML( pXML, "JumpPos", & m_vecJumpPos);

	return true;
}

bool i3LevelObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelElement3D::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->getAttr( "ObjType", (INT32 *) & m_nObjType);
	pXML->getAttr( "TeamType", (INT32 *) & m_nObjTeamType);
	pXML->getAttr( "Invisible", (INT32 *) & m_bInvisible);

	i3Vector::GetFromXML( pXML, "JumpPos", & m_vecJumpPos);
	return true;
}

