
// This file must be used in i3OptRenAttr.cpp

#ifndef _I3RENATTROPTIMIZER_PRIVATE_H_
#define _I3RENATTROPTIMIZER_PRIVATE_H_

// stl stuff
#include <algorithm>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <queue>

#include "i3Base.h"
#include "i3Scene/i3AttrSet.h"
#include "i3Scene/i3Body.h"
#include "i3Scene/i3BoneRef.h"


struct AnalyzedBoneRef;

template<class T, INT32 DoRecursive=1>
struct ChildParentRelationShipChecker
{
	bool operator()(const T* root) const
	{
		if(!root)
			return true;

		return RecursiveCheck(root, DoRecursive);
	}

private:
	bool RecursiveCheck(const i3Node* cur, INT32 doRecursive) const
	{
		i3Node* curParent = const_cast<i3Node*>(cur);
		INT32 numChild = curParent->GetChildCount();

		for(INT32 i=0; i<numChild; i++)
		{
			T* child = curParent->GetChild(i);

			if(!child)
			{
				I3ASSERT(child);
				return false;
			}

			if(child->GetParent() != curParent)
			{
				I3ASSERT(!child);
				return false;
			}

			if(doRecursive && !RecursiveCheck(child, doRecursive))
			{
				I3ASSERT(!child);
				return false;
			}
		}

		return true;
	}
};


struct ChildNodeRefStore
{
	void AddRef(i3Node* upper)
	{
		if(!upper)
			return;

		for(INT32 i=0; i<upper->GetChildCount(); i++)
		{
			i3Node* ins = upper->GetChild(i);

			m_nodes.push_back(ins);
			I3_MUST_ADDREF(m_nodes.back());
		}
	}

	void Release()
	{
		for(size_t i=0; i<m_nodes.size(); i++)
		{
			I3_MUST_RELEASE(m_nodes[i]);
		}

		m_nodes.clear();
	}

	void Clear()
	{
		m_nodes.clear();
	}

	std::vector<i3Node*>			m_nodes;
};


namespace i3OptRenAttr_Pri
{

//
// PersistInfoi3AttrSet
//

struct PersistInfoi3AttrSet
{
	PersistInfoi3AttrSet() : m_dest(nullptr), m_optimized(false), m_numOptimizeTry(0) {}
	PersistInfoi3AttrSet(const PersistInfoi3AttrSet& r) : m_dest(nullptr)
		, m_optimized(false), m_numOptimizeTry(0)
	{
		*this = r;
	}
	PersistInfoi3AttrSet& operator =(const PersistInfoi3AttrSet& r)
	{
		if(this == &r)
			return *this;

		m_dest = r.m_dest;
		m_optimized = r.m_optimized;
		m_numOptimizeTry = r.m_numOptimizeTry;

		return *this;
	}

	void Reset()
	{
		m_dest = nullptr;
		m_optimized = false;
		m_numOptimizeTry = 0;
	}

	bool GetOptimized() { return m_optimized; }
	void SetOptimized(bool opti) 
	{ 
		m_optimized = opti; 
	}

	void SetDest(i3AttrSet* node) { m_dest = node; }
	void IncOptimizeTry() { m_numOptimizeTry++; }

