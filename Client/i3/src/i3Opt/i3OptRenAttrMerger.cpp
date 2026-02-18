
#include "i3OptPCH.h"

#include "i3Scene/i3AttrSet.h"
#include "i3Scene/i3BoneRef.h"

#include "i3OptRenAttrMerger.h"
 
#include <algorithm>
#include <iterator> 
#include <set>
#include <list>
#include <vector>
#include <map>

#include "i3Scene/i3AllAttrs.h"
#include "i3OptRenAttr_Private.h"
 
 
I3_CLASS_INSTANCE(i3OptRenAttrMerger);


//namespace _i3OptRAMPriUtils
//{

typedef std::vector<i3Node*>								i3NodeVec;

const static i3::class_meta* gs_MergingDisabledAttr[] =
{
	i3RenderAttr::static_meta(),
};

const static INT32 NUM_MERGING_DISABLED 
	= sizeof(gs_MergingDisabledAttr) / sizeof(gs_MergingDisabledAttr[0]);

static bool CheckMergeableRenAttr(i3::class_meta* in)
{
	if(!in)
		return false;

	for(INT32 i=0; i<NUM_MERGING_DISABLED; i++)
		if(in == gs_MergingDisabledAttr[i])
			return false;

	return true;
}


//
// i3RenAttrMergeInfoGen
//

struct i3RenAttrMergeInfoGen
{
	typedef std::vector<i3AttrSet*>					InData;
	const static INT32								MIN_DUPP_MERGE = 2;	// 이갯수이상이면 머지수행

	struct Info
	{
		typedef std::map<i3::class_meta*, i3RenderAttr*>	BindAttrMap;

		// 실제내용비교, 포인터비교가아님
		bool operator !=(const Info& r) const { return !operator==(r); }
		bool operator ==(const Info& r) const;

		static bool EqualBindAttr(const BindAttrMap& l, const BindAttrMap& r);

		Info() {}
		Info(const Info& r);
		Info& operator =(const Info& r);

		i3::class_meta*								m_key = nullptr;		// i3RenderAttr의 메타클래스

		i3RenderAttr*								m_srcAttr = nullptr;
		BindAttrMap									m_bindAttr;	// i3XXXMapBindAttr등

		i3AttrSet*									m_owner = nullptr;
		// 주어진 상위I3Node 부모로부터 머지하려는 I3AttrSet으로 가는 노드패스상에서
		// 상위I3Node부모 바로아래 자식노드
		i3Node*										m_upperChild = nullptr;
	};

	typedef std::multimap<i3::class_meta*, Info>	AttrMetaClsMMap;
	typedef std::vector<Info*>						InfoVec;
	typedef std::map<i3AttrSet*, InfoVec>			AttrRowMap;

	struct GenAttrSetData
	{
		typedef std::map<i3AttrSet*, InfoVec>		AttrSetMap;
		std::vector<AttrSetMap>						m_data;
	};

	struct InfoGrpKey
	{
		InfoGrpKey() { }
		InfoGrpKey(i3RenAttrMergeInfoGen::Info* key);
		InfoGrpKey(const InfoGrpKey& r);

		InfoGrpKey& operator =(const InfoGrpKey& r);
		bool operator ==(const InfoGrpKey& r) const;
		bool operator !=(const InfoGrpKey& r) const { return *this != (r); }

		struct Hasher //: public std::tr1::hash<InfoGrpKey>
		{
			size_t operator()(const InfoGrpKey& key) const 
			{ return std::tr1::hash<size_t>()((size_t)(key.m_key->m_key)); }
		};

		struct Pred //: public std::equal_to<InfoGrpKey>
		{
			bool operator()(const InfoGrpKey& left, const InfoGrpKey& right) const
				{ return (*left.m_key) == (*right.m_key); } // 포인터주소가아닌 내용비교합니다.
		};

		i3RenAttrMergeInfoGen::Info*				m_key = nullptr;
	};

	typedef i3::unordered_map<InfoGrpKey, InfoVec, 
		InfoGrpKey::Hasher, InfoGrpKey::Pred>		CntPairInfoUOMap;

	i3RenAttrMergeInfoGen();
	~i3RenAttrMergeInfoGen();

	bool Do(i3Node* upper, InData& in, i3OptRenAttrMergerRst* rst);

	GenAttrSetData& GetRstData() { return m_rstData; }

	struct PairAttrSet
	{
		~PairAttrSet();
		void Clear(bool releaseRef=true);

		// <신규생성될AttrSet, 중복처리에 참여한 AttrSet들의 공통된부모의 자식노드>
		typedef std::map<i3AttrSet*, std::vector<i3Node*> >		PairMap;

		// case #0
		// key=2개의 대상i3AttrSet에서 중복된RenAttr을 계산해서 그항목을 클론한후 클론대상을 가지고있는 신규AttrSet
		// val=2개의 대상i3AttrSet이 가지고있던 자식노드들
		PairMap										m_attrSet;

	};

	void GenerateAttrSetPair(PairAttrSet* out, i3OptRenAttrMergerRst* rst= nullptr,
		bool removeRenAttrFromOriginal=false);

	static void DumpLog(AttrRowMap& in);
	static void DumpLog(GenAttrSetData& in);

	bool _GenerateMergeInfo(i3Node* upper, i3RenAttrMergeInfoGen::InData& in, 
		i3OptRenAttrMergerRst* rst);

	AttrMetaClsMMap& GetMergeInfo() { return m_mmapInfo; }

private:
	void AddMergeInfo(i3AttrSet* ownerAttr, i3RenderAttr* attr, i3Node* upperChild);
	void GenerateNewAttrSet(GenAttrSetData& out, AttrRowMap& in);

	AttrMetaClsMMap									m_mmapInfo;
	GenAttrSetData									m_rstData;

	bool											m_needCleaupEmptyi3AttrSet;
};


//
// Local Template Utility & functor class
//

template<class TCls>
static TCls CloneI3ClassFromInstance(TCls cloneSrc)
{
	if(!cloneSrc)
		return nullptr;
	
	TCls cloneDst = cloneSrc->create_instance();
	cloneSrc->CopyTo(cloneDst, I3_COPY_INSTANCE);

	return cloneDst;
}

template<class TMMapIn, class TOper>
static void MMapOperator(TMMapIn& in, TOper& oper)
{
	for(TMMapIn::iterator xIter = in.begin(); xIter != in.end();
		xIter=in.upper_bound((*xIter).first))
	{
		TMMapIn::_Pairii rangeIter = in.equal_range((*xIter).first);
		for(TMMapIn::iterator itemIter = rangeIter.first;
			itemIter != rangeIter.second; itemIter++)
		{
			oper((*itemIter).first, (*itemIter).second);
		}
	}
}

template<class TMapIn, class TOper>
static void MapOperator(TMapIn& in, TOper& oper)
{
	for(TMapIn::iterator xIter = in.begin(); xIter != in.end(); xIter++)
		oper((*xIter).first, (*xIter).second);
}

template<class TVecIn>
static bool Existi3NodeInVec(TVecIn& in, i3Node* inNode)
{
	for(size_t i=0; i<in.size(); i++)
		if(in[i] == inNode) 
			return true;

	return false;
}


