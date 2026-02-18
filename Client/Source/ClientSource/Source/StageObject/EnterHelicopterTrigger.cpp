#include "pch.h"
#include "EnterHelicopterTrigger.h"
#include "GameCharaBase.h"
#include "ObjectRidingHelicopter.h"

I3_CLASS_INSTANCE( CEnterHelicopterTrigger);//, i3Trigger);

CEnterHelicopterTrigger::CEnterHelicopterTrigger()
{
}

CEnterHelicopterTrigger::~CEnterHelicopterTrigger()
{
}

/*virtual*/ void CEnterHelicopterTrigger::OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
	if( i3::kind_of<CGameCharaBase*>(pDest))
	{
		setMyEnterHelicopter((CGameCharaBase*)pDest, true);
	}
	else if(i3::kind_of<i3PhysixShapeSet*>(pDest))
	{
		if( i3::kind_of<CGameCharaBase*>(((i3PhysixShapeSet*)pDest)->getEventReceiver()))
		{
			setMyEnterHelicopter((CGameCharaBase*)(((i3PhysixShapeSet*)pDest)->getEventReceiver()), true);
		}
	}
}

/*virtual*/ void CEnterHelicopterTrigger::OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
	if( i3::kind_of<CGameCharaBase*>(pDest))
	{
		setMyEnterHelicopter((CGameCharaBase*)pDest, false);
	}
	else if(i3::kind_of<i3PhysixShapeSet*>(pDest))
	{
		if( i3::kind_of<CGameCharaBase*>(((i3PhysixShapeSet*)pDest)->getEventReceiver()))
		{
			setMyEnterHelicopter((CGameCharaBase*)(((i3PhysixShapeSet*)pDest)->getEventReceiver()), false);
		}
	}
}

void CEnterHelicopterTrigger::setMyEnterHelicopter(CGameCharaBase* pChara, bool bFlag)
{
	if( pChara->isLocal())
	{
		for(INT32 i = 0; i < getLinkObjCount(); i++)
		{
			i3Object * pObj = getLinkObj( i);
			if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
			{
				((CGameObjectRidingHelicopter*)pObj)->setMyEnterHelicopter( bFlag);
			}
		}
	}
}



