#include "stdafx.h"
#include "OptionInfo.h"

COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;
	m_bGenC = TRUE;
	m_bExportI3S = TRUE;
	m_bRemoveSameImage = FALSE;
	m_bExportI3CHR = TRUE;
	m_bRotateAxis = TRUE;
	m_bGenMipmap = TRUE;
	m_bConvertDXT = TRUE;
	m_bDXTAlpha	= TRUE;
	m_szCPath[0] = 0;
	m_bExternTexture = TRUE;
	m_bOptAnim = FALSE;
	m_bGetAnimPack = FALSE;
	m_szCharaNames[0] = 0;

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
	str.ReleaseBuffer();		// 테스트..

	m_bExportI3S	= AfxGetApp()->GetProfileInt( "Option", "ExportI3S", TRUE);
	m_bRemoveSameImage = AfxGetApp()->GetProfileInt("Option", "RemoveSameImage", FALSE);
	m_bExportI3CHR	= AfxGetApp()->GetProfileInt( "Option", "ExportI3CHR", TRUE);
	m_bGenC			= AfxGetApp()->GetProfileInt( "Option", "GenC", TRUE);
	m_bGenMipmap	= AfxGetApp()->GetProfileInt( "Option", "GenMipmap", TRUE);
	m_bConvertDXT	= AfxGetApp()->GetProfileInt( "Option", "ConvertDXT", TRUE);
	m_bRotateAxis	= AfxGetApp()->GetProfileInt( "Option", "RotateAxis", TRUE);
	m_bDXTAlpha		= AfxGetApp()->GetProfileInt( "Option", "DXTAlpha", TRUE );
	m_bExternTexture = AfxGetApp()->GetProfileInt( "Option", "ExternTexture", TRUE );

	str = AfxGetApp()->GetProfileString( "Option", "CharaNames", "D-Fox\r\nHide\r\nLeopard\r\nREBEL_Female\r\nREBEL_Female(VipderRed)\r\nREBEL_Male\r\nSWAT_Female\r\nSWAT_Male");
	strcpy( m_szCharaNames, LPCTSTR( str));
	str.ReleaseBuffer();		// 테스트..

	str = AfxGetApp()->GetProfileString( "Option", "CPath", "");
	strcpy( m_szCPath, LPCTSTR( str));
	str.ReleaseBuffer();		// 테스트..

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
	i3ResourceFile::setSearchExternRefEnable( true);
}

void COptionInfo::Save(void)
{
	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);

	AfxGetApp()->WriteProfileInt( "Option", "GenC", m_bGenC);
	AfxGetApp()->WriteProfileInt( "Option", "ExportI3S", m_bExportI3S);
	AfxGetApp()->WriteProfileInt( "Option", "RemoveSameImage", m_bRemoveSameImage);

	AfxGetApp()->WriteProfileInt( "Option", "ExportI3CHR", m_bExportI3CHR);
	AfxGetApp()->WriteProfileInt( "Option", "RotateAxis", m_bRotateAxis);
	AfxGetApp()->WriteProfileInt( "Option", "GenMipamp",	m_bGenMipmap);
	AfxGetApp()->WriteProfileInt( "Option", "ConvertDXT", m_bConvertDXT);
	AfxGetApp()->WriteProfileInt( "Option", "DXTAlpha", m_bDXTAlpha);
	AfxGetApp()->WriteProfileInt( "Option", "ExternTexture", m_bExternTexture);

	AfxGetApp()->WriteProfileString( "Option", "CPath", m_szCPath);

	AfxGetApp()->WriteProfileString( "Option", "CharaNames", m_szCharaNames);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

COptionInfo* COptionInfo::Clone()
{
	COptionInfo* cloneInst = new COptionInfo;
	cloneInst->Apply(this, false);

	return cloneInst;
}

void COptionInfo::Apply(const COptionInfo* info, bool changeWorkDir)
{
	strcpy_s(m_szWorkPath, MAX_PATH, info->m_szWorkPath);
	m_bGenC				= info->m_bGenC			;
	m_bExportI3S		= info->m_bExportI3S	;
	m_bRemoveSameImage  = info->m_bRemoveSameImage;

	m_bExportI3CHR		= info->m_bExportI3CHR	;
	m_bRotateAxis		= info->m_bRotateAxis	;
	m_bGenMipmap		= info->m_bGenMipmap	;
	m_bConvertDXT		= info->m_bConvertDXT	;
	m_bDXTAlpha			= info->m_bDXTAlpha		;
	m_bExternTexture    = info->m_bExternTexture;
	strcpy_s(m_szCPath, MAX_PATH, info->m_szCPath);
	m_bOptAnim			= info->m_bOptAnim		;
	m_bGetAnimPack		= info->m_bGetAnimPack	;
	strcpy_s(m_szCharaNames, MAX_PATH, info->m_szCharaNames);

	if(changeWorkDir)
		i3ResourceFile::SetWorkingFolder(m_szWorkPath);
}
