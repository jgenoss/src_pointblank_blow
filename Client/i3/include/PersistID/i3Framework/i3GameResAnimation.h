#if !defined( __I3_GAME_RES_ANIMATION_H)
#define __I3_GAME_RES_ANIMATION_H

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResAnimation : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResAnimation);
protected:
	i3Animation * m_pAnim;

public:
	i3GameResAnimation(void);
	virtual ~i3GameResAnimation(void);

	i3Animation *	getAnimation(void)					{ return m_pAnim; }
	void			setAnimation( i3Animation * pAnim)	{ I3_REF_CHANGE( m_pAnim, pAnim); }

	BOOL			Load( const char * pszFile);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	/*
	virtual UINT32	LoadFromFile( i3Stream * pStream);
	virtual UINT32	SaveToFile( i3Stream * pStream);
	*/
};

#endif
