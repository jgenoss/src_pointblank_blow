#if !defined( __I3_GAME_RES_ANIMATION_H)
#define __I3_GAME_RES_ANIMATION_H

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResAnimation : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResAnimation, i3GameRes);
protected:
	i3Animation * m_pAnim = nullptr;

public:
	i3GameResAnimation(void);
	virtual ~i3GameResAnimation(void);

	i3Animation *	getAnimation(void)					{ return m_pAnim; }
	void			setAnimation( i3Animation * pAnim)	{ I3_REF_CHANGE( m_pAnim, pAnim); }

	bool			Load( const char * pszFile);

	virtual void	Destroy( void) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