	i3AttrSet*						m_dest;

protected:	
	bool							m_optimized;
	INT32							m_numOptimizeTry;
};


//
// IAnalyzeHelper
//
struct IAnalyzeHelper
{
	virtual PersistInfoi3AttrSet* Helper_GetPersistAttrSetInfo(i3Node* node) = 0;
	virtual bool Helper_RemovePersistAttrSetInfo(i3Node* node) = 0;
};


//
// LocalTemplate
//

struct TBatchChildDestroy
{	
	template< template<class T1, class T2=std::allocator<T1> > class TSTL, class T>
	void operator()(TSTL<T>& in)
	{
		TSTL<T>::iterator x = in.begin();
		
		while(x != in.end())
		{
			T item = *(x);
			x++;

			delete item;
		}

		in.clear();
	}
};


template<class TFindNodeType>
TFindNodeType* FindFirstLoweri3Node(i3Node* curNode)
{
	if(!curNode)
		return nullptr;

	int numChild = curNode->GetChildCount();
	for(int i=0; i<numChild; i++)
	{
		i3Node* childNode = curNode->GetChild(i);

		if(i3::kind_of<TFindNodeType*>(childNode))
			return (TFindNodeType*)childNode;

		i3Node* rst = FindFirstLoweri3Node<TFindNodeType>(childNode);

		if(rst)
			return (TFindNodeType*)rst;
	}

	return nullptr;
}

template<class TFindNodeType, class TRst>
int CollectFirstFoundChildi3Nodes(i3Node* curNode, TRst* rst= nullptr)
{
	int numCollectNode = 0;

	if(!curNode)
		return numCollectNode;

	if(i3::kind_of<TFindNodeType*>(curNode))
	{
		if(rst)
			rst->push_back((TFindNodeType*)curNode);

		++numCollectNode;

		return numCollectNode;
	}

	int numChild = curNode->GetChildCount();

	for(int i=0; i<numChild; i++)
	{
		i3Node* childNode = curNode->GetChild(i);

		if(i3::kind_of<TFindNodeType*>(childNode))
		{
			if(rst)
				rst->push_back((TFindNodeType*)childNode);

			++numCollectNode;
			continue;
		}

		numCollectNode += CollectFirstFoundChildi3Nodes<TFindNodeType>(childNode, rst);
	}

	return numCollectNode;
}

template<class TCntr>
void DelAllCntrItem(TCntr& cntr)
{
	TCntr::iterator x = cntr.begin();
	while(x != cntr.end())
	{
		delete (*x);
		x++;
	}
	cntr.clear();
}

template<class TOut>
static void CollectUpperI3BoneRefI3AttrSet(TOut& out, i3Node* node, i3Node* rootNode= nullptr)
{
	if(!node)
		return; 

	if(node == rootNode)
	{
		if(node)
			out.push_back(node);

		return;
	}

//	if(i3::kind_of<i3AttrSet*>(node) || i3::kind_of<i3BoneRef*>(node))
	out.push_back(node);

	CollectUpperI3BoneRefI3AttrSet(out, node->GetParent(), rootNode);
}


template<class TOut, class TItem>
static void _RecursiveCollectLeafAnalyzedItem(TOut& out, TItem item)
{
	if(!item)
		return;

	if(!item->GetNumChild())
	{
		out.push_back(item);
		return;
	}
	
	for(INT32 i=0; i<item->GetNumChild(); i++)
	{
		TItem child = (TItem)item->GetChild(i);
		_RecursiveCollectLeafAnalyzedItem(out, child);
	}
}

template<class TItem, class TOut, class TIn>
static void CollectLeafAnalyzedItem(TOut& out, TIn& in)
{
	for(size_t i=0; i<in.size(); i++)
	{
		TItem item = in[i];
		_RecursiveCollectLeafAnalyzedItem(out, item);
	}
}

template<class Type>
struct LargeDepthNodeCompareLess : public std::binary_function<Type, Type, bool>
{
	bool operator()(Type& left, Type& right)
	{
		return left->GetNodeDepth() < right->GetNodeDepth();
	}
};


template<class TPriQRst, class TItem>
static void RecursiveCollectLargestDepthNode(TPriQRst& rst, TItem item)
{		
	INT32 numChild = item->GetNumChild();
	if(numChild == 0)
	{
		rst.push(item);
		return;
	}

	for(INT32 i=0; i<numChild; i++)
		RecursiveCollectLargestDepthNode(rst, item->GetChild(i));
}

template<class TRst, class TItem>
static void CollectLargestDepthNode(TRst& rst, TItem item)
{	
	if(!item)
		return;

	typedef std::priority_queue<TItem, std::deque<TItem>, 
		LargeDepthNodeCompareLess<TItem> > PriQueue;
	PriQueue priQRst;

	for(INT32 i=0; i<item->GetNumChild(); i++)
		RecursiveCollectLargestDepthNode<PriQueue, TItem>(priQRst, item->GetChild(i));

	DWORD numRst = 0;
	numRst = priQRst.size();

	if(priQRst.empty())
		return;
		
	TItem firstItem = priQRst.top();	
	rst.push_back(firstItem);
	priQRst.pop();

	INT32 curDepth = firstItem->GetNodeDepth();
	while(!priQRst.empty())
	{
		TItem priItem = priQRst.top();

		if(priItem->GetNodeDepth() == curDepth)
			rst.push_back(priItem);
		else
			break;

		priQRst.pop();
	}
}

template<class TItem>
static bool AnalyzedItemDepthGreater(TItem lhs, TItem rhs)
{
	return lhs->GetNodeDepth() > rhs->GetNodeDepth();
}

template<class TCollectTarget, class TAnalyzed, class THelper>
struct AnalyzeItemTraveler
{
	AnalyzeItemTraveler(THelper* inHelper, i3Node* rootNode= nullptr)
		: m_helper(inHelper), m_rootNode(rootNode)
	{
	}

