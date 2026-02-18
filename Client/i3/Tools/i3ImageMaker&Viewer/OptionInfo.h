#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

#include "Defines.h"

class COptionInfo
{
public:
	char			m_szExportPath[ MAX_PATH];

	INT32			m_WndX;
	INT32			m_WndY;
	INT32			m_WndCX;
	INT32			m_WndCY;

	INT32			m_MinCX;
	INT32			m_MinCY;
	tex::FILTER		m_Filter;
	bool			m_bDither;
	bool			m_bSRGB_In;
	bool			m_bSRGB_Out;
	bool			m_bExportI3I;

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);
};

#endif
