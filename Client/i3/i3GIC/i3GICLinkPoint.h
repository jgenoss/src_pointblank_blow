#if !defined( __I3_GIC_LINK_POINT_H__)
#define __I3_GIC_LINK_POINT_H__

#include "i3GICShape.h"

#if defined( I3_DEBUG)

typedef UINT32	GIC_LINK_FLAG;

#define	GIC_LINK_FLAG_DRAG_MOVE	0x00000001			///< Drag Move에 대한 Link를 Update한다.
#define GIC_LINK_FLAG_DRAG_CP		0x00000002			///< Drag CP에 대한 Link를 Update한다.
#define GIC_LINK_FLAG_UPDATED		0x00000004			///< Link point에 Update 유무

class I3_EXPORT_GIC i3GICLinkPoint : public i3GICShape
{
	I3_EXPORT_CLASS_DEFINE( i3GICLinkPoint, i3GICShape);
private:
	GIC_LINK_FLAG	m_LinkFlags = 0;

	i3::vector<i3GICLinkPoint*> 	m_LinkedShapeList;

	// Volatile members
	VEC2D			m_vUpdatedPos;
	VEC2D			m_vDtTransformedPos;

	static HPEN		m_hpenLinkPoint;
	static INT32	m_hpenRefCnt;

protected:
	void			_CreateGDILinkPointObject( void);
	void			_DeleteGDILinkPointObject( void);

public:
	GIC_LINK_FLAG	getLinkFlags( void)						{ return m_LinkFlags; }
	void			setLinkFlags( GIC_LINK_FLAG flag)		{ m_LinkFlags = flag; }
	void			addLinkFlags( GIC_LINK_FLAG flag)		{ m_LinkFlags |= flag; }
	void			removeLinkFlags( GIC_LINK_FLAG flag)	{ m_LinkFlags &= ~flag; }

	INT32			getLinkedShapeCount( void)				{ return (INT32)m_LinkedShapeList.size(); }

	i3GICLinkPoint *	getLinkedShape( INT32 idx)
	{
		I3_BOUNDCHK( idx, (INT32)m_LinkedShapeList.size());
		return m_LinkedShapeList[ idx];
	}

	void			addLinkedShape( i3GICLinkPoint * pShape)	{ m_LinkedShapeList.push_back( pShape); }

	void			removeLinkedShape( i3GICLinkPoint * pShape)	{ i3::vu::remove(m_LinkedShapeList, pShape); }

	void			removeAllLinkedShape( void)					{ m_LinkedShapeList.clear(); }

	INT32			findLinkedShape( i3GICLinkPoint * pShape)
	{
		for(size_t i = 0; i < m_LinkedShapeList.size(); i++)
		{
			if( getLinkedShape( i) == pShape)
				return (INT32)i;
		}
		return -1;
	}

	VEC2D *			getUpdatedPos( void)					{ return &m_vUpdatedPos; }
	void			setUpdatedPos( VEC2D * pVec)			{ i3Vector::Copy( &m_vUpdatedPos, pVec); }

public:
	i3GICLinkPoint();
	virtual ~i3GICLinkPoint();

	/** \brief Link Point는 자신이 Drag하지 않는다.
		\return Always return false*/
	virtual bool	OnPreSize( REAL32 cx, REAL32 cy)		{ return false;}

	/** \brief HitTest By Point
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\param[in] x 좌표
		\param[in] y 좌표
		\return Hit 여부 */
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);

	/** \brief Shape을 그린다.
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\return N/A */
	virtual void	OnDraw( i3GICDiagramContext * pCtx);

	/** \brief Linker에 연결된 Shape에 대해 BeginDrag 이벤트를 준다. */
	void			BeginDragEventToLinkedShape( void);

	/** \brief Linker에 연결된 Shape에 대해 EndDrag 이벤트를 준다. */
	void			EndDragEventToLinkedShape( void);

	/** \brief Linker에 연결된 Shape에 대해 CancelDrag 이벤트를 준다. */
	void			CancelDragEventToLinkedShape( void);

	/** \brief Linker에 연결된 Shape을 업데이트한다. */
	void			UpdateLinkedShape( i3GICDiagramContext * pCtx, VEC2D * pUpdatedPos);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);
	
	/**	\brief 파일로 저장한다.
		\param[in] pResFile i3ResourceFile 객체 포인터
		\return 저장된 Data Size	*/
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);

	/**	\brief 파일에서 읽어온다.
		\param[in] pResFile i3ResourceFile 객체 포인터
		\return 읽어들인 Data Size	*/
	virtual	UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