struct AttrSetRowRemoverAndCopyer
{			
	AttrSetRowRemoverAndCopyer(i3RenAttrMergeInfoGen::AttrRowMap& in, 
		i3RenAttrMergeInfoGen::Info* contain, i3RenAttrMergeInfoGen::AttrRowMap& copyInto)
		: m_in(in), m_contain(contain), m_into(copyInto)
	{				
		MapOperator(in, (*this));

		for(std::set<i3AttrSet*>::iterator delIter = m_del.begin(); 
			delIter != m_del.end(); delIter++)
		{
			i3RenAttrMergeInfoGen::AttrRowMap::iterator x = m_in.find((*delIter));
			m_in.erase(x);
		}
	}
	 
	void operator()(const i3AttrSet* k, i3RenAttrMergeInfoGen::InfoVec& v)
	{
		if(!m_contain)
			return;
		
		bool foundSame = false;
		for(i3RenAttrMergeInfoGen::InfoVec::iterator x = v.begin();
			x != v.end(); x++)
		{
			i3RenAttrMergeInfoGen::Info* src = (*x);
			if((*src) == (*m_contain))
			{
				foundSame = true;				
				break;
			}
		}

		if(!foundSame)
		{
			i3RenAttrMergeInfoGen::AttrRowMap::value_type val(const_cast<i3AttrSet*>(k), v);
			m_into.insert(val);

#ifdef _USE_I3RENATTROPTIMIZER_LOG_
			{
				char buf[MAX_PATH]={0};
				sprintf_s(buf, MAX_PATH, "[AttrSetRowRemover] find[%s] \n",
					m_contain->m_key->class_name().c_str());
				OutputDebugStringA(buf);
			}
#endif

			m_del.insert(const_cast<i3AttrSet*>(k));
		}
	}
	
	i3RenAttrMergeInfoGen::AttrRowMap&				m_in;
	i3RenAttrMergeInfoGen::Info*					m_contain;
	i3RenAttrMergeInfoGen::AttrRowMap&				m_into;

	std::set<i3AttrSet*> m_del;
};

struct AttrMaxDuppAMIFinder
{
	AttrMaxDuppAMIFinder(i3RenAttrMergeInfoGen::CntPairInfoUOMap& in, 
		i3RenAttrMergeInfoGen::InfoVec* pass= nullptr, INT32 minDuppMerge=2)
		: m_src(in), m_pass(pass), m_minDuppMerge(minDuppMerge)
	{
		m_maxDupp = -1;
		m_pairExist = false;

		MapOperator(m_src, (*this));
	}

	void operator()(const i3RenAttrMergeInfoGen::InfoGrpKey& k, 
		i3RenAttrMergeInfoGen::InfoVec& v)
	{	
		INT32 num = (INT32)v.size();

		// 현재갯수가 최소 중복개수보다 작으면 스킵
		if(num < m_minDuppMerge)
			return;

		m_pairExist = true;

		for(INT32 i=0; i<num; i++)
		{
			// 현재순위보다 높은 순위일경우만처리
			if(m_maxDupp < num)
			{						
				bool skip = false;

				if(m_pass)
				{
					for(i3RenAttrMergeInfoGen::InfoVec::iterator x = m_pass->begin();
						x != m_pass->end(); x++)
					{
						if((*(*x)) == (*v[i]))
						{
							skip = true;
							break;
						}
					}
				}

				if(!skip)
				{
					m_maxDupp = num;
					m_rst = v[i];
				}
			}
		}
	}

	INT32 GetMaxDupplicated() { return m_maxDupp; }

	i3RenAttrMergeInfoGen::CntPairInfoUOMap&		m_src;
	i3RenAttrMergeInfoGen::InfoVec*					m_pass;

	const INT32										m_minDuppMerge;

	INT32											m_maxDupp;
	bool											m_pairExist; // pair가 한개라도있다.
	i3RenAttrMergeInfoGen::Info*					m_rst = nullptr;

};

struct CountPair
{
	typedef i3RenAttrMergeInfoGen::CntPairInfoUOMap OutMap;
	typedef i3RenAttrMergeInfoGen::InfoGrpKey		InfoGrpKey;
	typedef i3RenAttrMergeInfoGen::AttrRowMap		InMap;

	CountPair(InMap& in, OutMap& out, INT32 minPairNum=2)
	{		
		OutMap tmpOut;
		m_err = 0;

		m_minPairNum = minPairNum;
		MapOperator(in, (*this));	// result -> m_out

		// minPairNum 이하인 것들을 삭제
		MapOperator(m_out, (*this));

		for(size_t si=0; si<m_del.size(); si++)
		{
			const i3RenAttrMergeInfoGen::InfoGrpKey* key = m_del[si];
			
			OutMap::iterator x = m_out.find(*key);
			if(x != m_out.end())
				m_out.erase(x);
		}

		// i3RenderAttr meta() 이름순으로 재정렬
		std::vector<InfoGrpKey*> vecGrpKey;
		for(OutMap::iterator x = m_out.begin(); x != m_out.end(); x++)
			vecGrpKey.push_back((InfoGrpKey*)&((*x).first));

		struct orderGrpKeyPred
		{			
			bool operator()(const InfoGrpKey* lhs, const InfoGrpKey* rhs) const
			{
				return strcmp( lhs->m_key->m_key->class_name().c_str(), 
					           rhs->m_key->m_key->class_name().c_str() ) < 0 ? true : false;
			}
		};

		if(!vecGrpKey.empty())
		{
			std::sort(vecGrpKey.begin(), vecGrpKey.end(), orderGrpKeyPred());

			for(size_t i=0; i<vecGrpKey.size(); i++)
			{
				InfoGrpKey* key = vecGrpKey[i];

				OutMap::iterator x = m_out.find(*key);
				if(x == m_out.end())
				{
					m_err = 1000;
					return;
				}
				else
				{			
					OutMap::value_type val((*x).first, (*x).second);
					out.insert(val);
				}
			}
		}
	}

	void operator()(const i3AttrSet* k, i3RenAttrMergeInfoGen::InfoVec& v)
	{
		size_t numItem = v.size();

		for(size_t i=0; i<numItem; i++)
		{
			OutMap::iterator x = m_out.find(i3RenAttrMergeInfoGen::InfoGrpKey(v[i]));
			if(x == m_out.end())
			{
				i3RenAttrMergeInfoGen::InfoGrpKey nkey(v[i]);
				OutMap::value_type val(nkey, i3RenAttrMergeInfoGen::InfoVec());
				i3::pair<OutMap::iterator, bool> rst = m_out.insert(val);
				x = rst.first;
			}

			(*x).second.push_back(v[i]);
		}
	}

	// m_minPairNum이하 삭제작업떄 호출
	void operator()(const i3RenAttrMergeInfoGen::InfoGrpKey& k, i3RenAttrMergeInfoGen::InfoVec& v)
	{
		size_t numItem = v.size();

		if((INT32)numItem <  m_minPairNum)
			m_del.push_back(&k);
	}

