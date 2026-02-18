#include "pch.h"
#include "OptionInfo.h"

I3_CLASS_INSTANCE( COptionInfo); //, i3ElementBase);

COptionInfo::COptionInfo(void)
:	m_eTeamColor(TEAM_NONE)
{
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
	m_bShadowEnable		= _GetINT32( "ShadowEnable", FALSE);
	m_ShadowSize		= _GetINT32( "ShadowSize", 512);
	m_bGridShow			= _GetINT32( "Grid", TRUE);
	m_bGlow				= _GetINT32( "Glow", TRUE);

	_GetVEC4D( "Ambient",		(VEC4D *) &m_Ambient,		0.2f, 0.2f, 0.2f, 1.0f);
	_GetVEC4D( "LightDiffuse",	(VEC4D *) &m_LightDiffuse,	2.0f, 2.0f, 2.0f, 1.0f);
	_GetVEC4D( "LightSpecular",	(VEC4D *) &m_LightSpecular,	1.0f, 1.0f, 1.0f, 1.0f);
	_GetVEC4D( "BackColor",		(VEC4D *) &m_BackColor,		0.2f, 0.2f, 0.2f, 1.0f);

	m_LightIntensity= _GetREAL32( "LightIntensity", 2.0f);
	m_LightTheta	= _GetREAL32( "LightTheta", 30.0f);
	m_LightRho		= _GetREAL32( "LightRho", -65.0f);
	m_LightDist		= _GetREAL32( "LightDist", 30.0f);

	_GetString( (const char *) "ReflectMap", m_szReflectMap, sizeof( m_szReflectMap), "");

	m_vTranslation.x = _GetREAL32( "CamTranslationX", 0.f);
	m_vTranslation.y = _GetREAL32( "CamTranslationY", 0.5f);
	m_vTranslation.z = _GetREAL32( "CamTranslationZ", 1.f);
	m_vRotation.x = _GetREAL32( "CamRotationTheta", 0.f);
	m_vRotation.y = _GetREAL32( "CamRotationRho", 0.f);
}

void COptionInfo::Save(void)
{
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

	_SetREAL32( "CamTranslationX", m_vTranslation.x);
	_SetREAL32( "CamTranslationY", m_vTranslation.y);
	_SetREAL32( "CamTranslationZ", m_vTranslation.z);
	_SetREAL32( "CamRotationTheta", m_vRotation.x);
	_SetREAL32( "CamRotationRho", m_vRotation.y);
}
