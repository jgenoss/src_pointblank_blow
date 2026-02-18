#if !defined ( __I3_INSTANCE_MANAGER_H__)
#define __I3_INSTANCE_MANAGER_H__

#include "i3GameNode.h"


class I3_EXPORT_FRAMEWORK i3InstanceManager : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE( i3InstanceManager, i3GameNode);

protected:
	i3ClassMeta *	m_pInstMeta = nullptr;	//현재 매니저가 관리하는 클래스

	INT32			m_nAllocCount = 0;	//인스턴스의 갯수
	i3::vector<i3GameNode*>	m_InstList;	//인스턴스 리스트

protected:
	i3GameNode	*	_AddNewInstance();

public:
	i3ClassMeta *	getInstanceMeta()	{	return m_pInstMeta;}
	void			setInstanceMeta( i3ClassMeta * pMeta)		
	{	
		m_pInstMeta = pMeta;
	}

	bool			Create( i3ClassMeta * pMeta, INT32 nCount);
	void			Destroy();

	void	*		GetFreeInstance();
	INT32			GetAllocCount( void)		{	return m_nAllocCount;						}

#if defined( I3_DEBUG)
	void			Dump();
#endif

public:
	virtual ~i3InstanceManager();

	virtual	void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif