#if !defined( _CUIGRIP_OBJ_HANDLER_H_)
#define _CUIGRIP_OBJ_HANDLER_H_

class CUIGripObjHandler :	public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( CUIGripObjHandler, i3ElementBase);

protected:

public:
	CUIGripObjHandler(void);
	virtual ~CUIGripObjHandler(void);

public:
	virtual void	setSize( REAL32 width, REAL32 height)	= 0;
	virtual void	setPos( REAL32 left, REAL32 top)	= 0;

	virtual void	setObject( void * pElement)	= 0;
	virtual void	*	getObject(void)	= 0;

	virtual REAL32 getLeft( void)	= 0;
	virtual REAL32 getTop( void)	= 0;
	virtual REAL32 getRight(void)	= 0;
	virtual REAL32 getBottom(void)	= 0;

	virtual REAL32 getRelativeLeft(void) = 0;
	virtual REAL32 getRelativeTop(void) = 0;
	virtual REAL32 getRelativeRight(void) = 0;
	virtual REAL32 getRelativeBottom(void) = 0;

	virtual REAL32 getWidth( void)	= 0;
	virtual REAL32 getHeight( void)	= 0;

	virtual void	setPositionX( REAL32 left) = 0;
	virtual void	setPositionY( REAL32 top) = 0;
	virtual void	setWidth( REAL32 width) = 0;
	virtual void	setHeight( REAL32 height) = 0;

	virtual void	moveLeft( REAL32 left) = 0;
	virtual void	moveTop( REAL32 top) = 0;
	virtual void	moveRight( REAL32 right) = 0;
	virtual void	moveBottom( REAL32 bottom) = 0;

	virtual void	OnSelect(void) = 0;
	virtual void	OnUnSelect(void) = 0;

//	virtual void	start(void) = 0;	// 변경을 하기 전에 필요한 데이터를 저장해둔다.
//	virtual void	cancel(void) = 0;	// 변경 도중 취소, 저장 해둔 데이터로 복귀한다.
};

class CUIGripObjHandler_Shape :	public CUIGripObjHandler
{
	I3_CLASS_DEFINE( CUIGripObjHandler_Shape, CUIGripObjHandler);

protected:
	i3UIShape * m_pShape;

	//REAL32		m_X,m_Y,m_W,m_H;

public:
	CUIGripObjHandler_Shape(void);
	virtual ~CUIGripObjHandler_Shape(void);

public:
	virtual void				setObject( void * pElement) override {	m_pShape = (i3UIShape*)pElement;}
	virtual void			*	getObject(void) override {	return (void*)m_pShape;}

	virtual void				setSize( REAL32 width, REAL32 height) override {	m_pShape->setWidth( width); m_pShape->setHeight( height);}
	virtual void				setPos( REAL32 left, REAL32 top) override {	m_pShape->setPosX( left); m_pShape->setPosY( top);}

	virtual REAL32				getLeft( void) override {	return m_pShape->getPosX();}
	virtual REAL32				getTop( void) override {	return m_pShape->getPosY();}
	virtual REAL32				getRight(void) override {	return m_pShape->getPosX() + m_pShape->getWidth();}
	virtual REAL32				getBottom(void) override {	return m_pShape->getPosY() + m_pShape->getHeight();}

	virtual REAL32				getRelativeLeft(void) override { return getLeft();}
	virtual REAL32				getRelativeTop(void) override { return getTop();}
	virtual REAL32				getRelativeRight(void) override { return getRight();}
	virtual REAL32				getRelativeBottom(void)	 override { return getBottom();}

	virtual REAL32				getWidth( void) override {	return m_pShape->getWidth();}
	virtual REAL32				getHeight( void) override {	return m_pShape->getHeight();}

	virtual void				setPositionX( REAL32 left) override { m_pShape->setPosX( left);}
	virtual void				setPositionY( REAL32 top) override { m_pShape->setPosY( top);}
	virtual void				setWidth( REAL32 width)	 override { m_pShape->setWidth( width);}
	virtual void				setHeight( REAL32 height) override { m_pShape->setHeight( height);}

	virtual void				moveLeft( REAL32 left) override { setPositionX( left + getLeft()); setWidth( getWidth() - left);}
	virtual void				moveTop( REAL32 top) override { setPositionY( top + getTop()); setHeight( getHeight() - top);}
	virtual void				moveRight( REAL32 right) override { setWidth( getWidth() + right);}
	virtual void				moveBottom( REAL32 bottom) override { setHeight( getHeight() + bottom);}

	virtual void				OnSelect(void) override;
	virtual void				OnUnSelect(void) override;

//	virtual void				start(void)	{	m_X = getLeft(); m_Y = getTop(); m_W = getWidth(); m_H = getHeight();}	// 변경을 하기 전에 필요한 데이터를 저장해둔다.
//	virtual void				cancel(void){	setPos( m_X, m_Y); setSize( m_W, m_H);}									// 변경 도중 취소, 저장 해둔 데이터로 복귀한다.
};

class CUIGripObjHandler_RECT :	public CUIGripObjHandler
{
	I3_CLASS_DEFINE( CUIGripObjHandler_RECT, CUIGripObjHandler);

protected:
	i3::pack::RECT *	m_pRect;

	//LONG	left,top,right,bottom;	

public:
	CUIGripObjHandler_RECT(void);
	virtual ~CUIGripObjHandler_RECT(void);

public:
	virtual void				setObject( void * pElement) override {	m_pRect = (i3::pack::RECT*)pElement;}
	virtual void			*	getObject(void)	 override {	return (void*)m_pRect;}

