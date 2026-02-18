#include "i3FrameworkPCH.h"
#include "i3GameResObject.h"


I3_CLASS_INSTANCE( i3GameResObject);

i3GameResObject::i3GameResObject()
{
	m_Type	= I3_GAMERES_OBJECT;
}

i3GameResObject::~i3GameResObject()
{
	I3_SAFE_RELEASE( m_pObject);
}

void	i3GameResObject::SetGameObject( i3Object * pObject)
{
	I3_REF_CHANGE( m_pObject, pObject);

	m_pObject->SetEnable(false);
}

void i3GameResObject::Destroy( void)
{
	I3_SAFE_RELEASE( m_pObject);
}

void	i3GameResObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	i3GameResObject * pObj = (i3GameResObject*)pDest;

	if( method == I3_COPY_REF)
	{
		pObj->SetGameObject( getGameObject());
	}
	else
	{
		i3Object * pObject = i3Object::new_object_ref();

		getGameObject()->CopyTo( pObject, method);

		pObj->SetGameObject( pObject);
	}
}

void	i3GameResObject::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pObject != nullptr)
	{
		m_pObject->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct GAMERES_OBJECT
	{
		UINT8		m_ID[4] = { 'G', 'R', 'O', '1' };
		OBJREF64	m_pObj = 0;
		UINT32		pad[3] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameResObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::GAMERES_OBJECT data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_pObj = (OBJREF64)pResFile->GetObjectPersistID( getGameObject());

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::GAMERES_OBJECT data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pObj > 0)
	{
		i3Object * pObj = (i3Object*)pResFile->FindObjectByID((OBJREF)data.m_pObj);
		I3ASSERT( pObj != nullptr);

		SetGameObject( pObj);
	}

	return Result;
}