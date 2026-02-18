#if !defined( __I3_LV_RES_CHARA_H)
#define __I3_LV_RES_CHARA_H

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResChara : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResChara, i3LevelRes);

protected:
	UINT32			m_LODCount = 0;
	UINT32			m_AIStateCount = 0;
	UINT32			m_AnimCount = 0;

	i3Chara *		m_pChara = nullptr;

public:
	i3LevelResChara(void);
	virtual ~i3LevelResChara(void);

	UINT32			getLODCount(void)				{ return m_LODCount; }
	void			setLODCount( UINT32 cnt)		{ m_LODCount = cnt; }

	UINT32			getAIStateCount(void)			{ return m_AIStateCount; }
	void			setAIStateCount( UINT32 cnt)	{ m_AIStateCount = cnt; }

	UINT32			getAnimCount(void)				{ return m_AnimCount; }
	void			setAnimCount( UINT32 cnt)		{ m_AnimCount = cnt; }

	i3Chara *		getChara(void)					{ return m_pChara; }
	void			setChara( i3Chara * pChara);

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual bool				OnValidateBrief( bool bForce) override;
	virtual bool				OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *			CreateGameRes(void) override;

	virtual UINT32				OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32				OnLoad( i3ResourceFile * pResFile) override;
};

#endif
