#include "stdafx.h"
#include "OptionInfo.h"

I3_CLASS_INSTANCE( COptionInfo); //, i3ElementBase);

COptionInfo::COptionInfo(void)
{
	i3mem::FillZero( m_MRU, sizeof(m_MRU));

	Load();
}

COptionInfo::~COptionInfo(void)
{
	Save();
}

INT32	COptionInfo::_GetINT32( const char * pszEntry, INT32 def)
{
	return AfxGetApp()->GetProfileInt( "Option", pszEntry, def);
}

void	COptionInfo::_SetINT32( const char * pszEntry, INT32 val)
{
	AfxGetApp()->WriteProfileInt( "Option", pszEntry, val);
}

REAL32	COptionInfo::_GetREAL32( const char * pszEntry, REAL32 def)
{
	char conv[256];
	CString str;

	sprintf( conv, "%f", def);

	str = AfxGetApp()->GetProfileString( "Option", pszEntry, conv);

	return (REAL32) atof( LPCTSTR( str));
}

void	COptionInfo::_SetREAL32( const char * pszEntry, REAL32 val)
{
	char conv[256];

	sprintf( conv, "%f", val);

	AfxGetApp()->WriteProfileString( "Option", pszEntry, (const char *) conv);
}

void	COptionInfo::_GetString( const char * pszEntry, char * pszStr, INT32 len, const char * pszDef)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "Option", pszEntry, pszDef);

	i3::string_ncopy_nullpad( pszStr, LPCTSTR( str), len);
}

void	COptionInfo::_SetString( const char * pszEntry, const char * pszStr)
{
	AfxGetApp()->WriteProfileString( "Option", pszEntry, pszStr);
}

void	COptionInfo::_GetVEC3D( const char * pszEntry, VEC3D * pVec, REAL32 x, REAL32 y, REAL32 z)
{
	char conv[256], sep;
	CString str;

	sprintf( conv, "%f %f %f", x, y, z);

	str = AfxGetApp()->GetProfileString( "Option", pszEntry, conv);

	{
		i3LineLexer lex;

		lex.SetMasterString( (char *) LPCTSTR( str));

		// X
		lex.GetNextToken( conv, &sep);
		pVec->x = (REAL32) atof( conv);

		// Y
		lex.GetNextToken( conv, &sep);
		pVec->y = (REAL32) atof( conv);

		// Z
		lex.GetNextToken( conv, &sep);
		pVec->z = (REAL32) atof( conv);
	}
}

void	COptionInfo::_SetVEC3D( const char * pszEntry, VEC3D * pVec)
{
	char conv[256];

	sprintf( conv, "%f %f %f", pVec->x, pVec->y, pVec->z);

	AfxGetApp()->WriteProfileString( "Option", pszEntry, conv);
}


void	COptionInfo::_GetVEC4D( const char * pszEntry, VEC4D * pVec, REAL32 x, REAL32 y, REAL32 z, REAL32 w)
{
	char conv[256], sep;
	CString str;

	sprintf( conv, "%f %f %f %f", x, y, z, w);

	str = AfxGetApp()->GetProfileString( "Option", pszEntry, conv);

	{
		i3LineLexer lex;

		lex.SetMasterString( (char *) LPCTSTR( str));

		// X
		lex.GetNextToken( conv, &sep);
		pVec->x = (REAL32) atof( conv);

		// Y
		lex.GetNextToken( conv, &sep);
		pVec->y = (REAL32) atof( conv);

		// Z
		lex.GetNextToken( conv, &sep);
		pVec->z = (REAL32) atof( conv);

		// W
		lex.GetNextToken( conv, &sep);
		pVec->w = (REAL32) atof( conv);
	}
}

void	COptionInfo::_SetVEC4D( const char * pszEntry, VEC4D * pVec)
{
	char conv[256];

	sprintf( conv, "%f %f %f %f", pVec->x, pVec->y, pVec->z, pVec->w);

	AfxGetApp()->WriteProfileString( "Option", pszEntry, conv);
}

