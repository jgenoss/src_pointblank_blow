#if !defined( __CGAMEOBJECTMANAGER_H__)
#define __CGAMEOBJECTMANAGER_H__

#include "StageDef.h"
class CGameControledObject;

class CGameObjectManager : public i3::shared_ginst<CGameObjectManager>
{

private:
	CGameControledObject *	m_pDestructionObject[TEAM_COUNT];
	CGameControledObject *	m_pDefenceObject[ MAX_OBJECT_COUNT];

public:
	CGameObjectManager( void);
	~CGameObjectManager( void);

	void	SetGameDestructionObject( TEAM_TYPE team, i3Object * pObj);
	void	DestroyObject();

	void	SetGameDefenceObject( INT32 idx, i3Object * pObj);	
	void	EnableObject();	
};

#endif
