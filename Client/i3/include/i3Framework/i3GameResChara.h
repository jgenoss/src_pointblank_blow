#if !defined( __I3_GAME_RES_CHARA_H)
#define __I3_GAME_RES_CHARA_H

#include "i3GameResSceneGraph.h"
#include "i3Chara.h"

class I3_EXPORT_FRAMEWORK i3GameResChara : public i3GameResSceneGraph
{
	I3_EXPORT_CLASS_DEFINE( i3GameResChara, i3GameResSceneGraph);
protected:
	i3Chara *		m_pChara = nullptr;

public:
	i3GameResChara(void);
	virtual ~i3GameResChara(void);

	i3Chara *		getChara(void)				{ return m_pChara; }
	void			setChara( i3Chara * pChara)	{ I3_REF_CHANGE( m_pChara, pChara); }

	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
