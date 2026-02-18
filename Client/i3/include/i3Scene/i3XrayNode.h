#pragma once

#include "i3Node.h"
#include "i3UserAttr.h"

enum XRAY_OUTLINE_TYPE
{
	OUTLINE_LEFT = 0,
	OUTLINE_RIGHT,
	OUTLINE_UP,
	OUTLINE_DOWN,
	OUTLINE_MAX,
};

class i3AttrSet;
class i3RenderAttrListAttr;

class I3_EXPORT_SCENE i3XrayNode
{
	i3Node*					m_pOriNode = nullptr;
	i3AttrSet*				m_pLightAttSet = nullptr;
	i3RenderAttrListAttr*	m_pRenderAttrList = nullptr;
	i3Node*					m_pLayer = nullptr;

	enum {
		XRAY_TEAM_RED = 1,
		XRAY_TEAM_BLUE,
	};
	i3AttrSet*		m_pOutLineAttSet[OUTLINE_MAX] = { nullptr, };
	i3AttrSet*		m_pXrayAttSet = nullptr;

	UINT32			m_nTeam = 0;
	REAL32			m_fWindowWidth = I3_EPS;
	REAL32			m_fWindowHeight = I3_EPS;
public :
	i3XrayNode() { }
	~i3XrayNode();

	void Create(i3Node* pNode, i3Node* pLayer, i3AttrSet* pAttset);

	void Release();
	void ResetRenderAttList();
	void SetEnable(bool bState);
	void OnUpdate(REAL32 fDist);

	bool IsSameOriNode(const i3Node* pNode) {return m_pOriNode == pNode;}

	i3AttrSet*	GetLightAttrSet() const { return m_pLightAttSet; }
	i3RenderAttrListAttr* GetAttList() const { return m_pRenderAttrList; }
	i3Node* GetOriNode() const { return m_pOriNode; }
	void setTeam(UINT32 nTeam) { m_nTeam = nTeam; }
	void setWindowSize(REAL32 width, REAL32 height);

};