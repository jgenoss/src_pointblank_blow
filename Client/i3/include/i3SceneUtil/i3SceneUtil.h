#if !defined( __I3_SCENE_UTILITY_H)
#define __I3_SCENE_UTILITY_H

#include "i3Scene.h"
#include <hash_map> //stdext::hash_map

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3_SCENEUTIL_BUILD)
		#define I3_EXPORT_SGUT			__declspec( dllexport)
		#define I3_EXTERN_SGUT			extern
	#else
		#define I3_EXPORT_SGUT			__declspec( dllimport)
		#define I3_EXTERN_SGUT			__declspec( dllimport)
	#endif
#else
	#define I3_EXPORT_SGUT
	#define I3_EXTERN_SGUT				extern
#endif

#if defined( I3_PHYSX)
struct I3_VISIBLE_COLL_MESH_INFO
{
	i3PhysixShapeSet *	m_pShapeSet = nullptr;		// Out
	i3Geometry *		m_pGeo = nullptr;				// Out
	i3GeometryAttr *	m_pGeoAttr = nullptr;			// Out
} ;
#endif

namespace i3SceneUtil
{
	I3_EXPORT_SGUT i3Node *	BuildLine( i3VertexArray *&pVetexArray, INT32 nLineCountVEC3D, COLORREAL *pColor = nullptr );
	I3_EXPORT_SGUT i3Node *	BuildSphereMesh( REAL32 radius = 1.0f, bool bTri = true, bool bLine = true, UINT32 horzdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbientColor = nullptr, COLORREAL * pDiffuse = nullptr);
	I3_EXPORT_SGUT i3Node *	BuildAxisMesh( INT32 hdiv = 20, REAL32 Length = 1.0f, REAL32 rad = 0.05f, bool bTri = true, bool bLine = true, COLORREAL * pAxisColor = nullptr, COLORREAL * pConeColor = nullptr);
	I3_EXPORT_SGUT i3Node *	BuildBoxMesh( REAL32 size = 1.0f, bool bTri = true, bool bLine = true, COLORREAL * pTriColor = nullptr, COLORREAL * pLineColor = nullptr);
	I3_EXPORT_SGUT i3Node * BuildCapsuleMesh( REAL32 fRadius, REAL32 fLength, bool bTri = true, bool bLine = true, UINT32 horizdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbient = nullptr, COLORREAL * pDiffuse = nullptr);
	I3_EXPORT_SGUT i3Node * BuildHemiSphereMesh(REAL32 fRadius, bool bTri = true, bool bLine = true, bool isUpper = true, UINT32 horizdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbient = nullptr, COLORREAL * pDiffuse = nullptr);
	I3_EXPORT_SGUT i3Node * BuildCylinderMesh(REAL32 fRadius, REAL32 fLength, bool bTri = true, bool bLine = true, UINT32 horizdiv = 20, UINT32 vertdiv = 10, COLORREAL * plineColor = nullptr, COLORREAL * pAmbient = nullptr, COLORREAL * pDiffuse = nullptr);
	I3_EXPORT_SGUT i3Node *	BuildBoneMesh( REAL32 Length, REAL32 Size, bool bLine = false);
	I3_EXPORT_SGUT i3Node *	BuildSelectBoxMesh( COLOR * pColor);
	I3_EXPORT_SGUT i3Node * BuildArc( REAL32 fRadius, REAL32 fAngle, INT32 divcount = 5, bool bTri = true, bool bLine = true, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbient = nullptr, COLORREAL * pDiffuse = nullptr, MATRIX * pMtx = nullptr  );
	I3_EXPORT_SGUT i3Node * BuildTriangle( VEC3D * pV1, VEC3D * pV2, VEC3D * pV3, bool bTri = true, bool bLine = true, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbient = nullptr, COLORREAL * pDiffuse = nullptr );

	I3_EXPORT_SGUT i3Node *	RemoveNodeByFlag( i3Node * pNode, UINT32 mask);

	// Collision
	struct I3_EXPORT_SGUT BuildLogger
	{
	public:
		typedef std::multimap<std::string, std::string> LogRow; // key=dst, val=desc
		typedef std::map<std::string, LogRow >  LogMap; // key=log section

		enum ELOGFLAG
		{
			ELF_INC_PHYSHAPE_GEN		= 0x00000001,
			ELF_INC_PHYSHAPE_MISS		= 0x00000002,			

