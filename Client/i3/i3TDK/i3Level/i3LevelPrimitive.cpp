#include "stdafx.h"
#include "i3LevelPrimitive.h"
#include "i3LevelResPrim.h"
#include "i3LevelGlobalVariable.h"
#include "i3Base/string/ext/extract_directoryname.h"

#include "i3Base/string/system/is_file.h"

I3_CLASS_INSTANCE( i3LevelPrimitive);

i3LevelPrimitive::i3LevelPrimitive(void)
{
	i3::safe_string_copy( m_szInstanceClass, "i3GameObj", 128);

	if( m_pRoot != nullptr)
	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( FALSE);
		m_pRoot->AddAttr( pAttr);
	}
}

i3LevelPrimitive::~i3LevelPrimitive(void)
{
	I3_SAFE_RELEASE(m_pEventRes);		// 이것 소거가 없어서, 일단 추가..(찾기 어려웠음)..2012.09.11.수빈
	I3_SAFE_RELEASE( m_pTexture);
	I3_SAFE_RELEASE( m_pGameObj);
}

void i3LevelPrimitive::BindRes( i3LevelRes * pRes)
{
	// Primitive들은 모두 Resource를 가지지 않도록 변경.
	// i3LevelElement3D::BindRes( pRes);

	if( i3::kind_of<i3LevelResEvent* >(pRes))
	{
		setEventRes( (i3LevelResEvent*)pRes);
	}
	else
	{
		CreateSymbol();
	}
}

void i3LevelPrimitive::setTrigger( bool bState)
{
	m_bTrigger = bState;

	if( m_bTrigger)
	{
		if( stricmp( getInstanceClassName(), "i3GameObj") == 0)
			setInstanceClassName( "i3Trigger");
	}
	else
	{
		if( stricmp( getInstanceClassName(), "i3Trigger") == 0)
			setInstanceClassName( "i3GameObj");
	}
}

void i3LevelPrimitive::OnChangeColor(void)
{
}

void i3LevelPrimitive::setEventRes( i3LevelResEvent * pEvent)
{
	I3_REF_CHANGE( m_pEventRes, pEvent);
}

void i3LevelPrimitive::setTextureRes( i3LevelResTexture * pTex)
{
	i3TextureBindAttr * pTexBind;
	i3TextureEnableAttr * pTME;
	i3LightingEnableAttr * pLight;

	// i3TextureBindAttr을 찾는다.
	pTexBind = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( GetSymbol(), i3TextureBindAttr::static_meta());

	// i3TextureEnableAttr
	pTME = (i3TextureEnableAttr *) i3Scene::FindAttrByExactType( GetSymbol(),i3TextureEnableAttr::static_meta());

	pLight =  (i3LightingEnableAttr *) i3Scene::FindAttrByExactType( m_pRoot, i3LightingEnableAttr::static_meta());

	I3_REF_CHANGE( m_pTexture, pTex);

	if( m_pTexture != nullptr)
	{
		m_pTexture->OnValidate( g_pScene, false);

		if( pTexBind != nullptr)
		{
			pTexBind->SetTexture( pTex->getTexture());
		}

		if( pTME != nullptr)
		{
			pTME->Set( TRUE);
		}
		
		if( pLight != nullptr)
		{
			pLight->Set( TRUE);
		}
	}
	else
	{
		if( pTexBind != nullptr)
			pTexBind->SetTexture( nullptr);

		if( pTME != nullptr)
			pTME->Set( FALSE);

		if( pLight != nullptr)
		{
			pLight->Set( FALSE);
		}
	}

	i3TDK::Update( nullptr, I3_TDK_UPDATE_EDIT, this);
}

void i3LevelPrimitive::Select(void)
{
	i3LevelElement3D::Select();

	if( getGroup() != nullptr)
	{
		i3LevelElement3D * pElm = getGroup()->getElement( 0);
		if( pElm != nullptr && pElm != this)	pElm->Select();
	}
}

