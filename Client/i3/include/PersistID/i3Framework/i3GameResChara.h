#if !defined( __I3_GAME_RES_CHARA_H)
#define __I3_GAME_RES_CHARA_H

#include "i3GameResSceneGraph.h"
#include "i3Chara.h"

class I3_EXPORT_FRAMEWORK i3GameResChara : public i3GameResSceneGraph
{
	I3_CLASS_DEFINE( i3GameResChara);
protected:
	i3Chara *		m_pChara;

public:
	i3GameResChara(void);
	virtual ~i3GameResChara(void);

	i3Chara *		getChara(void)				{ return m_pChara; }
	void			setChara( i3Chara * pChara)	{ I3_REF_CHANGE( m_pChara, pChara); }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
