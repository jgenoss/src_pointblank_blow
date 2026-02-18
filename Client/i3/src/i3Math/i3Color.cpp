#include "i3MathType.h"
#include "i3Color.h"

void	i3Color::SetToXML( i3XMLElement * pXML, const char * pszName, COLOR * pColor)
{
	COLORREAL col;

	i3Color::Set( &col, pColor);

	SetToXML( pXML, pszName, &col);
}

void	i3Color::SetToXML( i3XMLElement * pXML, const char * pszName, COLORREAL * pColor)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "r", pColor->r);
	pChild->addAttr( "g", pColor->g);
	pChild->addAttr( "b", pColor->b);
	pChild->addAttr( "a", pColor->a);
}

void	i3Color::GetFromXML( i3XMLElement * pXML, const char * pszName, COLOR * pColor)
{
	COLORREAL col;

	GetFromXML( pXML, pszName, &col);

	i3Color::Set( pColor, &col);
}

void	i3Color::GetFromXML( i3XMLElement * pXML, const char * pszName, COLORREAL * pColor)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	
	if( pChild != nullptr)
	{
		pChild->getAttr( "r", &pColor->r);
		pChild->getAttr( "g", &pColor->g);
		pChild->getAttr( "b", &pColor->b);
		pChild->getAttr( "a", &pColor->a);
	}
	else
	{
		I3TRACE( "XML : Could not found ::%s node\n", pszName);
		i3Color::Set( pColor, 1.f, 1.f, 1.f, 1.f);
	}
}