	~AnalyzeItemTraveler()
	{
		Clear();
	}

	void operator()()
	{
		DelAllCntrItem(m_rst);

		INT32 numRst = 0;
		m_rst.push_back(TAnalyzed::CreateRoot());
		RecAnalyzei3Node(m_rootNode, m_rst.back(), 0, m_helper, &numRst, m_rootNode);

		if(!numRst)
		{
			delete m_rst.back();
			*(m_rst.end()-1) = nullptr;
		}
	}

	void Clear()
	{
		DelAllCntrItem(m_rst);
	}

	bool IsResultEmpty()
	{
		if(m_rst.empty())
			return true;

		bool empty = true;		
		for(size_t i=0; i<m_rst.size(); i++)
		{
			if(m_rst[i])
			{
				empty = false;
				break;
			}
		}

		return empty;
	}

	typedef	std::vector<TAnalyzed*>			RST;

	i3Node* GetRootNode() { return m_rootNode; }

	INT32 GetNumResult() { return (INT32)m_rst.size(); }
	TAnalyzed* GetResult(INT32 index) { return m_rst[index]; }

	RST& GetResults() { return m_rst; }

private:
	void RecAnalyzei3Node(i3Node* curNode, TAnalyzed* curRst,
		INT32 curNodeDepth, THelper* helper, INT32* numRst= nullptr, i3Node* rootNode= nullptr)
	{	
		if(i3::kind_of<TCollectTarget*>(curNode))
		{
			if(!curRst->TestInclude(curNode, helper))
			{
				;
			}
			else
			{
				curRst = (TAnalyzed*)curRst->AddNewChild();
				
				(*curRst)((TCollectTarget*)curNode, helper, rootNode);

				curRst->SetNodeDepth(curNodeDepth);

				if(numRst) 
					(*numRst)++;
			}
		}

		INT32 numChild = curNode->GetChildCount();
	
		for(INT32 i=0; i<numChild; i++)
		{
			i3Node* childNode = curNode->GetChild(i);
			RecAnalyzei3Node(childNode, curRst,	curNodeDepth+1, helper, numRst, rootNode);
		}
	}

private:
	THelper*								m_helper;
	i3Node*									m_rootNode;

	RST										m_rst;	
};


struct RenAttrStatusTraveler
{
	enum EventType
	{
		ENone = 0,
		EDupplicated = 1,
		EMAX
	};

	typedef std::list<std::string>			StrList;
	typedef std::set<i3RenderAttr*>			RenAttrsSet;

	RenAttrStatusTraveler(i3Node* rootNode= nullptr) : m_rootNode(rootNode) {}
	~RenAttrStatusTraveler() {}

	void Reset() 
	{ 
		m_events.clear();
	}

	void Do() { operator()(); }

	void operator()()
	{		
		Reset();

		if(!m_rootNode)
			return;

		INT32 numRst = 0;
		RecAnalyzei3Node(m_rootNode, &numRst);
	}

	INT32 GetNumEvent() { return (INT32)m_events.size(); }
	const char* GetEvent(INT32 index) 
	{ 
		StrList::iterator x = m_events.begin();

		for(INT32 i=0; i<index; i++)
			x++;

		return (*x).c_str();
	}

