#include "stdafx.h"
#include "OptionInfo.h"

I3_CLASS_INSTANCE( COptionInfo);

COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;

	Load();
}

COptionInfo::~COptionInfo(void)
{
	Save();
}

void COptionInfo::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "Option", "WorkPath", "");
	_GetVEC4D( "BackColor",		(VEC4D *) &m_BackColor,		0.2f, 0.2f, 0.2f, 1.0f);

	strcpy( m_szWorkPath, LPCTSTR( str));

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
	i3ResourceFile::setSearchExternRefEnable( true);

	
	
}

void COptionInfo::Save(void)
{
	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);
	_SetVEC4D( "BackColor", (VEC4D *) &m_BackColor);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
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