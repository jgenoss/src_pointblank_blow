#pragma once
#include "i3Node.h"
#include "i3UserAttr.h"
#include "i3AllAttrs.h"
class i3XrayNode;
class i3SceneGraphContext;

class I3_EXPORT_SCENE i3XrayScene
{
	i3::vector<i3XrayNode*>		m_vecXrayNode;

	i3ZTestEnableAttr*		m_pZTestAttr;				// [initialize at constructor]
	i3ZFuncAttr*			m_pFuncAttr;				// [initialize at constructor]
	bool					m_bXrayState = false;
	REAL32					m_ZoomRate = 70.0f;
	bool					m_bZoom = false;
	i3Node*					m_pXrayTargetNode = nullptr;

public :
	i3XrayScene();
	~i3XrayScene();

	void				AddXrayNode(i3Node* pNode, i3Node* pLayer, i3AttrSet* pAttset);
	void				RemoveXrayNode(const i3Node* pNode);
	void				ResetXrayRenderAttList();

	void				SetXrayZoomRate(REAL32 rate) { m_ZoomRate = rate; }
	void				SetXrayTargetNode(i3Node* pNode) { m_pXrayTargetNode = pNode; }
	void				SetZoom(bool bState) { m_bZoom = bState; }

	bool				IsXrayState() { return m_bXrayState; }
	void				SetXrayState(bool bState) { m_bXrayState = bState; }
	void				ToggleXrayState() { m_bXrayState = !m_bXrayState; }

	void				Trace(i3SceneGraphContext* pSceneGraphContext, i3SceneTracer* pTracer);

	i3XrayNode*			FindXrayNode(const i3Node* pNode);

};