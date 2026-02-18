
#include "i3OptPCH.h"

#include "i3OptRenAttr_Private.h"
#include "i3OptRenAttrMerger.h"
#include "i3SceneOptimizer.h"

#include "i3OptRenAttr.h"
#include "i3Opt/i3OptCreateBoundBox.h"

#include <algorithm>


using namespace i3OptRenAttr_Pri;


class RenAttrOptiAgent : public IAnalyzeHelper
{
public:
	typedef AnalyzeItemTraveler<i3AttrSet, AnalyzedAttrSet, IAnalyzeHelper> 
		AnalyzedAttrSetScanRst;

	RenAttrOptiAgent();
	~RenAttrOptiAgent();

	bool Do(i3SceneGraphInfo* inScene);
	bool Do(i3Node* inStartNode, bool skipReset=false);

	void Reset();

	CollectRenderAttrSetStaticsFromi3Scene& GetStatics(INT32 index)
		{ return m_statics[index]; }

	INT32 GetErr() { return m_err; }
	INT32 GetOptimizePass() { return m_optimizePass; }

	RenAttrStatusTraveler& GetRenAttrStatus() { return m_scanAttrSetStatus; }

private:
	IAnalyzeHelper* GetHelper() { return (IAnalyzeHelper*)this; }
	void SetErr(INT32 err);

	// persistinfo
	bool GetAttrSetPersistInfo(i3Node* toFind, PersistInfoi3AttrSet** rst);	
	void ApplyAttrSetPersistInfo(i3OptRenAttrMergerRst* rst);
	void ClearPersistInfo();

	void RecursiveCollectSameUpperAttrSet(i3AttrSetVec& out, i3Node* curNode);
	
	// optimizers
	void Optimize(i3Node* startNode, AnalyzedAttrSetScanRst& scanResult);
	INT32 Optimize_LeafAttrSet(AnalyzedAttrSetScanRst& scanResult); // 옵티마이즈된 AttrSet갯수

protected:
	// IAnalyzeHelper implement
	virtual PersistInfoi3AttrSet* Helper_GetPersistAttrSetInfo(i3Node* node);
	virtual bool Helper_RemovePersistAttrSetInfo(i3Node* node);

	INT32 _subOptimize_LeafAttrSet(i3Node* upperNode, i3Node* srcA, i3Node* srcB, i3OptRenAttrMergerRst* rst);

private:
	PersistInfoi3AttrSetMap							m_persistInfo;

	INT32											m_optimizePass = 0;
	INT32											m_err = 0;
	bool											m_finishedOptimize = false;
	INT32											m_numOperCancel = 0;

	i3OptRenAttrMerger*								m_merger = nullptr;
	RenAttrStatusTraveler							m_scanAttrSetStatus;

	CollectRenderAttrSetStaticsFromi3Scene			m_statics[2];
};


//
// IAnalyzeHelper implement
//

PersistInfoi3AttrSet* RenAttrOptiAgent::Helper_GetPersistAttrSetInfo(i3Node* node)
{
	if(!node)
		return nullptr;

	PersistInfoi3AttrSet* findRst = nullptr;
	GetAttrSetPersistInfo(node, &findRst);

	return findRst;
}

bool RenAttrOptiAgent::Helper_RemovePersistAttrSetInfo(i3Node* node)
{
	if(!node)
		return false;

	if(!i3::kind_of<i3AttrSet*>(node))
		return false;

	PersistInfoi3AttrSetMap::iterator x = m_persistInfo.find((i3AttrSet*)node);
	if(x == m_persistInfo.end())
		return false;

	m_persistInfo.erase(x);

	return true;
}


//
// RenAttrOptiAgent
//

RenAttrOptiAgent::RenAttrOptiAgent()
{
	Reset();
	m_merger = i3OptRenAttrMerger::new_object(); 
}

RenAttrOptiAgent::~RenAttrOptiAgent()
{
	Reset();

	I3_SAFE_RELEASE(m_merger);
}

bool RenAttrOptiAgent::Do(i3SceneGraphInfo* inScene)
{
	Reset();

	if(!inScene)
		return false;

	i3Node* curRootNode = inScene->getInstanceSg();
	
	if(!curRootNode)
		return false;

	// 옵티마이즈할 신루트 노드 모으기
	CollectSceneStartNodeFromi3Scene sceneRootNodes(curRootNode);
	for(INT32 i=0; i<sceneRootNodes.GetNumNode(); i++)
	{
		i3Node* sceneRootNode = sceneRootNodes.GetNode(i);

		if(!Do(sceneRootNode, true))
			return false;
	}

	return true;
}

