#if !defined( __SG_BUILDER_H)
#define __SG_BUILDER_H

#include "i3Export_Define.h"
#include "MeshInfo.h"

class i3Export;

struct SKELETON_INFO
{
	INode *			m_pBoneRoot = nullptr;
	i3Body *		m_pBody = nullptr;
	i3Skeleton *	m_pSkel = nullptr;
} ;

// Node에 대해 어떤 처리를 해야하는지 나타내는 Bitflag
#define NPT_MESH			0x00000001			// 해당 Node는 Mesh를 포함하고 있다.
#define NPT_LIGHT			0x00000002			// 해당 Node는 Light로 처리되어야 한다.
#define NPT_CAMERA			0x00000004			// 해당 Node는 Camera로 처리되어야 한다.
#define NPT_PHYSIX			0x00000008			// 해당 Node는 Physix로 처리되어야 한다.

struct JOB_INFO
{
	bool				m_bProcessed = false;
	UINT32				m_NPT = 0;
	INode *				m_pINode = nullptr;

	// Output
	i3PhysixShapeSet *	m_pShapeSet = nullptr;			// Joint 처리 속도를 올리기 위한 임시 변수
	i3Node *			m_pPhysixNode = nullptr;			// Joint 처리 속도를 올리기 위한 임시 변수
	INT32				m_idxBone = 0;				// Joint 처리 속도를 올리기 위한 임시 변수
} ;

class BONE_INFO : public i3TreeNode
{
	I3_CLASS_DEFINE( BONE_INFO, i3TreeNode);
public:
	INode *			m_pINode = nullptr;
	bool			m_bBone = false;
	bool			m_bSkinned = false;
	bool			m_bPhysix = false;
	i3Node *		m_pNode = nullptr;
	
	INode *			m_pSkinMesh = nullptr;

	void			dump( INT32 level = 0)
	{
		INT32 i;

		for( i = 0; i < level; i++)
			I3TRACE( "\t");

		if( m_pINode != NULL)
			I3TRACE( "[%s", m_pINode->GetName());

		if( m_bSkinned)
			I3TRACE( ":Skin]\n");
		else
			I3TRACE( "]\n");

		for( i = 0; i < getChildCount(); i++)
		{
			BONE_INFO * pChild = (BONE_INFO *) getChild( i);

			pChild->dump( level + 1);
		}
	}

};

enum BCR_TYPE
{
	BCR_NO_BONE = 0,
	BCR_BONE,
	BCR_PHSIX_BONE,
	BCR_SKIN_BONE,
	BCR_BILLBOARD_BONE,
};

enum PRGSS
{
	PRGSS_CLASSIFY_NODE = 0,
	PRGSS_BUILD_BONE_HIER,
	PRGSS_BUILD_FRAME_SG,
	PRGSS_PROCESS_JOB,
	PRGSS_EXTRACT_ANIM,

	PRGSS_MAX
};

struct SCAN_NODE_INFO
{
	INode *		m_pINode = nullptr;
	BCR_TYPE	m_BoneType = BCR_NO_BONE;
} ;

template<> struct i3::less<SCAN_NODE_INFO*> 
{
	bool operator()( const SCAN_NODE_INFO* p1, const SCAN_NODE_INFO* p2) const {  return p1->m_pINode < p2->m_pINode; }
	bool operator()( const SCAN_NODE_INFO* p1, INode* p2) const { return p1->m_pINode < p2; }
	bool operator()( INode* p1, const SCAN_NODE_INFO* p2 ) const { return p1 < p2->m_pINode; }
};

template<> struct i3::less<BONE_INFO*>
{
	bool operator()( const BONE_INFO* p1, const BONE_INFO* p2) const { return p1->m_pINode < p2->m_pINode; }
	bool operator()(  const BONE_INFO* p1,  INode* p2) const { return p1->m_pINode < p2; }
	bool operator()( INode* p1, const BONE_INFO* p2 ) const { return p1 < p2->m_pINode; }
};

class SgBuilder
{
protected:
	INode *				m_pIRoot;
	i3Export *			m_pExport;

	i3::vector_set<SCAN_NODE_INFO*>	m_ScanNodeList;

	i3::vector<JOB_INFO*>			m_JobList;
	i3::vector_set<	BONE_INFO*>		m_BoneList;
	i3::vector<SKELETON_INFO*>		m_SkelList;

	i3Transform *		m_pRoot;

	BONE_INFO *			m_pBoneRoot;