			ELF_INC_SKIP_NODE			= 0x00000010,
			ELF_INC_SKIP_ALL			= 0x00000020,

			ELF_MAX
		};

		BuildLogger(DWORD logFlag=ELF_INC_PHYSHAPE_GEN | ELF_INC_PHYSHAPE_MISS);
		~BuildLogger() {}

		void Reset();

		void SetSection(const char* fmt, ...);
		void Log(DWORD flag, const char* reason, const char* dst, const char* fmt, ...);
		void Save(const char* path);
		void ShowMsgBox(HWND hWnd);

		typedef std::vector<std::string> MsgStatus;

		INT32 GenerateStatusMsg(BuildLogger::MsgStatus* rst, DWORD flag=ELF_INC_PHYSHAPE_MISS);

		LogMap& GetLog() { return m_log; }

		INT32 GetNumGenColliders() { return m_numGenColliders; }
		INT32 GetNumGenFailColliders() { return m_numGenFailColliders; }

	protected:		
		LogMap							m_log;
		char							m_curLogSection[MAX_PATH] = { 0 };
		DWORD							m_flag = 0;

		INT32							m_numGenColliders = 0;
		INT32							m_numGenFailColliders = 0;
	};

	struct I3_EXPORT_SGUT RenColliderColorTable
	{
	public:
		enum ECATE
		{
			ECATE_DEFAULT,
			ECATE_HIT,
			ECATE_SM,
			ECATE_MAX,			
			ECATE_CUR = -1,
		};

		RenColliderColorTable(const char* iniPath=nullptr);
		~RenColliderColorTable();

		bool Reload(const char* path);
		void SetColorCate(ECATE cate) 
			{ if(cate != ECATE_CUR) m_cate = cate; }

		// [ECATE_HIT] prefix = MET_
		const I3COLOR& GetColorByNodePrefix(const char* prefix, ECATE cate=ECATE_CUR);

		struct Data
		{
			std::string						m_nodePrefix;
			I3COLOR							m_clr;
		};

		void SetDefault();
		static void GenerateDefaultIni(const char* path=nullptr);

	private:
		Data* GetByNodePrefix(const char* prefix, ECATE cate);
		bool ParseSectionRow_NameColor(char* rstBuf, I3COLOR& rstClr, const char* src);
		char* ToUpperStr(const char* in);

		typedef std::vector<Data>	TerTypeDataVec;		// index=codeName(I3_TERRAIN_TYPE)
		typedef stdext::hash_map<const char*, Data*>	PrefixColorHMap; // key=prefix(TH_CCR)

		TerTypeDataVec						m_data[ECATE_MAX];
		PrefixColorHMap						m_hmapPrefixColor[ECATE_MAX];
		I3COLOR								m_defaultClr[ECATE_MAX];

		char								m_upperBuf[MAX_PATH] = { 0 };
		ECATE								m_cate;
		char								m_iniFileName[MAX_PATH] = { 0 };

		static I3COLOR						ms_default_HitColor[I3_TERRAIN_TYPE_COUNT];		
	};

	I3_EXPORT_SGUT i3Node *	BuildRendableCollider( i3Node * pSrc, i3Node* pParent, bool bLine, bool bNormal, 
		bool bAutoAttr = true, i3::vector<I3_VISIBLE_COLL_MESH_INFO*>* pCollMeshInfoList = nullptr, 
		COLOR * pCol = nullptr, bool bRemoveVolatile = true, BuildLogger* logger=nullptr, INT32 testOnly=0, 
		RenColliderColorTable* colorTable=nullptr);

	I3_EXPORT_SGUT i3Node *	BuildCollideeMesh( i3Collidee * pColl, bool bSurface, bool bLine, bool bNormal, bool bColor = false);
	I3_EXPORT_SGUT i3Node * BuildVisibleBound( i3Node * pSrc);
	I3_EXPORT_SGUT i3Node *	BuildInstanceMesh( i3Node * pSrc, bool bLine, UINT8 alpha = 128);
	I3_EXPORT_SGUT i3Node *	BuildWireMesh( i3Node * pSrc, COLOR * pCol = nullptr);

	// Navigation Mesh
	I3_EXPORT_SGUT i3Node *	BuildRendableNavigationMesh( i3NavigationMesh * pSrc, bool bLine, bool bNormal, REAL32 dY );
};

#endif
