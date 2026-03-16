#if !defined( __I3_POLYGON_LIST_H)
#define __I3_POLYGON_LIST_H

#include "i3Base.h"
#include "i3Vector3List.h"

class I3_EXPORT_MATH i3PolygonList : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PolygonList);
protected:
	i3List			m_List;
	INT32			m_Count;

public:
	i3PolygonList(void);
	virtual ~i3PolygonList(void);

	void	setCount( INT32 cnt);
	INT32	getCount(void)				{ return m_Count; }

	void	setVectorCount( INT32 cnt);

	i3Vector3List *		getPolygon( INT32 idx)		{ return (i3Vector3List *) m_List.Items[idx]; }
	i3Vector3List *		getLastPolygon(void)		{ return (i3Vector3List *) m_List.Items[ m_Count-1]; }
	void				setPolygon( INT32 idx, i3Vector3List * pPoly)
	{
		I3ASSERT( m_Count > idx);
		i3Vector3List * pOld = (i3Vector3List *) getPolygon( idx);

		pPoly->AddRef();
		m_List.Items[ idx] = pPoly;

		I3_SAFE_RELEASE( pOld);
	}

	void	removeEnd(void);

	void	reset( bool bFreeAll = false);

	#if defined( I3_DEBUG)
	void	vlog( bool bPlot, UINT8 r, UINT8 g, UINT8 b);
	#endif

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
};

#endif