// Primitive는 자신만의 고유한 Resource를 가진다. 그래서 OnLoad() 함수 내에서는
// 일반적인 방법으로 ResourceDB에서 검색해 BindRes를 하지만,
// Clipboard에서 Paste하는 경우에는 
void i3LevelPrimitive::OnAfterPaste(void)
{
	if(m_pRes == nullptr)
		CreateSymbol();

	i3LevelElement3D::OnAfterPaste();
}

void i3LevelPrimitive::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	i3LevelElement3D::OnSetInstanceInfo( pInfo);

	if( isTrigger())
	{
		/*
		if( (getInstanceClassName()[0] == 0) ||
			i3::generic_is_iequal( getInstanceClassName(), "i3GameObj") )
			*/
		{
			i3::class_meta* meta = i3::class_meta_system::i()->find_class_meta(pInfo->nHashCodeInstClass);

			if ( !i3::kind_of<i3Trigger*>(meta) )
				pInfo->nHashCodeInstClass   = i3Trigger::static_meta()->class_crc();		// i3Trigger파생클래스가 게임소스안에 존재함. (2014.12.10.수빈)
																							// 파생클래스 CRC Value를 유지함.
			if( m_pEventRes != nullptr)
			{
				pInfo->nHashCodeResPath		= m_pEventRes->getHashCode()[0];
				pInfo->HashCode[0]			= m_pEventRes->getHashCode()[1];
				pInfo->HashCode[1]			= m_pEventRes->getHashCode()[2];
				pInfo->HashCode[2]			= m_pEventRes->getHashCode()[3];
			}
			else
			{
				pInfo->nHashCodeResPath		= 0;
				pInfo->HashCode[0]			= 0;
				pInfo->HashCode[1]			= 0;
				pInfo->HashCode[2]			= 0;
			}
		}
	}

	pInfo->nTriggerEnable		= ( m_bTrigger ) ? 1 : 0;
	pInfo->nTriggerMatchGroup   = (UINT8) m_TriggerCGroup;
	pInfo->nTriggerType			= 0;
}

i3GameObj *	i3LevelPrimitive::CreateGameInstance(void)
{
	i3ClassMeta * pMeta;

	if( getInstanceClassName()[0] == 0)
		return m_pGameObj;

	//Create Instance에서 그룹설정된 trigger primitive들은 그룹의 0번째 element와 같은 설정을 가지게 된다.
	i3LevelGroup * pGrp = getGroup();
	if( pGrp != nullptr)
	{
		i3LevelElement3D * pElement = (i3LevelElement3D*)pGrp->getElement( 0);
		if( this != pElement && i3::kind_of<i3LevelPrimitive*>(pElement))
		{
			i3LevelPrimitive * pPrim = (i3LevelPrimitive*)pElement;

			m_bTrigger			= pPrim->isTrigger();
			m_TriggerCGroup		= pPrim->getTriggerCollisionGroup();

			setEventRes( pPrim->getEventRes());
		}
	}

	pMeta = i3ClassMetaSystem::i()->find_class_meta( getInstanceClassName());
	if( isTrigger())
	{
		pMeta = i3Trigger::static_meta();
	}

	if( m_pGameObj != nullptr)
	{
		// 기존의 Object와 Meta가 일치하는지 확인
		if( m_pGameObj->meta() != pMeta)
		{
			I3_SAFE_RELEASE( m_pGameObj);
		}
	}

	if( m_pGameObj == nullptr)
	{
		// 새로 Meta로 생성
		if((pMeta != nullptr) &&	pMeta->get_class_meta_style() != i3::cms_abstract )	// (pMeta->IsAbstractClass() == FALSE))
		{
			m_pGameObj = (i3GameObj *) pMeta->create_instance();	//CREATEINSTANCE( pMeta);
			I3_MUST_ADDREF(m_pGameObj);

			m_pGameObj->SetName( GetName());
		}
	}

	I3_SAFE_ADDREF( m_pGameObj);

	setRefObject( m_pGameObj);

	//Create Instance내에서, 그룹설정된 trigger primitive들은 그룹의 0번째 element와 같은 설정을 가지게 되고 , gameobject는 0번의 gameobj를 부모로 갖게된다.
	//실제 편집에서는 0번째 element의 값만 변경시키고, export시에 그설정을 다른trigger에도 복사한다.
	if( pGrp != nullptr)
	{
		i3LevelElement3D * pElement = (i3LevelElement3D*)pGrp->getElement( 0);

		if( this != pElement && i3::kind_of<i3LevelPrimitive*>(pElement))
		{
			if( (m_pGameObj != nullptr) && (isTrigger()))
			{
				i3LevelPrimitive * pPrim = (i3LevelPrimitive*)pElement;

				i3Trigger * pTrig = (i3Trigger*)m_pGameObj;

				i3Trigger * pOwnerTrig = (i3Trigger*)pPrim->getGameObj();
				I3ASSERT( pOwnerTrig != nullptr);

				pTrig->setOwner( pOwnerTrig);
			}
		}
	}
	else
	{
		if( i3::kind_of<i3Trigger*>(m_pGameObj))
			((i3Trigger*)m_pGameObj)->setOwner( nullptr);
	}

	return m_pGameObj;
}

