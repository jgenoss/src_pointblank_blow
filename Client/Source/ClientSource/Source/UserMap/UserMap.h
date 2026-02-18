#if !defined( __USERMAP_H)
#define __USERMAP_H

#include "UserMap_Define.h"
#include "UserMap_Object.h"
#include "UserMap_Condition.h"
#include "UserMap_World.h"
#include "Grid.h"

class CUserMap : public i3GameObj
{
	I3_CLASS_DEFINE( CUserMap);
protected:
	USMAP::STAGE::INFO		m_StageInfo;

	i3List					m_ObjectList;

	i3List					m_StartConditionList;
	i3List					m_ConditionList;
	i3List					m_RoundEndConditionList;

	CGrid *					m_pGrid;

	CUserMap_World	*		m_pWorld;

protected:
	void					_InitBlocks();
	void					_InitSky(void);
	void					_GenGround( REAL32 cx, REAL32 cy);

public:
	CUserMap(void);
	virtual ~CUserMap(void);

	UINT32					CalcBufferSize(void);

	virtual BOOL	Create( i3Node *pParentNode = NULL, BOOL bEnable = TRUE );

	INT32					getObjectCount(void)						{ return m_ObjectList.GetCount(); }
	CUserMap_Object *		getObject( INT32 idx)
	{
		I3_BOUNDCHK( idx, getObjectCount());

		return (CUserMap_Object *) m_ObjectList.Items[idx];
	}
	void					addObject( CUserMap_Object * pObj);
	CUserMap_Object *		addObject( USMAP::BLOCK::ID id, VEC3D * pPos = NULL);
	void					removeObject( CUserMap_Object * pObj);

	INT32					getStartConditionCount(void)				{ return m_StartConditionList.GetCount(); }
	CUserMap_Condition *	getStartCondition( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_StartConditionList.GetCount());

		return (CUserMap_Condition *) m_StartConditionList.Items[idx];
	}

	INT32					getConditionCount(void)						{ return m_ConditionList.GetCount(); }
	CUserMap_Condition *	getCondition( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_ConditionList.GetCount());

		return (CUserMap_Condition *) m_ConditionList.Items[idx];
	}

	INT32					getRoundEndConditionCount(void)				{ return m_RoundEndConditionList.GetCount(); }
	CUserMap_Condition *	getRoundEndCondition( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_RoundEndConditionList.GetCount());

		return (CUserMap_Condition *) m_RoundEndConditionList.Items[idx];
	}

	void					RemoveAllObject(void);
	void					RemoveAllCondition(void);

	bool					SaveToFile( const char * pszPath);
	bool					LoadFromFile( const char * pszPath);

	CUserMap_Object	*		GetSelectedObject( VEC3D * pvStart, VEC3D *pvDir);
};

#endif
