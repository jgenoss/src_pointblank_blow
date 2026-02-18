#if !defined( __I3_GAME_RES_TEXTURE_H)
#define __I3_GAME_RES_TEXTURE_H

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResTexture : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResTexture, i3GameRes);

protected:
	i3Texture *	m_pTex = nullptr;

public:
	i3GameResTexture(void);
	virtual ~i3GameResTexture(void);

	i3Texture *		getTexture(void)				{ return m_pTex; }
	void			setTexture( i3Texture * pTex)	{ I3_REF_CHANGE( m_pTex, pTex); }

	bool			Load( const char * pszFile);

	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
