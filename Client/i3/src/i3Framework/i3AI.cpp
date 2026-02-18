#include "i3FrameworkPCH.h"
#include "i3AI.h"
#include "i3Base/itl/algorithm_util.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/compare/generic_is_iless.h"


I3_CLASS_INSTANCE( i3AI);


bool operator<(const i3AIState& a1, const i3AIState& a2)
{
	if( a1.hasName() == false)
		return true;

	if( a2.hasName() == false)
		return false;

	return i3::generic_is_iless( a1.GetNameString(), a2.GetNameString());
}


struct _AIStateStrFindProc
{
	bool operator()( const i3AIState* p1, const char* pszName) const
	{
		if (p1->hasName() == false) return true;
		return i3::generic_is_iless(p1->GetNameString(), pszName);
	}
	bool operator()(const char* pszName, const i3AIState* p1 ) const
	{
		if (p1->hasName() == false) return false;
		return i3::generic_is_iless(pszName, p1->GetNameString());
	}

};



i3AI::~i3AI(void)
{
	RemoveAIStateAll();
	I3_SAFE_RELEASE( m_pDriver);
}

void i3AI::RemoveAIStateAll( void)
{
	INT32  i;

	for( i = 0; i < getAIStateCount(); i++)
	{  
		i3AIState * pAIState = getAIState( i);
		
		I3_SAFE_RELEASE( pAIState);
	}

	m_AIStateList.clear();
}

INT32 i3AI::AddAIState( i3AIState * pNew)
{
	I3_MUST_ADDREF( pNew);
	
	i3::vector_multiset<i3AIState*>::iterator it = m_AIStateList.insert(pNew);
	return	(INT32) i3::distance(m_AIStateList.begin(), it);
}

void i3AI::RemoveAIStateFromIndex( INT32 index)
{
	i3AIState * pState = getAIState( index);
	I3_MUST_RELEASE( pState);
	m_AIStateList.erase(m_AIStateList.begin() + index);
}

void i3AI::RemoveAIStateFromId( INT32 id)
{
	INT32 idx = FindAIStateByID( id);
	I3ASSERT( idx != -1);

	RemoveAIStateFromIndex( idx);		
}

void i3AI::RemoveAIState( i3AIState * pAIState)
{
	size_t sidx = i3::vu::index_of(m_AIStateList.get_vector(), pAIState);
	INT32 idx = (sidx != m_AIStateList.size()) ? INT32(sidx) : -1;
		// m_AIStateList.IndexOf( pAIState);
	I3ASSERT( idx != -1);

	RemoveAIStateFromIndex( idx);
}

INT32 	i3AI::FindAIStateByID( INT32 id)
{
	INT32 i;
	i3AIState * pAIState = nullptr;

	for( i = 0; i < getAIStateCount(); i++)
	{
		pAIState = getAIState( i);
		
		if( id == pAIState->getID())
		{
			return i;
		}		
	}

	return -1;
}

INT32	i3AI::FindAIStateByName( const char * pszName)
{
	
	//INT32 crc;

	// m_AIStateList.SetCompareProc( (COMPAREPROC) _AIStateStrFindProc);
	
	i3::pair<i3::vector_multiset<i3AIState*>::iterator, bool> pr = i3::au::binary_find(m_AIStateList.begin(), m_AIStateList.end(), pszName, _AIStateStrFindProc());
	
	if (pr.second == false) return -1;
	
	// i = m_AIStateList.FindItem( (void *) pszName);
	// m_AIStateList.SetCompareProc( (COMPAREPROC) _AIStateStrCompProc);
	return (INT32)i3::distance(m_AIStateList.begin(), pr.first);
}

void i3AI::setLuaSourceName( const char * pszPath)
{
	m_strLuaPath.clear();

	if( pszPath != nullptr)
	{
		m_strLuaPath = pszPath;
	}
}

void i3AI::setDriverMetaName( const char * pszPath)
{
	m_strDriverMetaName.clear();

	if( pszPath != nullptr)
	{
		m_strDriverMetaName = pszPath;
	}
}


void i3AI::Sort(void)
{
//	m_AIStateList.Sort( (COMPAREPROC) _AIStateStrCompProc);
	i3::sort(m_AIStateList.begin(), m_AIStateList.end(), i3::less<i3AIState*>());
}


////////////////////////////////////////////////////////////////////////////
void	i3AI::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3AIState * pState;

	if( IsExternRes() == false)
	{
		for( i = 0; i < getAIStateCount(); i++)
		{
			pState = getAIState( i);

			pState->OnBuildObjectList( List);
		}
	}

	i3ResourceObject::OnBuildObjectList( List);
}

