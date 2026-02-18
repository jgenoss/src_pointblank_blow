
#include "stdafx.h"

#include "CtlLVI3AMapper.h"


IMPLEMENT_DYNCREATE(CtlLVI3AMapper, CListView)

BEGIN_MESSAGE_MAP(CtlLVI3AMapper, CListView)
	ON_WM_SIZE()
//	ON_NOTIFY_RANGE(LVN_COLUMNCLICK,0,0xffff,OnColumnClick)
END_MESSAGE_MAP()


CtlLVI3AMapper::CtlLVI3AMapper()
{
	m_lv = &GetListCtrl();
	GenerateHeaderColumnData();
}

CtlLVI3AMapper::~CtlLVI3AMapper()
{
	ClearView();

	for(INT32 i=0; i<(INT32)m_columns.size(); i++)
		delete m_columns[i];
}

void CtlLVI3AMapper::RefreshAIState()
{
	ClearView();
	GenerateRow();
}

void CtlLVI3AMapper::ClearView()
{
	m_lv->DeleteAllItems();
}

void CtlLVI3AMapper::GenerateHeaderColumnData()
{
	assert(m_columns.empty());

	char* labels[]		= { "AIStateName", "I3AName", "I3APath" };
	const INT32 labelWidth[]	= { 50, 50, 150 };
	const static INT32 numLabel = sizeof(labels) / sizeof(labels[0]);

	for(INT32 i=0; i<numLabel; i++)
	{
		LVCOLUMN* ins = new LVCOLUMN;
		memset(ins, 0, sizeof(LVCOLUMN));

		ins->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ins->fmt = LVCFMT_LEFT;
		ins->cx = labelWidth[i];
		ins->pszText = (LPSTR)labels[i];
		ins->cchTextMax = strlen(labels[i]);
		ins->iSubItem = i;

		m_columns.push_back(ins);
	}	
}

void CtlLVI3AMapper::GenerateRow()
{
	// fill header column
	for(INT32 i=0; i<(INT32)m_columns.size(); i++)
		m_lv->SetColumn(0, m_columns[i]);

	// fill data
	
}

//
// Handle AfxMessage
//

void CtlLVI3AMapper::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);
}

int CtlLVI3AMapper::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	

	return TRUE;
}