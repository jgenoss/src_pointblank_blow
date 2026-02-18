#if !defined( __I3_GIC_SHAPE_TEMPLATE_H__)
#define __I3_GIC_SHAPE_TEMPLATE_H__

#if defined( I3_DEBUG)

class i3GICShape;
class i3GICOffsetStack;
class i3GICShapeRect;

class I3_EXPORT_GIC i3GICShapeTemplate : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeTemplate, i3NamedElement);

private:
	i3GICShapeRect *	m_pTemplateRoot = nullptr;

	i3GICOffsetStack * m_pStack = nullptr;

public:
	i3GICShapeRect *	getRoot( void)							{ return m_pTemplateRoot; }

protected:
	static void		_Rec_FindRect( i3GICOffsetStack * pStack, i3GICShape * pShape, VEC2D * pMin, VEC2D * pMax);
	static void		_Rec_SubPos( i3GICOffsetStack * pStack, i3GICShape * pShape, VEC2D * pPos);

	
public:
	i3GICShapeTemplate() {}
	virtual ~i3GICShapeTemplate();

	void			Create( const char * pszName);

	void			EditMode( bool bEdit);

	void			BakeTemplate( void);

	void			BuildObjectInfo( void);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};


#endif	// I3_DEBUG

#endif