bool RenAttrOptiAgent::Do(i3Node* inStartNode, bool skipReset)
{
	if(!skipReset)
		Reset();

	i3Node* curRootNode = inStartNode;

	if(!curRootNode)
		return false;

	// 1개이상의 중복 되는 RenAttr등이 있는지 검사
	m_scanAttrSetStatus.SetRootNode(curRootNode);
	m_scanAttrSetStatus.Do();

	INT32 numEvent = m_scanAttrSetStatus.GetNumEvent();

	if(numEvent > 0)
	{
		SetErr(8000);
		return false;
	}

	m_statics[0].Add(curRootNode);

	m_finishedOptimize = false;
	ClearPersistInfo();
	AnalyzedAttrSetScanRst scanAttrSetRst(GetHelper(), curRootNode);

	while(!m_finishedOptimize)
	{		
		scanAttrSetRst();

		if(scanAttrSetRst.IsResultEmpty())
			m_finishedOptimize = true;

		if(m_finishedOptimize)
			break;

		m_optimizePass++;

		if(m_optimizePass > 9999)
		{
			// 옵티마이즈이상
			SetErr(9000);
			return false;
		}

		Optimize(curRootNode, scanAttrSetRst);
	}

	if(m_err)
		return false;

	m_statics[1].Add(curRootNode);
	m_statics[1].Add(CollectRenderAttrSetStaticsFromi3Scene::EPRM_NUM_TOT_CANCELOPER, m_numOperCancel);

	return true;
}

void RenAttrOptiAgent::Reset()
{
	m_optimizePass = 0;
	m_err = 0;
	m_finishedOptimize = false;
	m_numOperCancel = 0;

	m_scanAttrSetStatus.Reset();

	ClearPersistInfo();

	for(INT32 i=0; i< sizeof(m_statics) / sizeof(m_statics[0]); i++)
		m_statics[i].Reset();
}

void RenAttrOptiAgent::SetErr(INT32 err) 
{
	m_err = err; 
	m_finishedOptimize=true; 

//	I3ASSERT(0);
}

void RenAttrOptiAgent::Optimize(i3Node* startNode, AnalyzedAttrSetScanRst& scanResult)
{
	if(m_finishedOptimize)
		return;

	// RenderAttr이 한개도 없는 AttrSet정리, 삭제된AttrSet이있으면 그와관련된 PersistInfo도 삭제해준다.
	AttrSetCleaner()(startNode, GetHelper());

	I3ASSERT(ChildParentRelationShipChecker<i3Node>()(startNode));

	Optimize_LeafAttrSet(scanResult);

	I3ASSERT(ChildParentRelationShipChecker<i3Node>()(startNode));

	// RenderAttr이 한개도 없는 AttrSet정리, 삭제된AttrSet이있으면 그와관련된 PersistInfo도 삭제해준다.
	AttrSetCleaner()(startNode, GetHelper()); 

	I3ASSERT(ChildParentRelationShipChecker<i3Node>()(startNode));
}

