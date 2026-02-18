#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3Shader.h"
#include "i3ShaderGen.h"
#include "i3GfxGlobalVariable.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/itl/algorithm/sort.h"

#include "i3Base/string/ext/extract_directoryname.h"

I3_CLASS_INSTANCE( i3ShaderCache);

static i3ShaderCache *		s_pShaderCache = nullptr;

static UINT32 g_ISSVersion = 38;	// 엔진 셰이더 버전. Cache.i3GL 버전이 아님에 유의. 

/*
static INT32	_ShaderCompareProc( i3Shader * p1, i3Shader * p2)
{
	if( p1->getCode() > p2->getCode())
		return 1;
	else if( p1->getCode() < p2->getCode())
		return -1;

	return 0;
}

static INT32	_ShaderFindProc( i3Shader * p1, i3ShaderCode * p2)
{
	if( p1->getCode() > p2->getCode())
		return 1;
	else if( p1->getCode() < p2->getCode())
		return -1;

	return 0;
}
*/

i3ShaderCache::i3ShaderCache(void) 
{
	m_ShaderList.reserve( 256);

	#if defined( SPLIT_ACTIVE_SHADER)
		m_ActiveShaderList.reserve( 256);
	#endif

	s_pShaderCache = this;
	m_ISSVersion = GetRequestedISSVersion();
}

i3ShaderCache::~i3ShaderCache(void)
{
	RemoveAllShaders();
}

void i3ShaderCache::addShader( i3Shader * pShader)
{
	I3_MUST_ADDREF( pShader);
	pShader->setWorkDir( m_szWorkDir);

	m_ShaderList.insert( pShader );

	m_bModified = true;

	#if defined( SPLIT_ACTIVE_SHADER)
		// Shader를 Add만 하는 것으로는 Active Shader List에 추가되지 않는다.
	#endif
}

void i3ShaderCache::RemoveShader( i3Shader * pShader)
{
	//INT32 idx;

	I3ASSERT( pShader != nullptr);

	// idx = m_ShaderList.IndexOf( pShader);
	i3::pair<i3::vector_multiset<i3Shader*, ShaderComp>::iterator, i3::vector_multiset<i3Shader*, ShaderComp>::iterator> pr = 
		m_ShaderList.equal_range( pShader );

	i3::vector_multiset<i3Shader*, ShaderComp>::iterator it = i3::find(pr.first, pr.second, pShader);			// 선형검색해야하는듯 하다..
	
	I3ASSERT( it != m_ShaderList.end() );

	m_ShaderList.erase( it );

	#if defined( SPLIT_ACTIVE_SHADER)
	{
		i3::vector_set<i3Shader*, ShaderComp>::iterator it = m_ActiveShaderList.find(pShader);

		if( it != m_ActiveShaderList.end() )
		{
			m_ActiveShaderList.erase( it);

			I3_MUST_RELEASE( pShader);
		}
	}
	#endif

	I3_MUST_RELEASE( pShader);

	m_bModified = true;
}

void i3ShaderCache::RemoveAllShaders(void)
{
	

	i3::cu::for_each_release_clear(m_ShaderList, &i3Shader::Release);


	#if defined( SPLIT_ACTIVE_SHADER)
				
		i3::cu::for_each_release_clear(m_ActiveShaderList, &i3Shader::Release);
	#endif

}

i3Shader*			i3ShaderCache::FindShaderByName( const char * pszName) const 
{
	i3ShaderCode code;

	code.SetName( pszName);

	return FindShaderByCode( &code);
}

