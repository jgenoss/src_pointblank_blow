////////////////////////////////////////////////////////////////////////////////
//
// XmlNodeWrapper.cpp: implementation of the XmlNodeWrapper class.
//
/////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "XmlWrapper.h"



//////////////////////////////////////////////////////////////////////////////////
//
// Xml node wrapper class
//
//////////////////////////////////////////////////////////////////////////////////
XmlNodeWrapper::XmlNodeWrapper()
{
	m_xmlnode = NULL;
	m_bAutoRelease = FALSE;
}

XmlNodeWrapper::XmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease)
{
	m_xmlnode = pNode;
	m_bAutoRelease = bAutoRelease;
}

XmlNodeWrapper::~XmlNodeWrapper()
{
	if (!m_bAutoRelease)
		m_xmlnode.Detach();
}

void XmlNodeWrapper::operator=(MSXML2::IXMLDOMNodePtr pNode)
{
	if (IsValid())
		m_xmlnode.Release();

	m_xmlnode = pNode;
}

std::string XmlNodeWrapper::GetValue(LPCTSTR valueName)
{
	if (!IsValid())
		return "";

	MSXML2::IXMLDOMNodePtr attribute = m_xmlnode->Getattributes()->getNamedItem(valueName);
	if (!attribute)
	{
		return "";
	}

	std::string strValue((LPCSTR)attribute->Gettext());

	return strValue;
}

BOOL XmlNodeWrapper::IsValid()
{
	if (m_xmlnode == NULL)
	{
		return FALSE;
	}

	if (m_xmlnode.GetInterfacePtr() == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::GetPrevSibling()
{
	if (!IsValid())
		return NULL;

	return m_xmlnode->GetpreviousSibling().Detach();
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::GetNextSibling()
{
	if (!IsValid())
		return NULL;

	return m_xmlnode->GetnextSibling().Detach();
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::GetNode(LPCTSTR nodeName)
{
	if (!IsValid())
		return NULL;

	try
	{
		return m_xmlnode->selectSingleNode(nodeName).Detach();
	}
	catch (_com_error e)
	{
		// std::string err = e.ErrorMessage();
		// print log
	}

	return NULL;
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::GetNode(int nodeIndex)
{
	if (!IsValid())
		return NULL;

	return m_xmlnode->GetchildNodes()->Getitem(nodeIndex).Detach();
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::FindNode(LPCTSTR searchString)
{
	if (!IsValid())
		return NULL;

	try
	{
		return m_xmlnode->selectSingleNode(searchString).Detach();
	}
	catch (_com_error e)
	{
		// std::string err = e.ErrorMessage();
		// print log
	}

	return NULL;
}

MSXML2::IXMLDOMNode* XmlNodeWrapper::Detach()
{
	if (!IsValid())
		return NULL;
	
	return m_xmlnode.Detach();
}

INT32 XmlNodeWrapper::NumNodes()
{
	if (!IsValid())
		return 0;
	
	return m_xmlnode->GetchildNodes()->Getlength();
}


std::string XmlNodeWrapper::GetXML()
{
	if (!IsValid())
		return "";

	return (LPCSTR)m_xmlnode->Getxml();
}



//////////////////////////////////////////////////////////////////////////////////
//
// Xml Document wrapper class
//
//////////////////////////////////////////////////////////////////////////////////
XmlDocumentWrapper::XmlDocumentWrapper()
{
	CoInitialize(NULL);

	m_xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
}

XmlDocumentWrapper::XmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc)
{
	m_xmldoc = pDoc;
}

XmlDocumentWrapper::~XmlDocumentWrapper()
{
}

BOOL XmlDocumentWrapper::IsValid()
{
	if (m_xmldoc == NULL)
		return FALSE;

	if (m_xmldoc.GetInterfacePtr() == NULL)
		return FALSE;

	return TRUE;
}

MSXML2::IXMLDOMDocument* XmlDocumentWrapper::Detach()
{
	if (!IsValid())
		return NULL;

	return m_xmldoc.Detach();
}

BOOL XmlDocumentWrapper::Load(LPCTSTR path)
{
	OleInitialize(NULL);

	if (!IsValid())
		return FALSE;

	_variant_t v(path);
	m_xmldoc->put_async(VARIANT_FALSE);
	VARIANT_BOOL success = m_xmldoc->load(v);

	if (success != VARIANT_TRUE)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL XmlDocumentWrapper::LoadXML(LPCTSTR xmlFileName)
{
	if (!IsValid())
		return FALSE;

	VARIANT_BOOL success = m_xmldoc->loadXML(xmlFileName);

	if (success != VARIANT_TRUE)
	{
		return FALSE;
	}
	
	return TRUE;
}

MSXML2::IXMLDOMNode* XmlDocumentWrapper::AsNode()
{
	if (!IsValid())
		return NULL;

	return m_xmldoc->GetdocumentElement().Detach();
}

std::string XmlDocumentWrapper::GetXML()
{
	if (!IsValid())
	{
		return "";
	}
	
	return (LPCSTR)m_xmldoc->Getxml();
}

MSXML2::IXMLDOMNodeList* XmlNodeWrapper::FindNodes(LPCTSTR searchStr)
{
	if(!IsValid())
	{
		return NULL;
	}

	try
	{
		return m_xmlnode->selectNodes(searchStr).Detach();
	}
	catch (_com_error e)
	{
		// std::string err = e.ErrorMessage();

		return NULL;
	}	
}

std::string XmlNodeWrapper::GetName()
{
	if (!IsValid())
	{
		return "";
	}

	return (LPCSTR)m_xmlnode->GetbaseName();
}


std::string XmlNodeWrapper::GetText()
{
	if (!IsValid())
	{
		return "";
	}
	
	return (LPCSTR)m_xmlnode->Gettext();
}

INT32 XmlNodeWrapper::NumAttributes()
{
	if (!IsValid())
		return 0;

	MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

	if(!attribs)
		return 0;

	return attribs->Getlength();
}

std::string XmlNodeWrapper::GetAttribName(int index)
{
	if (!IsValid())
		return "";

	MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

	if(!attribs)
		return "";
	
	
	MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

	if(!attrib)
		return "";

	return (LPCSTR)attrib->Getname();
}

std::string XmlNodeWrapper::GetAttribVal(int index)
{
	if (!IsValid())
		return "";

	MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

	if(!attribs)
		return "";

	MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

	if (!attrib)
		return "";

	return (LPCSTR)attrib->Gettext();
}

std::string XmlNodeWrapper::GetNodeType()
{
	if (!IsValid())
	{
		return "";	
	}
	
	return (LPCSTR)m_xmlnode->GetnodeTypeString();
}
