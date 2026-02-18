#if !defined( __OBJECT_JUMPHOLD_H__)
#define __OBJECT_JUMPHOLD_H__

#include "GameControledObject.h"

class WeaponBase;
class CGameCharaBase;

class CGameObjectJumpHold : public CGameControledObject
{
private:
	I3_EXPORT_CLASS_DEFINE(CGameObjectJumpHold, CGameControledObject);
private:
	i3Animation*		m_pAnim;

	REAL32				m_rJumpPower;
	REAL32				m_rJumpHeight;
	REAL32				m_rJumpDig;
	REAL32				m_rJumpAnimSpeed;
	REAL32				m_rManaulClampTime;
public:
	CGameObjectJumpHold(void);
	virtual ~CGameObjectJumpHold(void);

	virtual UINT32	OnQueryGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;

#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	virtual void	Create(i3Framework * pFramework, i3GameResObject * pRes) override;
	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject(REAL32 rDeltaSeconds) override;
	virtual void	SetControledObjectState(INT32 state) override {}
	virtual INT32	GetControledObjectState(void) override { return -1; }

	REAL32	getJumpPower()					{ return m_rJumpPower; }
	void	setJumpPower(REAL32 power)		{ m_rJumpPower = power; }
	REAL32	getJumpDig()					{ return m_rJumpDig; }
	void	setJumpDig(REAL32 dig)			{ m_rJumpDig = dig; }
	REAL32	getJumpHeight()					{ return m_rJumpHeight; }
	void	setJumpHeight(REAL32 height)	{ m_rJumpHeight = height; }
	REAL32	getJumpAnimSpeed()				{ return m_rJumpAnimSpeed; }
	void	setJumpAnimSpeed(REAL32 speed)	{ m_rJumpAnimSpeed = speed; }

	BOOL	JumpToChara(const NxShape* pShape, CGameCharaBase * pChara);
	void	PlayJumpAnim();
};

#endif