i3Shader*			i3ShaderCache::FindShaderByCode( i3ShaderCode * pCode) const
{
//	m_ShaderList.SetCompareProc( (COMPAREPROC) _ShaderFindProc);
//	INT32 idx = m_ShaderList.FindItem( pCode);
//	m_ShaderList.SetCompareProc( (COMPAREPROC) _ShaderCompareProc);
	i3::pair<i3::vector_multiset<i3Shader*, ShaderComp>::const_iterator, i3::vector_multiset<i3Shader*, ShaderComp>::const_iterator> pr
		= m_ShaderList.equal_range_other( pCode );

	if ( pr.first != pr.second )
	{
		i3Shader* pTarget = *(pr.first);

	#if defined( SPLIT_ACTIVE_SHADER)
	//if( idx != -1)
	
		// 동일한 이름의 여러 버전이 존재할 수 있기 때문에...
		// 우선 동일한 이름을 가진 Shader들의 처음을 찾는다.
		// 2012.05.23.수빈  equal_range로 동일값구간을 잡는걸로 대신한다..
		// 동일한 code 값이 반복되는 동안 현재 GPU에 가장 적합한 Shader를 찾는다.
		i3Shader* pShader;
	
		i3GfxCaps * pCaps = i3RenderContext::GetCaps();
		I3G_GPU_VENDOR	lastVendor = I3G_GPU_VENDOR_NA;
		
		for( i3::vector_multiset<i3Shader*, ShaderComp>::const_iterator it = pr.first; it != pr.second ; ++it)
		{
			pShader = *it;

			if( pShader->getTargetGPU() == pCaps->getGPUVendor())
			{
				pTarget = pShader;
				lastVendor = pShader->getTargetGPU();
			}
			else if( (pShader->getTargetGPU() == I3G_GPU_VENDOR_STANDARD) && ( lastVendor == I3G_GPU_VENDOR_NA))
			{
				// 아직 특정 GPU에 대한 Shader가 발견되지 않은 상태에서
				// 표준 Shader Profile로 생성된 Shader가 있다면 그것을 찾아둔다.
				pTarget = pShader;
				lastVendor = I3G_GPU_VENDOR_STANDARD;
			}
		}
	#endif

		return pTarget;
	}

	return  nullptr;
}

UINT64			i3ShaderCache::_getDowngradeShader( UINT64 code)
{
	// HW Caps의 여부에 맞추기 위해 한단계 낮은 Shader Code를 검색해준다.

	i3ShaderCode temp;

	temp.setCode( code);

	// Fresnel Term을 제거
	if( temp.getFresnelEnable())
	{
		temp.setFresnelEnable( false);
		return temp.getCode();
	}

	if( temp.getLightingEnable())
	{
		// Light가 2개 이상인 경우, 1개를 제거해본다.
		if( temp.getLightType( 1) != I3G_SHADER_FLAG_LIGHT_TYPE_NONE)
		{
			temp.setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_NONE);

			return temp.getCode();
		}

		// 그림자가 있다면 제거해본다.
		if( temp.getShadowType( 0) != I3G_SHADER_FLAG_SHADOW_NONE)
		{
			temp.setShadowType( 0, I3G_SHADER_FLAG_SHADOW_NONE);

			return temp.getCode();
		}
	}

	// Specular Map을 제거해본다.
	if( temp.getSpecularMapEnable())
	{
		temp.setSpecularMapEnable( false);
		return temp.getCode();
	}

	// Normal Map을 제거해본다.
	if( temp.getNormalMapEnable())
	{
		temp.setNormalMapEnable( false);
		return temp.getCode();
	}

	// PPL을 제거해본다.
	if( temp.isPPLShader())
	{
		temp.setPPLShader( false);

		return temp.getCode();
	}

	if( temp.getReflectMaskMapEnable())
	{
		temp.setReflectMaskMapEnable( false);

		return temp.getCode();
	}

	if (temp.getEmissiveMapEnable() )
	{
		temp.setEmissiveMapEnable(false);
		return temp.getCode();
	}

	if( temp.getReflectMapEnable())
	{
		temp.setReflectMapEnable( true);

		return temp.getCode();
	}

	if( temp.getXrayColorEnable() )
	{
		temp.setXrayColorEnable( false);
		return temp.getCode();
	}

	temp.setLightingEnable( 0);

	return temp.getCode();
}

i3Shader *		i3ShaderCache::_genShader( i3ShaderCode * pCode)
{
	i3Shader * pShader;

	// 없다면 Generate를 한다.
	pShader = i3Shader::new_object_ref();

	char szName[ 256];

	pCode->GenDefName( szName, sizeof( szName)-1);

	pShader->SetName( szName);
	pShader->setISSVersion( GetRequestedISSVersion());

	if( pShader->Create( pCode, m_szWorkDir) == false)
	{
		// 생성에 문제가 발생한 경우에도 우선 반환한다.
	}

	return pShader;
}