	static void DumpLog(OutMap& in)
	{
#ifdef _USE_I3RENATTROPTIMIZER_LOG_
		OutputDebugStringA("\n Listing CountPair Start \n================\n");
		for(OutMap::iterator xDbgIter = in.begin(); xDbgIter != in.end(); xDbgIter++)
		{
			OutputDebugStringA( (((*xDbgIter).first.m_key)->m_srcAttr->meta()->class_name()).c_str() );
			OutputDebugStringA( "/" );
			i3RenAttrMergeInfoGen::InfoVec& v = (*xDbgIter).second;
			for(i3RenAttrMergeInfoGen::InfoVec::iterator x = v.begin(); x != v.end(); x++)
			{
				INT32 cid = (*x)->m_srcAttr->GetID();

				char buf[256];
				sprintf(buf, "[%03d],", cid);
				OutputDebugStringA(buf);
			}
			OutputDebugStringA("\n==============\n");
		}
		OutputDebugStringA("\n Listing CountPair End \n");
#endif
	}

	struct UniqueRowChecker
	{
		UniqueRowChecker() { m_uniqueRow = true; }
		
		void operator()(const i3RenAttrMergeInfoGen::InfoGrpKey& key, 
			i3RenAttrMergeInfoGen::InfoVec& val)
		{
			if(!m_uniqueRow)
				return;

			MetaMap::iterator x = m_mapMeta.find(key.m_key->m_key);
			if(x == m_mapMeta.end())
			{
				MetaMap::value_type v(key.m_key->m_key, 0);
				m_mapMeta.insert(v);
			}
			else
			{
				m_uniqueRow = false;
				return;
			}
		}

		bool IsUniqueRow() { return m_uniqueRow; }

	private:
		typedef std::map<i3::class_meta* , INT32>	MetaMap;
		MetaMap										m_mapMeta;

		bool										m_uniqueRow;
	};

	static bool CheckUniqueRenAttrMetaRow(i3RenAttrMergeInfoGen::CntPairInfoUOMap& in)
	{
		UniqueRowChecker checker;
		MapOperator(in, checker);

		return checker.IsUniqueRow();
	}

	// numCut이하를 가지는 행을 없앰
	static void CutRowByDuppNum(i3RenAttrMergeInfoGen::CntPairInfoUOMap& inout, INT32 numCut)
	{
		i3RenAttrMergeInfoGen::CntPairInfoUOMap::iterator x = inout.begin();
		while(x != inout.end())
		{
			if(numCut > (INT32)((*x).second.size()))
				x = inout.erase(x);
			else
				x++;
		}
	}

	INT32 GetErr() { return m_err; }
	bool IsErr() { return m_err != 0; }

	OutMap													m_out;
	INT32													m_minPairNum;
	std::vector<const i3RenAttrMergeInfoGen::InfoGrpKey*>	m_del;

	INT32													m_err;

};

struct RenAttrRowBuilder
{
	RenAttrRowBuilder(i3RenAttrMergeInfoGen::AttrMetaClsMMap& in, 
		i3RenAttrMergeInfoGen::AttrRowMap& out) : m_out(out) 
	{
		MMapOperator(in, (*this));
	}
	
	void operator()(const i3::class_meta* meta, i3RenAttrMergeInfoGen::Info& info)
	{
		i3AttrSet* owner = info.m_owner;

		i3RenAttrMergeInfoGen::AttrRowMap::iterator x = m_out.find(owner);

		if(x == m_out.end())
		{
			i3RenAttrMergeInfoGen::AttrRowMap::value_type 
				val(owner, i3RenAttrMergeInfoGen::InfoVec());

			i3RenAttrMergeInfoGen::AttrRowMap::_Pairib rst = m_out.insert(val);
			x = rst.first;					
		}

		i3RenAttrMergeInfoGen::InfoVec& insTo = (*x).second;
		insTo.push_back(&info);
	}

	i3RenAttrMergeInfoGen::AttrRowMap& 			m_out;
};

static i3Node* RecursiveFindUpperChildBetween(i3Node* curNode, i3Node *upper, 
	std::list<i3Node*>* rstNodePath) 
{
	if(!curNode)
		return nullptr;

	i3Node* parent = curNode->GetParent();

	if(rstNodePath)
		rstNodePath->push_back(parent);

	if(!parent)
		return nullptr;

	if(parent == upper)
		return curNode;

	return RecursiveFindUpperChildBetween(parent, upper, rstNodePath);
}

static i3Node* ValidatePathAndGetUpperChild(i3Node* cur, i3Node* upper)
{
	if(!cur || !upper)
		return nullptr;

	std::list<i3Node*> nodePath;
	i3Node* upperChild = RecursiveFindUpperChildBetween(cur, upper, &nodePath);

	return upperChild;
}

struct GenerateNewAttrSetOrder
{
	friend struct MergeWithUpperRenAttr;

	typedef i3RenAttrMergeInfoGen::PairAttrSet::PairMap		PairMap;

	struct PassedChildInfo
	{
		PassedChildInfo()
		{
			m_inPairMap = false;
			m_src = nullptr;
			m_order = -1;
		}

		PassedChildInfo(const PassedChildInfo& rhs) : m_inPairMap(false)
			, m_src(nullptr), m_order(-1) { *this = rhs; }

		PassedChildInfo& operator=(const PassedChildInfo& rhs)
		{
			if(this == &rhs) return *this;

			m_inPairMap = rhs.m_inPairMap;
			m_iterPairMap = rhs.m_iterPairMap;			
			m_iterItemPairMap = rhs.m_iterItemPairMap;
			m_src = rhs.m_src;
			m_order = rhs.m_order;

			return *this;
		}
		
		bool						m_inPairMap;
		PairMap::iterator			m_iterPairMap;
		i3NodeVec::iterator			m_iterItemPairMap;
		i3Node*						m_src;
		INT32						m_order;
	};

	struct NewAttrInsOrder
	{
		NewAttrInsOrder() : m_child(nullptr), m_refAttrSet(nullptr), m_insNewAttrSet(false), m_forceOptimized(false) {}
		NewAttrInsOrder(const NewAttrInsOrder& rhs) : NewAttrInsOrder() { *this = rhs; }

		NewAttrInsOrder& operator =(const NewAttrInsOrder& rhs)
		{
			if(this == &rhs) return *this;

			m_child			= rhs.m_child;
			m_insNewAttrSet = rhs.m_insNewAttrSet;
			m_refAttrSet	= rhs.m_refAttrSet;
			m_attrSetChild  = rhs.m_attrSetChild;
			m_forceOptimized = rhs.m_forceOptimized;

			return *this;
		}


		i3Node*					m_child;

		bool					m_insNewAttrSet;

		// 자식순서에 맞추어 정렬하다보니 옵티마이즈한게 원래상태가되어 
		// 재옵티마이즈대상이되어 무한옵티마이즈에 빠질수있음
		bool					m_forceOptimized;	
		i3AttrSet*				m_refAttrSet;

		i3NodeVec				m_attrSetChild;
	};

	typedef std::vector<NewAttrInsOrder>					NewAttrInsOrderRst;
	typedef std::vector<PassedChildInfo>					PassedInfoVec;

