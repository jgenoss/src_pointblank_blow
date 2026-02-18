#include "stdafx.h"
#include "OptionInfo.h"
#include "i3Base/string/ext/ftoa.h"

COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;
	m_bExportI3S = TRUE;
	m_bExportI3OBJ = TRUE;
	m_bRotateAxis = TRUE;
	m_bGenMipmap = TRUE;
	m_bConvertDXT = TRUE;

	m_bGlobalHull = TRUE;
	m_bDynamicHull = TRUE;

	m_Elasticity = 0.3f;
	m_Friction = 0.3f;

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
	strcpy( m_szWorkPath, LPCTSTR( str));

	m_bExportI3S	= AfxGetApp()->GetProfileInt( "Option", "ExportI3S", TRUE);
	m_bExportI3OBJ	= AfxGetApp()->GetProfileInt( "Option", "ExportI3OBJ", TRUE);
	m_bGenMipmap	= AfxGetApp()->GetProfileInt( "Option", "GenMipmap", TRUE);
	m_bConvertDXT	= AfxGetApp()->GetProfileInt( "Option", "ConvertDXT", TRUE);
	m_bRotateAxis	= AfxGetApp()->GetProfileInt( "Option", "RotateAxis", TRUE);

	m_bGlobalHull	= AfxGetApp()->GetProfileInt( "Option", "GlobalHull", TRUE);
	m_bDynamicHull	= AfxGetApp()->GetProfileInt( "Option", "DynamicHull", TRUE);

	str = AfxGetApp()->GetProfileString( "Option", "Elasticity", "0.3");
	m_Elasticity = (REAL32) atof( LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "Option", "Friction", "0.3");
	m_Friction = (REAL32) atof( LPCTSTR( str));

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
	i3ResourceFile::setSearchExternRefEnable( true);
}

void COptionInfo::Save(void)
{
	char conv[256];

	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);

	AfxGetApp()->WriteProfileInt( "Option", "ExportI3S", m_bExportI3S);
	AfxGetApp()->WriteProfileInt( "Option", "ExportI3OBJ", m_bExportI3OBJ);
	AfxGetApp()->WriteProfileInt( "Option", "RotateAxis", m_bRotateAxis);
	AfxGetApp()->WriteProfileInt( "Option", "GenMipamp",	m_bGenMipmap);
	AfxGetApp()->WriteProfileInt( "Option", "ConvertDXT", m_bConvertDXT);

	AfxGetApp()->WriteProfileInt( "Option", "GlobalHull",	m_bGlobalHull);
	AfxGetApp()->WriteProfileInt( "Option", "DynamicHull", m_bDynamicHull);

	i3::ftoa( m_Elasticity, conv);
	AfxGetApp()->WriteProfileString( "Option", "Elasticity", conv);

	i3::ftoa( m_Friction, conv);
	AfxGetApp()->WriteProfileString( "Option", "Friction", conv);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

