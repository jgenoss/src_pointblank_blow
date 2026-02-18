#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo
{
public:
	char		m_szWorkPath[ MAX_PATH];

	BOOL		m_bExportI3S;
	BOOL		m_bRemoveSameImage;

	BOOL		m_bExportI3CHR;
	BOOL		m_bGenC;
	BOOL		m_bGenMipmap;
	BOOL		m_bConvertDXT;
	BOOL		m_bRotateAxis;
	BOOL		m_bDXTAlpha;
	BOOL		m_bExternTexture;
	BOOL		m_bOptAnim;
	BOOL		m_bGetAnimPack;

	char		m_szCPath[MAX_PATH];
	char		m_szCharaNames[512];

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);

	COptionInfo* Clone();
	void		 Apply(const COptionInfo* info, bool changeWorkDir=true);
};

#endif