INT32 RenAttrOptiAgent::Optimize_LeafAttrSet(AnalyzedAttrSetScanRst& scanResult)
{
	if(m_err)
		return 0;

	INT32 numOpti = 0;
	AnalyzedItemVec leafItem;

	// 말단AttrSet들을 leafItem에 모은다.
	CollectLeafAnalyzedItem<AnalyzedAttrSet*>(leafItem, scanResult.GetResults());

	// 말단노드 깊이순으로 내림차순정렬
	std::sort(leafItem.begin(), leafItem.end(), AnalyzedItemDepthGreater<AnalyzedItem*>);

	if(leafItem.empty())
		return numOpti;

	i3OptRenAttrMergerRst rst;
	
	for(size_t i=0; i<leafItem.size(); i++)
	{
		i3Node* cur = leafItem[i]->GetSrci3Node();
		
		if(!i3::kind_of<i3AttrSet*>(cur))
		{
			SetErr(-1000);
			return 0;
		}

		AnalyzedAttrSet* curAnalyzed = (AnalyzedAttrSet*)leafItem[i];
		std::vector<i3Node*>& upperNodes = curAnalyzed->GetAllUpperNodes();

		if(upperNodes.empty())
		{
			SetErr(-1001);
			return 0;
		}

		bool optimized = false;

		// 스캔할때 대상이된 AttrSet의 UpperNode들을 넣는데, 마지막엔 nullptr을 넣어둔다.
		I3ASSERT(upperNodes[upperNodes.size()-1] == nullptr);
		for(size_t k=0; k<(upperNodes.size()-1); k++)
		{			
			PersistInfoi3AttrSet* pinfo = nullptr;
			GetAttrSetPersistInfo(cur, &pinfo);

			if( pinfo == nullptr)
				continue;

			if( pinfo->GetOptimized())
			{
				optimized = true;
				break;
			}
			
			{				
				i3Node* upperNode = upperNodes[k];

				i3AttrSetVec rstSameUppered;
				// 같은부모를 가진 i3AttrSet들을 모은다. 이미 옵티마이즈된것은 제외
				RecursiveCollectSameUpperAttrSet(rstSameUppered, upperNode);

				if(rstSameUppered.size() < 2)
				{
					if(upperNodes[k+1] == nullptr)
					{
						// 마지막노드이고 동일부모를 가진 i3AttrSet이 1개(자신)만있을경우 옵티마이즈종료
						pinfo->SetOptimized(true);
						optimized = true;
						break;
					}
					else
						continue; // 현재시도한부모의 상위부모(I3BoneRef,I3AttrSet에 시도)
				}

				// 공통부모노드에서 머지대상 AttrSet들의 경로를 조사해 중간에 
				// 공통부모를가진AttrSet이 있는지확인, 있다면 그AttrSet은 옵티마이즈될 필요가있다.
				if(!CheckChildAttrSetPathIsUnique(rstSameUppered, upperNode))
					break;



				///////////////////////////////////////////////////////////////////////////////
				// 드로잉순서고려때문에 자식노드의 순서를 고려해야하므로 자식노드 2개씩 비교한다.
				///////////////////////////////////////////////////////////////////////////////

				i3Node *srcA, *srcB;
				INT32 dstCmpIndex = 1;
				INT32 leafAttrRst;
				INT32 numCmp = (INT32)rstSameUppered.size();

				srcA = rstSameUppered[0];
				srcB = rstSameUppered[dstCmpIndex];

				for(INT32 mgrPass=0; ;mgrPass++)
				{
					i3OptRenAttrMergerRst cmpRst;
					leafAttrRst = _subOptimize_LeafAttrSet(upperNode, srcA, srcB, &cmpRst);

					i3OptRenAttrMergerRst::NodeSet& cmpRstOpti = cmpRst.GetOptiNodes();
					i3OptRenAttrMergerRst::NodeSet::iterator cmpRstOptiIter = cmpRstOpti.begin();

					while(cmpRstOptiIter != cmpRstOpti.end())
					{
						rst.AddOptiNode(*cmpRstOptiIter);
						cmpRstOptiIter++;
					}

					if(leafAttrRst == -1) // 중복된게없다.
					{
//						I3ASSERT(cmpRst.GetOptiNodes().size() == 0);
												
						if(mgrPass == 0)
						{
							// 첫번째 노드는 옵티마이즈 처리
							rst.AddOptiNode(srcA);
							srcA = rstSameUppered[dstCmpIndex];
						}
						else
						{
							srcA = srcB;
						}
					}
					else
					{
						I3ASSERT(cmpRst.GetNewlyCreatedNode().size() == 1);

						// 비교대상이된 노드를 옵티마이즈처리
						if(rstSameUppered.end() != std::find(rstSameUppered.begin(), rstSameUppered.end(), srcA))
							rst.AddOptiNode(srcA);

						if(rstSameUppered.end() != std::find(rstSameUppered.begin(), rstSameUppered.end(), srcB))
							rst.AddOptiNode(srcB);

						srcA = (*cmpRst.GetNewlyCreatedNode().begin());

						rst.AddNewlyCreatedNode((*cmpRst.GetNewlyCreatedNode().begin()));
					}

					dstCmpIndex++;

					if(dstCmpIndex > (numCmp-1))
					{
						// 마지막 비교노드를 옵티마이즈처리
						rst.AddOptiNode(srcB);
						break;
					}

					srcB = rstSameUppered[dstCmpIndex];

				} // end of for mgrPass


				//if(rst.GetOptiNodes().empty() && !rst.GetNonOptiNodes().empty() )
				//{
				//	// 옵티마이즈항목이 한개도없고, 옵티마이즈안된것만있다면
				//	// 이번에 참여한 AttrSet들은 옵티마이즈할 항목이없는것이다.
				//	// 다음번 옵티마이즈에서 빼기위해서 옵티마이즈처리
				//	rst.MergeNonOptiIntoOpti();
				//}

				// 1개이상은 있어야함
				if(rst.GetOptiNodes().empty() && rst.GetNonOptiNodes().empty() )
				{
					SetErr(-2000);
					m_finishedOptimize = true;

					return 0;
				}
				
				bool rescanAttrSet = false;

				// 머지결과로 신규로 i3AttrSet이 생겼다면 새로 노드들을 재스캔
				if(rst.GetNewlyCreatedNode().size())
					rescanAttrSet = true;

				numOpti += (INT32)rst.GetOptiNodes().size();
				
				m_numOperCancel += (INT32)rst.GetCancelOperNode().size();

				ApplyAttrSetPersistInfo(&rst);

				if(rescanAttrSet)
					return numOpti; // 리스캔!
			}

		} // end of for
		
	} // end of for leafitem

	return numOpti;
}