void i3LevelPrimitive::SetName( const i3::rc_string& strName)
{
	bool bChange;

	bChange = (i3::generic_is_iequal( GetName(), strName) == false);

	if( bChange == false)
		return;

	i3LevelElement3D::SetName( strName);

	AdjustResPathByName();
}

void i3LevelPrimitive::AdjustResPathByName(void)
{
	// Res의 이름 역시 변경한다.
	i3LevelRes * pRes = getRes();

	if( pRes != nullptr)
	{
		// 원래의 이름을 복사해둔다.
		char szOldName[ MAX_PATH];

		// Resource DB에서 제거
		I3_MUST_ADDREF(pRes);
		g_pResDB->RemoveRes( pRes);

		i3::safe_string_copy( szOldName, pRes->getPath(), _countof( szOldName));

		// 새 이름을 설정한다.
		char szPath[ MAX_PATH], szNew[ MAX_PATH];

		i3::extract_directoryname( szOldName, szPath);
	//	i3String::SplitPath( szOldName, szPath);
	//	i3::remove_end_slash( szPath);

		i3::snprintf( szNew, _countof( szNew), "%s/%s.i3s", szPath, GetName());

		pRes->setPath( szNew);

		// Resource DB에 추가.
		g_pResDB->AddRes( pRes);
		I3_MUST_RELEASE(pRes);

		// 기존의 파일을 삭제한다.
		{
			TCHAR szOldPath[MAX_PATH], szNewPath[MAX_PATH];

			i3::snprintf( szOldPath, _countof( szOldPath), _T("%s/%s"), i3ResourceFile::GetWorkingFolder(), szOldName);
			i3::snprintf( szNewPath, _countof( szNewPath), _T("%s/%s"), i3ResourceFile::GetWorkingFolder(), szNew);

			if( i3::is_file( szOldPath))
				::MoveFile( szOldPath, szNewPath);
		}
	}
}