i3Shader *		i3ShaderCache::_findDowngradeShader( i3ShaderCode * pCode, i3RenderContext * pCtx)
{
	i3ShaderCode code;
	i3Shader * pShader = nullptr;
	bool bResult = false;

	code.setCode( pCode->getCode());

	while( (bResult == false) && (code.getCode() != 0))
	{
		code.setCode( _getDowngradeShader( code.getCode()));

		// 우선 해당 Code의 Shader가 있는지 검색
		pShader = FindShaderByCode( &code);

		if( pShader == nullptr)
		{
			pShader = _genShader( &code);
			
			// 생성에 실패한 경우에도 Shader DB에는 넣어둔다.
			// 그것은 다시 생성하는 경우가 없도록 하기 위함이다.
			// 단, 이것은 Cache 파일에 저장되지는 않는다.
			addShader( pShader);
		}

		if( pShader->isAllCompiled())
		{
			bResult = pShader->Validate( pCtx);
		}
	}

	return pShader;
}

#if defined( SPLIT_ACTIVE_SHADER)
i3Shader* i3ShaderCache::FindActiveShaderByCode( i3ShaderCode * pCode)
{
	i3::vector_set<i3Shader*, ShaderComp>::iterator it = m_ActiveShaderList.find_other(pCode);
	return (it != m_ActiveShaderList.end() ) ? *it : nullptr;
}
#endif

i3Shader *		i3ShaderCache::Instanciate( i3ShaderCode * pCode)
{
	i3Shader * pShader, * pTempShader;
	bool bRv = false;
	bool bNew = false;
	
	pShader = nullptr;

	#if defined( SPLIT_ACTIVE_SHADER)
		pShader = FindActiveShaderByCode( pCode);

		if (pShader == nullptr)
		{
			pShader = FindShaderByCode( pCode);

			if( pShader != nullptr)
			{
				m_ActiveShaderList.insert( pShader);
				I3_MUST_ADDREF( pShader);
			}
		}
	#else
		// 우선 해당 Code의 Shader가 있는지 검색
		pShader = FindShaderByCode( pCode);

	#endif

	if( pShader != nullptr)
	{
		if( pShader->getDowngradeShader() != nullptr)
			pShader = pShader->getDowngradeShader();
	}
	else
	{
		pShader = _genShader( pCode);

		addShader( pShader);

		#if defined( SPLIT_ACTIVE_SHADER)
		I3_SAFE_ADDREF( pShader);
		m_ActiveShaderList.insert(pShader);
		#endif
	}

	// pShader->getISSVersion() : Cache.i3GL 파일에 들어있는 바이너리 데이터 버전.
	// GetRequestedISSVersion() : 엔진소스 코드에 박혀있는 셰이더 캐시 버전.
	// 결론적으로 Cache.i3GL 파일의 셰이더 바이너리 데이터를 새로 갱신하려면 엔진의 셰이더 캐시 버전을 높이면 된다.
	if( pShader->getISSVersion() < GetRequestedISSVersion())
	{
		// 이 Shader는 새로 Build해야 하는 놈이다.
		pShader->Rebuild();

		m_bModified = true;
	}

	// 만약 성능이 문제가 되어 Shader를 Validate하지 못했던 shader라면
	// Downgraded Shader로 바꾸어준다.
	if( pShader->isAllCompiled())
	{
		bRv = pShader->Validate( m_pCtx);
	}

	if( bRv == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[GFX] Shader Downgrading!!! : %0016XL\n", pCode->getCode());

		pTempShader = _findDowngradeShader( pCode, m_pCtx);

		pShader->setDowngradeShader( pTempShader);
		pShader = pTempShader;

		bNew = false;
	}

	FlushCacheFile();

	return pShader;
}


void i3ShaderCache::Sort(void)
{
//	m_ShaderList.Sort( (COMPAREPROC) _ShaderCompareProc);

	i3::sort(m_ShaderList.begin(), m_ShaderList.end(), ShaderComp() );

}