	void SetRootNode(i3Node* node) { m_rootNode =  node; }

private:
	void RecAnalyzei3Node(i3Node* curNode, INT32* numRst)
	{	
		if(i3::kind_of<i3AttrSet*>(curNode))
		{
			*numRst += CheckStatus((i3AttrSet*)curNode);
		}

		INT32 numChild = curNode->GetChildCount();

		for(INT32 i=0; i<numChild; i++)
		{
			i3Node* childNode = curNode->GetChild(i);
			RecAnalyzei3Node(childNode, numRst);
		}
	}

	INT32 CheckStatus(i3AttrSet* attrSet)
	{
		INT32 numRet = 0;
		INT32 numAttrCnt = attrSet->GetAttrCount();

		RenAttrsSet			dupplicated;
		
		for(INT32 k=0; k<numAttrCnt-1; k++)
		{
			i3RenderAttr* s = attrSet->GetAttr(k);

			for(INT32 i=k+1; i<numAttrCnt; i++)
			{				
				i3RenderAttr* d = attrSet->GetAttr(i);

				if(s->meta() == d->meta())
				{
					dupplicated.insert(s);
					dupplicated.insert(d);
				}
			}
		}

		// analyze result
		if(!dupplicated.empty())
		{
			numRet += (INT32)dupplicated.size();
			LogAttrSetEvent(EDupplicated, attrSet, dupplicated);
		}

		return numRet;
	}

	void LogAttrSetEvent(EventType e, i3AttrSet* src, RenAttrsSet& dstRenAttrs)
	{
		char buf[1024] = {0};
		
		static const char* eventMsg[EMAX] = 
		{
			"None",
			"Dupplicated Render Attr"
		};

		RenAttrsSet::iterator x = dstRenAttrs.begin();
		while(x != dstRenAttrs.end())
		{
			char srcAttrSetPath[1024] = {0};
			std::vector<i3Node*> nodeInPath;
			CollectUpperI3BoneRefI3AttrSet(nodeInPath, src, m_rootNode);

			for(size_t i=(nodeInPath.size()-1); i>0; i--)
			{
				strcat_s(srcAttrSetPath, 1024, nodeInPath[i]->GetName());
				strcat_s(srcAttrSetPath, 1024, "->");
			}
			strcat_s(srcAttrSetPath, 1024, src->GetName());

			const char* metaName = (*x)->meta()->class_name().c_str();

			sprintf_s(buf, 1024, "ERR! - %s|%s|%s", eventMsg[e], srcAttrSetPath, metaName);
			m_events.push_back(buf);

			x++;
		}
	}

	i3Node*								m_rootNode;
	StrList								m_events;
};

struct CollectSceneStartNodeFromi3Scene
{
	CollectSceneStartNodeFromi3Scene(i3Node* root)
	{
		m_nodes.clear();

		if(!root)
			return;

		// find i3Body
		i3Body* body = nullptr;
		bool noBody = false;

		if(i3::kind_of<i3Body*>(root))
			body = (i3Body*)root;
		else
			body = FindFirstLoweri3Node<i3Body>(root);

		if(!body)
			noBody = true;

		if(body)
		{
			// body child     = none lod scene nodes
			// body getLOD(x) = lod scene nodes
			//      - LOD
			//      - ShapeNode   static mesh
			//      - Skeleton    skinned

			INT32 numLOD = body->getLODCount();
			for(INT32 i=0; i<numLOD; i++)
			{
				// process shape node
				AddNode(body->getLOD(i)->getShapeNode());
				// oper.AddRootNode(body->getLOD(i)->getSkin ??);
			}
		}
	
		// non lod
		for(INT32 i=0; i<root->GetChildCount(); i++)
			AddNode(root->GetChild(i));
	}

	void AddNode(i3Node* node)
	{
		if(node)
			m_nodes.push_back(node);
	}

	INT32 GetNumNode() { return (INT32)m_nodes.size(); }
	i3Node* GetNode(INT32 index) { return m_nodes[index]; }

