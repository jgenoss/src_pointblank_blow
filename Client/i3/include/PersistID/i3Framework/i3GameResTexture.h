#if !defined( __I3_GAME_RES_TEXTURE_H)
#define __I3_GAME_RES_TEXTURE_H

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResTexture : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResTexture);

protected:
	i3Texture *	m_pTex;

public:
	i3GameResTexture(void);
	virtual ~i3GameResTexture(void);

	i3Texture *		getTexture(void)				{ return m_pTex; }
	void			setTexture( i3Texture * pTex)	{ I3_REF_CHANGE( m_pTex, pTex); }

	BOOL			Load( const char * pszFile);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
