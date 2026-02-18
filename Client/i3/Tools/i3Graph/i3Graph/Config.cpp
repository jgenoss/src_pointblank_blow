#include "stdafx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	Load();
}

CConfig::~CConfig(void)
{
	Save();
}

void CConfig::ParseColor( const char * pszStr, COLORREAL * pColor)
{
	i3LineLexer lex;
	char szToken[256], sep;

	lex.SetMasterString( (char *) pszStr);

	if( lex.GetNextToken( szToken, &sep, " ,") > 0)
		pColor->r = (REAL32) atof( szToken);

	if( lex.GetNextToken( szToken, &sep, " ,") > 0)
		pColor->g = (REAL32) atof( szToken);

	if( lex.GetNextToken( szToken, &sep, " ,") > 0)
		pColor->b = (REAL32) atof( szToken);

	pColor->a = 1.0f;
}


void CConfig::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "Option", "LightDiffuse", "1.0 1.0 1.0");
	ParseColor( LPCTSTR( str), &m_LightDiffuse);

	str = AfxGetApp()->GetProfileString( "Option", "LightSpecular", "1.0 1.0 1.0");
	ParseColor( LPCTSTR( str), &m_LightSpecular);

	m_ShadowMapSize = AfxGetApp()->GetProfileInt( "Option", "ShadowMapSize", 512);

	str = AfxGetApp()->GetProfileString( "Option", "Ambient", "0.3 0.3 0.3");
	ParseColor( LPCTSTR( str), &m_Ambient);

	str = AfxGetApp()->GetProfileString( "Option", "BackColor", "0.4 0.4 0.4");
	ParseColor( LPCTSTR( str), &m_BackColor);
}

void CConfig::Save(void)
{
	char conv[256];
	CString str;

	sprintf( conv, "%f %f %f", m_LightDiffuse.r, m_LightDiffuse.g, m_LightDiffuse.b);
	AfxGetApp()->WriteProfileString( "Option", "LightDiffuse", conv);

	sprintf( conv, "%f %f %f", m_LightSpecular.r, m_LightSpecular.g, m_LightSpecular.b);
	AfxGetApp()->WriteProfileString( "Option", "LightSpecular", conv);

	AfxGetApp()->WriteProfileInt( "Option", "ShadowMapSize", m_ShadowMapSize);

	sprintf( conv, "%f %f %f", m_Ambient.r, m_Ambient.g, m_Ambient.b);
	AfxGetApp()->WriteProfileString( "Option", "Ambient", conv);

	sprintf( conv, "%f %f %f", m_BackColor.r, m_BackColor.g, m_BackColor.b);
	AfxGetApp()->WriteProfileString( "Option", "BackColor", conv);
}

