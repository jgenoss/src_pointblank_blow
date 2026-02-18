#include "i3FrameworkPCH.h"
#include "i3GameResWorld.h"

I3_CLASS_INSTANCE( i3GameResWorld);

i3GameResWorld::i3GameResWorld()
{
	m_Type	= I3_GAMERES_WORLD;
}

i3GameResWorld::~i3GameResWorld()
{
	I3_SAFE_RELEASE( m_pWorld);
}

void	i3GameResWorld::Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone)
{
	i3GameResSceneGraph::Bind( pFramework, pObj, bInstancing, bClone);
}

void i3GameResWorld::Destroy( void)
{
	I3_SAFE_RELEASE( m_pWorld);
}

void	i3GameResWorld::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameResSceneGraph::CopyTo( pDest, method);

	i3GameResWorld * pWorldRes = (i3GameResWorld *) pDest;

	if( method == I3_COPY_REF)
	{
		pWorldRes->setWorld( getWorld());
	}
	else
	{
		i3World * pWorld = i3World::new_object_ref();

		getWorld()->CopyTo( pWorld, method);

		pWorldRes->setWorld( pWorld);
	}
}

void	i3GameResWorld::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pWorld != nullptr)
	{
		m_pWorld->OnBuildObjectList( List);
	}

	i3GameResSceneGraph::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED WORLD
	{
		UINT8		m_ID[4] = { 'W', 'R', 'D', '1' };
		OBJREF64	m_pWorld = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameResWorld::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::WORLD world;

	Result = i3GameResSceneGraph::OnSave( pResFile);
	I3_CHKIO( Result);
	
	world.m_pWorld = (OBJREF64)pResFile->GetObjectPersistID( m_pWorld);

	Rc = pStream->Write( &world, sizeof( world));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResWorld::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result , Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::WORLD world;

	Result = i3GameResSceneGraph::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &world, sizeof( world));
	I3_CHKIO( Rc);
	Result += Rc;

	if( world.m_pWorld > 0)
	{
		i3World * pWorld = (i3World *)pResFile->FindObjectByID((OBJREF)world.m_pWorld);
		I3ASSERT( pWorld);

		setWorld( pWorld);
	}

	return Result;
}