	std::vector<i3Node* >			m_nodes;
};

template<class TOutMap>
static void RecursiveCheckUpperNodeRef(TOutMap& out, i3Node* cur, i3Node* to)
{
	if(!cur)
		return;

	i3Node* parent = cur->GetParent();

	if(!parent || parent == to)
		return;

//	if(i3::kind_of<i3AttrSet*>(parent) || i3::kind_of<i3BoneRef*>(parent))
	{
		TOutMap::iterator x = out.find(parent);
		if(x == out.end())
		{
			INT32 v = 0;
			TOutMap::value_type val(parent, v);
			TOutMap::_Pairib rst = out.insert(val);
			x = rst.first;
		}

		(*x).second++;
	}
		
	RecursiveCheckUpperNodeRef(out, parent, to);
}

template<class TIn>
static bool CheckChildAttrSetPathIsUnique(TIn& in, i3Node* upper)
{
	size_t num = in.size();

	if(num < 2)
		return true;

	UpperNodeRefCntMap	mapUpperNodeRef;
	for(size_t i=0; i<num; i++)
		RecursiveCheckUpperNodeRef(mapUpperNodeRef, in[i], upper);

	for(UpperNodeRefCntMap::iterator x = mapUpperNodeRef.begin();
		x != mapUpperNodeRef.end(); x++)
	{
		if( (*x).second > 1 )
			return false;
	}
		
	return true;
}


template<class TItem>
static TItem RecursiveFindItemBySrcNode(TItem in, i3Node* node)
{
	if(in->GetSrci3Node() == node)
		return in;

	INT32 numChild = in->GetNumChild();
	for(INT32 i=0; i<numChild; i++)
	{
		TItem findRst;
		if(findRst = RecursiveFindItemBySrcNode((TItem)in->GetChild(i), node))
			return findRst;
	}	
	return nullptr;
}


//
// AnalyzedItem
//

struct AnalyzedItem
{
private:
	AnalyzedItem() {}

public:
	enum EFLAG
	{	
		FL_ANALZYED		= 0x10000000,
		FL_ROOT			= 0x80000000,
	};

	void RemoveFlag(EFLAG rmFlag) { m_flag = m_flag & ~rmFlag; }
	void SetFlag(EFLAG setFlag)	  { m_flag |= setFlag; }
	bool IsSetFlag(EFLAG setFlag) { return m_flag & setFlag ? true : false; }

	AnalyzedItem(AnalyzedItem* parent)
		: m_parent(parent)
	{
		m_depth = parent ? parent->m_depth+1 : 0;
		m_flag = 0;
		m_strBuf[0] = 0;

		m_nodeDepth = 0;
	}

	virtual ~AnalyzedItem()
	{
		TBatchChildDestroy()(m_child);
	}

	virtual AnalyzedItem* NewInstance(AnalyzedItem* parent) = 0;

	AnalyzedItem* AddNewChild()
	{
		AnalyzedItem* inst = NewInstance(this);
		m_child.push_back(inst);

		return inst;
	}

	AnalyzedItem* GetChild(INT32 index)
	{
		INT32 i=-1;

		RefChild::iterator x = m_child.begin();
		while(x != m_child.end())
		{
			i++;
			if(i==index)
				return (*x);
			x++;
		}

		return nullptr;
	}

	INT32 GetNumChild() { return m_child.size(); }
	DWORD GetFlag() { return m_flag; }

	void DelChildRef(AnalyzedItem* in)
	{
		RefChild::iterator x = find(m_child.begin(), m_child.end(), in);
		
		I3ASSERT(x != m_child.end());

		if(x != m_child.end())
			m_child.erase(x);
	}

	virtual bool TestInclude(i3Node* node, IAnalyzeHelper* helper)
	{	
		return true; 
	}

	virtual const char* GetText() { return "----"; }

	void SetNodeDepth(INT32 depth) { m_nodeDepth = depth; }
	INT32 GetNodeDepth() { return m_nodeDepth; }

	virtual i3Node* GetSrci3Node() { return nullptr; }

public:
	AnalyzedItem*						m_parent;
	INT32								m_depth; // root :0, of resultset
	INT32								m_nodeDepth;
	
	typedef std::list<AnalyzedItem*>	RefChild;
	RefChild							m_child;

	DWORD								m_flag;

