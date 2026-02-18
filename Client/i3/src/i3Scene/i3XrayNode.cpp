#include "i3SceneDef.h"
#include "i3XrayNode.h"
#include "i3AllAttrs.h"

#define RED_OUTLINE_COLOR	COLOR((UINT8)212, 174, 73, 0);	//레드
#define RED_BG_COLOR		COLOR((UINT8)173, 17, 8, 0);
#define RED_MAXBG_VALUE		VEC3D(173, 17, 8);
#define RED_MINBG_VALUE		VEC3D(197, 88, 82);

#define BLUE_OUTLINE_COLOR	COLOR((UINT8)102, 176, 136, 0); //블루
#define BLUE_BG_COLOR		COLOR((UINT8)30, 54, 159, 0);
#define BLUE_MAXBG_VALUE	VEC3D(30, 54, 159);
#define BLUE_MINBG_VALUE	VEC3D(97, 114, 188);

i3XrayNode::~i3XrayNode()
{
	Release();
}

void i3XrayNode::Release()
{
	m_pRenderAttrList->Reset();
	I3_SAFE_RELEASE(m_pRenderAttrList);
	I3_SAFE_RELEASE(m_pOriNode);

	m_pLayer->RemoveChild(m_pXrayAttSet);
	m_pXrayAttSet = NULL;
	for (INT32 i = 0; i < OUTLINE_MAX; i++)
	{
		m_pLayer->RemoveChild(m_pOutLineAttSet[i]);
		m_pOutLineAttSet[i] = NULL;
	}

	I3_SAFE_RELEASE(m_pLayer);
}

void i3XrayNode::ResetRenderAttList()
{
	m_pRenderAttrList->Reset();
}

void i3XrayNode::Create(i3Node* pNode, i3Node* pLayer, i3AttrSet* pAttset)
{
	if (!pNode) return;
	if (!pLayer) return;

	//오리지널 정보 저장
	m_pOriNode = pNode;
	m_pLayer = pLayer;

	I3_SAFE_ADDREF(pNode);
	I3_SAFE_ADDREF(m_pLayer);

	m_pLightAttSet = pAttset;
	m_pRenderAttrList = i3RenderAttrListAttr::new_object();

	//xray정보 셋팅
	//outline
	for (int i = 0; i < OUTLINE_MAX; i++)
	{
		m_pOutLineAttSet[i] = i3AttrSet::new_object_ref();

		i3ZTestEnableAttr* pZTestAttr = i3ZTestEnableAttr::new_object_ref();
		pZTestAttr->Set(true);
		m_pOutLineAttSet[i]->AddAttr(pZTestAttr);

		i3XrayColorAttr* pColorAttr = i3XrayColorAttr::new_object_ref();
		m_pOutLineAttSet[i]->AddAttr(pColorAttr);

		i3ZFuncAttr* pZFunc = i3ZFuncAttr::new_object_ref();
		pZFunc->Set(I3G_COMP_GREATER);
		m_pOutLineAttSet[i]->AddAttr(pZFunc);


		i3ProjectOffsetAttr* pProj = i3ProjectOffsetAttr::new_object_ref();

		m_pOutLineAttSet[i]->AddAttr(pProj);

		m_pOutLineAttSet[i]->AddChild(pNode);
		pLayer->AddChild(m_pOutLineAttSet[i]);
	}

	//실제적용되는 node
	m_pXrayAttSet = i3AttrSet::new_object_ref();

	i3ZTestEnableAttr* pZTestAttr = i3ZTestEnableAttr::new_object_ref();
	pZTestAttr->Set(true);
	m_pXrayAttSet->AddAttr(pZTestAttr);


	i3ZFuncAttr* pZFunc = i3ZFuncAttr::new_object_ref();
	pZFunc->Set(I3G_COMP_GREATER);
	m_pXrayAttSet->AddAttr(pZFunc);


	i3XrayColorAttr* pColorAttr = i3XrayColorAttr::new_object_ref();
	m_pXrayAttSet->AddAttr(pColorAttr);


	i3ProjectOffsetAttr* pProj = i3ProjectOffsetAttr::new_object_ref();
	m_pXrayAttSet->AddAttr(pProj);

	m_pXrayAttSet->AddChild(pNode);

	pLayer->AddChild(m_pXrayAttSet);
}


