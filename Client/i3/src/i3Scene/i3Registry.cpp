#include "i3SceneDef.h"
#include "i3Registry.h"

I3_EXPORT_SCENE i3RegistrySet * g_pRegistry	= nullptr;

I3_EXPORT_SCENE
UINT32 i3Registry::Init(const char * pszFileName)
{
	UINT32 Result = 0;

	Destroy();

	I3ASSERT( g_pRegistry == nullptr);
	g_pRegistry = i3RegistrySet::new_object();

	if( pszFileName != nullptr)
	{
		Result = g_pRegistry->LoadRegistrySet( pszFileName);
		I3_CHKIO( Result); 
	}

	return Result;
}

I3_EXPORT_SCENE
bool i3Registry::Save( const char * pszFileName)
{
	if(( pszFileName != nullptr) && (g_pRegistry != nullptr))
	{
		UINT32 nResult = g_pRegistry->SaveRegistrySet( pszFileName);
		if( nResult == STREAM_ERR)
		{
			return false;
		}

		return true;
	}

	return false;
}

I3_EXPORT_SCENE
void i3Registry::End( const char * pszFileName)
{
	if( pszFileName != nullptr)
	{
		Save( pszFileName);
	}

	Destroy();
}

I3_EXPORT_SCENE
void i3Registry::Destroy()
{
	if( g_pRegistry != nullptr)
	{
		I3_SAFE_RELEASE( g_pRegistry);
	}
}

I3_EXPORT_SCENE 
i3RegKey	* i3Registry::FindKey( const char * pszKeyName, i3RegKey * pRoot)
{
	i3RegKey * FindRoot = pRoot;
	if( pRoot == nullptr)
	{
		FindRoot = g_pRegistry->getRoot();
	}

	i3RegKey * pKey = (i3RegKey *)i3TreeNode::FindNodeByName( FindRoot, pszKeyName);

	return pKey;
}

I3_EXPORT_SCENE
i3RegData	* i3Registry::FindData(i3RegKey * pKey,  const char * pszDataName)
{
	I3ASSERT( pszDataName != nullptr);
	I3ASSERT( pKey		 != nullptr);

	i3RegData * pData = pKey->FindDataByName( pszDataName);

	return pData;
}
/*
I3_EXPORT_SCENE
INT32	*	i3Registry::FindData( INT32 * nValue, const char * pszDataName, i3RegKey * pKey)
{
	i3RegData * pData = pKey->FindDataByName( pszDataName);

	if( pData->getDataType() & I3REG_TYPE_INT32)
	{
		IR_INT32 * prInt = (IR_INT32*)pData;
		*nValue = *prInt->getValue();
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "Check Variable's Type");
		I3ASSERT_0;
	}
}*/


static INT32 LocationIndex = 0;
static INT32 MaxLocationCount = 0;

I3_EXPORT_SCENE void i3Registry::Set_LocationIndex(UINT32 Index, UINT32 MaxCount)
{
	LocationIndex = Index;
	MaxLocationCount = MaxCount;
}

I3_EXPORT_SCENE INT32 i3Registry::Get_LocationIndex()
{
	return LocationIndex;
}

I3_EXPORT_SCENE INT32 i3Registry::Get_MaxLocationCount()
{
	return MaxLocationCount;
}

static bool EnableLoadStamp = true;
I3_EXPORT_SCENE void i3Registry::Set_LoadStamp(bool LoadStamp)
{
	EnableLoadStamp = LoadStamp;
}

I3_EXPORT_SCENE bool i3Registry::Get_LoadStamp()
{
	return EnableLoadStamp;
}

I3_EXPORT_SCENE bool i3Registry::IsStampString(const char* Name)
{
	if( EnableLoadStamp )
		return false;

	if( Name[0] == '_' && Name[1] == '_' )  //또는, __이게 포함 안 된 경우
	{
		if( Name[2] == 'M' || Name[2] == 'U' )
		{
			return true;
		}
	}

	return false;
}