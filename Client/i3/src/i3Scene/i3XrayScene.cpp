#include "i3SceneDef.h"
#include "i3XrayScene.h"
#include "i3XrayNode.h"
#include "i3AttrSet.h"

#include "i3SceneTracer.h"

i3XrayScene::i3XrayScene()
{
	m_pZTestAttr = i3ZTestEnableAttr::new_object();
	m_pZTestAttr->Set(TRUE);

	m_pFuncAttr = i3ZFuncAttr::new_object();
	m_pFuncAttr->Set(I3G_COMP_LEQUAL);
}
i3XrayScene::~i3XrayScene()
{
	I3_SAFE_RELEASE(m_pZTestAttr);
	I3_SAFE_RELEASE(m_pFuncAttr);

	for (size_t i = 0; i < m_vecXrayNode.size(); i++)
	{
		I3_SAFE_DELETE(m_vecXrayNode[i]);
	}
	m_vecXrayNode.clear();
}

void i3XrayScene::AddXrayNode(i3Node* pNode, i3Node* pLayer, i3AttrSet* pAttset)
{
	i3XrayNode *pXrayNode = new i3XrayNode;
	pXrayNode->Create(pNode, pLayer, pAttset);

	m_vecXrayNode.push_back(pXrayNode);
}

void i3XrayScene::RemoveXrayNode(const i3Node* pNode)
{
	for (size_t i = 0; i < m_vecXrayNode.size(); i++)
	{
		if (m_vecXrayNode[i]->IsSameOriNode(pNode))
		{
			I3_SAFE_DELETE(m_vecXrayNode[i]);
			m_vecXrayNode.erase(m_vecXrayNode.begin() + i);

			break;
		}
	}
}

void i3XrayScene::ResetXrayRenderAttList()
{
	for (size_t i = 0; i < m_vecXrayNode.size(); i++)
	{
		m_vecXrayNode[i]->ResetRenderAttList();
	}
}

void i3XrayScene::Trace(i3SceneGraphContext* pSceneGraphContext, i3SceneTracer* pTracer)
{
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();

	for (INT32 i = 0; i < (INT32)m_vecXrayNode.size(); i++)
	{
		i3XrayNode* pXrayNode = m_vecXrayNode[i];
		if (pXrayNode->IsSameOriNode(m_pXrayTargetNode) && m_bZoom && m_ZoomRate < 60.0f)
			continue;

		pTracer->PushOutputList(pXrayNode->GetAttList());
		pStackManager->PushAttr(m_pZTestAttr);
		pStackManager->PushAttr(m_pFuncAttr);
		pStackManager->PushAttrs(pSceneGraphContext->get3DRoot()->GetAttrs());
		pStackManager->PushAttrs(pXrayNode->GetLightAttrSet()->GetAttrs());

		i3ZWriteEnableAttr* pZWirteAttr = (i3ZWriteEnableAttr*)i3Scene::FindAttrByType(pXrayNode->GetOriNode(), i3ZWriteEnableAttr::static_meta(), true);
		if (pZWirteAttr)
		{
			pZWirteAttr->Set(true);

			i3Node* pSceneNode = pXrayNode->GetOriNode();
			pSceneNode->PreTrace(pTracer);

			pTracer->Trace(pSceneNode);

			pZWirteAttr->Set(false);
		}

		pStackManager->PopAttr(m_pZTestAttr->GetID());
		pStackManager->PopAttr(m_pFuncAttr->GetID());
		pStackManager->PopAttrs(pSceneGraphContext->get3DRoot()->GetAttrs());
		pStackManager->PopAttrs(pXrayNode->GetLightAttrSet()->GetAttrs());
		pTracer->PopOutputList();

		pTracer->AddAttributeToOutput(pXrayNode->GetAttList());
	}
}

i3XrayNode*	i3XrayScene::FindXrayNode(const i3Node* pNode)
{
	for (INT32 i = 0; i < (INT32)m_vecXrayNode.size(); i++)
	{
		if (m_vecXrayNode[i]->IsSameOriNode(pNode))
			return m_vecXrayNode[i];
	}

	return NULL;
}