	typedef std::vector<PassedChildInfo*>					PassedChildInfoPtrVec;
	typedef std::vector<PassedChildInfoPtrVec>				PCIGroup;

	typedef std::multimap<i3AttrSet*, NewAttrInsOrder*>		AttrSetDuppMMap;

	PassedInfoVec											m_passed;
	PCIGroup												m_grpPCI;

	NewAttrInsOrderRst										m_rst;
	AttrSetDuppMMap											m_dupp;

	INT32													m_err;


	GenerateNewAttrSetOrder()
	{
		Reset();
	}

	void Reset()
	{
		m_passed.clear();
		m_grpPCI.clear();
		m_rst.clear();
		m_dupp.clear();

		m_err = 0;
	}

	void FindInPairMap(i3RenAttrMergeInfoGen::PairAttrSet& in, PassedChildInfo& outInfo, 
		i3Node* toFind)
	{
		if(!toFind)
			return;

		for(PairMap::iterator x = in.m_attrSet.begin(); x != in.m_attrSet.end(); x++)
		{
			i3NodeVec& childs = (*x).second;

			for(i3NodeVec::iterator childIter = childs.begin(); 
				childIter != childs.end(); childIter++)
			{
				if((*childIter) == toFind)
				{
					outInfo.m_inPairMap = true;
					outInfo.m_iterItemPairMap = childIter;
					outInfo.m_iterPairMap = x;
				}
			}			
		}
	}

	void SetErr(INT32 err)
	{
		assert(0);
		m_err = err;
	}

	bool operator()(i3Node* upper, i3RenAttrMergeInfoGen::PairAttrSet& inout)
	{
		Reset();

		INT32 numChild = upper->GetChildCount();
		for(INT32 i=0; i<numChild; i++)
		{
			PassedChildInfo info;
			i3Node* child = nullptr;

			child = upper->GetChild(i);

			info.m_src = child;
			info.m_order = i;

			FindInPairMap(inout, info, child);

			m_passed.push_back(info);
		}

		CollectGroupPCI();

		if(!ResolveChildOrder())
			return false;

		// 다음과같은경우 무한옵티마이즈에 빠지지않게하기위해 완료처리
		/* 
		    자식 순서처리하지않고 머지했을때, merged attrset은 2개의 AttrSet이 합쳐진거
			upper
			  merged attrset
			    _ attrset 0
				- attrset 1
			  i3node child


		    자식순서를 맞추기위해 mergerAttrSet을 분리
			upper
			  merged attrset 0
			    - attrset 0
			  i3node child
			  merged attrset 1
			    - attrset 1

		    머지해서 상위노드로 새로생성해 올렸는데 순서처리때문에 
			다시분리되어 다시 옵티마이즈대상이되어 무한루프			
		*/

		for(size_t i=0; i<m_rst.size(); i++)
		{
			if(m_rst[i].m_insNewAttrSet)
			{
				AttrSetDuppMMap::value_type val(m_rst[i].m_refAttrSet, &m_rst[i]);
				m_dupp.insert(val);
			}
		}

		// AttrSet이 자식순서때문에 나뉜것은 강제 옵티마이즈완료 처리
		for(AttrSetDuppMMap::iterator x = m_dupp.begin(); 
			x != m_dupp.end(); x = m_dupp.upper_bound((*x).first) )
		{
			if(m_dupp.count((*x).first) > 1)
			{
				AttrSetDuppMMap::_Pairii rangeIterPair = m_dupp.equal_range((*x).first);
				for(AttrSetDuppMMap::iterator rangeIter=rangeIterPair.first;
					rangeIter != rangeIterPair.second; rangeIter++)
				{
					((*rangeIter).second)->m_forceOptimized = true;
				}
			}			
		}
	
		return true;
	}

	void CollectGroupPCI()
	{
		// attrset map#0 item0 <<
		// attrset map#0 item1 <<
		// child << 중간에 일반 자식노드 들어갈수있다.
		// attrset map#0 item2 <<
		// attrset map#1 item1 

		// attrset map#0 item0 <<
		// attrset map#0 item1 <<
		// attrset map#1 item1 <<
		// attrset map#1 item2 
		// attrset map#0 item2 <<
		// attrset map#0 item3 <<

		m_grpPCI.clear();

		for(size_t i=0; i<m_passed.size(); i++)
		{
			PassedChildInfo& cur = m_passed[i];

			if(cur.m_inPairMap)
			{
				PassedChildInfo& startPCI = m_passed[i];

				PassedChildInfoPtrVec newerGrp;
				newerGrp.push_back(&startPCI);

				for(size_t k=i+1; k<m_passed.size(); k++, i++)
				{
					PassedChildInfo& nextPCI = m_passed[k];

					// 일반 노드거나 , 다른 PairMap에있을경우 분리
					if(!nextPCI.m_inPairMap || (startPCI.m_iterPairMap != nextPCI.m_iterPairMap))
						break;
					else
						newerGrp.push_back(&nextPCI);
				}

				m_grpPCI.push_back(newerGrp);
			}
			else
			{
				// 일반노드
				PassedChildInfoPtrVec newerGrp;
				newerGrp.push_back(&cur);

				m_grpPCI.push_back(newerGrp);
			}
		}
	}

	bool ResolveChildOrder()
	{	
		m_rst.clear();

		for(size_t i=0; i<m_grpPCI.size(); i++)
		{
			PassedChildInfoPtrVec& curPtrVec = m_grpPCI[i];

			size_t numCurPtr = curPtrVec.size();

			if(numCurPtr == 0)
			{
				SetErr(600);
				return false;
			} 
			else if(numCurPtr > 0)
			{
				if(numCurPtr == 1 && !curPtrVec[0]->m_inPairMap)
				{
					// 일반노드
					NewAttrInsOrder order;
					order.m_child = curPtrVec[0]->m_src;
					
					m_rst.push_back(order);
				}
				else
				{
					// 그루핑된 AttrSet
					NewAttrInsOrder order;
					order.m_child = curPtrVec[0]->m_src;
					order.m_insNewAttrSet = true;
					order.m_refAttrSet = (i3AttrSet*)(*curPtrVec[0]->m_iterPairMap).first;

					for(size_t k=0; k<numCurPtr; k++)
					{						
						PassedChildInfo& pci = *(curPtrVec[k]);
						order.m_attrSetChild.push_back(pci.m_src);
					}

					m_rst.push_back(order);
				}
			}
		}

		return true;
	}

};


struct MergeWithUpperRenAttr
{
		