void	i3AI::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	INT32 i;
	i3AIState * pState = nullptr;

	i3ResourceObject::CopyTo( pDest, method);

	i3AI * pAI = (i3AI *) pDest;

	pAI->setStyle( getStyle());
	
	if( getLuaSourceName())
	{
		pAI->setLuaSourceName( getLuaSourceName());
	}

	if( getDriverMetaName())
	{
		pAI->setDriverMetaName( getDriverMetaName());
	}

	pAI->setAIDriver( getAIDriver());

	for( i = 0; i < getAIStateCount(); i++)
	{
		pState = getAIState( i);
		if(pState == nullptr)
			continue;

		if( method == I3_COPY_REF)
		{
			pAI->AddAIState( pState);
		}
		else
		{
			i3AIState * pNewState = nullptr;

			INT32 idx = pAI->FindAIStateByName( pState->GetName());
			if( idx == -1)
			{
				pNewState = i3AIState::new_object_ref();
				pState->CopyTo( pNewState, method);
				pAI->AddAIState( pNewState);
			}
			else
			{
				pNewState = pAI->getAIState( idx);
				pState->CopyTo( pNewState, method);
			}
		}
	}

	if( method == I3_COPY_INSTANCE)
	{
		// 각 i3AIState들의 Next를 재조정해주어야 한다.
		for( i = 0; i < pAI->getAIStateCount(); i++)
		{
			pState = pAI->getAIState( i);

			if( pState->getNext() == nullptr)
				continue;

			INT32 idx = pAI->FindAIStateByName( pState->getNext()->GetName());
			I3ASSERT( idx != -1);

			pState->setNext( pAI->getAIState( idx));
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct AI
	{
		UINT8			m_ID[4] = { 'A', 'I', '_', '0' };
		UINT32			m_Style = 0;
		UINT32			m_LuaPathLength = 0;
		UINT32			m_DriverMetaNameLength = 0;
		UINT32			pad[5] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3AI::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	i3Stream * pStream = pResFile->GetStream();
	pack::AI data;

	result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_Style = getStyle();

	data.m_LuaPathLength = m_strLuaPath.length();	

	data.m_DriverMetaNameLength = m_strDriverMetaName.length();

	rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = SaveFromListToResourceFile(m_AIStateList.get_vector(), pResFile);			// m_AIStateList.SaveTo( pResFile);
	I3_CHKIO( rc);
	result += rc;

	if( data.m_LuaPathLength > 0)
	{
		rc = pResFile->Write( &m_strLuaPath[0], data.m_LuaPathLength);
		I3_CHKIO( rc);
		result += rc;
	}

	if( data.m_DriverMetaNameLength > 0)
	{
		rc = pResFile->Write( &m_strDriverMetaName[0], data.m_DriverMetaNameLength);
		I3_CHKIO( rc);
		result += rc;
	}


	return result;
}

UINT32	i3AI::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	i3Stream * pStream = pResFile->GetStream();
	pack::AI data;

	result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	{
		i3::vector<i3AIState*> vtemp;
		vtemp.reserve( 32);
//		rc = m_AIStateList.LoadFrom( pResFile, true);
		rc = LoadFromResourceFileToList(vtemp, pResFile, true);
		
		m_AIStateList.clear();
		m_AIStateList.insert(vtemp.begin(), vtemp.end());
				
		I3_CHKIO( rc);
		result += rc;

	}

	setStyle( data.m_Style);

	//m_AIStateList.Sort( (COMPAREPROC) _AIStateCompProc);

	I3ASSERT( data.m_LuaPathLength < 128);
	I3ASSERT( data.m_DriverMetaNameLength < 128);

	if( data.m_LuaPathLength > 0)
	{
		char conv[128];
		rc = pResFile->Read( conv, data.m_LuaPathLength);
		I3_CHKIO( rc);
		result += rc;

		conv[ data.m_LuaPathLength - 1] = 0;

		setLuaSourceName( conv);
	}

	if( data.m_DriverMetaNameLength > 0)
	{
		char conv[128];

		rc = pResFile->Read( conv, data.m_DriverMetaNameLength);
		I3_CHKIO( rc);
		result += rc;

		conv[ data.m_DriverMetaNameLength] = 0;
		setDriverMetaName( conv);

		{
			i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(conv); // ::FindClassMetaByName( conv);

			if( pMeta != nullptr)
			{
				i3AIDriver * pDriver = (i3AIDriver *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
				I3ASSERT( pDriver != nullptr);

				setAIDriver( pDriver);
			}
			else
			{
				//I3PRINTLOG(I3LOG_WARN,  "Could not create %s AI driver object.\n");
				I3TRACE( "Could not create %s AI driver object.", conv);
			}
		}
	}

	return result;
}

bool i3AI::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->addAttr("Style", getStyle());

	SaveFromListToXML(m_AIStateList.get_vector(), "AIStateList", pFile, pXML, true);			// m_AIStateList.SaveTo( pResFile);

	pXML->addAttr("LuaPath", &m_strLuaPath[0]);
	pXML->addAttr("DriverMetaName", &m_strDriverMetaName[0]);

	return true;
}

bool i3AI::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	UINT32 style;
	pXML->getAttr("Style", &style);
	setStyle(style);

	i3::vector<i3AIState*> vtemp;
	vtemp.reserve(32);
	LoadFromXMLToList(vtemp, "AIStateList", pFile, pXML, true);

	m_AIStateList.clear();
	m_AIStateList.insert(vtemp.begin(), vtemp.end());

	i3::rc_string str = pXML->getAttr("LuaPath");
	setLuaSourceName(str.c_str());

	{
		str = pXML->getAttr("DriverMetaName");
		setDriverMetaName(str.c_str());
		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(str.c_str()); // ::FindClassMetaByName( conv);

		if (pMeta != nullptr)
		{
			i3AIDriver * pDriver = (i3AIDriver *)pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			I3ASSERT(pDriver != nullptr);

			setAIDriver(pDriver);
		}
		else
		{
			//I3PRINTLOG(I3LOG_WARN,  "Could not create %s AI driver object.\n");
			I3TRACE("Could not create %s AI driver object.", m_strDriverMetaName);
		}
	}

	return true;
}