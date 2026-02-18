#if !defined( I3_XML_H)
#define I3_XML_H

/*************************************************************************

class: TinyXML (C++ XML parser)
update: 2008-01-04
담당자: 백성민

**************************************************************************/

#include "tinyxml.h"

typedef TiXmlDocument		i3XMLDocument;
typedef TiXmlNode			i3XMLNode;
typedef TiXmlComment		i3XMLComment;
typedef TiXmlUnknown		i3XMLUnknown;
typedef TiXmlAttribute		i3XMLAttribute;
typedef TiXmlText			i3XMLText;
typedef TiXmlDeclaration	i3XMLDeclaration;
typedef TiXmlParsingData	i3XMLParsingData;

#include "i3XMLElement.h"

#endif
