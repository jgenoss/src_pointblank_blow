#if !defined( __I3_TEXTURE_RES_MANAGER_H)
#define __I3_TEXTURE_RES_MANAGER_H

#include "i3Texture.h"
#include "i3ImageFileBGLoader.h"
#include "i3Base/itl/queue.h"

class I3_EXPORT_GFX i3TextureResManager : public i3NamedResourceManager
{
	I3_EXPORT_CLASS_DEFINE( i3TextureResManager, i3NamedResourceManager);		// 익스포트클래스가 아니라서, 일단 익스포트뺌..(12.09.27.수빈)
protected:
	i3Texture *			_TryToLoad( const char * pszPath, bool bBackgroundLoad);
	i3ImageFileBGLoader *	m_pBGLoader = nullptr;

	i3::queue<tagSPLITTEX_LOADINFO*>	m_SplitLoadQue;

	UINT32				m_CPUCount = 0;

public:
	UINT32	getCPUCount()			{ return m_CPUCount; }

public:
	i3TextureResManager(void);
	virtual ~i3TextureResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName, bool bBackgroundLoad = false) override;
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj) override;

	// 분할 로딩하는 텍스쳐에 대한 정보를 업데이트한다.
	void	UpdateSplitLoadQue( void);
	void	RequestSplitLoad( i3Texture * pTempTex, UINT16 w, UINT16 h, UINT16 lv, I3G_IMAGE_FORMAT format, UINT32 usage);
};

#endif