	char								m_strBuf[MAX_PATH];
};

//#if defined(_DEBUG)
//
//#define ANALYZEDITEM_CLS_DEF(cls) public: \
//	static cls##* CreateRoot() { cls##* root = _NewInst(0); root->SetFlag(FL_ROOT); return root; } \
//	virtual AnalyzedItem##* NewInstance(AnalyzedItem##* parent) { return _NewInst(parent); } \
//	static cls##* _NewInst(AnalyzedItem##* parent) { return new(__FILE__,__LINE__) cls##(parent); }
//
//#else

#define ANALYZEDITEM_CLS_DEF(cls) public: \
	static cls##* CreateRoot() { cls##* root = _NewInst(0); root->SetFlag(FL_ROOT); return root; } \
	virtual AnalyzedItem##* NewInstance(AnalyzedItem##* parent) { return _NewInst(parent); } \
	static cls##* _NewInst(AnalyzedItem##* parent) { return new cls##(parent); }

//#endif

struct AnalyzedAttrSet : public AnalyzedItem
{
	ANALYZEDITEM_CLS_DEF(AnalyzedAttrSet)

public:
	AnalyzedAttrSet(AnalyzedItem* parent)
		: AnalyzedItem(parent), m_src(nullptr)	{ }

	virtual ~AnalyzedAttrSet() { }

	bool operator()(i3Node* node, IAnalyzeHelper* helper, i3Node* rootNode)
	{
		I3ASSERT(!IsSetFlag(FL_ANALZYED));

		if(!i3::kind_of<i3AttrSet*>(node))
			return false;

		m_src = (i3AttrSet*)node;

		PersistInfoi3AttrSet* persistInfo 
			= helper->Helper_GetPersistAttrSetInfo(node);

		I3ASSERT(!persistInfo->GetOptimized());		

		CollectUpperI3BoneRefI3AttrSet(m_upperNodes, node, rootNode);
		m_upperNodes.push_back(nullptr);

		if(m_parent)
			m_depth = m_parent->m_depth+1;

		SetFlag(FL_ANALZYED);

		return true;
	}

	virtual bool TestInclude(i3Node* node, IAnalyzeHelper* helper)
	{	
		PersistInfoi3AttrSet* persistInfo 
			= helper->Helper_GetPersistAttrSetInfo(node);

		if(persistInfo && persistInfo->GetOptimized())
			return false;

		return true;
	}

	virtual i3Node* GetSrci3Node() { return (i3Node*)m_src; }
	
	i3Node* GetUpperNode(INT32 index=-1) 
	{ 
		if(m_upperNodes.empty())
			return nullptr;

		if(index == -1)
			return m_upperNodes.back(); 

		return m_upperNodes[index];
	}

	INT32 GetNumUpperNodes() { return m_upperNodes.size(); }
	std::vector<i3Node*>& GetAllUpperNodes() { return m_upperNodes; }

	const char* GetText()
	{
		if(m_src)
		{
			sprintf_s(m_strBuf, MAX_PATH, "[%s] %s, nodedepth=%02d p=%08x par=%08x",
				m_src->meta()->class_name().c_str(), m_src->GetName(), GetNodeDepth(),
				this, m_parent);
		}
		else
		{
			sprintf_s(m_strBuf, MAX_PATH, "[root] p=%08x par=%08x",
				this, m_parent);
		}

		return m_strBuf;
	}

public:
	std::vector<i3Node*>				m_upperNodes;  // i3BoneRef or i3AttrSet
	i3AttrSet*							m_src;
};

// 반드시 삭제하던 상위노드의 자식노드위치를 지킬것
struct AttrSetCleaner
{
	AttrSetCleaner() { m_start = nullptr; }

	void operator()(i3Node* in, IAnalyzeHelper* helper)
	{
		m_start = in;

		if(!in)
		{			
			return;
		}	

		RecCollectForClean(in);

		for(size_t i=0; i<m_toClean.size(); i++)
		{
			helper->Helper_RemovePersistAttrSetInfo(m_toClean[i]);

			I3ASSERT(m_toClean[i]->GetParent());
			Clean(m_toClean[i]);
		}
	}