INT32 RenAttrOptiAgent::_subOptimize_LeafAttrSet(i3Node* upperNode, i3Node* srcA, i3Node* srcB, 
	i3OptRenAttrMergerRst* rst)
{
	i3AttrSetVec cmpASVec;

	i3AttrSet* a = i3::kind_of<i3AttrSet*>(srcA) ? ((i3AttrSet*)srcA) : nullptr;
	i3AttrSet* b = i3::kind_of<i3AttrSet*>(srcB) ? ((i3AttrSet*)srcB) : nullptr;

	I3ASSERT(a && b);

	cmpASVec.push_back(a); 
	cmpASVec.push_back(b);

	m_merger->Merge(upperNode, &cmpASVec, rst);

	i3OptRenAttrMergerRst::NodeSet& newlyCreated = rst->GetNewlyCreatedNode();
	
	if(newlyCreated.empty())
	{
		// 2개의 대상AttrSet사이에 중복된 RenAttr이 존재하지 않는다.
		return -1;
	}

	I3ASSERT(newlyCreated.size() == 1);

	return 0;
}

void RenAttrOptiAgent::RecursiveCollectSameUpperAttrSet(i3AttrSetVec& out, i3Node* curNode)
{	
	if(!curNode)
		return;

	for(INT32 i=0; i<curNode->GetChildCount(); i++)
	{
		i3Node* childNode = curNode->GetChild(i);
		if(childNode)
		{
			 if(i3::kind_of<i3AttrSet*>(childNode))
			 {
				PersistInfoi3AttrSet* pinfo = nullptr;
				GetAttrSetPersistInfo(childNode, &pinfo);

				if(pinfo)
				{
					if(pinfo->GetOptimized())
						return;		
				}

				out.push_back((i3AttrSet*)childNode);

				// 제일처음발견되는 AttrSet만 넣는다.
				continue;				
			 }

			 RecursiveCollectSameUpperAttrSet(out, childNode);
		}		
	}

}

	//
	// PersistInfo
	//
bool RenAttrOptiAgent::GetAttrSetPersistInfo(i3Node* toFind, PersistInfoi3AttrSet** rst)
{
	*rst = nullptr;

	if(!i3::kind_of<i3AttrSet*>(toFind))
		return false;
	
	PersistInfoi3AttrSetMap::iterator x = m_persistInfo.find((i3AttrSet*)toFind);

	if(x == m_persistInfo.end())
	{	
		PersistInfoi3AttrSet v;
		v.Reset();

		v.SetDest((i3AttrSet*)toFind);

		PersistInfoi3AttrSetMap::value_type val((i3AttrSet*)toFind, v);
		PersistInfoi3AttrSetMap::_Pairib r = m_persistInfo.insert(val);
		x = r.first;
	}

	*rst = &(*x).second;

	return true;
}

