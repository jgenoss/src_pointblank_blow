#include "stdafx.h"
#include "i3GICShapeTemplate.h"
#include "i3GICOffsetStack.h"
#include "i3GICShapeRect.h"
#include "i3GICLinkPoint.h"
#include "i3GICControlPoint.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeTemplate);


i3GICShapeTemplate::~i3GICShapeTemplate()
{
	I3_SAFE_RELEASE( m_pTemplateRoot);
	I3_SAFE_RELEASE( m_pStack);
}

void i3GICShapeTemplate::Create( const char * pszName)
{
	SetName( pszName);
	m_pTemplateRoot = i3GICShapeRect::new_object();
	m_pTemplateRoot->SetName( _T("ShapeRoot"));
	m_pTemplateRoot->OnCreate();
}

void i3GICShapeTemplate::_Rec_FindRect( i3GICOffsetStack * pStack, i3GICShape * pShape, VEC2D * pMin, VEC2D * pMax)
{
	INT32 i;

	// position
	if( pShape->getPosX() + getX( pStack->getTop()) < getX( pMin))
	{
		setX( pMin, pShape->getPosX() + getX( pStack->getTop()));
	}

	if( pShape->getPosY() + getY( pStack->getTop()) < getY( pMin))
	{
		setY( pMin, pShape->getPosY() + getY( pStack->getTop()));
	}

	// size
	if( i3::kind_of<i3GICShapeRect*>(pShape))
	{
		if( pShape->getPosX() + getX( pStack->getTop()) + pShape->getWidth() > getX( pMax))
		{
			setX( pMax, pShape->getPosX() + getX( pStack->getTop()) + pShape->getWidth());
		}

		if( pShape->getPosY() + getY( pStack->getTop()) + pShape->getHeight() > getY( pMax))
		{
			setY( pMax, pShape->getPosY() + getY( pStack->getTop()) + pShape->getHeight());
		}
	}

	pStack->push( pShape->getPos() );

	for( i = 0; i < pShape->getChildCount(); i++)
	{
		_Rec_FindRect( pStack, (i3GICShape*) pShape->getChild( i), pMin, pMax);
	}

	pStack->pop();
}

void i3GICShapeTemplate::_Rec_SubPos( i3GICOffsetStack * pStack, i3GICShape * pShape, VEC2D * pPos)
{
	pStack->push( pShape->getPos());

	REAL32 rX = getX( pStack->getTop()) - getX( pPos);
	REAL32 rY = getY( pStack->getTop()) - getY( pPos);

	pShape->setPos( rX, rY);

	/*INT32 i;
	for( i = 0; i < pShape->getChildCount(); i++)
	{
		_Rec_SubPos( pStack, (i3GICShape*) pShape->getChild( i), pPos);
	}*/

	pStack->pop();
}

void i3GICShapeTemplate::EditMode( bool bEdit)
{
	// Edit Mode에서는 Root를 감춘다.
	if( m_pTemplateRoot == nullptr)
		return;

	if( bEdit)	m_pTemplateRoot->addStyle( I3GIC_STYLE_INVISIBLE | I3GIC_STYLE_NOSELECT_ONLY);
	else		m_pTemplateRoot->removeStyle( I3GIC_STYLE_INVISIBLE | I3GIC_STYLE_NOSELECT_ONLY);

	INT32 i;
	for( i = 0; i < m_pTemplateRoot->getChildCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*)m_pTemplateRoot->getChild( i);

		if( i3::same_of<i3GICLinkPoint* >(pShape)
			|| i3::same_of<i3GICControlPoint* >(pShape))
		{
			if( bEdit)	pShape->addStyle( I3GIC_STYLE_INVISIBLE | I3GIC_STYLE_NOSELECT);
			else		pShape->removeStyle( I3GIC_STYLE_INVISIBLE | I3GIC_STYLE_NOSELECT);
		}
	}
}

void i3GICShapeTemplate::BakeTemplate( void)
{	
	if( m_pTemplateRoot == nullptr)
		return;

	VEC2D vMin, vMax;

	i3Vector::Set( &vMin, 100000.f, 100000.f);
	i3Vector::Set( &vMax, -100000.f, -100000.f);

	m_pStack = i3GICOffsetStack::new_object();

	// 최상단 좌표와 크기를 구한다.
	INT32 i;

	for( i = 0; i < m_pTemplateRoot->getChildCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) m_pTemplateRoot->getChild( i);

		if( !pShape->isStyle( I3GIC_STYLE_CONTROL) &&
			!pShape->isStyle( I3GIC_STYLE_LINKER) )
		{
			_Rec_FindRect( m_pStack, pShape, &vMin, &vMax);
		}
	}

	// 모든 Shape에서 뺀다
	if( getX( &vMin) == 100000.f)
		setX( &vMin, 0.f);

	if( getY( &vMin) == 100000.f)
		setY( &vMin, 0.f);

	for( i = 0; i < m_pTemplateRoot->getChildCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) m_pTemplateRoot->getChild( i);

		if( !pShape->isStyle( I3GIC_STYLE_CONTROL) &&
			!pShape->isStyle( I3GIC_STYLE_LINKER) )
		{
			_Rec_SubPos( m_pStack, pShape, &vMin);
		}
	}

	i3Vector::Sub( &vMax, &vMax, &vMin);

	// Template 의 크기 설정
	m_pTemplateRoot->setWidth( getX( &vMax));
	m_pTemplateRoot->setHeight( getY( &vMax));

	i3GICShape::BakeReformHierarchy( m_pTemplateRoot);

	//_EditMode( true);

	I3_SAFE_RELEASE( m_pStack);
}

void i3GICShapeTemplate::BuildObjectInfo( void)
{
	
}

/*virtual*/ void i3GICShapeTemplate::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3NamedElement::OnBuildObjectList( List);

	if( m_pTemplateRoot != nullptr)
	{
		m_pTemplateRoot->OnBuildObjectList( List);
	}
}

/*virtual*/ bool i3GICShapeTemplate::OnSaveXML(i3XMLFile *pFile, i3XMLElement *pXML)
{
	if( i3NamedElement::OnSaveXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  _T("save fail."));
		return false;
	}

	OBJREF ref;

	ref = pFile->GetObjectPersistID( m_pTemplateRoot);

	pXML->addAttr( _T("TemplateRoot"), (INT32) ref);


	return true;
}


/*virtual*/ bool i3GICShapeTemplate::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3NamedElement::OnLoadXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  _T("save fail."));
		return false;
	}

	OBJREF ref;

	if( pXML->getAttr( _T("TemplateRoot"), (INT32 *) &ref) != nullptr)
	{
		m_pTemplateRoot = (i3GICShapeRect *) pFile->FindObjectByID( ref);
		I3ASSERT( m_pTemplateRoot != nullptr);
		I3_SAFE_ADDREF( m_pTemplateRoot);
		m_pTemplateRoot->SetName( _T("ShapeRoot"));

		// default로 Edit mode
		EditMode( true);
	}


	return true;
}

#endif	// I3_DEBUG
