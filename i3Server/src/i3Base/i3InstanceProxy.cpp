#include "i3CommonType.h"
#include "i3InstanceProxy.h"
#include "i3String.h"
#include "i3List.h"
#include "i3ResourceFile.h"
#include "itl/vector.h"

I3_CLASS_INSTANCE( i3InstanceProxy, i3PersistantElement);

i3InstanceProxy::i3InstanceProxy(void)
{
	m_pszProxyClass = NULL;
	m_pInst = NULL;
}

i3InstanceProxy::~i3InstanceProxy(void)
{
	I3_SAFE_RELEASE( m_pInst);
	I3MEM_SAFE_FREE( m_pszProxyClass);
}

void	i3InstanceProxy::SetProxyClassName( const char * pszName)
{
	I3MEM_SAFE_FREE( m_pszProxyClass);
	
	if( pszName != NULL)
	{
		UINT32 len = i3String::Length( pszName);

		m_pszProxyClass = (char *) i3MemAlloc( len + 1);

		i3String::Copy( m_pszProxyClass, pszName, len + 1);
	}
}

void	i3InstanceProxy::OnQueryStrings( i3StringList * pStrList)
{
	i3PersistantElement::OnQueryStrings( pStrList);

	if( m_pszProxyClass != NULL)
	{
		pStrList->Add( m_pszProxyClass);
	}
}

void	i3InstanceProxy::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pInst != NULL)
	{
		m_pInst->OnBuildObjectList( List);

		// m_pInst 자체는 List에서 제거한다.
		// 이것은 Loading 시에 Proxy Class로 대체 생성하기 위한 방법이다.
		// 그럼에도 불구하고 m_pInst::OnBuildObjectList() 함수를 호출하는 이유는
		// 해당 Class가 내포하고 있을 다른 Object들은 저장되어야 하기 때문이다.
		//pList->Remove( m_pInst);
		i3::vector<i3PersistantElement*>::iterator it =	i3::find(List.begin(), List.end(), m_pInst);
		if (it != List.end() ) List.erase(it);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistInstanceProxy
{
	UINT8		m_ID[4];
	INT32		m_idxProxyClassName;
	UINT32		pad[2];
} I3_PERSIST_INSTANCE_PROXY;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3InstanceProxy::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	I3_PERSIST_INSTANCE_PROXY data;

	I3ASSERT( m_pInst != NULL);
	I3ASSERT( m_pszProxyClass != NULL);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "IPC1", 4);

	data.m_idxProxyClassName =		pResFile->GetStringTable()->IndexOf( m_pszProxyClass);
	I3ASSERT( data.m_idxProxyClassName != -1);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// Class 자체는 저장하지 않으면서 OnSave는 호출한다.
	// Loading 시에는 Proxy Class를 생성하고, 그 Proxy Instance의 OnLoading을 호출한다.
	// 올바르게 동작하기 위해서는 현재 Instance와 Proxy Instance의 OnLoading이 동일하게
	// 동작할 필요가 있다.
	rc = m_pInst->OnSave( pResFile);
	I3_CHKIO( rc);
	result += rc;

	return result;

}

UINT32	i3InstanceProxy::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	I3_PERSIST_INSTANCE_PROXY data;

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// Instance Proxy는 Loading 중에 사라질 것이기 때문에
	// Name 등을 저장하기 위해 메모리를 할당하지 않는다.

	{
		char * pszProxyClass = pResFile->GetStringTable()->Items[ data.m_idxProxyClassName];
		i3ClassMeta * pMeta;

		pMeta = i3ClassMeta::FindClassMetaByName( pszProxyClass);
		I3ASSERT( pMeta != NULL);


		m_pInst = (i3PersistantElement *) CREATEINSTANCE( pMeta);
		I3ASSERT( m_pInst != NULL);

		m_pInst->AddRef();
	}

	rc = m_pInst->OnLoad( pResFile);
	I3_CHKIO( rc);
	result += rc;

	return result;
}