void COptionInfo::Load(void)
{
	m_ViewerMode		= (VIEWER_MODE) _GetINT32( "ViewerMode", VIEWER_MODE_NORMAL);
	m_bShadowEnable		= _GetINT32( "ShadowEnable", FALSE);
	m_ShadowSize		= _GetINT32( "ShadowSize", 512);
	m_bGridShow			= _GetINT32( "Grid", TRUE);
	m_bGlow				= _GetINT32( "Glow", TRUE);

	_GetVEC4D( "Ambient",		(VEC4D *) &m_Ambient,		200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f, 1.0f);
	_GetVEC4D( "LightDiffuse",	(VEC4D *) &m_LightDiffuse,	1.0f, 1.0f, 1.0f, 1.0f);
	_GetVEC4D( "LightSpecular",	(VEC4D *) &m_LightSpecular,	1.0f, 1.0f, 1.0f, 1.0f);
	_GetVEC4D( "BackColor",		(VEC4D *) &m_BackColor,		0.2f, 0.2f, 0.2f, 1.0f);

	m_LightIntensity= _GetREAL32( "LightIntensity", 1.0f);
	m_LightTheta	= _GetREAL32( "LightTheta", 30.0f);
	m_LightRho		= _GetREAL32( "LightRho", -65.0f);
	m_LightDist		= _GetREAL32( "LightDist", 30.0f);

	_GetString( (const char *) "ReflectMap", m_szReflectMap, sizeof( m_szReflectMap), "");

	INT32 i;

	for( i = 0; i < MAX_MRU; i++)
	{
		char szEntry[ 256];

		sprintf( szEntry, "MRU%d", i);

		_GetString( (const char *) szEntry, m_MRU[i], MAX_PATH, "");
	}

	m_fovDegree = _GetINT32("CamFOV", 45);
	
	// Recorded Camera
	for( i = 0; i < MAX_RECCAM; i++)
	{
		char szEntry[ 256];

		sprintf( szEntry, "CamPos%d", i);
		_GetVEC3D( szEntry, &m_CamPos[i], 0.0f, 0.0f, 0.0f);

		sprintf( szEntry, "CamRot%d", i);
		_GetVEC4D( szEntry, &m_CamRot[i], 1.0f, 0.0f, 0.0f, 0.0f);
	}
}

void COptionInfo::Save(void)
{
	_SetINT32( "ViewerMode", m_ViewerMode);
	_SetINT32( "ShadowEnable", m_bShadowEnable);
	_SetINT32( "ShadowSize", m_ShadowSize);
	_SetINT32( "Grid", m_bGridShow);
	_SetINT32( "Glow", m_bGlow);

	_SetVEC4D( "Ambient",	(VEC4D *) &m_Ambient);
	_SetVEC4D( "LightDiffuse", (VEC4D *) &m_LightDiffuse);
	_SetVEC4D( "LightSpecular", (VEC4D *) &m_LightSpecular);
	_SetVEC4D( "BackColor", (VEC4D *) &m_BackColor);

	_SetREAL32( "LightIntensity", m_LightIntensity);
	_SetREAL32( "LightTheta", m_LightTheta);
	_SetREAL32( "LightRho", m_LightRho);
	_SetREAL32( "LightDist", m_LightDist);

	_SetString( "ReflectMap", m_szReflectMap);

	INT32 i;

	for( i = 0; i < MAX_MRU; i++)
	{
		char szEntry[256];

		sprintf( szEntry, "MRU%d", i);

		_SetString( (const char *) szEntry, m_MRU[i]);
	}

	_SetINT32("CamFOV", m_fovDegree);

	// Recorded Camera
	for( i = 0; i < MAX_RECCAM; i++)
	{
		char szEntry[ 256];

		sprintf( szEntry, "CamPos%d", i);
		_SetVEC3D( szEntry, &m_CamPos[i]);

		sprintf( szEntry, "CamRot%d", i);
		_SetVEC4D( szEntry, &m_CamRot[i]);
	}
}

INT32 COptionInfo::getMRUCount(void)
{
	INT32 i, cnt;

	for( i = 0, cnt = 0; i < MAX_MRU; i++)
	{
		if( m_MRU[i][0] != 0)
			cnt++;
	}

	return cnt;
}

void COptionInfo::addMRU( const char * pszPath)
{
	INT32 i;

	// 기존에 동일한 파일이 있는지 확인한다.
	for( i = 0; i < MAX_MRU; i++)
	{
		if( i3::generic_is_iequal( m_MRU[i], pszPath) )
		{
			// 이미 있다. 이 놈을 삭제한다.
			if( (i + 1) < MAX_MRU)
			{
				memmove( m_MRU[i], m_MRU[i + 1], MAX_PATH * (MAX_MRU - (i + 1)));
			}
			break;
		}
	}

	// 전체를 한 줄씩 밀어낸다.
	memmove( m_MRU[1], m_MRU[0], MAX_PATH * (MAX_MRU - 1));

	// 최상위 List에 넣는다.
	i3::safe_string_copy( m_MRU[0], pszPath, MAX_PATH );
}