void RenAttrOptiAgent::ApplyAttrSetPersistInfo(i3OptRenAttrMergerRst* rst)
{
	// optimized
	for(i3OptRenAttrMergerRst::NodeSet::iterator x= rst->GetOptiNodes().begin();
		x != rst->GetOptiNodes().end(); x++)
	{
		PersistInfoi3AttrSet* info = nullptr;

		if(GetAttrSetPersistInfo(*x, &info))
		{
			info->SetOptimized(true);
			info->IncOptimizeTry();			
		}
		else
		{
			I3ASSERT(!info);
		}
	}

	rst->Clear();
}

void RenAttrOptiAgent::ClearPersistInfo()
{
	m_persistInfo.clear();
}


//
//	i3OptRenAttr
//

I3_CLASS_INSTANCE(i3OptRenAttr);


static INT32 ApplyResult(i3OptRenAttr::OptiRst* rst, RenAttrOptiAgent& agent)
{
	if(!rst)
		return 0;

	for(INT32 i=0; i<2; i++)
	{
		rst->m_numAttrSet[i] = agent.GetStatics(i).GetParam(
			CollectRenderAttrSetStaticsFromi3Scene::EPRM_NUM_TOT_ATTRSET);
		rst->m_numRenAttr[i] = agent.GetStatics(i).GetParam(
			CollectRenderAttrSetStaticsFromi3Scene::EPRM_NUM_TOT_RENATTR);
	}

	rst->m_numCancelOper = agent.GetStatics(1).GetParam(
		CollectRenderAttrSetStaticsFromi3Scene::EPRM_NUM_TOT_CANCELOPER);

	return agent.GetOptimizePass();
}


i3OptRenAttr::i3OptRenAttr(void) : m_bOptimized(false)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style = OPT_STYLE_APPLY_SG;

	m_supportProgress = true;
	m_numMaximumPass = 100;
}

i3OptRenAttr::~i3OptRenAttr(void)
{
}

void i3OptRenAttr::Trace(i3Node * pRoot)
{
	if(!pRoot)
		return;

	m_bOptimized = false;
	m_rst.Clear();

	if(m_supportProgress)
		ReportStart(100, "Begin optimize render attribute in scene ...");

	m_bOptimized = DoOptimize(pRoot, &m_rst);

	if(m_bOptimized)
	{
		if(m_supportProgress)
		{
			char text[MAX_PATH] = {0};
			bool nothingIsOptimized = m_rst.IsNothingOptimized();

			if(nothingIsOptimized)
				sprintf_s(text, MAX_PATH, "Nothing is optimized !!.. total optimize pass = %d ", m_rst.m_numPass);
			else
				sprintf_s(text, MAX_PATH, "Successfully optimized render attributes.. total optimize pass = %d ", m_rst.m_numPass);

			ReportProgress(100, text);

			if(!nothingIsOptimized)
			{
				// to print out %, use %%%% in sprintf_s
				sprintf_s(text, MAX_PATH, "Result - i3AttrSet(%3.0f)%%%% , i3RenAttr(%3.0f)%%%%",
					((FLOAT)m_rst.m_numAttrSet[1] / m_rst.m_numAttrSet[0])*100, 
					((FLOAT)m_rst.m_numRenAttr[1] / m_rst.m_numRenAttr[0])*100);
				ReportProgress(100, text);

				sprintf_s(text, MAX_PATH, "Result - i3AttrSet[Prev=%03d -> Cur=%03d], i3RenAttr[%03d -> %03d]",
					m_rst.m_numAttrSet[0], m_rst.m_numAttrSet[1], m_rst.m_numRenAttr[0], m_rst.m_numRenAttr[1]);
				ReportProgress(100, text);
			}
		}

		{
			i3ClassMeta * pMeta = i3OptCreateBoundBox::static_meta();
			i3OptCreateBoundBox* optCBB = (i3OptCreateBoundBox*)pMeta->create_instance();
			optCBB->AddRef();

			optCBB->Optimize(getSg());

			I3_SAFE_RELEASE(optCBB);
		}
	}
	else
	{
		ReportStart(100, "ERR! - Failed to optimize ...");
		MessageBox(nullptr, "Sorry, Fatal error occured in optimizing...!! \n\nThis scene is corrupted.."
			"so you can't save this scene.. \nPlease Close and reload this file..", "Fatal Error", MB_SYSTEMMODAL);
	}

	if(m_supportProgress)
		ReportEnd();
}

