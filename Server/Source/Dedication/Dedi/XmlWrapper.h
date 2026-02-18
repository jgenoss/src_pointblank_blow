/////////////////////////////////////////////////////////////////////////////////
//
// XmlWrapper.h: interface for the XmlWrapper class.
//
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#import "MSXML3.dll" named_guids 


//////////////////////////////////////////////////////////////////////////////////
//
// Xml Document wrapper class
//
//////////////////////////////////////////////////////////////////////////////////
class XmlDocumentWrapper
{
public:
	// external interface
	BOOL Load(LPCTSTR path); 
	BOOL LoadXML(LPCTSTR xmlFileName); 
	MSXML2::IXMLDOMNode* AsNode(); 

public:
	// constructor / destructor
	XmlDocumentWrapper();
	XmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc);
	virtual ~XmlDocumentWrapper();

private:
	// internal function / member
	BOOL IsValid();
	std::string GetXML();
	MSXML2::IXMLDOMDocument* Detach(); 

private:
	MSXML2::IXMLDOMDocumentPtr m_xmldoc;
};



//////////////////////////////////////////////////////////////////////////////////
//
// Xml node wrapper class
//
//////////////////////////////////////////////////////////////////////////////////
class XmlNodeWrapper  
{
public:
	// external interface
	// get node/attribute count
	INT32 NumNodes(); 
	INT32 NumAttributes(); 

	// get string type of nodeType, attribute value/name, Text and Name
	std::string GetNodeType();
	std::string GetAttribVal(int index); 
	std::string GetAttribName(int index); 
	std::string GetText(); 
	std::string GetName();
	
	// get data of node for parsing
	MSXML2::IXMLDOMNode* GetNode(LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* GetNode(int nodeIndex); 
	MSXML2::IXMLDOMNode* FindNode(LPCTSTR searchString);
	
public:
	// constructor / destructor
	XmlNodeWrapper();
	XmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease = TRUE);
	virtual ~XmlNodeWrapper();

private:
	// internal function / member
	BOOL IsValid();
	std::string GetXML();
	MSXML2::IXMLDOMNode* GetPrevSibling();
	MSXML2::IXMLDOMNode* GetNextSibling();
	std::string GetValue(LPCTSTR valueName);
	MSXML2::IXMLDOMNodeList* FindNodes(LPCTSTR searchStr);
	MSXML2::IXMLDOMNode* Detach();
	void operator=(MSXML2::IXMLDOMNodePtr pNode);

private:
	BOOL m_bAutoRelease;
	MSXML2::IXMLDOMNodePtr m_xmlnode;
};



