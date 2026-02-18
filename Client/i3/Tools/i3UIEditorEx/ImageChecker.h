#pragma once

class CImageChecker
{
protected:
 	struct IMG_INFO
 	{
		char szPath[MAX_PATH] = { 0 };
 		UINT32 nWidth = 0;
 		UINT32 nHeight = 0;
 	};

	bool m_bSetting = false;

	std::map< std::string, IMG_INFO >	m_mapDefImage;
	std::vector< std::string >			m_conErrorFile;

public:
	CImageChecker();
	virtual ~CImageChecker();

protected:
	void _Init();

public:

	bool IsEnableCheck();
	bool LoadDefaultImageInfo();
	bool LoadDefaultImageInfo( const char * pszFolder );

public:
	bool CheckNationImageInfo( const char * pszNation );
	bool LoadNationImageInfo( const char * pszFolder );

public:
	bool GetNationFromVirtualTexture( char * pszNation );

};