void i3LevelPrimitive::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelElement3D::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of< i3LevelPrimitive* >(pDest));

	i3LevelPrimitive * pPrim = (i3LevelPrimitive*)pDest;

	pPrim->setEventRes( getEventRes());
	pPrim->setVisible( isVisible());
	pPrim->setTrigger( isTrigger());
	pPrim->setTriggerCollisionGroup( getTriggerCollisionGroup());
	
	pPrim->setTextureRes( getTextureRes());
	pPrim->setHorzTiling( getHorzTiling());
	pPrim->setVertTiling( getVertTiling());
	pPrim->setMaterialColor( getMaterialColor());
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)				// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct PRIMITIVE
	{
		UINT8		m_ID[4] = { 'P', 'R', 'M', '2' };
		UINT32		m_Color = 0;
		UINT32		m_bTrigger = 0;
		char		m_szEventRes[MAX_PATH] = { 0 };
		INT32		m_TriggerCGroup = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelPrimitive::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::PRIMITIVE data;

	result = i3LevelElement3D::OnSave( pResFile);
	I3_CHKIO( result);
	
	{
		COLOR col;

		i3Color::Set( &col, &m_Color);
		data.m_Color = i3Color::ConvertRGBA32( &col);
	}

	if( m_bTrigger && (m_pEventRes != nullptr))
	{
		data.m_bTrigger			= 1;
		
		i3::string_ncopy_nullpad( data.m_szEventRes, m_pEventRes->getPath(), _countof( data.m_szEventRes) - 1);
	}

	data.m_TriggerCGroup = getTriggerCollisionGroup();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3LevelPrimitive::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::PRIMITIVE data;

	result = i3LevelElement3D::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	/*
	{
		COLOR col;

		i3Color::ConvertRGBA32( data.m_Color, &col);
		i3Color::Set( &m_Color, &col);

		SetMaterialDiffuse( m_Color.r, m_Color.g, m_Color.b, m_Color.a);
		SetColor( &col);
	}
	*/

	m_bTrigger = ( data.m_bTrigger) ? true : false;
	setTrigger( m_bTrigger);

	if( m_bTrigger && (data.m_szEventRes[0] != 0))
	{
		i3LevelRes * pRes;

		pRes = (i3LevelRes *)g_pResDB->FindResByPath( data.m_szEventRes);

		if( pRes == nullptr)
		{
			// 직접 Laoding 시도.
			pRes = g_pResDB->AddRes( data.m_szEventRes);
		}

		if( pRes == nullptr)
		{
			AddState( I3_LEVEL_STATE_FAILED_BINDRES);
		}
		else
		{
			pRes->OnValidate( g_pScene, false);

			setEventRes( (i3LevelResEvent*)pRes);
		}

		if( data.m_TriggerCGroup != 0)
		{
			setTriggerCollisionGroup( data.m_TriggerCGroup);
		}
	}

	AdjustPhysixSize();

	return result;
}

bool i3LevelPrimitive::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelElement3D::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	i3Color::SetToXML( pXML, "PrimColor", & m_Color);

	if( m_bTrigger && (m_pEventRes != nullptr))
	{
		pXML->addAttr( "isTrigger",	1);
		pXML->addAttr( "EventRes",	m_pEventRes->getPath());
	}

	pXML->addAttr( "TriggerCollGroup",		getTriggerCollisionGroup());

	return true;
}

bool i3LevelPrimitive::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelElement3D::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	if( getRes() == nullptr)
	{
		// 새로운 방식으로 변경된 버전.
		// 이전 버전에서는 Resource가 파일로 존재했으나, Runtime 생성하는 방식으로 변경.
		CreateSymbol();
	}

	{
		i3Color::GetFromXML( pXML, "PrimColor", & m_Color);

		SetMaterialDiffuse( m_Color.r, m_Color.g, m_Color.b, m_Color.a);
	}

	m_bTrigger = pXML->getAttr( "isTrigger") != nullptr;
	setTrigger( m_bTrigger);

	if( m_bTrigger)
	{
		const char * pszResPath = pXML->getAttr( "EventRes");

		if( pszResPath == nullptr)
		{
			I3ERRLOG( i3Error::_INVALIDFORMAT);
			return false;
		}

		i3LevelRes * pRes;

		pRes = g_pResDB->FindResByPath( pszResPath);

		if( pRes == nullptr)
		{
			// 직접 Laoding 시도.
			pRes = g_pResDB->AddRes( pszResPath);
		}

		if( pRes == nullptr)
		{
			AddState( I3_LEVEL_STATE_FAILED_BINDRES);
		}
		else
		{
			pRes->OnValidate( g_pScene, false);

			I3_REF_CHANGE( m_pEventRes, (i3LevelResEvent*)pRes);
		}
	}

	pXML->getAttr( "TriggerCollGroup", & m_TriggerCGroup);

	AdjustPhysixSize();

	return true;
}

