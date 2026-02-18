#if !defined( __I3_POLYGON_LIST_H)
#define __I3_POLYGON_LIST_H

#include "i3Base.h"
#include "i3Vector3List.h"

class I3_EXPORT_MATH i3PolygonList : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PolygonList, i3ElementBase);
protected:
	i3::vector<i3Vector3List*>	m_List;
	INT32						m_Count = 0;

public:
	i3PolygonList(void);
	virtual ~i3PolygonList(void);

	void	setCount( INT32 cnt);
	INT32	getCount(void)				{ return m_Count; }

	void	setVectorCount( INT32 cnt);

	i3Vector3List *		getPolygon( INT32 idx)	const	{ return m_List[idx]; }
	i3Vector3List *		getLastPolygon(void)	const	{ return m_List[ m_Count-1]; }
	void				setPolygon( INT32 idx, i3Vector3List * pPoly)
	{
		I3ASSERT( m_Count > idx);
		I3_REF_CHANGE( m_List[idx], pPoly);
	}

	void	removeEnd(void);

	void	reset( bool bFreeAll = false);

	#if defined( I3_DEBUG)
	void	vlog( bool bPlot, UINT8 r, UINT8 g, UINT8 b);
	#endif

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
};

#endif
