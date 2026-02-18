#if !defined( __I3_GIC_SHAPE_POLYGON_H__)
#define __I3_GIC_SHAPE_POLYGON_H__

#if defined( I3_DEBUG)

#include "i3GICShapeFigure.h"

/** \brief 최소 5개 이상의 Point를 갖는 다각형 */

class I3_EXPORT_GIC i3GICShapePolygon : public i3GICShapeFigure
{
	I3_CLASS_DEFINE( i3GICShapePolygon);

private:
	i3List			m_PointList;
	POINT *			m_pPtArray;

public:
	I3GIC_POINT *	getPoint( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_PointList.GetCount());
		return (I3GIC_POINT*) m_PointList.Items[ idx];
	}

	INT32			getPointCount( void)							{ return m_PointList.GetCount();  }
	void			setPointCount( INT32 iCount)
	{
		INT32 i;
		I3GIC_POINT * pPoint;

		if( m_PointList.GetCount() < iCount)
		{
			INT32 AddCount = iCount - m_PointList.GetCount();
			
			for( i = 0; i < AddCount; i++)
			{
				pPoint = (I3GIC_POINT*) i3MemAlloc( sizeof( I3GIC_POINT));
				m_PointList.Add( pPoint);
			}
		}
		else if( m_PointList.GetCount() > iCount)
		{
			i3List listTemp;

			INT32 SubCount = m_PointList.GetCount() - iCount;
			for( i = 0; i < SubCount; i++)
			{
				_RemovePoint( i + iCount);
			}

			for( i = 0; i < iCount; i++)
				listTemp.Add( m_PointList.Items[ i]);

			m_PointList.SetCount( iCount);

			if( iCount > 0)
			{
				for( i = 0; i < iCount; i++)
					m_PointList.Items[ i] = listTemp.Items;
			}
		}

		I3MEM_SAFE_FREE( m_pPtArray);
		m_pPtArray = (POINT*) i3MemAlloc( sizeof( POINT) * iCount);
	}

protected:
	void			_CreatePoint( INT32 idx);
	void			_RemovePoint( INT32 idx);

public:
	i3GICShapePolygon();
	virtual ~i3GICShapePolygon();

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, I3RECT * pRect, i3List * pList);

	virtual	bool	OnBeginDrag(void);
	virtual	bool	OnCancelDrag(void);
	
	virtual bool	OnDragCP( i3GICDiagramContext * pCtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//								for Property
public:
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
