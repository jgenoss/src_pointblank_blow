
//
// sub utility class for i3OptRenAttr
//

#ifndef _I3OPTRENATTRMERGER_H_
#define _I3OPTRENATTRMERGER_H_

#include "i3Base.h"
#include "i3Scene/i3SceneDef.h"

#include <vector>
#include <set>

class i3AttrSet;
class i3RenderAttr;
class i3BoneRef;


struct I3_EXPORT_OPT i3OptRenAttrMergerRst : public i3ElementBase
{
	void Clear();
	
	void AddOptiNode(i3Node* node);
	void AddNonOptiNode(i3Node* node);
	void AddNewlyCreatedNode(i3Node* node);
	void AddCancelOper(i3Node* node);

	void MergeNonOptiIntoOpti();

	INT32						m_status;

	typedef std::set<i3Node*>	NodeSet;

	NodeSet& GetOptiNodes() { return m_opti; }
	NodeSet& GetNonOptiNodes() { return m_nonOpti; }
	NodeSet& GetNewlyCreatedNode() { return m_newlyCreated; }
	NodeSet& GetCancelOperNode() { return m_cancelOper; }

protected:
	NodeSet						m_opti;
	NodeSet						m_nonOpti;
	NodeSet						m_newlyCreated;

	NodeSet						m_cancelOper;
};


class I3_EXPORT_OPT i3OptRenAttrMerger : public i3ElementBase
{
	I3_CLASS_DEFINE(i3OptRenAttrMerger, i3ElementBase);

public:
	typedef std::vector<i3AttrSet*>		AttrSetIn;

	i3OptRenAttrMerger();
	virtual ~i3OptRenAttrMerger();

	// upper는 in에 담겨있는 i3AttrSet의 상위경로에있는 i3BoneRef나 i3AttrSet이됨
	// i3BoneRef는 경로에있는 어떤것이나 상관없으나, 
	// i3AttrSet인경우 in에 담겨있는 i3AttrSet과의경로에있는 첫번째i3AttrSet이어야함
	bool Merge(i3Node* upper, AttrSetIn* in, i3OptRenAttrMergerRst* rst= nullptr);

protected:
	// i3ElementBase 
	virtual	void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override
		{ i3ElementBase::CopyTo(pDest, method); }

};


#endif