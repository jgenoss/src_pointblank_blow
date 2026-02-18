#include "pch.h"
#include "FilterNode.h"


namespace
{

	struct FilterNodeComp
	{
		bool operator()(const CStrFilterNodePtr& l, wchar_t r) const { return l->GetChar() < r; }
		bool operator()(wchar_t l, const CStrFilterNodePtr& r) const { return l < r->GetChar(); }
	};

	CStrFilterNode*		InsertChildImp(wchar_t child_val, bool is_end, i3::vector_set<CStrFilterNodePtr>& inout_childlist)
	{
		i3::vector_set<CStrFilterNodePtr>::insert_commit_data commit_data;

		typedef i3::vector_set<CStrFilterNodePtr>::iterator		iterator;

		i3::pair<iterator, bool> pr = inout_childlist.insert_check_other(child_val, FilterNodeComp(), commit_data);

		if (pr.second)	// 삽입 가능..
		{
			iterator res_it = inout_childlist.insert_commit( i3::make_shared<CStrFilterNode>(child_val, is_end), commit_data );
			return (*res_it).get();
		}

		CStrFilterNode* p = (*pr.first).get();

		if ( is_end && !p->IsEnd() )							// 엔드마크가 없는 경우에 한해..
			p->SetEnd(is_end);						// 엔드마크만큼은 처리...
		return p;	
	}

	CStrFilterNode*		FindChildImp(wchar_t child_val, const i3::vector_set<CStrFilterNodePtr>& child_list)
	{
		typedef i3::vector_set<CStrFilterNodePtr>::const_iterator		const_iterator;
		const_iterator it = child_list.find_other(child_val, FilterNodeComp());

		return (it == child_list.end()) ? nullptr : (*it).get();
	}

}


CStrFilterRoot::CStrFilterRoot()
{

}

CStrFilterRoot::~CStrFilterRoot()
{

}

CStrFilterNode*		CStrFilterRoot::InsertChild(wchar_t child_val, bool is_end)
{
	return InsertChildImp(child_val, is_end, m_ChildList);
}

CStrFilterNode*		CStrFilterRoot::FindChild(wchar_t child_val) const
{
	return FindChildImp(child_val, m_ChildList);
}



CStrFilterNode::CStrFilterNode(wchar_t c, bool bEnd) : m_wcText(c), m_bEnd(bEnd)
{

}

CStrFilterNode::~CStrFilterNode()
{

}

CStrFilterNode*		CStrFilterNode::InsertChild(wchar_t child_val, bool is_end)
{
	return InsertChildImp(child_val, is_end, m_ChildList);
}

CStrFilterNode*		CStrFilterNode::FindChild(wchar_t child_val) const
{
	return FindChildImp(child_val, m_ChildList);
}