	i3tool::exporter::SKIN_VERTEX *	m_pWeights;
	INT32				m_WeightCount;

protected:
	// Job
	JOB_INFO	*	addJob( INode * pINode, UINT32 npt);
	INT32			getJobCount(void)							{ return (INT32)m_JobList.size(); }
	JOB_INFO *		getJob( INT32 idx)							{ return m_JobList[idx]; }
	void			removeAllJobs(void);
	void			ProcessJobs(void);
	void			dumpJob(void);
	void			SearchJobByPhysixProxy( const char * pszProxy, i3::vector<JOB_INFO*>& List);

protected:
	// Scanned Node List
	SCAN_NODE_INFO *	FindScanNode( INode * pNode);
	void				RemoveAllScanNode(void);

protected:
	// Bone Info
	INT32			getBoneCount(void)							{ return (INT32)m_BoneList.size(); }
	BONE_INFO *		getBone( INT32 idx)							{ return m_BoneList.get_vector()[idx]; }
	BONE_INFO *		addBone( INode * pINode, bool bSkinned, INode * pSkinMesh = NULL);
	INT32			findBoneByINode( INode * pINode);
	INT32			findBoneByName( const char * pszName);
	void			findBoneBySkinMesh( INode * pIMeshNode, i3::vector<BONE_INFO*>& List);
	void			removeAllBones(void);
	void			dumpBone(void);
	void			RemoveActorDesc(NxActorDesc& desc);

	// Bone List를 돌면서 발견된 Skin Mesh를 검색한다.
	void			GatherSkinMesh( BONE_INFO * pNode, i3::vector<INode*>& List);
	BONE_INFO *		FindSharedMostParent(const i3::vector<BONE_INFO*>& BoneList);

protected:
	// Skeleton Info
	INT32			getSkelCount(void)							{ return (INT32)m_SkelList.size(); }
	SKELETON_INFO *	getSkel( INT32 idx)							{ return m_SkelList[idx]; }
	void			removeAllSkeleton(void);

public:
	static bool		isTargetToExport( INode * pINode, i3Export* pExporter);

protected:	
	void			ClassifyNode( INode * pINode);

protected:
	// Progress
	INT32			m_ProgressPhase;
	INT32			m_ProgressCur;
	INT32			m_ProgressMax;
	char			m_szProgressText[ 512];

	void			ResetProgress(void);
	void			SetProgress( PRGSS phase, INT32 max);
	void			Progress(void);

protected:
	// Util
	i3Node *		GetAttachPoint( INode * pINode, i3AttrSet * pHighAttrSet, INode * pSkinRoot);
	i3BoneRef *		FindBoneRef( i3Body * pBody, INT32 idxBone);
	void			GatherBoneHier( BONE_INFO * pBone, i3::vector<BONE_INFO *>& List);

	void			CheckSameName(void);
	void			_Rec_CheckSameName( INode * pNode, i3::vector<UINT32>& List);

	void			SortNode(void);


protected:
	void			ScanUsedBone( INode * pINode, bool bScanVertex = true, INT32 depth = 0);
	void			ScanUsedBone_Skin( INode * pINode, Modifier * pMod);
	void			ScanUsedBone_Physique( INode * pINode, Modifier * pMod);
	BCR_TYPE		getBoneTreatType( INode * pINode);

	BONE_INFO *		FindParentBone( INode * pBone, bool bDump = false);
	void			BuildBoneHierarchy(void);
	void			_Rec_SearchBoneHierarchy( INode * pINode, INT32 Depth, INT32 SkinDepth);
	i3Body *		BuildSkeleton( BONE_INFO * pInfo);
	i3Body *		FindSkeleton( BONE_INFO * pInfo);
	INT32			_Rec_BuildSkeleton( BONE_INFO * pInfo, i3Skeleton * pSkel, INT32 cnt, INT32 depth);
	void			ATTACH_NODE( i3Node * pNode, INode * pINode, bool bGeometry = false);
	void			_Rec_BuildFrameSg( BONE_INFO * pInfo, BONE_INFO * pParentInfo);

	void			_OnJobMesh( JOB_INFO * pJob);
	void			_OnJobLight( JOB_INFO * pJob);
	void			_OnJobCamera( JOB_INFO * pJob);
	void			_OnJobPhysix( JOB_INFO * pJob);
	void			_OnJobJoint( JOB_INFO * pJob);

	void			ExtractAnimation(void);

	//////////////////////////////////
	// Geometry 관련
	protected:
		i3::vector<CMeshInfo*>	m_MeshList;

		INode *			_ExtractWeight_Skin( INode * pNode, Modifier * pMod);
		INode *			_ExtractWeight_Physique( INode * pNode, Modifier * pMod);
		i3GeometryAttr * _ExtractGeometry( INode * pNode, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount, bool bTangent);

		void			_RemoveAllMesh(void);
		INT32			_FindMesh( INode * pNode, Mesh * pMesh);
		CMeshInfo *		_RegisterMesh( INode * pNode, Mesh * pMesh, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount, bool bTangent);

		void			_DumpMeshes(void);

	public:
		INT32			getMeshCount( void)					{ return (INT32)m_MeshList.size(); }
		CMeshInfo *		getMesh( INT32 idx)					{ return m_MeshList[idx]; }


public:
	SgBuilder(void);
	~SgBuilder(void);

	void		BuildFrameSg(void);

	i3Node *	getRoot(void)						{ return m_pRoot; }

	void		Export( INode * pNode, i3Export * pExp);
};

#endif