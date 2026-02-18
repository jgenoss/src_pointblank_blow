#include "stdafx.h"
#include "i3LevelWorld.h"
#include "i3LevelResWorld.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelWorld);

i3LevelWorld::i3LevelWorld(void)
{
	i3::safe_string_copy( m_szInstanceClass, "i3World", 128);
}

void i3LevelWorld::BindRes( i3LevelRes * pRes)
{
	I3ASSERT( i3::kind_of<i3LevelResWorld*>(pRes));

	i3LevelElement3D::BindRes( pRes);
	i3LevelResWorld * pWorldRes = (i3LevelResWorld *) pRes;

	// World는 Clone을 생성하지 않고 직접 참조한다.
	// Chara나 Object와는 달리, 해당 Resource에 대한 편집이 직접 Level Tool에서
	// 이루어지기 때문...
	m_pWorld = pWorldRes->getWorld();

	setRefObject( m_pWorld);

	if( hasName())
	{
		m_pWorld->SetName( GetName());
	}
	
	SetSymbol( m_pWorld->getSceneObject());

	if( m_pWorld->getSceneObject() != nullptr ) 
	{
		i3Scene::ModifyDynamicState( m_pWorld->getSceneObject(), I3_PHYSIX_STATE_DISABLE, 0 );
	}

	if((m_pWorld->getWorldCollision() != nullptr) && (m_pWorld->getWorldCollision()->getInstanceSg() != nullptr))
	{
		i3Scene::ModifyDynamicState( m_pWorld->getWorldCollision()->getInstanceSg(), I3_PHYSIX_STATE_DISABLE, 0 );
	}

	if((m_pWorld->getHitCollision() != nullptr) && (m_pWorld->getHitCollision()->getInstanceSg() != nullptr))
	{		
		i3Scene::ModifyDynamicState( m_pWorld->getHitCollision()->getInstanceSg(), 0, I3_PHYSIX_STATE_DISABLE );
	}
}

i3GameObj *	i3LevelWorld::CreateGameInstance(void)
{
	I3_MUST_ADDREF(m_pWorld);

	return m_pWorld;
}

void i3LevelWorld::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelElement3D::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of< i3LevelWorld* >(pDest));

	i3LevelWorld * pLVWorld = (i3LevelWorld*)pDest;

	if( method == I3_COPY_REF)
	{
		pLVWorld->setWorld( getWorld());
	}
	else
	{
		if( m_pWorld == nullptr)
			return;

		//특이하지만 월드는 resource에있는것을 가져다 쓴다.
		//따라서 새로 생성하지않고 리소스의 정보를 변경한다.
		i3World * pWorld = pLVWorld->getWorld();

		m_pWorld->CopyTo( pWorld, I3_COPY_INSTANCE);

		I3_MUST_ADDREF(pWorld);
	}
}

void	i3LevelWorld::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pWorld != nullptr)
	{
		m_pWorld->SetExternRes( FALSE);
		m_pWorld->OnBuildObjectList( List);
	}

	i3LevelElement3D::OnBuildObjectList( List);
}


void		i3LevelWorld::OnInstantiate()
{
	if( (m_pWorld->getHitCollision() != nullptr) && (m_pWorld->getHitCollision()->getInstanceSg() != nullptr) )
	{
		i3MatrixStack mtxStack;
		I3_ONINSTANCING_INFO info;
		info._pBase			= this;
		info._pMatrixStack	= &mtxStack;

		MATRIX mtx;
		mtxStack.PushPre( &mtx, GetTransform() );

		m_pWorld->getHitCollision()->getInstanceSg()->OnInstancing( &info );

		mtxStack.Pop();
	}
	else
	{
		i3LevelElement3D::OnInstantiate();
	}
}

void		i3LevelWorld::OnQueryCollisionNode( i3::vector<i3Node*>& List)
{
	//i3LevelElement3D::OnQueryCollisionNode( List );

	//if( m_pWorld->getWorldCollision() != nullptr)
	//	List.push_back( m_pWorld->getWorldCollision()->getInstanceSg());

	if( m_pWorld->getHitCollision() != nullptr)
	{
		List.push_back( m_pWorld->getHitCollision()->getInstanceSg() );
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 LEVEL_WORLD
	{
		UINT8		m_ID[4] = { 'T', 'W', 'L', '1' };
		OBJREF		m_pWorld = 0;
		UINT32		m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelWorld::OnSave( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	pack::LEVEL_WORLD Info;

	Result = i3LevelElement3D::OnSave( pFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
		
	Info.m_pWorld = (OBJREF)pFile->GetObjectPersistID( m_pWorld);

	Rc  = pFile->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelWorld::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( (IsExternRes() == FALSE) && (m_pWorld != nullptr))
	{
		m_pWorld->SetExternRes();
	}

	return Result;
}

UINT32	i3LevelWorld::OnLoad( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	pack::LEVEL_WORLD Info;

	Result = i3LevelElement3D::OnLoad( pFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pFile->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelWorld::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( Info.m_pWorld > 0)
	{
		i3World * pWorld = (i3World*)pFile->FindObjectByID( (OBJREF)Info.m_pWorld);

		setWorld( pWorld);
	}

	return Result;
}

void	i3LevelWorld::OnAttachScene( i3LevelScene * pScene )
{
	if((m_pWorld->getHitCollision() != nullptr) && (m_pWorld->getHitCollision()->getInstanceSg() != nullptr))
	{		
		i3Scene::ModifyDynamicState( m_pWorld->getHitCollision()->getInstanceSg(), 0, I3_PHYSIX_STATE_DISABLE);
	}
}

void	i3LevelWorld::OnDetachScene( i3LevelScene * pScene )
{
	if((m_pWorld->getHitCollision() != nullptr) && (m_pWorld->getHitCollision()->getInstanceSg() != nullptr))
	{		
		i3Scene::ModifyDynamicState( m_pWorld->getHitCollision()->getInstanceSg(), I3_PHYSIX_STATE_DISABLE, 0 );
	}
}