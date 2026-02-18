#include "i3FrameworkPCH.h"
#include "i3UILTreeNode_Template.h"
#include "i3UITemplate.h"

I3_CLASS_INSTANCE( i3UILTreeNode_Template);


i3UILTreeNode_Template::~i3UILTreeNode_Template(void)
{
	I3_SAFE_RELEASE( m_pTemplate);	// ref--
}

void i3UILTreeNode_Template::SetTemplate( i3UITemplate* pTemplate)
{
	//I3ASSERT( pTemplate != nullptr);

	I3_REF_CHANGE( m_pTemplate, pTemplate);		// ref++
}

void i3UILTreeNode_Template::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3TreeNode::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3UILTreeNode_Template*>(pDest));

	i3UILTreeNode_Template * pDestNode = (i3UILTreeNode_Template*)pDest;

	i3UITemplate* pTemplate = m_pTemplate;
	if( method == I3_COPY_INSTANCE)
	{
		if( m_pTemplate != nullptr)
		{
			pTemplate =	m_pTemplate->create_instance(); 	// (i3UITemplate*)CREATEINSTANCE( m_pTemplate->GetMeta());	// ref 0

			m_pTemplate->CopyTo( pTemplate , I3_COPY_INSTANCE);
		}
	}

	pDestNode->SetTemplate( pTemplate);	// 1
}

void i3UILTreeNode_Template::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	m_pTemplate->OnBuildObjectList( List);

	i3TreeNode::OnBuildObjectList( List);
}

UINT32 i3UILTreeNode_Template::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;

	Result = i3TreeNode::OnSave( pResFile);
	if ( Result == STREAM_ERR)
		return STREAM_ERR;

	I3ASSERT( m_pTemplate != nullptr);

	OBJREF ref = pResFile->GetObjectPersistID( m_pTemplate);

	Rc = pResFile->Write( &ref, sizeof(ref));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UILTreeNode_Template::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;
	OBJREF nID;

	i3Stream * pStream = pResFile->GetStream();

	Result = i3TreeNode::OnLoad( pResFile);

	if ( Result == STREAM_ERR)
		return STREAM_ERR;

	Rc = pStream->Read( &nID, sizeof(nID));
	I3_CHKIO( Rc);
	Result += Rc;

	if ( nID != 0)
	{
		I3_REF_CHANGE(m_pTemplate, (i3UITemplate*) pResFile->FindObjectByID(nID));
		I3ASSERT( m_pTemplate != nullptr);
	}

	return Result;
}

bool i3UILTreeNode_Template::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnSaveXML( pFile, pXML);
	
	if( bRv)
	{
		I3ASSERT( m_pTemplate != nullptr);
		OBJREF nID = pFile->GetObjectPersistID( m_pTemplate);
		pXML->addAttr( "Template", nID);
	}

	return bRv;
}

bool i3UILTreeNode_Template::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3TreeNode::OnLoadXML( pFile, pXML);

	if( bRv)
	{
		OBJREF iVal;
		pXML->getAttr( "Template", &iVal);
		I3ASSERT( iVal != 0);

		SetTemplate( (i3UITemplate*)pFile->FindObjectByID( iVal));
	}
	
	return bRv;
}

void i3UILTreeNode_Template::SetName( const i3::rc_string& strName )
{
	if( m_pTemplate != nullptr)
	{
		//m_pTemplate->SetName( strName);

		// Checksum이 갱신되기 위해서는 SetName()이 아니라 SetResourcePath() 함수를 호출해야 한다.
		// SetResourcePath() 함수 내에서 SetName() 함수를 호출한다. 단, 이름이 Unix Path Style로 변경된다.
		m_pTemplate->SetResourcePath( strName.c_str());
	}

	i3TreeNode::SetName( strName);
}