#if !defined( __I3_GIC_SHAPE_H)
#define __I3_GIC_SHAPE_H

#include "i3Math.h"
#include "i3GICObject.h"
#include "i3GICDefine.h"

#if defined( I3_DEBUG)

class i3GICDiagram;
class i3GICControlPoint;
class i3GICShapeLine;
class i3GICDiagramContext;

struct GIC_SHAPE_TREENODE_LINE_INFO
{
	INT32				_iChildIdx = 0;
	i3GICShapeLine *	_pLine = nullptr;
} ;

class I3_EXPORT_GIC i3GICShape : public i3GICObject
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3GICShape, i3GICObject);

protected:
	// Persist member
	I3GIC_STYLE	m_Style = 0;			// Shape Style
	I3GIC_STATE	m_State = 0;			// Shape State

	VEC2D		m_vPos;
	VEC2D		m_vSize;
	REAL32		m_rDepth = 0.0f;			///< View에서 깊이값


	// Volatile members
	VEC2D		m_vSavedPos;		///< Drag mode에서 사용
	VEC2D		m_vSavedSize;

	bool		m_bBindDiagram = false;		// Diagram이 보여지는지에 대한 flag. GDI 객체 생성을 관리하기 위한 용도이다.

protected:
	void		_GetStyleNamed( char * pszOut, INT32 len);

	I3GIC_STYLE	_GetStyleByStr( const char * pszStr);

public:
	I3GIC_STYLE getStyleByStr( const char * pszStr) { return _GetStyleByStr( pszStr); }


public:
	/** \brief Shape의 스타일을 반환합니다. 
		\return I3GIC_STYLE에 정의된 스타일	????	*/
	I3GIC_STYLE	getStyle(void)						{ return m_Style; }

	void		setStyle( I3GIC_STYLE style)		{ m_Style = style; }

	/** \brief Shape의 스타일을 추가합니다. 
		\param mask I3GIC_STYLE에 정의된 스타일
		\return none		*/
	void		addStyle( I3GIC_STYLE mask)			{ m_Style |= mask; }

	/** \brief Shape의 스타일을 제거합니다. 
		\param mask I3GIC_STYLE에 정의된 스타일
		\return none		*/
	void		removeStyle( I3GIC_STYLE mask)		{ m_Style &= ~mask; }

	/** \brief Shape의 스타일을 검사합니다. 
		\param mask I3GIC_STYLE에 정의된 스타일
		\return mask스타일이면 true, 아니면 false			*/
	bool		isStyle( I3GIC_STYLE mask)			{ return (getStyle() & mask) == mask; }


	/** \brief Shape의 상태를 반환합니다. 
		\return I3GIC_STATE에 정의된 상태	????	*/
	I3GIC_STATE	getState(void)						{ return m_State; }

	void		setState( I3GIC_STATE state)		{ m_State = state; }

	/** \brief Shape의 상태를 추가합니다. 
		\param mask I3GIC_STATE에 정의된 상태
		\return none		*/
	void		addState( I3GIC_STATE mask)			{ m_State |= mask; }

	/** \brief Shape의 상태를 제거합니다. 
		\param mask I3GIC_STATE에 정의된 상태
		\return none		*/
	void		removeState( I3GIC_STATE mask)		{ m_State &= ~mask; }

	/** \brief Shape의 상태를 검사합니다. 
		\param mask I3GIC_STATE에 정의된 상태
		\return mask상태이면 true, 아니면 false			*/
	bool		isState( I3GIC_STATE mask)			{ return (getState() & mask) == mask; }

	bool		isSelected(void)					{ return isState( I3GIC_STATE_SELECTED); }

	VEC2D *		getPos( void)						{ return &m_vPos; }

	/** \brief Shape의 x,y값을 설정합니다.
		\param x x값
		\param y y값
		\return none		*/
	void		setPos( REAL32 x, REAL32 y)			{ if( OnPreDragMove( x, y)) { i3Vector::Set( &m_vPos, x, y); OnAfterDragMove( x, y); } }

	void		setPos( VEC2D * pVec)				{ setPos( getX( pVec), getY( pVec)); }

	/** \brief Shape의 x값을 반환합니다.
		\return x값		*/
	REAL32		getPosX(void)						{ return getX( &m_vPos); }

	/** \brief Shape의 x값을 설정합니다.
		\param x x값
		\return none		*/
	void		setPosX( REAL32 x)					{ setPos( x, getY( &m_vPos)); }

	/** \brief Shape의 y값을 반환합니다.
		\return y값		*/
	REAL32		getPosY(void)						{ return getY( &m_vPos); }

	/** \brief Shape의 y값을 설정합니다.
		\param y y값
		\return none		*/
	void		setPosY( REAL32 y)					{ setPos( getX( &m_vPos), y); }


	/** \brief Shape의 width,height값을 설정합니다.
		\param w width값
		\param h height값
		\return none		*/
	void		setSize( REAL32 w, REAL32 h)		{ if( OnPreSize( w, h)) { i3Vector::Set( &m_vSize, w, h); OnAfterSize( w, h); } }

	void		setSize( VEC2D * pVec)				{ setSize( getX( pVec), getY( pVec)); }

	/** \brief Shape의 width값을 반환합니다.
		\return width값		*/
	REAL32		getWidth(void)						{ return getX( &m_vSize); }

	/** \brief Shape의 width값을 설정합니다.
		\param w width값		\return none		*/
	void		setWidth( REAL32 w)					{ setSize( w, getY( &m_vSize));}

	/** \brief Shape의 height값을 반환합니다.
		\return height값		*/
	REAL32		getHeight(void)						{ return getY( &m_vSize); }

	/** \brief Shape의 height값을 설정합니다.
		\param h height값		\return none		*/
	void		setHeight( REAL32 h)				{ setSize( getX( &m_vSize), h);}
	

	/** \brief Shape의 left,top,right,bottom값을 반환합니다. 		\param [out] pRect left,top,right,bottom값을 받을 i3::pack::RECT구조체의 포인터		\return none		*/
	void		getRect( i3::pack::RECT * pRect)
	{
		I3ASSERT( pRect != nullptr);

		pRect->left = getPosX();
		pRect->top = getPosY();
		pRect->right = getPosX() + getWidth();
		pRect->bottom = getPosY() + getHeight();
	}

	/** \brief Shape의 x,y,width,height값을 설정합니다. 		\param [in] pRect left,top,right,bottom값을 갖는 i3::pack::RECT구조체의 포인터		\return none		*/
	void		setRect( i3::pack::RECT * pRect)
	{
		I3ASSERT( pRect != nullptr);

		if( OnPreDragMove( pRect->left, pRect->top) )
		{
			i3Vector::Set( &m_vPos, pRect->left, pRect->top);

			REAL32 w = pRect->right - pRect->left;
			REAL32 h = pRect->bottom - pRect->top;

			if( OnPreSize( w, h))
			{
				i3Vector::Set( &m_vSize, w, h);

				OnAfterDragMove( getX( &m_vPos), getY( &m_vPos));
				OnAfterSize( getX( &m_vSize), getY( &m_vSize));
			}
		}
	}

	VEC2D *		getSavedPos( void)					{ return &m_vSavedPos; }
	void		setSavedPos( VEC2D * pVec)			{ i3Vector::Copy( &m_vSavedPos, pVec); }

	VEC2D *		getSavedSize( void)					{ return &m_vSavedSize; }
	void		setSavedSize( VEC2D * pVec)			{ i3Vector::Copy( &m_vSavedSize, pVec); }

	REAL32		getDepth( void)	const					{ return m_rDepth; }
	void		setDepth( REAL32 rDepth)			{ m_rDepth = rDepth; }

	bool		isBindDiagram( void)				{ return m_bBindDiagram; }