	MergeWithUpperRenAttr(i3AttrSet* upper, i3RenAttrMergeInfoGen::InData& in, 
		i3OptRenAttrMergerRst* rst)
	{
		m_err = 0;

		// upper노드의 모든 1단계 자식AttrSet은 모두 in에 들어와야한다.
		if(in.empty())
			return;
		
		i3AttrSet* dstAttrSet = nullptr;
		bool cancelOptimize = false;
		std::vector<i3RenderAttr*> removeChildRenAttr;

		if(in.size() > 1)
		{
			if(!GenerateDupplicatedAttrSet(upper, in, rst, removeChildRenAttr))
				cancelOptimize = true;
		}
		else if(in.size() == 1)
		{
			dstAttrSet = in[0];

			for(INT32 i=0; i<dstAttrSet->GetAttrCount(); i++)
				removeChildRenAttr.push_back(dstAttrSet->GetAttr(i));

			if(removeChildRenAttr.empty())
				cancelOptimize = true;			
		}
		else
		{
			m_err = -1;
			return;
		}

		{
			std::vector<i3RenderAttr*> removeFromChildVec;
			std::vector<i3RenderAttr*> addToUpperVec;

			for(size_t i=0; i<removeChildRenAttr.size(); i++)
			{
				i3RenderAttr* toFind = removeChildRenAttr[i];
				
				bool foundMetaOnUpper = false;
				bool removeFromChild = false;

				for(INT32 k=0; k<upper->GetAttrCount(); k++)
				{
					i3RenderAttr* upperRenAttr = upper->GetAttr(k);

					if(toFind->meta() == upperRenAttr->meta())
					{
						foundMetaOnUpper = true;

						if(toFind->IsSame(upperRenAttr))
						{
							// 값이 같으면 차일드에서 삭제
							removeFromChild = true;
						}
						else
						{
							// 동일한 Meta, 값이같지않으면 걍나둠
						}

						break;
					}
				}

				if(!foundMetaOnUpper)
				{
					addToUpperVec.push_back(toFind);		// 못찾았다면 차일드것을 Upper에 복사
					removeFromChild = true;					// 차일드에서 삭제
				}

				if(removeFromChild)
					removeFromChildVec.push_back(toFind);	// 동일한 renAttr meta를 찾음				
			}

			if(!addToUpperVec.empty())
			{
				for(size_t i=0; i<addToUpperVec.size(); i++)
				{
					i3RenderAttr* cloned = addToUpperVec[i]->create_instance();
					addToUpperVec[i]->CopyTo(cloned, I3_COPY_INSTANCE);

					upper->AddAttr(cloned);
				}
			}

			if(!removeFromChildVec.empty())
			{
				typedef std::map<i3AttrSet*, std::vector<i3RenderAttr*> > DelRenAttrMap;
				DelRenAttrMap delRenAttr;

				for(size_t i=0; i<in.size(); i++)
				{
					i3AttrSet* dstChild = in[i];
					I3ASSERT(dstChild);

					DelRenAttrMap::value_type val(dstChild, std::vector<i3RenderAttr*>());
					delRenAttr.insert(val);
											
					for(size_t m=0; m<removeFromChildVec.size(); m++)
					{
						i3RenderAttr* rmRenAttr = removeFromChildVec[m];

						bool foundSame = false;						
						for(INT32 k=0; k<dstChild->GetAttrCount(); k++)
						{
							i3RenderAttr* childRenAttr = dstChild->GetAttr(k);

							if(rmRenAttr->meta() == childRenAttr->meta())
							{
								if(childRenAttr->IsSame(rmRenAttr))
								{
									DelRenAttrMap::iterator x = delRenAttr.find(dstChild);
									if(x != delRenAttr.end())
									{
										((*x).second).push_back(childRenAttr);
									}
									else
									{
										m_err = -2;
										return;
									}
									
									foundSame = true;
									break;
								}
								else
								{
									m_err = -3;
									return;
								}
							}
						}

						if(!foundSame)
						{
							m_err = -9;
							return;
						}
						
					}
				}

				for(DelRenAttrMap::iterator delIter = delRenAttr.begin(); 
					delIter != delRenAttr.end(); delIter++)
				{
					std::vector<i3RenderAttr*>& del = (*delIter).second;

					for(size_t t=0; t<del.size(); t++)
						((*delIter).first)->RemoveAttr(del[t]);
				}

			}

		}
		
		// 현재시도한 자식들은 모두 옵티마이즈처리
		if(rst) 
		{
			for(size_t i=0; i<in.size(); i++)
			{
				rst->AddNonOptiNode((i3Node*)in[i]);
			}
		}
	}

	bool GenerateDupplicatedAttrSet(i3AttrSet* upper, i3RenAttrMergeInfoGen::InData& in, 
		i3OptRenAttrMergerRst* rst, std::vector<i3RenderAttr*>& rstRenAttr)
	{
		if(!m_generator._GenerateMergeInfo(upper, in, nullptr))
			return false;

		i3RenAttrMergeInfoGen::AttrRowMap mapRenAttrRow;
		RenAttrRowBuilder builder(m_generator.GetMergeInfo(), mapRenAttrRow);

		if(mapRenAttrRow.size() != in.size())
			return false;

		// AttrRow들에서 중복되는 RenerAttr수를 센다.
		i3RenAttrMergeInfoGen::CntPairInfoUOMap mapCntPairRenAttr;
		CountPair counter(mapRenAttrRow, mapCntPairRenAttr, 2); // 2개이상 중복된것만

		if(mapCntPairRenAttr.empty())
			return false;

		AttrMaxDuppAMIFinder finder(mapCntPairRenAttr, nullptr, 2);

		if(!finder.m_pairExist)
			return false;

		for(i3RenAttrMergeInfoGen::CntPairInfoUOMap::iterator x = mapCntPairRenAttr.begin();
			x != mapCntPairRenAttr.end(); x++)
		{	
			i3RenAttrMergeInfoGen::InfoVec& info = (*x).second;

			if((INT32)info.size() == finder.m_maxDupp)
			{
				const i3RenAttrMergeInfoGen::InfoGrpKey& igk = (*x).first;				
				rstRenAttr.push_back(igk.m_key->m_srcAttr);
			}
		}

		return true;
	}

	INT32 GetErr() { return m_err; }

protected:
	i3RenAttrMergeInfoGen::PairAttrSet					m_pairSet;
	i3RenAttrMergeInfoGen								m_generator;
	INT32												m_err;
};

//} // end of namespace _i3OptRAMPriUtils
// using namespace _i3OptRAMPriUtils;     // 컴파일시 심볼길이 초과오류방지


//
// i3OptRenAttrMergerRst
// 

void i3OptRenAttrMergerRst::Clear()
{
	m_status=0; 

	m_opti.clear(); 
	m_nonOpti.clear();
	m_newlyCreated.clear(); 
	m_cancelOper.clear();
}

void i3OptRenAttrMergerRst::AddOptiNode(i3Node* node)
{
	if(i3::kind_of<i3AttrSet*>(node))
	{
	//	if(find(m_opti.begin(), m_opti.end(), node) == m_opti.end()) 
	// m_opti가 set임
			m_opti.insert(node);
	}
	else
	{
		assert(0);
	}
}

void i3OptRenAttrMergerRst::AddNonOptiNode(i3Node* node)
{
	if(i3::kind_of<i3AttrSet*>(node))
	{
		m_nonOpti.insert(node);
	}
	else
	{
		assert(0);
	}
}

void i3OptRenAttrMergerRst::AddNewlyCreatedNode(i3Node* node)
{
	if(i3::kind_of<i3AttrSet*>(node))
	{
		m_newlyCreated.insert(node);
	}
	else
	{
		assert(0);
	}
}

