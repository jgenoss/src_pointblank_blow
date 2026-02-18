#include "i3FrameworkPCH.h"
#include "i3UILTreeElement.h"
#include "i3UITemplate.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3UILTreeElement);

i3UILTreeElement::~i3UILTreeElement(void)
{
	// m_ElmList.SafeReleaseAll();
	i3::for_each(m_ElmList.begin(), m_ElmList.end(), i3::fu::functor_safe(i3::mem_fun(&i3UITemplate::Release)));
}

void i3UILTreeElement::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	
	for(size_t i = 0;i < m_ElmList.size(); ++i)
	{
		i3UITemplate * pTemplate = m_ElmList[i];

		pTemplate->OnBuildObjectList( List);
	}

	i3TreeNode::OnBuildObjectList( List);
}

INT32 i3UILTreeElement::AddElement( i3UITemplate * pElm)
{
	I3ASSERT( pElm->same_of( m_pClassMeta));

	I3_SAFE_ADDREF( pElm);

	m_ElmList.push_back(pElm);

	return m_ElmList.size() - 1;
}

void i3UILTreeElement::removeElement( i3UITemplate * pElm)
{
	I3ASSERT( pElm->same_of( m_pClassMeta));

	size_t idx = i3::vu::index_of(m_ElmList, pElm);		//m_ElmList.Find( pElm);
	if( idx != m_ElmList.size() )
	{
		//m_ElmList.Delete( idx);
		m_ElmList.erase(m_ElmList.begin() + idx);

		I3_SAFE_RELEASE( pElm);
	}
}

bool i3UILTreeElement::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr( "Count", (INT32)m_ElmList.size());

	if( m_pClassMeta != nullptr)
		pXML->addAttr( "ManagingClass", m_pClassMeta->class_name().c_str());
	else
		pXML->addAttr( "ManagingClass", "");

	
	char conv[MAX_PATH];
	for(size_t i = 0;i < m_ElmList.size(); ++i)
	{
		i3::snprintf( conv, MAX_PATH-1, "Template_%d", i);

		i3UITemplate * pTemplate = m_ElmList[i];

		pXML->addAttr( conv, pFile->GetObjectPersistID( pTemplate));
	}

	return true;
}


bool i3UILTreeElement::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 i, iVal, count;

	pXML->getAttr( "Count", &count);

	const char * pszClassName = pXML->Attribute("ManagingClass");
	m_pClassMeta = i3ClassMetaSystem::i()->find_class_meta( pszClassName);

	char conv[MAX_PATH];
	for( i = 0;i < count; ++i)
	{
		i3::snprintf( conv, MAX_PATH-1, "Template_%d", i);

		pXML->getAttr( conv, &iVal);

		I3ASSERT( iVal != 0);

		i3UITemplate * pTemplate = (i3UITemplate*)pFile->FindObjectByID( iVal);
		I3ASSERT( pTemplate != nullptr);

		AddElement( pTemplate);
	}

	return true;
}