public:
	i3GICShape(void) {}
	virtual ~i3GICShape(void);

	/** \breif Shape을 생성할시 호출
		\note Link Point나 Control Point가 필요한 경우(EditMode) 생성한다.
		*/
	virtual void	OnCreate( void);

	/** \brief pShape을 부모로 하여 붙인다.
	*/
	void			AttachToShape( i3GICShape * pShape);

	/** \brief Parent Shape에서 띄어 낸다.
	*/
	void			DetachFromParentShape( void);

	void			GetTransformedPos( VEC2D * pPos);

	void			RecSetStyle( i3GICShape * pShape, I3GIC_STYLE addStyle, I3GIC_STYLE removeStyle);

	void			RecSetState( i3GICShape * pShape, I3GIC_STATE addState, I3GIC_STATE removeState);

	static void		BakeReformHierarchy( i3GICShape * pShape);
	
	/** \brief Shape을 선택여부를 호출한다.
		\param bFlag 선택 여부
		\return true : 선택가능 Shape, false : 선택할 수 없는 Shape */
	virtual	bool	OnSelect( bool bFlag);

	/** \brief Diagram에 추가될때 호출된다. */
	virtual	void	OnAttach( i3GICDiagram * pDiagram);

	/** \brief Diagram에서 삭제될때 호출된다. */
	virtual	void	OnDetach( i3GICDiagram * pDiagram);

	/** \brief Size 변경시 호출
		\param[in] cx 이동량
		\param[in] cy 이동량
		\return true : Resize 가능, false : Resize 불가능 */
	virtual	bool	OnPreSize( REAL32 cx, REAL32 cy)		{ return true;	}

	/** \brief Control Point를 Dragging한다.
		\param[in] pDiagram i3GICDiagram 객체 포인터
		\return Drag 성공 여부 */
	virtual	bool	OnDragCP( i3GICDiagramContext * pCtx);

	/** \brief Control Value를 Drag값으로 처리하는 함수*/
	virtual bool	OnDragCV( i3GICDiagramContext * pCtx)	{ return true; }

	/** \brief Size를 변경 후 호출
		\param[in] cx 이동량
		\param[in] cy 이동량
		\return N/A */
	virtual	void	OnAfterSize( REAL32 cx, REAL32 cy)	{}

	/** \brief Shape을 이동하기 전에 호출된다
		\param[in] x 이동량
		\param[in] y 이동량
		\return 이동 가능 여부 */
	virtual	bool	OnPreDragMove( REAL32 x, REAL32 y)		{ return true;	}

	/** \brief 모든 Linker를 다른 Shape의 Linker와 Snap되는지 확인 */
	virtual bool	OnAllLinkerSnapToLink( i3GICDiagramContext * pCtx, REAL32 * pX, REAL32 * pY)	{ return true; }

	/** \brief Shape을 이동시킨다.
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\return 이동 성공 여부 */
	virtual	bool	OnDragMoving( i3GICDiagramContext * pCtx);

	/** \brief Shape을 이동시키고 호출
		\param[in] x 이동량
		\param[in] y 이동량
		\return N/A */
	virtual	void	OnAfterDragMove( REAL32 x, REAL32 y)	{}

	virtual	bool	OnBeginDrag(void);
	virtual	bool	OnEndDrag(void);
	virtual	bool	OnCancelDrag(void);

	/** \brief Draw 여부를 반별하여 반환한다.
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\return true : 가능. false : 불가능 */
	bool			IsEnableDraw( i3GICDiagramContext * pCtx);

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);

	/** \brief HitTest 여부를 판별하여 반환한다.
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\return true : 가능. false : 불가능 */
	bool			IsEnableHitTest( i3GICDiagramContext * pCtx);

	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);

	/** \brief 인스턴스 정보를 pDest로 복사합니다. 
		\sa i3TreeNode::CopyTo		*/	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

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

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//										for Property
protected:
	const char *		_getTypeName( void);
	const char *		_getStateName( void);