void i3OptRenAttrMergerRst::AddCancelOper(i3Node* node)
{
	if(i3::kind_of<i3AttrSet*>(node))
	{
		m_cancelOper.insert(node);
	}
	else
	{
		assert(0);
	}
}

void i3OptRenAttrMergerRst::MergeNonOptiIntoOpti()
{
	for(NodeSet::iterator x = m_nonOpti.begin(); x != m_nonOpti.end(); x++)
		m_opti.insert((*x));

	m_nonOpti.clear();
}


//
// i3RenAttrMergeInfoGen
//
i3RenAttrMergeInfoGen::i3RenAttrMergeInfoGen() : m_needCleaupEmptyi3AttrSet(false)
{
}

bool i3RenAttrMergeInfoGen::_GenerateMergeInfo(i3Node* upper, i3RenAttrMergeInfoGen::InData& in, 
	i3OptRenAttrMergerRst* rst)
{
	size_t num = in.size();

	if(num < 2)
	{
		if(num == 0)
			return false;

		// 입력데이터가 1개인경우 옵티마이즈대상이아님
		if(rst)
			rst->AddOptiNode((i3Node*)in[0]);

		return false;
	}

	// 주어진 i3AttrSet들을 모두순환하면서 소속된 i3RenderAttr을 모은다.
	// i3TextureEnableAttr & i3TextureBindAttr 등 짝이되는것은 하나로처리
	// 옵티마이즈 대상 Attr만 모아진다.
	for(size_t i=0; i<num; i++)
	{
		i3AttrSet* cur = in[i];

		// 현재(cur)에서 upper노드까지의 경로중에 upper의자식노드중에 있는지확인과 얻어오기
		i3Node* upperChild = ValidatePathAndGetUpperChild(cur, upper);
		if(!upperChild)
		{
			// 현재 i3AttrSet에서 루트로의 트리경로에 부모 i3AttrSet이나 i3BoneRef가없음
			// 옵티마이즈 완료처리
			if(rst) rst->AddOptiNode((i3Node*)cur);
			continue;
		}

		for(INT32 k=0; k<cur->GetAttrCount(); k++)
		{
			i3RenderAttr* curRenAttr = cur->GetAttr(k);
			AddMergeInfo(cur, curRenAttr, upperChild);
		}
	}

	return true;
}

bool i3RenAttrMergeInfoGen::Do(i3Node* upper, i3RenAttrMergeInfoGen::InData& in, 
	i3OptRenAttrMergerRst* rst)
{
	if(!_GenerateMergeInfo(upper, in, rst))
		return false;

	AttrRowMap mapRenAttrRow;
	RenAttrRowBuilder builder(m_mmapInfo, mapRenAttrRow); // 최대 중복된 i3RenderAttr를 구한다.

	GenAttrSetData newlyCreateAttrSetDat;

	{
		bool bCont = true;
		do
		{
			bool doNextPass = false;

			InfoVec passAMI;
			AttrRowMap nextPassRow;

			do
			{
				if(mapRenAttrRow.size() < MIN_DUPP_MERGE)
				{
					// 비어있거나 1개만있으면 처리를 종료
					if(!mapRenAttrRow.empty())
					{
						if(rst)
						{
							AttrRowMap::iterator Iter = mapRenAttrRow.begin();
							rst->AddNonOptiNode((i3Node*)((*Iter).first));
						}
					}

					bCont = false;
					break;
				}

				// 현재 결과에서 중복되는 RenderAttr을 센다.
				CntPairInfoUOMap mapCntPairRenAttr;
				CountPair counter(mapRenAttrRow, mapCntPairRenAttr, MIN_DUPP_MERGE);

				if(counter.IsErr())
					return false;

				CountPair::DumpLog(mapCntPairRenAttr);

				// 최대중복수를 가진 AttrMergeInfo를 골라냄
				AttrMaxDuppAMIFinder finder(mapCntPairRenAttr, &passAMI, MIN_DUPP_MERGE);

				if(!finder.m_pairExist)
				{
					if(rst) {
						for(AttrRowMap::iterator iter = mapRenAttrRow.begin();
							iter != mapRenAttrRow.end(); iter++)
						{
							rst->AddNonOptiNode((i3Node*)((*iter).first));
						}
					}

					bCont = false;
					break;
				}

				if(!finder.m_rst)
				{
					// 현재 찾을려는 항목이 없는 항목이 더이상없음 다음패스로 진행
					doNextPass = true;
					break;
				}

				passAMI.push_back(finder.m_rst); // 다음번검색때 제외

				// maxAMI를 가지지않는 Row를 다른패스때 사용할곳으로 복사하면서 제거
				AttrSetRowRemoverAndCopyer(mapRenAttrRow, finder.m_rst, nextPassRow);

				i3RenAttrMergeInfoGen::DumpLog(mapRenAttrRow);

			} while(bCont);

			if(doNextPass)
			{
				// 현재까지 남은 i3AttrSet들이 새로이 생성할 i3AttrSet에 들어간다.
				I3ASSERT(mapRenAttrRow.size() > 1);
				GenerateNewAttrSet(m_rstData, mapRenAttrRow);

				// 다음패스 준비작업
				mapRenAttrRow = nextPassRow;

				if(mapRenAttrRow.empty())
				{
					bCont = false;
					break;
				}
			}			

		} while(bCont);

	}

	return true;
}

i3RenAttrMergeInfoGen::~i3RenAttrMergeInfoGen()
{
}

void i3RenAttrMergeInfoGen::AddMergeInfo(i3AttrSet* ownerAttr, i3RenderAttr* attr, 
	i3Node* upperChild)
{	
	i3::class_meta* attrMeta = attr->meta();

	if(!CheckMergeableRenAttr(attrMeta))
		return;
	
	Info info;
	info.m_key = attrMeta;
	info.m_srcAttr = attr;
	info.m_owner = ownerAttr;
	info.m_upperChild = upperChild;

	AttrMetaClsMMap::value_type val(attrMeta, info);
	m_mmapInfo.insert(val);
}

void i3RenAttrMergeInfoGen::GenerateNewAttrSet(GenAttrSetData& out, AttrRowMap& in)
{
	CntPairInfoUOMap mapCntPairRenAttr;
	CountPair counter(in, mapCntPairRenAttr);

	if(mapCntPairRenAttr.empty())  // 중복되는 항목이없다. 입력데이터 이상
	{		
		assert(0);
		return;
	}

	if(!CountPair::CheckUniqueRenAttrMetaRow(mapCntPairRenAttr))
	{
		// 같은 RenAttr 타입이 2개이상있는지 체크
		assert(0);
		return;
	}

	AttrMaxDuppAMIFinder finder(mapCntPairRenAttr);
	INT32 numMaxDupp = finder.GetMaxDupplicated(); // 최대 중복수
	if(numMaxDupp == -1)
	{
		assert(0);
		return;
	}
	
	// 현재 Row갯수만큼의 항목을 가지는 RenAttr만 모으기
	CountPair::CutRowByDuppNum(mapCntPairRenAttr, numMaxDupp);
	
	GenAttrSetData::AttrSetMap asetMap;
	for(AttrRowMap::iterator x=in.begin(); x != in.end(); x++)
	{
		const i3AttrSet* attrSet = (*x).first;

		InfoVec& src = (*x).second;
		InfoVec infoVec;

		for(size_t i=0; i<src.size(); i++) 
		{
			CntPairInfoUOMap::iterator findIter 
				= mapCntPairRenAttr.find(i3RenAttrMergeInfoGen::InfoGrpKey(src[i]));

			if(findIter != mapCntPairRenAttr.end())
				infoVec.push_back(src[i]);
		}

		GenAttrSetData::AttrSetMap::value_type val(const_cast<i3AttrSet*>(attrSet), infoVec);
		asetMap.insert(val);
	}

	out.m_data.push_back(asetMap);
	i3RenAttrMergeInfoGen::DumpLog(out);
}