	void RecCollectForClean(i3Node* cur)
	{
		if(!cur)
			return;

		if(cur != m_start)
		{
			I3ASSERT(cur->GetParent());
		}

		if(i3::kind_of<i3AttrSet*>(cur))
		{			
			i3AttrSet* curAS = (i3AttrSet*)cur;

			if(curAS->GetAttrCount() == 0)
			{
				
				m_toClean.push_back(cur);
			}
		}

		for(INT32 i=0; i<cur->GetChildCount(); i++)
		{
			i3Node* child = cur->GetChild(i);
			RecCollectForClean(child);
		}
	}

	void Clean(i3Node* node)
	{
		if(!node)
			return;

		if(!i3::kind_of<i3AttrSet*>(node))
			return;

		i3Node* parent = node->GetParent();

		if(!parent)
			return;

		std::vector<i3Node*> childNodes;

		for(size_t i=0; i< (size_t)node->GetChildCount(); i++)
		{
			i3Node* child = node->GetChild(i);
//			I3_MUST_ADDREF(child); // 아래 RemoveFromParent대비
			childNodes.push_back(child);

			I3ASSERT(child->GetParent() == node);
		}

		INT32 numChildNodeAdd = childNodes.size();


		//
		// 삭제하려는 상위노드의 위치를 맞추면서 삽입작업
		//
		
		std::vector<i3Node*> parentChilds;
		for(size_t i=0; i<(size_t)parent->GetChildCount(); i++)
			parentChilds.push_back(parent->GetChild(i));

		ChildNodeRefStore refStore;

		refStore.AddRef(parent);
		parent->RemoveAllChild();

		I3ASSERT(numChildNodeAdd == node->GetChildCount());

		bool match = false;
		for(size_t i=0; i<(size_t)parentChilds.size(); i++)
		{
			if(parentChilds[i] == node)
			{
				match = true;

				I3ASSERT(numChildNodeAdd == (INT32)childNodes.size());

				for(size_t i=0; i<childNodes.size(); i++)
				{
					I3ASSERT(childNodes[i]->GetParent() == node);

					childNodes[i]->AddRef();
					childNodes[i]->RemoveFromParent();
					parent->AddChild(childNodes[i]);
					childNodes[i]->Release();					

				}
			}
			else
			{
				parent->AddChild(parentChilds[i]);
			}
		}
		I3ASSERT(match);

		refStore.Release();
	}

	std::vector<i3Node*>			m_toClean;
	i3Node*							m_start;
};

struct CollectRenderAttrSetStaticsFromi3Scene
{
	enum EPARAM
	{
		EPRM_NUM_TOT_ATTRSET,
		EPRM_NUM_TOT_RENATTR,
		EPRM_NUM_TOT_CANCELOPER,
		EPRM_MAX
	};

	CollectRenderAttrSetStaticsFromi3Scene()
	{
		Reset();
	}

	void Add(i3Node* rootNode)
	{
		RecDo(rootNode);
	}

	void Add(EPARAM prm, INT32 val)
	{
		m_param[prm] = val;
	}

	void Reset()
	{
		memset(m_param, 0, sizeof(m_param));
	}

	void RecDo(i3Node* cur)
	{
		if(!cur)
			return;

		if(i3::kind_of<i3AttrSet*>(cur))
		{
			m_param[EPRM_NUM_TOT_ATTRSET]++;
			m_param[EPRM_NUM_TOT_RENATTR]+= ((i3AttrSet*)cur)->GetAttrCount();
		}

		INT32 numChild = cur->GetChildCount();
		for(INT32 i=0; i<numChild; i++)
		{
			i3Node* child = cur->GetChild(i);
			RecDo(child);
		}
	}

	unsigned long GetParam(EPARAM prmIndex) { return m_param[prmIndex]; }

	unsigned long			m_param[EPRM_MAX];
};


typedef	std::vector<i3Node*>							i3NodeVec;
typedef	std::vector<i3AttrSet*>							i3AttrSetVec;
typedef std::vector<AnalyzedItem*>						AnalyzedItemVec;
typedef std::map<i3Node*, INT32 >						UpperNodeRefCntMap;
typedef std::map<i3AttrSet*, PersistInfoi3AttrSet>		PersistInfoi3AttrSetMap;
typedef std::vector<AnalyzedAttrSet*>					AnalyzedAttrSetVec;

} // end of namespace i3OptRenAttr_Pri


#endif  // end of i3OptRenAttr_Private.h