public:
	/** \brief PropertyGridCtrl에 Property를 등록한다.
		\param[in] pCtrl CMFCPropertyGridCtrl 객체 포인터
		\return N/A */
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);

	/** \brief Shape Type Name을 반환한다.
		\return Shape Type Name */
	const char *		getTypeName( void)		{ return _getTypeName(); }

	////////////////////////////////////////////////////////////////////////////////////////////////
	//									for Link Shape
public:
	virtual void	OnLinkShape( i3GICLinkPoint * pChildLinker, i3GICLinkPoint * pAttachPoint);
	virtual void	OnUnlinkShape( i3GICLinkPoint * pChildLinker, i3GICLinkPoint * pAttachedPoint);
	virtual void	OnUnlinkAllShape( i3GICLinkPoint * pChildLinker);

	/** \brief Link Ponit에 연결된 Shape에 대해서 Drag Begin을 알려준다. */
	virtual void	OnBeginDragLinkedShape( void);

	/** \brief Link Ponit에 연결된 Shape에 대해서 Drag End을 알려준다. */
	virtual void	OnEndDragLinkedShape( void);

	/** \brief Link Ponit에 연결된 Shape에 대해서 Drag Cancel을 알려준다. */
	virtual void	OnCancelDragLinkedShape( void);

	/** \brief Link Point에 연결된 Shape을 Drag에 따라 Update한다.		*/
	virtual void	OnDragLinkedShape( i3GICDiagramContext * pCtx, VEC2D * pUpdatedPos);

	/** \brief 다른 Linker Shape이 이 함수를 호출하여 Update시켜준다.
		\note 영향을 받는 경우 여기에 정의한다.
		\param[in] pEffector 영향을 주는 Linker
		\param[in] Shape에 연결된 Linker
		*/
	virtual void	OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pEffector, i3GICLinkPoint * pLinker);

	////////////////////////////////////////////////////////////////////////////////////////
	//									for Tree Node List
protected:
	i3::vector<GIC_SHAPE_TREENODE_LINE_INFO*>		m_TreeLineList;	///< Parent 관계를 나타내는 Line (TreeNode Edit Mode)

public:
	GIC_SHAPE_TREENODE_LINE_INFO *	getTreeNodeInfo( INT32 idx)
	{
		I3_BOUNDCHK( idx, (INT32)m_TreeLineList.size());
		return m_TreeLineList[ idx];
	}

	void			addTreeNodeInfo( GIC_SHAPE_TREENODE_LINE_INFO * pInfo)	{ m_TreeLineList.push_back( pInfo); }

	void			CreateTreeNodeLine( i3GICShape * pChild, INT32 idx);
	void			DeleteTreeNodeLine( i3GICShape * pChild);
	INT32			FindTreeNodeLine( INT32 iChildIdx);

#if defined( I3_DEBUG_DUMP)
	void			Dump( void);
#endif
};

#endif	// I3_DEBUG

#endif