void i3RenAttrMergeInfoGen::DumpLog(AttrRowMap& in)
{
#ifdef _USE_I3RENATTROPTIMIZER_LOG_
	OutputDebugStringA("\n== AttrRowMap List Start ==\n");

	for(AttrRowMap::iterator x=in.begin(); x != in.end(); x++)
	{
		const i3AttrSet* attrSet = (*x).first;
		InfoVec& src = (*x).second;

		for(i3RenAttrMergeInfoGen::InfoVec::iterator x = src.begin(); x != src.end(); x++)
		{
			INT32 cid = (*x)->m_srcAttr->GetID();

			char buf[256];
			sprintf(buf, "[%03d],", cid);
			OutputDebugStringA(buf);
		}
		OutputDebugStringA("\n===========================\n");
	}

	OutputDebugStringA("\n== AttrRowMap List End ==\n");
#endif
}

void i3RenAttrMergeInfoGen::DumpLog(GenAttrSetData& in)
{
#ifdef _USE_I3RENATTROPTIMIZER_LOG_
	size_t numRow = in.m_data.size();

	char buf[MAX_PATH] = {0};
	OutputDebugStringA("\n====== DumpLog GenAttrSetData Start ======== \n");

	for(size_t row = 0; row<numRow; row++)
	{
			sprintf_s(buf, MAX_PATH, "Row[%02d]\n", row);
			OutputDebugStringA(buf);

		GenAttrSetData::AttrSetMap& curMap = in.m_data[row];
		for(GenAttrSetData::AttrSetMap::iterator x = curMap.begin();
			x != curMap.end(); x++)
		{	
			InfoVec& curVec = (*x).second;

			for(size_t k=0; k<curVec.size(); k++)
			{
				INT32 cid = curVec[k]->m_srcAttr->GetID();

				sprintf(buf, "[%03d][%s],", cid, curVec[k]->m_owner->GetName());
				OutputDebugStringA(buf);
			}
			OutputDebugStringA("\n");
		}
		OutputDebugStringA("\n");
	}

	OutputDebugString("\n====== DumpLog GenAttrSetData End ======== \n");
#endif
}

void i3RenAttrMergeInfoGen::GenerateAttrSetPair(PairAttrSet* out, i3OptRenAttrMergerRst* rst,
	bool removeRenAttrFromOriginal)
{
	typedef std::set<i3Node*> AdjustChildSet;
	AdjustChildSet adjustChild;

	typedef GenAttrSetData::AttrSetMap	GenASMap;
	size_t numCreateNewAttrSet = GetRstData().m_data.size();
	
	for(size_t i=0; i<numCreateNewAttrSet; i++)
	{		
		GenASMap& curMap = GetRstData().m_data[i];
		GenASMap::iterator xIter = curMap.begin();

		adjustChild.clear();

		// 신규 i3AttrSet생성
		i3AttrSet* newAttrSet = i3AttrSet::new_object_ref(); // ref=0
		bool firstInsert = true;
			
		while(xIter != curMap.end())
		{
			i3RenAttrMergeInfoGen::InfoVec& itemVec = (*xIter).second;
			I3ASSERT(!itemVec.empty());
				
			// 중복되는 i3RenderAttr을 복제해서 신규 i3AttrSet에 넣어주고 
			// 원본은 부모i3AttrSet에서 제거(removeRenAttrFromOriginal=true)
			for(size_t k=0; k<itemVec.size(); k++)
			{
				i3RenAttrMergeInfoGen::Info* curAMI = itemVec[k];
				
				if(firstInsert)
				{
					i3RenderAttr* clonedRenAttr = CloneI3ClassFromInstance(curAMI->m_srcAttr);
					newAttrSet->AddAttr(clonedRenAttr);
				}

				if(removeRenAttrFromOriginal)
					curAMI->m_owner->RemoveAttr(curAMI->m_srcAttr); // 이후 renAttr이 없는 빈 i3AttrSet이 발생할수있다.

				// bindAttr
				{
					i3RenAttrMergeInfoGen::Info::BindAttrMap& bindAttr = curAMI->m_bindAttr;
					if(!bindAttr.empty())
					{
						i3RenAttrMergeInfoGen::Info::BindAttrMap::iterator bindIter = bindAttr.begin();
						while(bindIter != bindAttr.end())
						{
							i3RenderAttr* bindRenAttr = (*bindIter).second;
							if(bindRenAttr) // nullptr일수있음
							{
								if(firstInsert)
								{
									i3RenderAttr* clonedBindRenAttr = CloneI3ClassFromInstance(bindRenAttr);
									I3ASSERT(clonedBindRenAttr);
									newAttrSet->AddAttr(clonedBindRenAttr);
								}

								if(removeRenAttrFromOriginal)
									curAMI->m_owner->RemoveAttr(bindRenAttr);
							}

							bindIter++;
						}
					}
				}

				// 자식노드 재조정할것들
				if(curAMI->m_upperChild)
					adjustChild.insert(curAMI->m_upperChild);

				if(rst)
					rst->AddOptiNode(curAMI->m_owner);

			} // end of for

			firstInsert = false;
			xIter++;

		} // end of while

		I3_MUST_ADDREF(newAttrSet); // ref+1

		std::vector<i3Node*> adjChildVec;

		if(!adjustChild.empty())
			std::copy(adjustChild.begin(), adjustChild.end(), std::back_inserter(adjChildVec));

		PairAttrSet::PairMap::value_type val(newAttrSet, adjChildVec);
		out->m_attrSet.insert(val);

	} // end of for

}

//
// i3RenAttrMergeInfoGen::PairAttrSet
//
i3RenAttrMergeInfoGen::PairAttrSet::~PairAttrSet()
{
	Clear(true);
}

void i3RenAttrMergeInfoGen::PairAttrSet::Clear(bool releaseRef)
{
	if(m_attrSet.empty())
		return;

	if(releaseRef)
	{
		for(i3RenAttrMergeInfoGen::PairAttrSet::PairMap::iterator x = m_attrSet.begin(); 
			x != m_attrSet.end(); x++)
		{
			i3AttrSet* p = const_cast<i3AttrSet*>((*x).first);
			I3_MUST_RELEASE(p);
		}
	}

	m_attrSet.clear();
}


