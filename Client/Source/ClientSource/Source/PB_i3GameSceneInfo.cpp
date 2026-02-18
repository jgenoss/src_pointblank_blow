#include "pch.h"
#include "PB_i3GameSceneInfo.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( PB_i3GameSceneInfo);//, i3GameSceneInfo);

PB_i3GameSceneInfo::PB_i3GameSceneInfo(void)
{
}

PB_i3GameSceneInfo::~PB_i3GameSceneInfo(void)
{
}

UINT32 PB_i3GameSceneInfo::InitInstance(void)
{
	char name[MAX_PATH];

	sprintf_s( name, "Scene/%s/%s.i3ins", m_strStageName.c_str(), m_strStageName.c_str());		// ÀÏ¹Ý

	if( i3FileStream::isFile( name))
		return LoadInstance( name);
	return 0;
}

bool PB_i3GameSceneInfo::Load( const char * pszStageName)
{
	setStageName( pszStageName);

	char name[MAX_PATH];

	sprintf_s( name, "Scene/%s/%s.RSC", pszStageName, pszStageName);

	if( !i3FileStream::isFile( name))
	{
		I3TRACE("%s have no SceneInfoResource\n", pszStageName);

		return false;
	}

	return m_pResManager->LoadRSC( name);
}