void i3OptRenAttr::SetProperty(char* pszFieldName, char* pszValue)
{
}

const char* i3OptRenAttr::getDesc(void)
{
	return "Optimize i3RenderAttr in scene, remove dupplicated i3RenderAttr";
}

bool i3OptRenAttr::isSupportProgress(void)
{
	return m_supportProgress;
}

bool i3OptRenAttr::DoOptimize(i3SceneGraphInfo* inScene, OptiRst* rst)
{
	if(!inScene)
		return false;

	return DoOptimize(inScene->getInstanceSg(), rst);
}


bool i3OptRenAttr::DoOptimize(i3Node* inStartNode, OptiRst* rst)
{
	if(!inStartNode)
		return false;

	OptiRst sumRst;
	bool failed = false;

	if(rst)
		sumRst.m_errOutput = rst->m_errOutput;

	//
	// 1st - Root에서 i3Body 노드까지만
	//

	{
		OptiRst innerRst;
		innerRst.m_errOutput = sumRst.m_errOutput;

		if(!_DoOptimize(inStartNode, &innerRst))
			failed = true;

		sumRst = sumRst + innerRst;
	}


	if(!failed)
	{
		//
		// 2nd - i3Body의 LOD에 대해 처리
		//
		std::vector<i3Body*> opti3BodyNodes;
		CollectFirstFoundChildi3Nodes<i3Body>(inStartNode, &opti3BodyNodes);

		for(INT32 i=0; i<(INT32)opti3BodyNodes.size(); i++)
		{
			i3Body* body = opti3BodyNodes[i];
			INT32 lodCount = body->getLODCount();

			for(INT32 k=0; k<lodCount; k++)
			{
				i3LOD* lod = body->getLOD(k);
				i3Node* lodStartNode = lod->getShapeNode();

				OptiRst innerRst;
				innerRst.m_errOutput = sumRst.m_errOutput;

				if(!_DoOptimize(lodStartNode, &innerRst))
				{
					failed = true;
					sumRst.m_operRstFlag |= innerRst.m_operRstFlag;
					break;
				}

				sumRst = sumRst + innerRst;
			}

			if(failed)
				break;
		}
	}

	if(!sumRst.IsAttrSetRenAttrOptimized())
	{
		if(!sumRst.IsError())
			sumRst.m_operRstFlag |= i3OptRenAttr::OptiRst::ORF_NOTHING_IS_OPTIMIZED;
	}

	if(rst)
		*rst = sumRst;

	return !failed;
}

bool i3OptRenAttr::_DoOptimize(i3Node* inStartNode, OptiRst* rst)
{
	if(!inStartNode)
		return false;

	RenAttrOptiAgent agent;

	INT32 numPass = 0;

	OptiRst firstResult, operRst;
	bool firstPass = true;

	if(rst)
		operRst.m_errOutput = rst->m_errOutput;

	do 
	{
		if(!agent.Do(inStartNode))
		{
			if(m_supportProgress)
			{
				char txtBuf[MAX_PATH]={0}; 
				sprintf_s(txtBuf, MAX_PATH, "ERR! - failed to optimize render attribute.. errNum=%d", agent.GetErr());
				ReportProgress(0, txtBuf);
			}

			RenAttrStatusTraveler renAttrStatus = agent.GetRenAttrStatus();
			INT32 numRenAttrStatus = renAttrStatus.GetNumEvent();

			for(INT32 i=0; i<numRenAttrStatus; i++)
			{
				if(m_supportProgress)
				{
					const char* errEvtText = renAttrStatus.GetEvent(i);
					ReportProgress(0, errEvtText);

					if(operRst.m_errOutput)
						operRst.m_errOutput(errEvtText);
				}
			}
			
			operRst.m_numPass = numPass;
			
			{
				INT32 agentErr = agent.GetErr();
				UINT32 convFlag = (UINT32)OptiRst::ORF_ERR_INTERNAL_OPERATION;

				switch(agentErr)
				{
				case 8000:	convFlag = (UINT32)OptiRst::ORF_ERR_DUPPLICATED_RENATTR; break;
				case 9000:	
				case -1000: 
				case -1001: 
				case -2000: 
				default: break;
				}

				operRst.m_operRstFlag |= convFlag;
			}
			

			if(rst)
				*rst = operRst;

			return false;
		}

		ApplyResult(&operRst, agent);
		numPass++;

		if(firstPass)
		{
			firstResult = operRst;
			firstPass = false;
		}

		if(operRst.m_numAttrSet[0] == operRst.m_numAttrSet[1] && operRst.m_numRenAttr[0] == operRst.m_numRenAttr[1])
			break;

	} while(numPass < m_numMaximumPass); // try optimize maximum 100 times..

	operRst.m_numAttrSet[0] = firstResult.m_numAttrSet[0];
	operRst.m_numRenAttr[0] = firstResult.m_numRenAttr[0];
	operRst.m_numPass = numPass;

	if(rst)
		*rst = operRst;

	return true;
}