	virtual void				setSize( REAL32 width, REAL32 height) override { setWidth(width); setHeight(height);}
	virtual void				setPos( REAL32 left, REAL32 top) override {setPositionX(left); setPositionY(top);}

	virtual REAL32				getLeft( void) override {	return m_pRect->left;}
	virtual REAL32				getTop( void) override {	return m_pRect->top;}
	virtual REAL32				getRight(void) override {	return m_pRect->right;}
	virtual REAL32				getBottom(void) override {	return m_pRect->bottom;}

	virtual REAL32				getRelativeLeft(void) override { return getLeft();}
	virtual REAL32				getRelativeTop(void) override { return getTop();}
	virtual REAL32				getRelativeRight(void) override { return getRight();}
	virtual REAL32				getRelativeBottom(void)	 override { return getBottom();}

	virtual REAL32				getWidth( void) override {	return (m_pRect->right - m_pRect->left);}
	virtual REAL32				getHeight( void) override {	return (m_pRect->bottom - m_pRect->top);}

	virtual void				setPositionX( REAL32 left) override;
	virtual void				setPositionY( REAL32 top) override;
	virtual void				setWidth( REAL32 width) override { m_pRect->right = (m_pRect->left + width);};
	virtual void				setHeight( REAL32 height) override { m_pRect->bottom = (m_pRect->top + height);};

	virtual void				moveLeft( REAL32 left) override { m_pRect->left += left;}
	virtual void				moveTop( REAL32 top) override { m_pRect->top += top;}
	virtual void				moveRight( REAL32 right) override { m_pRect->right += right;}
	virtual void				moveBottom( REAL32 bottom) override { m_pRect->bottom += bottom;}

	virtual void				OnSelect(void) override {}
	virtual void				OnUnSelect(void) override {}

//	virtual void				start(void)	{	m_X = getLeft(); m_Y = getTop(); m_W = getWidth(); m_H = getHeight();}	// 변경을 하기 전에 필요한 데이터를 저장해둔다.
//	virtual void				cancel(void){	setPos( left, top); setSize( right, bottom);}									// 변경 도중 취소, 저장 해둔 데이터로 복귀한다.
};




class CUIGripObjHandler_UIControl :	public CUIGripObjHandler
{
	I3_CLASS_DEFINE( CUIGripObjHandler_UIControl, CUIGripObjHandler);

protected:
	i3UIControl	*				m_pControl;

public:
	CUIGripObjHandler_UIControl(void);
	virtual ~CUIGripObjHandler_UIControl(void);

public:
	virtual void				setObject( void * pElement) override {	m_pControl = (i3UIControl*)pElement;}
	virtual void			*	getObject(void) override {	return (void*)m_pControl;}

	virtual void				setSize( REAL32 width, REAL32 height) override { setWidth(width); setHeight(height);}
	virtual void				setPos( REAL32 left, REAL32 top) override {setPositionX(left); setPositionY(top);}

	virtual REAL32				getLeft( void) override {	return m_pControl->getPos()->x;}
	virtual REAL32				getTop( void) override {	return m_pControl->getPos()->y;}
	virtual REAL32				getRight(void) override {	return m_pControl->getPos()->x + m_pControl->getWidth();}
	virtual REAL32				getBottom(void) override {	return m_pControl->getPos()->y + m_pControl->getHeight();}

	virtual REAL32				getRelativeLeft(void) override {	return m_pControl->getAccumulatedPos()->x;}
	virtual REAL32				getRelativeTop(void) override {	return m_pControl->getAccumulatedPos()->y;}
	virtual REAL32				getRelativeRight(void) override {	return m_pControl->getAccumulatedPos()->x + m_pControl->getWidth();}
	virtual REAL32				getRelativeBottom(void)	 override {	return m_pControl->getAccumulatedPos()->y + m_pControl->getHeight();}

	virtual REAL32				getWidth( void) override {	return (m_pControl->getWidth());}
	virtual REAL32				getHeight( void) override {	return (m_pControl->getHeight());}

	virtual void				setPositionX( REAL32 left) override { m_pControl->setPos( left, getTop());}
	virtual void				setPositionY( REAL32 top) override { m_pControl->setPos( getLeft(), top);}
	virtual void				setWidth( REAL32 width)	 override { m_pControl->setSize( width, getHeight());}
	virtual void				setHeight( REAL32 height) override { m_pControl->setSize( getWidth(), height);}

	virtual void				moveLeft( REAL32 left) override { setPositionX( getLeft() + left); setWidth( getWidth() - left);}
	virtual void				moveTop( REAL32 top) override { setPositionY( getTop() + top); setHeight( getHeight() - top);}
	virtual void				moveRight( REAL32 right) override { setWidth( getWidth() + right);}
	virtual void				moveBottom( REAL32 bottom) override { setHeight( getHeight() + bottom);}

	virtual void				OnSelect(void) override {}
	virtual void				OnUnSelect(void) override {}

//	virtual void				start(void)	{	m_X = getLeft(); m_Y = getTop(); m_W = getWidth(); m_H = getHeight();}	// 변경을 하기 전에 필요한 데이터를 저장해둔다.
//	virtual void				cancel(void){	setPos( left, top); setSize( right, bottom);}									// 변경 도중 취소, 저장 해둔 데이터로 복귀한다.
};
#endif