void i3XrayNode::SetEnable(bool bState)
{
	if (!bState)
	{
		m_pXrayAttSet->AddFlag(I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_pXrayAttSet->RemoveFlag(I3_NODEFLAG_DISABLE);
	}


	for (INT32 i = 0; i < OUTLINE_MAX; i++)
	{
		if (!bState)
			m_pOutLineAttSet[i]->AddFlag(I3_NODEFLAG_DISABLE);
		else
			m_pOutLineAttSet[i]->RemoveFlag(I3_NODEFLAG_DISABLE);
	}

	i3XrayColorAttr* pXrayColor = NULL;

	pXrayColor = (i3XrayColorAttr*)m_pXrayAttSet->FindAttr(i3XrayColorAttr::static_meta());
	if (m_nTeam & XRAY_TEAM_RED)
	{
		COLOR col = RED_BG_COLOR;
		pXrayColor->Set(&col);
	}
	else
	{
		COLOR col = BLUE_BG_COLOR;
		pXrayColor->Set(&col);
	}


	COLOR col;
	for (INT32 i = 0; i < OUTLINE_MAX; i++)
	{
		pXrayColor = (i3XrayColorAttr*)m_pOutLineAttSet[i]->FindAttr(i3XrayColorAttr::static_meta());
		if (m_nTeam & XRAY_TEAM_RED)
		{
			col = RED_OUTLINE_COLOR;
			pXrayColor->Set(&col);
		}
		else
		{
			col = BLUE_OUTLINE_COLOR;
			pXrayColor->Set(&col);
		}
	}

}

void i3XrayNode::OnUpdate(REAL32 fDist)
{
	if (!m_pXrayAttSet) return;

	i3ProjectOffsetAttr* pOffset = (i3ProjectOffsetAttr*)m_pXrayAttSet->FindAttr(i3ProjectOffsetAttr::static_meta());

	if (pOffset == NULL) return;

	if (fDist > 99)
		fDist = 99;
	if (fDist <= 1)
		fDist = 1;

	REAL32 fColorDist = fDist;

	fDist = 100 - fDist;

	fDist = (fDist * 0.07f) - 2.0f;

	VEC3D vec;
	
	pOffset->Set(&vec);

	pOffset = (i3ProjectOffsetAttr*)m_pOutLineAttSet[OUTLINE_LEFT]->FindAttr(i3ProjectOffsetAttr::static_meta());
	vec.x = (-fDist / m_fWindowWidth);
	pOffset->Set(&vec);
	pOffset = (i3ProjectOffsetAttr*)m_pOutLineAttSet[OUTLINE_RIGHT]->FindAttr(i3ProjectOffsetAttr::static_meta());
	vec.x = -vec.x;
	pOffset->Set(&vec);

	i3Vector::Zero(&vec);
	pOffset = (i3ProjectOffsetAttr*)m_pOutLineAttSet[OUTLINE_UP]->FindAttr(i3ProjectOffsetAttr::static_meta());
	vec.y = (-fDist / m_fWindowHeight);
	pOffset->Set(&vec);
	pOffset = (i3ProjectOffsetAttr*)m_pOutLineAttSet[OUTLINE_DOWN]->FindAttr(i3ProjectOffsetAttr::static_meta());
	vec.y = -vec.y;
	pOffset->Set(&vec);

	VEC3D vMin, vMax;
	COLORREAL colorOri;
	if (m_nTeam & XRAY_TEAM_RED)
	{
		vMin = RED_MINBG_VALUE;
		vMax = RED_MAXBG_VALUE;

		COLOR col = RED_BG_COLOR;
		i3Color::Set(&colorOri, (UINT8)col.r, col.g, col.b, 0);
	}
	else
	{
		vMin = BLUE_MINBG_VALUE;
		vMax = BLUE_MAXBG_VALUE;

		COLOR col = BLUE_BG_COLOR;
		i3Color::Set(&colorOri, (UINT8)col.r, col.g, col.b, 0);
	}

	VEC3D vTemp;
	i3Vector::Sub(&vTemp, &vMin, &vMax);

	fColorDist = fColorDist * 2;

	if (fColorDist > 100)
		fColorDist = 100;

	fColorDist = fColorDist;

	if (fColorDist < 1.0f)
		fColorDist = 1.0f;

	vTemp.x = fColorDist / 100 * vTemp.x;
	vTemp.y = fColorDist / 100 * vTemp.y;
	vTemp.z = fColorDist / 100 * vTemp.z;

	COLORREAL color;
	i3Color::Set(&color, (UINT8)vTemp.x, (UINT8)vTemp.y, (UINT8)vTemp.z, 0);
	i3Color::Add(&colorOri, &colorOri, &color);

	i3XrayColorAttr* pXrayColor = (i3XrayColorAttr*)m_pXrayAttSet->FindAttr(i3XrayColorAttr::static_meta());
	pXrayColor->Set(&colorOri);
}

void i3XrayNode::setWindowSize(REAL32 width, REAL32 height) 
{
	m_fWindowWidth = width;
	m_fWindowHeight = height;
}