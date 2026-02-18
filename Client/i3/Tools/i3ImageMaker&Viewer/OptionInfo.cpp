#include "stdafx.h"
#include "OptionInfo.h"

COptionInfo::COptionInfo(void)
{
	m_szExportPath[0] = 0;

	m_WndX			= CW_USEDEFAULT;
	m_WndY			= CW_USEDEFAULT;
	m_WndCX			= CW_USEDEFAULT;
	m_WndCY			= CW_USEDEFAULT;

	m_MinCX			= 64;
	m_MinCY			= 64;
	m_Filter		= tex::FILTER_TRIANGLE;
	m_bDither		= true;
	m_bSRGB_In		= false;
	m_bSRGB_Out		= false;
	m_bExportI3I	= true;

	Load();
}

COptionInfo::~COptionInfo(void)
{
	Save();
}

void COptionInfo::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "Option", "ExportPath", "");
	strcpy( m_szExportPath, LPCTSTR( str));

	m_WndX			= AfxGetApp()->GetProfileInt( "Frame", "X", CW_USEDEFAULT);
	m_WndY			= AfxGetApp()->GetProfileInt( "Frame", "Y", CW_USEDEFAULT);
	m_WndCX			= AfxGetApp()->GetProfileInt( "Frame", "CX", CW_USEDEFAULT);
	m_WndCY			= AfxGetApp()->GetProfileInt( "Frame", "CY", CW_USEDEFAULT);

	m_MinCX			= AfxGetApp()->GetProfileInt( "Option", "MinCX", 64);
	m_MinCY			= AfxGetApp()->GetProfileInt( "Option", "MinCY", 64);

	m_Filter		= (tex::FILTER) AfxGetApp()->GetProfileInt( "Option", "Filter", tex::FILTER_TRIANGLE);
	m_bDither		= AfxGetApp()->GetProfileInt( "Option", "Dither", TRUE) == TRUE;
	m_bSRGB_In		= AfxGetApp()->GetProfileInt( "Option", "SRGB_In", FALSE) == TRUE;
	m_bSRGB_Out		= AfxGetApp()->GetProfileInt( "Option", "SRGB_Out", FALSE) == TRUE;
	m_bExportI3I	= AfxGetApp()->GetProfileInt( "Option", "ExportI3I", TRUE) == TRUE;

	i3ResourceFile::SetWorkingFolder( m_szExportPath);
}

void COptionInfo::Save(void)
{
	AfxGetApp()->WriteProfileString( "Option", "ExportPath", m_szExportPath);

	AfxGetApp()->WriteProfileInt( "Frame", "X",		m_WndX);
	AfxGetApp()->WriteProfileInt( "Frame", "Y",		m_WndY);
	AfxGetApp()->WriteProfileInt( "Frame", "CX",	m_WndCX);
	AfxGetApp()->WriteProfileInt( "Frame", "CY",	m_WndCY);

	AfxGetApp()->WriteProfileInt( "Option", "MinCX",		m_MinCX);
	AfxGetApp()->WriteProfileInt( "Option", "MinCY",		m_MinCY);
	AfxGetApp()->WriteProfileInt( "Option", "Filter",		(INT32) m_Filter);
	AfxGetApp()->WriteProfileInt( "Option", "Dither",		(m_bDither == true));
	AfxGetApp()->WriteProfileInt( "Option", "SRGB_In",		(m_bSRGB_In == true));
	AfxGetApp()->WriteProfileInt( "Option", "SRGB_Out",		(m_bSRGB_Out == true));
	AfxGetApp()->WriteProfileInt( "Option", "ExportI3I",	(m_bExportI3I == true));
}

