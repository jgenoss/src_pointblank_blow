#include "i3CommonType.h"
#include "i3InstanceProxy.h"
#include "i3String.h"
#include "i3ResourceFile.h"
#include "itl/vector.h"
#include "itl/vector_util.h"
#include "itl/algorithm/find.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3InstanceProxy);

i3InstanceProxy::~i3InstanceProxy(void)
{
	I3_SAFE_RELEASE( m_pInst);
}

void	i3InstanceProxy::SetProxyClassName( const char * pszName)
{
	m_strProxyClass = pszName; 
}

void	i3InstanceProxy::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	i3PersistantElement::OnQueryStrings( StrList);

	if( !m_strProxyClass.empty() )
	{
		StrList.push_back(m_strProxyClass);
	}
}

void	i3InstanceProxy::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pInst != nullptr)
	{
		m_pInst->OnBuildObjectList( List);

		// m_pInst 자체는 List에서 제거한다.
		// 이것은 Loading 시에 Proxy Class로 대체 생성하기 위한 방법이다.
		// 그럼에도 불구하고 m_pInst::OnBuildObjectList() 함수를 호출하는 이유는
		// 해당 Class가 내포하고 있을 다른 Object들은 저장되어야 하기 때문이다.
		i3::vector<i3PersistantElement*>::iterator it =	i3::find(List.begin(), List.end(), m_pInst);
		if (it != List.end() ) List.erase(it);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 INSTANCE_PROXY
	{
		UINT8		m_ID[4] = { 'I', 'P', 'C', '1' };
		INT32		m_idxProxyClassName = 0;
		UINT32		pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3InstanceProxy::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	pack::INSTANCE_PROXY data;

	I3ASSERT( m_pInst != nullptr);

	data.m_idxProxyClassName = i3::vu::int_index_of( pResFile->GetStringTable(), m_strProxyClass);

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
	pack::INSTANCE_PROXY data;

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// Instance Proxy는 Loading 중에 사라질 것이기 때문에
	// Name 등을 저장하기 위해 메모리를 할당하지 않는다.

	{
		i3::rc_string	strProxyClass = pResFile->GetStringTable()[ data.m_idxProxyClassName];
		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(strProxyClass);
		I3ASSERT( pMeta != nullptr);
		m_pInst = (i3PersistantElement *) pMeta->create_instance();
		I3_MUST_ADDREF( m_pInst);
	}

	rc = m_pInst->OnLoad( pResFile);
	I3_CHKIO( rc);
	result += rc;

	return result;
}
