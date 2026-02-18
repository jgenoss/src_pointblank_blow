#include "i3FrameworkPCH.h"
#include "i3UILTreeFilter.h"


I3_CLASS_INSTANCE( i3UILTreeFilter);


void i3UILTreeFilter::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3TreeNode::OnBuildObjectList( List);
}

void i3UILTreeFilter::AddFilter( i3UILTreeFilter * pFilter)
{
	I3ASSERT( i3::same_of<i3UILTreeFilter*>(pFilter));

	AddChild( pFilter);
}

void i3UILTreeFilter::RemoveFilter( i3UILTreeFilter * pFilter)
{
	if( pFilter == nullptr)
		return;

	RemoveChild( pFilter);
}

i3UILTreeElement * i3UILTreeFilter::FindElementByMeta( i3ClassMeta * pMeta)
{
	INT32 i;
	for( i = 0;i < getChildCount(); ++i)
	{
		i3TreeNode * pNode = getChild( i);

		if( i3::same_of< i3UILTreeElement* >(pNode))
		{
			i3UILTreeElement * pElm = (i3UILTreeElement*)pNode;
			if( pElm->getElementMeta()->same_of( pMeta))
				return pElm;
		}
	}

	return nullptr;
}

//
void i3UILTreeFilter::AddTemplate( i3UITemplate * pTemplate)
{
	I3ASSERT( i3::kind_of<i3UITemplate* >(pTemplate));

	i3UILTreeElement * pElm = FindElementByMeta( pTemplate->static_meta());		// ???? static_meta or meta???? (2012.08.08.수빈) ..일단 원본유지..
	if( pElm == nullptr)
	{
		pElm = i3UILTreeElement::new_object_ref();
		pElm->setElementMeta( pTemplate->static_meta());							// ???? static_meta or meta???? (2012.08.08.수빈)  ..일단 원본유지..

		AddChild( pElm);
	}

	pElm->AddElement( pTemplate);
}

bool i3UILTreeFilter::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	return true;
}

bool i3UILTreeFilter::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	return true;
}
