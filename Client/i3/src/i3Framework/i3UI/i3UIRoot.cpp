#include "i3FrameworkPCH.h"
#include "i3UIRoot.h"
#include "i3UIEditBox.h"
#include "i3UIManager.h"
#include "i3Base/itl/vector_util.h"


I3_CLASS_INSTANCE(i3UIRoot);

i3UIRoot::~i3UIRoot()
{
	RemoveAll();
}

void i3UIRoot::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	size_t count = m_ChildList.size();

	for( size_t i = 0;i < count; ++i)
	{
		i3GameNode * pNode = m_ChildList[i];
		I3ASSERT( pNode != nullptr);

		pNode->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

void i3UIRoot::AddFromUIManager( i3UIManager * pManager)
{
	I3ASSERT( pManager != nullptr);

	i3UIControl * pChild = static_cast<i3UIControl*>(pManager->getFirstChild());
	
	while( pChild != nullptr)
	{
		I3_MUST_ADDREF( pChild);
		m_ChildList.push_back( pChild);
		pChild = static_cast<i3UIControl*>(pChild->getNext());
	}

	m_nWidth = pManager->getWidth();
	m_nHeight = pManager->getHeight();
}

INT32 i3UIRoot::AddControl( i3UIControl * pCtrl)
{
	I3_MUST_ADDREF( pCtrl);

	m_ChildList.push_back( pCtrl);
	return (INT32)m_ChildList.size() - 1;
}

INT32 i3UIRoot::RemoveControl( i3UIControl * pCtrl)
{
	I3ASSERT( pCtrl != nullptr);

	I3_MUST_RELEASE( pCtrl);
	
	INT32 idx = (INT32)i3::vu::index_of(m_ChildList, pCtrl);

	if (idx == (INT32)m_ChildList.size() ) 
		return -1;

	m_ChildList.erase(m_ChildList.begin() + idx);
	return idx;
}

void i3UIRoot::OnUpdate(REAL32 rDeltaSeconds)
{	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3UIControl * pCtrl = m_ChildList[i];

		pCtrl->OnUpdate( rDeltaSeconds);
	}
}

void i3UIRoot::RemoveAll( void)
{
	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3UIControl * pCtrl = m_ChildList[i];
		I3_MUST_RELEASE( pCtrl);
	}

	m_ChildList.clear();
}

#if !defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct UIROOT
	{
		UINT8			m_ID[4] = { 'U', 'I', 'R', '1' };
		INT32			m_Width = 0;
		INT32			m_Height = 0;

		UINT32			pad[32] = { 0 };
	};
}

#if !defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3UIRoot::OnSave( i3ResourceFile * pResFile)
{
	pack::UIROOT data;
	UINT32 rc, result;

	result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( result);
	
	data.m_Width = m_nWidth;
	data.m_Height = m_nHeight;

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = SaveFromListToResourceFile(m_ChildList, pResFile); // m_ChildList.SaveTo( pResFile);
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3UIRoot::OnLoad( i3ResourceFile * pResFile)
{
	pack::UIROOT data;
	UINT32 rc, result;

	result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	m_nWidth = data.m_Width;
	m_nHeight = data.m_Height;

	rc = LoadFromResourceFileToList(m_ChildList, pResFile, true);		// m_ChildList.LoadFrom( pResFile, true);
	I3_CHKIO( rc);
	result += rc;

	return result;
}

bool i3UIRoot::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3ResourceObject::OnSaveXML( pFile, pXML);

	pXML->addAttr( "Width", m_nWidth);
	pXML->addAttr( "Height", m_nHeight);

	SaveFromListToXML(m_ChildList, "ChildItem", pFile, pXML, true);

	return bResult;
}

bool i3UIRoot::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3ResourceObject::OnLoadXML( pFile, pXML);

	pXML->getAttr( "Width", &m_nWidth);
	pXML->getAttr( "Height", &m_nHeight);

	LoadFromXMLToList(m_ChildList, "ChildItem", pFile, pXML, true);

	return bResult;
}
