#include "i3OptPCH.h"
#include "i3SceneOptimizer.h"

I3_CLASS_INSTANCE( i3SceneOptimizer);

i3SceneOptimizer::~i3SceneOptimizer(void)
{
	I3_SAFE_RELEASE( m_pSg);
	I3_SAFE_RELEASE( m_pStack);
}

void i3SceneOptimizer::SetTraceRoot( i3Node * pNewRoot)
{
	if( m_pSg != nullptr)
	{
		switch( m_Phase)
		{
			case PHASE_COMMON_SG :
				m_pSg->setCommonSg( pNewRoot);
				break;

			default :
				m_pSg->setInstanceSg( pNewRoot);
				break;
		}
	}

	m_pTraceRoot = pNewRoot;
}

void i3SceneOptimizer::SetMatrixStackEnable( bool bFlag)
{
	m_bMatrixStackEnable = bFlag;

	if( bFlag)
	{
		if( m_pStack == nullptr)
		{
			m_pStack = i3MatrixStack::new_object();
		}
	}
	else
	{
		I3_SAFE_RELEASE( m_pStack);
	}
}

void i3SceneOptimizer::Rec_Trace( i3Node * pNode, i3Node * pParent)
{
	INT32 i;
	i3Node * pChild;
	bool bPushTrans = false;

	if( OnPreNode( pNode, pParent) == false)
		return;

	if( (m_bMatrixStackEnable) && (i3::kind_of<i3Transform*>(pNode)))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		// Matrix Stack에 반영..
		m_pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());

		bPushTrans = true;
	}

	if( OnNode( pNode, pParent) == false)
	{
		if( bPushTrans)
			m_pStack->Pop();
		return;
	}

	if( i3::kind_of< i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			if( pLOD->getShapeNode() != nullptr)
			{
				Rec_Trace( pLOD->getShapeNode(), pNode);
			}
		}
	}
	else if(i3::kind_of< i3Particle* >( pNode))
	{
		i3Particle * pParticle = (i3Particle *) pNode;

		if( pParticle->GetMesh() != nullptr)
		{
			Rec_Trace( pParticle->GetMesh(), pNode);
		}
	}
	else if( i3::kind_of<i3BinaryPartitionNode* >(pNode))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pNode;

		if( pBSP->getFront() != nullptr)
			Rec_Trace( pBSP->getFront(), pNode);

		if( pBSP->getBack() != nullptr)
			Rec_Trace( pBSP->getBack(), pNode);
	}
	else if( i3::kind_of< i3OcTreeNode* >(pNode))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pNode;

		for( i = 0; i < 8; i++)
		{
			pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
				Rec_Trace( pChild, pNode);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_Trace( pChild, pNode);
	}

	if( bPushTrans)
	{
		m_pStack->Pop();
	}

	if( OnPostNode( pNode, pParent) == false)
		return;
}

void i3SceneOptimizer::Trace( i3Node * pRoot)
{
	if( m_pStack != nullptr)
	{
		m_pStack->Reset();
	}

	m_pTraceRoot = pRoot;

	Rec_Trace( pRoot, nullptr);
}

void i3SceneOptimizer::Optimize( i3SceneGraphInfo * pSg)
{
	setSg( pSg);

	if( m_pSg->getCommonSg())
	{
		m_Phase = PHASE_COMMON_SG;
		Trace( m_pSg->getCommonSg());
	}

	if( m_pSg->getInstanceSg())
	{
		m_Phase = PHASE_INSTANCE_SG;
		Trace( m_pSg->getInstanceSg());
	}
}

void i3SceneOptimizer::CallOptimizer( i3ClassMeta * pMeta)
{
	I3ASSERT( pMeta != nullptr);

	i3SceneOptimizer * pOpt = (i3SceneOptimizer *) pMeta->create_instance();

	I3_MUST_ADDREF( pOpt);

	pOpt->Optimize( getSg());

	// Optimizer에 의해, Root Node의 Pointer가 변했다.
	I3_MUST_RELEASE( pOpt);
}

const char * i3SceneOptimizer::getHelpText( void)
{
	return nullptr;
}

bool i3SceneOptimizer::isSupportProgress(void)
{
	return false;
}

const char * i3SceneOptimizer::getDescName(void)
{
	return meta()->class_name().c_str();
}

void i3SceneOptimizer::GetConfigString( char * pszStr, INT32 len)
{
	I3ASSERT( pszStr != nullptr);

	pszStr[0] = 0;
}

static I3OPT_PROGRESS	s_pUserLogProc = nullptr;
static void *			s_pUserLogProcData = nullptr;

void i3SceneOptimizer::SetOptimizeProgressProc( I3OPT_PROGRESS proc, void * pUserData)
{
	s_pUserLogProc = proc;
	s_pUserLogProcData = pUserData;
}

void i3SceneOptimizer::ReportStart( INT32 max, const char * pszMsg)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_START, max, 0, pszMsg, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportEnd(void)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_END, 0, 0, nullptr, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportSubStart( INT32 max, const char * pszMsg)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_SUBSTART, max, 0, pszMsg, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportSubEnd(void)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_SUBEND, 0, 0, nullptr, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportCancel(void)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_CANCEL, 0, 0, nullptr, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportProgress( INT32 pos, const char * pszMsg)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_POS, pos, 0, pszMsg, s_pUserLogProcData);
	}
}

void i3SceneOptimizer::ReportSubProgress( INT32 pos, const char * pszMsg)
{
	if( s_pUserLogProc != nullptr)
	{
		s_pUserLogProc( I3OPT_PROGRESS_REPORT_SUBPOS, pos, 0, pszMsg, s_pUserLogProcData);
	}
}

/*virtual*/ bool i3SceneOptimizer::SaveToXML( i3XMLElement * pXML)
{
	return true;
}

/*virtual*/ bool i3SceneOptimizer::LoadFromXML( i3XMLElement * pXML)
{
	return true;
}


