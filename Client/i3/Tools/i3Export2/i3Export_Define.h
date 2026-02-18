#if !defined( __I3EXPORT_DEFINE_H)
#define __I3EXPORT_DEFINE_H

#define CAPSULE_CLASS_ID			Class_ID( 0x6D3D77AC, 0x79C939A9)
#define	PX_HULL_CLASS_ID			Class_ID( 0x00023A06, 0x0008967D)
#define PX_AGGREGATE_HULL_CLASS_ID	Class_ID( 0x00023A07, 0x0008967E)
#define PX_D6_JOINT_CLASS_ID		Class_ID( 0x5FA2E45D, 0x016E7A9E)
#define	REACTOR_RBCOLLECTION		Class_ID( 0x0455463F, 0x7BB62F5F)
#define	REACTOR_CONSTRAINT_SOLVER	Class_ID( 0x1DE12669, 0x684A07E9)
#define	REACTOR_HINGE_JOINT			Class_ID( 0x754274C6, 0x4B6750EB)
#define	UNDERHULL_CLASS_ID			Class_ID( 0x0013DD6C, 0x0013DC24)

namespace i3tool
{
	namespace exporter
	{
		struct MESH_GROUP_INFO
		{
			MtlBase *		m_pMtl = nullptr;
			i3::vector<INT32> m_FaceList;
		};

		struct TEXTURE_INFO
		{
			Bitmap *	m_pBitmap = nullptr;
			i3Texture * m_pTex = nullptr;
		};

#define		MAX_BLEND_MATRIX			4

		struct SKIN_VERTEX
		{
			INT32		m_WeightCount = 0;
			INode *		m_pBone[MAX_BLEND_MATRIX] = { nullptr, nullptr, nullptr, nullptr };
			REAL32		m_Weight[MAX_BLEND_MATRIX] = { 0.0f };
			Point3		m_Offset[MAX_BLEND_MATRIX] = { { 0.0f, 0.0f, 0.0f } };
		};

		struct SKIN_JOINT
		{
			INode *		m_pBoneNode = nullptr;
			INT32		m_Depth = 0;
		};

		struct SKELETON_INFO
		{
			i3Skin *		m_pSkin = nullptr;
			i3Skeleton *	m_pSkeleton = nullptr;
			INode *			m_pBoneRoot = nullptr;
		};

		struct TEXTURE_SET_INFO
		{
			Mtl *		m_pMtl = nullptr;
			Texmap *	m_pBitmap = nullptr;
			StdUVGen *	m_pUVGen = nullptr;
			Matrix3		m_Matrix;
		};

		typedef i3TBandBuffer< I3_PHYSIX_TERRAIN>		TTerrainList;
		typedef i3TBandBuffer< I3_PHYSIX_TRI>			TFaceList;

		enum POSTPROCESS_TYPE
		{
			POSTPROCESS_SHAPESET = 0,
			POSTPROCESS_JOINT,
		};

		struct POSTPROCESS_INFO : i3::class_common_pool<POSTPROCESS_INFO>		// 16바이트 정렬 경고문제로 정렬보장해주는 메모리풀로 대체...
		{
			bool					m_bProcessed = false;
			POSTPROCESS_TYPE		m_Type = POSTPROCESS_SHAPESET;
			INode *					m_pINode = nullptr;
			i3Node *				m_pNode = nullptr;
			MATRIX					m_LocalTransform;
			i3PhysixShapeSet *		m_pShapeSet = nullptr;
			INT32					m_Index = 0;
		};

		struct SHAPEDESC_INFO
		{
			TTerrainList *		m_pTerrainList = nullptr;
			NxShapeDesc *		m_pShapeDesc = nullptr;
		} ;

		enum ATTACHPOINT
		{
			ATTACH_TO_TRANSFORM,			// i3Transform Node의 Child로...
			ATTACH_TO_BONE,					// i3Skeleton의 Bone을 참조하는 경우...
		};
	}
}

#endif
