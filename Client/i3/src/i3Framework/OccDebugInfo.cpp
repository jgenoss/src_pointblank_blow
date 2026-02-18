#include "i3FrameworkPCH.h"
#include "OccDebugInfo.h"

#include "i3Framework.h"
#include "i3Math/octree/occ_culler.h"

namespace
{
	class Statistics_Callback : public i3::occ_statistics_callback
	{
	public:
		Statistics_Callback() : m_pDXTextNode(nullptr)
		{

		}

		void SetDXTextNode(i3TextNodeDX2* pDXTextNode) { m_pDXTextNode = pDXTextNode; }

	private:
		virtual void on_change(const i3::occ_statistics_info& info)
		{
			if (m_pDXTextNode == nullptr)
				return;

			if (m_pDXTextNode->IsFlag(I3_NODEFLAG_DISABLE))
				return;

			m_str.clear();
			i3::stack_wstring wstrStack;

			const i3::wliteral_range& dashline = L"--------------------------------------------------\n";

			m_str += dashline;

			i3::sprintf(wstrStack, L"Total QUAD Count : %04d\n", info.tot_occluder_count);
			m_str += wstrStack;
			i3::sprintf(wstrStack, L"Total Chara Count : %04d\n", info.tot_occludee_count);
			m_str += wstrStack;

			m_str += dashline;

			i3::sprintf(wstrStack, L"Octree Visible QUAD Count : %04d\n", info.octree_visible_occluder_count);
			m_str += wstrStack;
			i3::sprintf(wstrStack, L"Octree Visible Chara Count : %04d\n", info.octree_visible_occludee_count);
			m_str += wstrStack;

			m_str += dashline;
			
			i3::sprintf(wstrStack, L"GeomOcc Visible QUAD Count : %04d\n", info.geom_occ_visible_occluder_count);
			m_str += wstrStack;
			i3::sprintf(wstrStack, L"GeomOcc Visible Chara Count : %04d\n", info.geom_occ_visible_occludee_count);
			m_str += wstrStack;

			i3::sprintf(wstrStack, L"GeomOCC 100%% Visible Chara Count : %04d\n", info.geom_occ_surely_visible_occludee_count);
			m_str += wstrStack;

			m_str += dashline;

			i3::sprintf(wstrStack, L"HW Query Participating QUAD Count : %04d\n", info.hw_query_calc_occluder_count);
			m_str += wstrStack;

			i3::sprintf(wstrStack, L"HW Query Participating Chara Count : %04d\n", 
				info.geom_occ_visible_occludee_count - info.geom_occ_surely_visible_occludee_count);
			m_str += wstrStack;
					
			i3::sprintf(wstrStack, L"HW Query Visible Chara Count : %04d\n", info.hw_query_visible_occludee_count);
			m_str += wstrStack;

			m_str += dashline;


			i3::sprintf(wstrStack, L"Final Rendering Chara Count : %04d\n",
				info.geom_occ_surely_visible_occludee_count + info.hw_query_visible_occludee_count);
			m_str += wstrStack;

			m_str += dashline;

			m_pDXTextNode->SetText(m_str, false);
			m_pDXTextNode->setSize(1024, 768);
		}

		i3TextNodeDX2*  m_pDXTextNode;
		i3::wstring	m_str;
	};

}


OccDebugInfo::OccDebugInfo()
{
	m_callback = new Statistics_Callback;
}

OccDebugInfo::~OccDebugInfo()
{
	delete m_callback;
	I3_SAFE_RELEASE(m_pDebugTextNode);
}

void	OccDebugInfo::Create()
{
	m_pDebugTextNode = i3TextNodeDX2::new_object();
	m_pDebugTextNode->Create("±¼¸²Ã¼", 4096, 9, FW_NORMAL, false, false, DEFAULT_QUALITY, true);
	m_pDebugTextNode->setAutoWrapEnable(true);
	m_pDebugTextNode->SetText(L"");
	m_pDebugTextNode->setPos(5, 5);
	m_pDebugTextNode->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDebugTextNode->SetColor(255, 255, 255, 255);
	m_pDebugTextNode->AddFlag(I3_NODEFLAG_DISABLE);

	static_cast<Statistics_Callback*>(m_callback)->SetDXTextNode(m_pDebugTextNode);
	
	m_bTextEnabled = false;
	m_bAABBRenderEnabled = false;
	m_bWorldQuadRenderEnabled = false;
}

void OccDebugInfo::ClearText()
{
	if (m_pDebugTextNode == nullptr || m_pDebugTextNode->IsFlag(I3_NODEFLAG_DISABLE))
		return;

	const i3::wstring& wstr = m_pDebugTextNode->getTextString();
	if (!wstr.empty())
		m_pDebugTextNode->SetText(L"", false);
}

void	OccDebugInfo::OnChangeFramework(i3Framework* pFrame)
{
	if (m_pDebugTextNode == nullptr)
		return;

	m_pDebugTextNode->RemoveFromParent();

	if (pFrame != nullptr)
	{
		pFrame->GetDebugTextRoot()->AddChild(m_pDebugTextNode);
	}
}

void	OccDebugInfo::setDebugTextPos(REAL32 x, REAL32 y)
{
	m_pDebugTextNode->setPos((UINT32)x, (UINT32)y);
}

void OccDebugInfo::SetTextEnable(bool bEnable) 
{ 
	if (bEnable == m_bTextEnabled)
		return;

	m_bTextEnabled = bEnable;

	if (bEnable)
	{
		m_pDebugTextNode->RemoveFlag(I3_NODEFLAG_DISABLE);
		OccDebugInfo::ClearText();
	}
	else
	{
		m_pDebugTextNode->AddFlag(I3_NODEFLAG_DISABLE);
	}
}

void OccDebugInfo::SetAABBRenderEnable(bool bEnable)
{
	m_bAABBRenderEnabled = bEnable;
}

bool OccDebugInfo::IsAABBRenderEnabled() const
{
	return m_bAABBRenderEnabled;
}

void OccDebugInfo::SetWorldQuadRenderEnable(bool bEnable)
{
	m_bWorldQuadRenderEnabled = bEnable;

	bool bRenderEnable = bEnable;
	
	i3Framework* pFramework = i3Framework::getCurrentFramework();

	if (pFramework)
	{
		i3Stage* pStage =  pFramework->GetCurrentStage();

		if (pStage)
		{
			i3World* pWorld = pStage->getWorld();

			if (pWorld)
			{
				pWorld->EnableOccCullMesh(bRenderEnable);
			}
		}
	}
}

bool OccDebugInfo::IsWorldQuadRenderEnabled() const
{
	return m_bWorldQuadRenderEnabled;
}

bool OccDebugInfo::HasWorldOccQuads() const
{
	i3Framework* pFramework = i3Framework::getCurrentFramework();

	if (pFramework)
	{
		i3Stage* pStage = pFramework->GetCurrentStage();

		if (pStage)
		{
			i3World* pWorld = pStage->getWorld();

			if (pWorld)
			{
				return pWorld->HasWorldOccQuads();
			}
		}
	}

	return false;
}
