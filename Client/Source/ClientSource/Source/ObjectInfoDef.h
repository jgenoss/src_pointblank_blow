#if !defined( __OBJECT_INFO_DEF_H__)
#define __OBJECT_INFO_DEF_H__

struct OBJECT_COUNT_INFO
{
	i3::vector<i3Object*>		m_pNoneNetObjectList;
	i3::vector<i3Object*>		m_pStaticObjectList;
	i3::vector<i3Object*>		m_pMoveObjectList;
	i3::vector<i3Object*>		m_pDynamicObjectList;
	i3::vector<i3Object*>		m_pAnimObjectList;
	i3::vector<i3Object*>		m_pControledObjectList;
	i3::vector<i3Object*>		m_pEventObjectList;
	i3::vector<i3Object*>		m_pTotalObjectList;

	void	Reset()
	{
		m_pNoneNetObjectList.clear();
		m_pStaticObjectList.clear();
		m_pMoveObjectList.clear();
		m_pDynamicObjectList.clear();
		m_pAnimObjectList.clear();
		m_pControledObjectList.clear();
		m_pEventObjectList.clear();
		m_pTotalObjectList.clear();
	}
	
};

#endif
