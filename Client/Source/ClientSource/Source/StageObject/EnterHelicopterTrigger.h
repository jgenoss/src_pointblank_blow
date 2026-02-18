#if !defined( __ENTER_HELICOPTER_TRIGGER_H__)
#define __ENTER_HELICOPTER_TRIGGER_H__

#include "LinkObjTrigger.h"

class CEnterHelicopterTrigger : public CLinkObjTrigger
{
	I3_CLASS_DEFINE( CEnterHelicopterTrigger, CLinkObjTrigger);
protected:

public:
	CEnterHelicopterTrigger(void);
	virtual ~CEnterHelicopterTrigger(void);

	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;

	void setMyEnterHelicopter(CGameCharaBase* pChara, bool bFlag);
};

#endif