bool i3OptRenAttr::DoOptimize(const char* i3sfilePath, OptiRst* rst, bool saveI3SOnOptiOk)
{
	if(!i3sfilePath)	
		return false;

	i3SceneFile fileScene;

	if(!fileScene.Load(i3sfilePath))
		return false;

	i3SceneGraphInfo* scene = nullptr;
	scene = fileScene.getSceneGraphInfo(); // ref = 2, (1 for i3SceneFile)

	bool r = DoOptimize(scene, rst);

	if(saveI3SOnOptiOk && r)
	{
//		fileScene.setSceneGraphInfo(scene);		
		bool save = true;

		if(rst)
		{
			if(!rst->IsAttrSetRenAttrOptimized())
				save = false;

			//if(rst->m_numAttrSet[0] == 0 ||
			//   (rst->m_numAttrSet[0] == rst->m_numAttrSet[1] && rst->m_numRenAttr[0] == rst->m_numRenAttr[1]))
			//	save = false; // skip when no attrset is in scene or being same result with previous
		}
		
		if(save)
		{
			// saveRst = written bytes on i3s file stream
			UINT32 saveRst = fileScene.Save(i3sfilePath);

			if(rst && rst->m_errOutput)
			{
				if(saveRst != STREAM_ERR)
				{
					rst->m_errOutput("OK! - Saving optimized scene is success..");
					rst->m_operRstFlag |= i3OptRenAttr::OptiRst::ORF_OPTIMIZED_AND_SAVED;
				}
				else
				{
					rst->m_errOutput("ERR! - Failed to save optimized scene ..");
					rst->m_operRstFlag |= i3OptRenAttr::OptiRst::ORF_ERR_FAILED_SAVE_OPTIMIZED;
				}
			}
		}
	}

	return r;
}


//
//	i3OptRenAttr::Result
//
void i3OptRenAttr::OptiRst::Clear()
{
	m_numPass = 0;
	memset(m_numAttrSet, 0, sizeof(m_numAttrSet));
	memset(m_numRenAttr, 0, sizeof(m_numRenAttr));
		
	m_numCancelOper = 0;
	m_operRstFlag = 0;
	m_errOutput = nullptr;
}

i3OptRenAttr::OptiRst i3OptRenAttr::OptiRst::operator+(const OptiRst& rhs) const
{
	OptiRst rst;

	rst.m_numPass = m_numPass + rhs.m_numPass;

	for(INT32 i=0; i<2; i++)
	{
		rst.m_numAttrSet[i] = m_numAttrSet[i] + rhs.m_numAttrSet[i];
		rst.m_numRenAttr[i] = m_numRenAttr[i] + rhs.m_numRenAttr[i];
	}
	
	rst.m_numCancelOper = m_numCancelOper + rhs.m_numCancelOper;
	rst.m_operRstFlag = m_operRstFlag | rhs.m_operRstFlag;
	rst.m_errOutput = m_errOutput;

	return rst;
}

i3OptRenAttr::OptiRst& i3OptRenAttr::OptiRst::operator=(const OptiRst& rhs)
{
	if(this == &rhs)
		return *this;

	m_numPass = rhs.m_numPass;

	for(INT32 i=0; i<2; i++)
	{
		m_numAttrSet[i] = rhs.m_numAttrSet[i];
		m_numRenAttr[i] = rhs.m_numRenAttr[i];
	}

	m_numCancelOper = rhs.m_numCancelOper;
	m_operRstFlag = rhs.m_operRstFlag;
	m_errOutput = rhs.m_errOutput;

	return *this;	
}

