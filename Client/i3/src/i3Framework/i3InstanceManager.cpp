#include "i3FrameworkPCH.h"
#include "i3InstanceManager.h"


I3_CLASS_INSTANCE( i3InstanceManager);


i3InstanceManager::~i3InstanceManager()
{
	Destroy();
}

bool	i3InstanceManager::Create( i3ClassMeta * pMeta, INT32 nCount)
{
	I3ASSERT( pMeta != nullptr);

	bool	bRet = true;

	UINT32 unit = MAX( 16, nCount);

	m_pInstMeta = pMeta;

	m_InstList.reserve(unit);

	INT32 i;
	for( i = 0; i < nCount; ++i)
	{
		i3GameNode * pNode = _AddNewInstance();
		if( pNode == nullptr)
			bRet = false;
	}

	return bRet;
}

i3GameNode * i3InstanceManager::_AddNewInstance()
{
	I3ASSERT( m_pInstMeta != nullptr);

	i3GameNode * pNew = (i3GameNode *)	m_pInstMeta->create_instance(); 		// CREATEINSTANCE( m_pInstMeta);
	if( pNew != nullptr)
	{
		//I3TRACE("[i3InstanceManager::_AddNewInstance(): Created: %s\n", m_pInstMeta->GetClassName());
		I3_MUST_ADDREF( pNew);
		pNew->Create(nullptr);

		m_InstList.push_back( pNew);
		m_nAllocCount++;

		return pNew;
	}
	else
		return nullptr;
}

void	i3InstanceManager::Destroy()
{
	INT32 i;

	for( i = 0; i < m_nAllocCount; ++i)
	{
		i3GameNode * pNode = m_InstList[i];
		//I3TRACE("RefCount: %d\n", pNode->GetRefCount());
		I3_SAFE_RELEASE( pNode);
	}

	m_InstList.clear();
//	I3_SAFE_RELEASE( m_pInstList);
}

#if defined( I3_DEBUG)
void i3InstanceManager::Dump()
{
	
	for(size_t i = 0; i < m_InstList.size(); ++i)
	{
		i3GameNode * pNode = m_InstList[i];

		I3TRACE("%d: ref(%d):(%s)\n", i, pNode->GetRefCount(), pNode->GetName());
	}
}
#endif

void *	i3InstanceManager::GetFreeInstance()
{
	//I3TRACE("%s\t%d\t", m_pInstMeta->GetClassName(), m_nAllocCount);

	i3GameNode * pFreeNode = nullptr;
	INT32 i;
	for( i = 0; i < m_nAllocCount; ++i)
	{
		i3GameNode * pNew = m_InstList[i];
		if( pNew->GetRefCount() == 1 && pNew->same_of( m_pInstMeta))
		{
			pFreeNode = pNew;			
			break; 
		}
	}

	if( pFreeNode == nullptr && (( m_GameNodeStyle & I3_GAMENODE_STYLE_FIXEDCOUNT) == 0))
	{
		pFreeNode = _AddNewInstance();
		//I3TRACE("Added : %d", m_nAllocCount);
	}

	I3_SAFE_ADDREF( pFreeNode);

//	I3TRACE("\n");

	return pFreeNode;
}

void	i3InstanceManager::OnUpdate( REAL32 rDeltaSeconds)
{
}