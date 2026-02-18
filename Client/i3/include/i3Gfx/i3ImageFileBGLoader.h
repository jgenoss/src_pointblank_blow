#if !defined( __I3IMAGEFILE_BGLOADER_H__)
#define __I3IMAGEFILE_BGLOADER_H__

#include "i3Texture.h"

struct tagSPLITTEX_LOADINFO {
	i3Texture *		_pTempTex = nullptr;
	i3Texture *		_pLoadableTex = nullptr;
};

class I3_EXPORT_GFX i3ImageFileBGLoader : public i3Thread
{
	I3_EXPORT_CLASS_DEFINE( i3ImageFileBGLoader, i3Thread);

private:
	tagSPLITTEX_LOADINFO	m_LoadInfo;

	i3Signal *		m_pSignal = nullptr;
	UINT32			m_SignalTime = 1000;
	volatile bool	m_bRun = true;
	volatile bool	m_bExitResult = true;

public:
	virtual ~i3ImageFileBGLoader();

	virtual bool	Create( char * pszName, UINT32 Attr = 0, UINT32 StackSize = 4096, void * pUserData = nullptr, INT32 priority = 3) override;
	/** \brief child class에서 호출하여 쓰레드를 종료시켜야 한다. */
	void			Destroy( void);
	virtual UINT32	OnRunning( void * pData) override;

	void			Run( void);

	void			RequestLoad( tagSPLITTEX_LOADINFO * pInfo);

	bool			IsEmpty( void)			{ return (m_LoadInfo._pTempTex == nullptr) && (m_LoadInfo._pLoadableTex == nullptr); }

	void			LoadTexture( tagSPLITTEX_LOADINFO * pInfo);
};

#endif