//
// i3RenAttrMergeInfoGen::Info
//

bool i3RenAttrMergeInfoGen::Info::EqualBindAttr(const BindAttrMap& l, const BindAttrMap& r)
{
	BindAttrMap::const_iterator leftIter = l.begin();
	
	for(;leftIter != l.end(); leftIter++)
	{
		i3::class_meta* clsm = (*leftIter).first;
		i3RenderAttr* renALeft = (*leftIter).second;

		BindAttrMap::const_iterator rightIter = r.find(clsm);

		if(rightIter == r.end())
			return false;

		i3RenderAttr* renARight = (*rightIter).second;

		if( (!renALeft && renARight) || (renALeft && !renARight) )
			return false;

		if(renALeft && renARight)
		{
			if(!renALeft->IsSame(renARight))
				return false;
		}
		else
		{
			I3ASSERT(!renALeft && !renARight);
		}
	}
	
	return true;
}

bool i3RenAttrMergeInfoGen::Info::operator ==(const Info& r) const
{
	if((m_key != r.m_key)) 
		return false;

	//i3MaterialAttr의경우 거의같은항목이라도  컬러의 소수점비교떄 
	//약간의차로 다르게판정된다.

	if(!m_srcAttr->IsSame(r.m_srcAttr))
		return false;

	if(!EqualBindAttr(m_bindAttr, r.m_bindAttr))
		return false;

	return true;
}


i3RenAttrMergeInfoGen::Info::Info(const Info& r)
{
	(*this) = r;
}

i3RenAttrMergeInfoGen::Info& i3RenAttrMergeInfoGen::Info::operator =(const Info& r)
{
	if(this == &r)
		return *this;

	m_key = r.m_key;
	m_srcAttr = r.m_srcAttr;
	m_bindAttr = r.m_bindAttr;
	m_owner = r.m_owner;
	m_upperChild = r.m_upperChild;

	return *this;
}


//
// i3RenAttrMergeInfoGen::InfoGrpKey
//

i3RenAttrMergeInfoGen::InfoGrpKey::InfoGrpKey(i3RenAttrMergeInfoGen::Info* key)
	: m_key(key)
{	
}

i3RenAttrMergeInfoGen::InfoGrpKey::InfoGrpKey(const i3RenAttrMergeInfoGen::InfoGrpKey& r)
	: m_key(nullptr)
{
	*this = r;
}

i3RenAttrMergeInfoGen::InfoGrpKey& i3RenAttrMergeInfoGen::
InfoGrpKey::operator =(const i3RenAttrMergeInfoGen::InfoGrpKey& r)
{
	if(this == &r)
		return *this;

	m_key = r.m_key;

	return *this;
}

bool i3RenAttrMergeInfoGen::InfoGrpKey::operator ==(
	const i3RenAttrMergeInfoGen::InfoGrpKey& r) const
{
	return *m_key == *(r.m_key);
}


//
// i3OptRenAttrMerger
//

i3OptRenAttrMerger::i3OptRenAttrMerger()
{
}

i3OptRenAttrMerger::~i3OptRenAttrMerger()
{
}

bool i3OptRenAttrMerger::Merge(i3Node* upper, AttrSetIn* in, i3OptRenAttrMergerRst* rst)
{
	bool isI3AttrSetUpper = false;

	if(i3::kind_of<i3AttrSet*>(upper))
		isI3AttrSetUpper = true;

	I3ASSERT(in->size() == 2);

	// 주어진 i3AttrSet들에게서 중복되는 i3RenderAttr들을 검색해 병합에 필요한 정보를 만든다.
	i3RenAttrMergeInfoGen generator;
	
	if(!generator.Do(upper, *in, rst))
		return false;

	i3RenAttrMergeInfoGen::PairAttrSet pairSet;

	// 생성된 정보에따라 신규로 생성할 중복된 i3RenderAttrSet을 넣어진 i3AttrSet목록을 만든다.
	// 이때 병합에 참여한 i3AttrSet에서 중복된 i3RenderAttr을 삭제한다.
	generator.GenerateAttrSetPair(&pairSet, rst, true);

	typedef i3RenAttrMergeInfoGen::PairAttrSet::PairMap		PairMap;
	typedef std::vector<i3Node*> ChildVec;

	if(pairSet.m_attrSet.empty())
	{
		// 중복노드가없습니다. 옵티마이즈처리후 리턴
		if(rst)
			rst->m_status = 1;

		return true;
	}

	I3ASSERT(pairSet.m_attrSet.size() == 1);
	
	PairMap::iterator x = pairSet.m_attrSet.begin();
	i3AttrSet* newAttrSet = i3AttrSet::new_object_ref();

	i3AttrSet* generatedAttrSet = (*x).first;
	generatedAttrSet->CopyTo(newAttrSet, I3_COPY_INSTANCE);

	std::vector<i3Node*> oriUpperChild;	
	for(INT32 i=0; i<upper->GetChildCount(); i++)
		oriUpperChild.push_back(upper->GetChild(i));

	ChildParentRelationShipChecker<i3Node>()(upper);

	ChildNodeRefStore refStore;
	refStore.AddRef(upper);  // upper의 차일드노드들에 대해 Ref+1

	upper->RemoveAllChild();

	std::vector<i3Node*>& childs = (*x).second;

	// upper의 자식노드순서대로 삽입
	bool firstNewAttrSetInsert = false;

	for(INT32 i=0; i<(INT32)oriUpperChild.size(); i++)
	{
		i3Node* c = oriUpperChild[i];

		if(c == nullptr)
			continue;

		INT32 foundIndex = -1;
		for(INT32 k=0; k<(INT32)childs.size(); k++)
		{
			if(c == childs[k])
			{
				if(!firstNewAttrSetInsert)
				{
					oriUpperChild[i] = newAttrSet; // 최초발견된 자식노드 위치에 넣는다.
					firstNewAttrSetInsert = true;
				}
				else
				{
					oriUpperChild[i] = nullptr; // 현재 upper의 차일드가 신규AttrSet에 자식으로 들어간다.
				}

				foundIndex = k;
				break;
			}
		}

		if(foundIndex == -1)
			continue;

		newAttrSet->AddChild(childs[foundIndex]);
	}

	I3ASSERT(firstNewAttrSetInsert);
	I3ASSERT((INT32)childs.size() == newAttrSet->GetChildCount());

	// 위에과정에서 newAttrSet으로 옮겨간 child를 제거처리
	std::vector<i3Node*> oriUpperChildTemp;
	for(INT32 i=0; i<(INT32)oriUpperChild.size(); i++)
	{
		if(oriUpperChild[i])
			oriUpperChildTemp.push_back(oriUpperChild[i]);
	}

	oriUpperChild = oriUpperChildTemp;

	ChildParentRelationShipChecker<i3Node>()(upper);

	for(INT32 i=0; i<(INT32)oriUpperChild.size(); i++)
		upper->AddChild(oriUpperChild[i]);

	refStore.Release();

	ChildParentRelationShipChecker<i3Node>()(upper);

	rst->AddNewlyCreatedNode(newAttrSet);

	if(rst)
		rst->m_status = 1;

	return true;
}