void	i3ShaderCache::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{

	i3Shader * pShader;

	i3::vector_multiset<i3Shader*, ShaderComp>::iterator it_end = m_ShaderList.end();
	for ( i3::vector_multiset<i3Shader*, ShaderComp>::iterator it = m_ShaderList.begin() ; it != it_end ; ++it)
	{
		pShader = *it;
		pShader->OnBuildObjectList( List);
	}

/*
	for( i = 0; i < getShaderCount(); i++)
	{
		pShader = getShader( i);

		pShader->OnBuildObjectList( List);
	}
*/

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct I3_SHADER_CACHE_INFO
{
	UINT8		m_ID[4] = { 'S', 'H', 'C', '2' };
	UINT32		m_Count = 0;
	UINT32		m_ISSVersion = 0;
	UINT32		pad[31] = { 0 };
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3ShaderCache::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	I3_SHADER_CACHE_INFO data;

	data.m_Count		= (INT32)m_ShaderList.size();
	data.m_ISSVersion	= getISSVersion();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	i3::vector<i3Shader*> tempList;

	// 실제로 저장될 List를 만든다.
	// Validate시 실패한 Shader들은 저장하지 않는다.
	for( size_t i = 0; i < m_ShaderList.size(); i++)
	{
		i3Shader * pShader = m_ShaderList.get_vector()[i];

		bool bValidVS = pShader->isValidated( I3G_SHADER_TYPE_VERTEX);
		bool bValidPS = pShader->isValidated( I3G_SHADER_TYPE_PIXEL);
		bool bLoaded = (pShader->getState() & I3G_SHADER_STATE_LOADED) != 0;

		if( ( bValidVS && bValidPS) || bLoaded)
			tempList.push_back( pShader);
	}

	I3TRACE( "Total %d shaders are serialized.\n", (INT32)tempList.size());

	rc = SaveFromListToResourceFile(tempList, pResFile);		// tempList.SaveTo( pResFile);
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3ShaderCache::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	

	I3_SHADER_CACHE_INFO data;

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setISSVersion( data.m_ISSVersion);

	{
		i3::vector<i3Shader*> v;
		v.reserve( 64);
		rc = LoadFromResourceFileToList(v, pResFile, true);
		m_ShaderList.clear();
		
		for (size_t i = 0 ; i <v.size() ; ++i )
		{
			m_ShaderList.insert( v[i] );
		}
	//	rc = m_ShaderList.LoadFrom( pResFile, true);
		I3_CHKIO( rc);
		result += rc;
	}

//	i3String::SplitPath( pResFile->getFileName(), m_szWorkDir);
	i3::extract_directoryname(pResFile->getFileName(), m_szWorkDir);

	for(size_t i = 0; i < m_ShaderList.size(); i++)
	{
		i3Shader * pShader = m_ShaderList.get_vector()[i];
		pShader->setWorkDir( m_szWorkDir);
		pShader->addState( I3G_SHADER_STATE_LOADED);
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////
i3Shader *	i3ShaderCache::LoadShader(const char * pszPath, I3G_SHADER_VERSION versionVS, I3G_SHADER_VERSION versionPS, I3G_GPU_VENDOR target, bool bAddCache)
{
	// 우선 같은 이름으로 존재하는지 확인
	i3Shader * pShader = nullptr;
	bool bNeedToAdd = false;

#if defined( I3_DEBUG)
	if( bAddCache)
#endif
	{
		pShader = s_pShaderCache->FindShaderByName( pszPath);
	}

	if( pShader != nullptr)
	{
		if( pShader->getISSVersion() < GetRequestedISSVersion())
		{
			pShader->SetName( pszPath);
			pShader->setTargetGPU( target);
			pShader->Rebuild();

			s_pShaderCache->m_bModified = true;
			s_pShaderCache->FlushCacheFile();
		}
	}
	else
	{
		pShader = i3Shader::new_object_ref();

		pShader->setTargetGPU( target);

		if( pShader->Create( s_pShaderCache->m_szWorkDir, pszPath, 0, versionVS, versionPS) == false)
		{
			I3PRINTLOG(I3LOG_WARN,  "Could not compile %s shader.", pszPath);
			I3_SAFE_ADDREF( pShader);
			I3_SAFE_RELEASE( pShader);
			return nullptr;
		}

		bNeedToAdd = true;
	}

	if( pShader->Validate( g_pRenderContext) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not validate %s shader.", pszPath);
		I3_SAFE_ADDREF( pShader);
		I3_SAFE_RELEASE( pShader);
		return nullptr;
	}

	if( bAddCache && bNeedToAdd)
	{
		s_pShaderCache->addShader( pShader);

		s_pShaderCache->FlushCacheFile();
	}

	return pShader;
}

i3Shader *	i3ShaderCache::CloneShader( i3Shader * pShader, I3G_GPU_VENDOR target)
{
	i3Shader * pNew;
	bool rv;

	pNew = i3Shader::new_object_ref();

	pNew->setTargetGPU( target);
	pNew->SetName( pShader->GetNameString());

	if( pShader->isCustomShader())
	{
		rv = pNew->Create( (char *) s_pShaderCache->m_szWorkDir, (char *) pShader->GetName(), pShader->getStyle(), 
					pShader->getRequestedVSVersion(), pShader->getRequestedPSVersion());
	}
	else
	{
		rv = pNew->Create( pShader->getShaderCode(), s_pShaderCache->m_szWorkDir, pShader->getStyle(), 
					pShader->getRequestedVSVersion(), pShader->getRequestedPSVersion());
	}

	if( rv == false)
	{
		I3_MUST_RELEASE( pNew);
		pNew = nullptr;
	}

	return pNew;
}

i3ShaderCache *		i3ShaderCache::Load( const char * pszPath)
{
	i3ResourceFile file;
	i3ShaderCache * pCache;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s Shader Cache 파일을 읽어 들이지 못했습니다.", pszPath);
		return nullptr;
	}

	pCache = (i3ShaderCache *) file.getKeyObject();

	if( pCache == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s Shader Cache 파일을 읽어 들이지 못했습니다.", pszPath);
		return nullptr;
	}

	I3_MUST_ADDREF( pCache);

	return pCache;
}

bool i3ShaderCache::Save( const char * pszPath)
{
	i3ResourceFile file;

	file.setKeyObject( this);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s shader cache 파일을 저장할 수 없습니다.", pszPath);
		return false;
	}

	return true;
}

i3ShaderCache *		i3ShaderCache::LoadCacheFile( char * pszWorkDir)
{
	char szPath[ MAX_PATH];

	sprintf( szPath, "%s/Cache.i3GL", pszWorkDir);

	i3ShaderCache * pCache = nullptr;

	if( i3FileStream::isFile( szPath))
	{
		i3ResourceFile file;

		if( file.Load( szPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "%s Shader Cache 파일을 읽어 들이지 못했습니다.", szPath);
			return nullptr;
		}

		pCache = (i3ShaderCache *) file.getKeyObject();
		I3_SAFE_ADDREF( pCache);
	}

	if( pCache == nullptr)
	{
		// 해당 파일이 없다. 새로 생성.
		// 읽어 들이지 못했다면 빈 Shader Cache Class를 생성
		pCache = i3ShaderCache::new_object();
	}

	pCache->setWorkDir( pszWorkDir);

	return pCache;
}

bool i3ShaderCache::SaveCacheFile(void)
{
	i3ResourceFile file;
	char szPath[ MAX_PATH];

	sprintf( szPath, "%s/Cache.i3GL", m_szWorkDir);

	file.setKeyObject( this);

	if( file.Save( szPath) == STREAM_ERR)
	{
		//I3PRINTLOG(I3LOG_FATAL,  "%s Shader Cache 파일을 저장할 수 없습니다.");
		return false;
	}

	return true;
}

void i3ShaderCache::FlushCacheFile(void)
{
	if( m_bModified == false)
		return;

	SaveCacheFile();

	m_bModified = false;
}

//static StringList *		s_pShaderSearchPathList = nullptr;
i3::vector<i3::rc_string>	s_ShaderSearchPathList;

void i3ShaderCache::AddShaderSearchPath( const char * pszPath)
{
	s_ShaderSearchPathList.push_back(pszPath);
}

void	i3ShaderCache::ResetShaderSearchPath(void)
{
	s_ShaderSearchPathList.clear();
//	I3_SAFE_RELEASE( s_pShaderSearchPathList);
}

INT32	i3ShaderCache::GetShaderSearchPathCount(void)
{
	return (INT32)s_ShaderSearchPathList.size();
}

const char *	i3ShaderCache::GetShaderSearchPath( INT32 idx)
{
	return s_ShaderSearchPathList[idx].c_str();
}

void i3ShaderCache::Dump(void)
{
	INT32 i;

	I3PRINTF( "--------------- Shaders Dump -------------------\n");

	for( i = 0; i < getShaderCount(); i++)
	{
		i3Shader * pShader = m_ShaderList.get_vector()[i];

		I3PRINTF( "SHADER[%04d] - %016i64X - %s\n", i, pShader->getCode(), pShader->GetName());
	}

	I3PRINTF( "--------------------------------------------\n");
}

UINT32		i3ShaderCache::GetRequestedISSVersion(void)	
{ 
	return g_ISSVersion;
}

void		i3ShaderCache::SetRequestedISSVersion(UINT32 ver)
{
	g_ISSVersion = ver;
}

