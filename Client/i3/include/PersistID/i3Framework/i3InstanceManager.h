#if !defined ( __I3_INSTANCE_MANAGER_H__)
#define __I3_INSTANCE_MANAGER_H__

#include "i3GameNode.h"

class I3_EXPORT_FRAMEWORK i3InstanceManager : public i3GameNode
{
	I3_CLASS_DEFINE( i3InstanceManager);

protected:
	i3ClassMeta *	m_pInstMeta;	//현재 매니저가 관리하는 클래스

	INT32			m_nAllocCount;	//인스턴스의 갯수
	i3List		*	m_pInstList;	//인스턴스 리스트

protected:
	i3GameNode	*	_AddNewInstance();

public:
	i3ClassMeta *	getInstanceMeta()	{	return m_pInstMeta;}
	void			setInstanceMeta( i3ClassMeta * pMeta)		
	{	
		m_pInstMeta = pMeta;
	}

	BOOL			Create( i3ClassMeta * pMeta, INT32 nCount);
	void			Destroy();

	void	*		GetFreeInstance();
	INT32			GetAllocCount( void)		{	return m_nAllocCount;						}

public:
	i3InstanceManager();
	virtual ~i3InstanceManager();

	virtual	void	OnUpdate( RT_REAL32 rDeltaSeconds);
};

#endif