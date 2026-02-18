#if !defined( __I3_EXPORT_H)
#define __I3_EXPORT_H

#include "i3EXPPosData.h"
#include "i3EXPFrameMsgHandler.h"
#include "guplib.h"
#include "OptionInfo.h"
#include "i3ExportTracer.h"
#include "i3ExpProcessInfo.h"
#include "SgBuilder.h"
#include "i3Export_Define.h"

#include "i3OptiExpPlug.h"

class IOptiExpSettings;


class i3Export : public GUP, public OptiExpPlugCallBack
{
public:
	HWND					m_hwnd;
	HWND					m_hwndDialog;
	HWND					m_hwndLog;

	i3ExportPosData		*	m_pPosData;
	i3EXPFrameMsgHandler *	m_pMsgHandler;

	i3tool::exporter::pack::OPTION_INFO		m_Option;
	bool					m_enabledOptiExpPlug;

	HFONT					m_hFont;

	TimeValue				m_timeStart;
	TimeValue				m_timeEnd;

	INT32					m_CameraCount;
	INT32					m_LightCount;

	INODE_TRAVERSE_PROC		m_pUserProc;
	void *					m_pUserData;

	i3::unordered_set<INode*>	m_ExcludeNodeList;				// 유일성이 있다고 가정..
	i3::vector<i3::rc_string>	m_ExcludeAnimList;

	bool					m_bUpdateOption;
	bool					m_bResetAnimExcludeList;
	bool					m_bExcludeBiped;

	i3::vector<i3tool::exporter::SKELETON_INFO*> m_SkelList;

	i3::vector<i3ExpProcessInfo*> m_ProcessInfoList;
	i3::vector<i3tool::exporter::POSTPROCESS_INFO*>	m_PostProcessList;

	bool					m_bNoAddProcessInfo;
	bool					m_bNoCheckHide;

	// Physix
	i3PhysixContext *		m_pPhysixCtx;

	enum EPhysExpLog
	{
		EPHYSEXPLOG_CANBECONVEX_SKIP		= 0x00000001,
		EPHYSEXPLOG_CANBECONVEX_OK			= 0x00000002,
		EPHYSEXPLOG_CANBECONVEX_FAIL		= 0x00000004,
		EPHYSEXPLOG_CANBECONVEX_ALLFLAG		= 0x000000ff
	};
	DWORD					m_physExportLogFlag;

	bool					m_loggingToi3Export;

	SgBuilder	 *			m_pSgBuilder;

protected:
	
	// 전처리 1단계
	// 사용 Bone들을 검색하기 위해 임시로 사용됨.


protected:
	////////////////////////////////////////
	// Export Context
	i3Node *				m_pRoot;
	i3::vector<i3tool::exporter::TEXTURE_INFO*>	m_TexList;
	i3TStack<i3Node *>		m_NodeStack;
	bool					m_bVertexColor;
	bool					m_bTexMap;

	INT32					m_TotalNodeCount;
	INT32					m_ProcessedNodeCount;

	i3ExpProcessInfo *		m_pCurProcessInfo;


	// Joint Info. Table 관련
		i3::vector<i3tool::exporter::SKIN_JOINT*>	m_JointList;
		void				_AddJointInfo( INode * pBone, INT32 depth);
		void				_RemoveAllJointInfo(void);
		INT32				_getJointInfoCount()				{ return (INT32)m_JointList.size(); }
		i3tool::exporter::SKIN_JOINT *	_getJointInfo( INT32 idx)			{ return m_JointList[idx]; }
	//
	////////////////////////////////////////

	void			AddSkeletonInfo( i3Skin * pSkin, i3Skeleton * pSkel, INode * pBoneRoot);
	void			RemoveAllSkeletonInfo(void);
	i3tool::exporter::SKELETON_INFO *	FindSkeletonByBoneRoot( INode * pBoneRoot);

	i3ExpProcessInfo *	addProcessInfo( INode * pNode);
	INT32				getProcessInfoCount(void)				{ return (INT32)m_ProcessInfoList.size(); }
	i3ExpProcessInfo *	getProcessInfo( INT32 idx)				{ return m_ProcessInfoList[idx]; }
	void				removeAllProcessInfo(void);
	void				excludeNode( INode * pNode);
	i3ExpProcessInfo *	findProcessInfo( INode * pNode);

	void			_ResetExportContext(void);

	i3Node *		_getTopNode(void)			{ return * m_NodeStack.GetTop(); }

	i3GeometryAttr * _BuildGeometry( INode * pNode, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount = 0);
	void			_ResolvePhysique( INode * pNode, Modifier * pMod);
	void			_Rec_BuildJointInfo( INT32 depth, INode * pNode);
	INT32			_FindBoneByRef( INode * pNode);
	i3Skeleton *	_CreateSkeleton( INode * pNode, Modifier * pMod);

	void			_ResolveISkin( INode * pNode, Modifier * pMod);
	i3Skeleton *	_CreateSkinSkeleton( INode * pNode, Modifier * pMod);
	void			_ProcessBoneRef( INode * pSkinNode, i3Skin * pSkin, i3Skeleton * pSkel);

	void			_PlayAnim(void);

protected:	
	void			_OnFileSelect(void);
	void			_OnFileExport(void);
	void			_OnOption(void);
	void			_OnHelp(void);
	void			_OnOptiExport(void);
	void			_OnLogToggle(void);
	void			_OnMultiExport(void);
	void			_OnPopupMenu(void);

	void			_LoadOption(void);
	void			_SaveOption(void);
	bool			_Rec_Traverse( INT32 Level, INode * pRoot);

//	void			_BuildTimeStamp( ::Control * pCtrl, List * pList);			// 함수 정의가 없어서 주석..

	//	void			_Rec_ExtractTransformSeq( INode * pNode, List * pTransList, List * pIdxList);		// 리스트들이 쓰이지 않아 인수에서 뺀다..
	void			_Rec_ExtractTransformSeq( INode * pNode);											// 이 함수로 대체..(2012.05.28.수빈)

	NxBoxShapeDesc *			_BakeBoxShape( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, Matrix3 * pLocalMtx);
	NxConvexShapeDesc *			_BakeConvexMesh( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, Matrix3 * pLocalMtx);
	NxTriangleMeshShapeDesc *	_BakeTriMesh( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, i3tool::exporter::TFaceList * pFaceLists, i3PhysixShapeSet * pShapeSet, Matrix3 * pLocalMtx);
	bool						_ExtractGeom( INode * pNode, i3Vector3Array * pVA, i3::vector<INT32>& IA, i3tool::exporter::TTerrainList * pTerrainList, i3tool::exporter::TFaceList * pFaceLists, Matrix3 *);
	i3tool::exporter::SHAPEDESC_INFO *			_BuildShapeDesc( INode * pNode, Matrix3 * pLocalMtx, i3PhysixShapeSet * pShapeSet, bool bTriMeshEnable = false);

public:
	i3tool::exporter::TEXTURE_SET_INFO	m_TexSetInfo[8];

	i3::vector<i3tool::exporter::MESH_GROUP_INFO*>	m_MeshGroupList;

	// Skinning 관련 Context
	i3tool::exporter::SKIN_VERTEX	*	m_pWeights;
	INT32					m_WeightCount;
	INode *					m_pSkinBoneRoot;
	Matrix3					m_SkinInitTM;
	i3::vector<INode*>		m_BoneRefList;

	i3Animation *			m_pAnim;

public:

	i3Light *		CreateLightObject( INode * pINode);
	i3Camera *		CreateCamera( INode * pNode);

	INT32			getTotalNodeCount(void)						{ return m_TotalNodeCount; }

	void			_OnExportCommonNode( INode * pNode, ObjectState * pState);
	void			_OnExportTexture( char * pszName, Bitmap * pBitmap);
	bool			_OnExportMaterial( INode * pNode, MtlBase * pMtl, i3AttrSet * pAttrSet);
	bool			_OnExportTransform( INode * pNode, ObjectState * pState);
	void			_OnExportGeometry( INode * pNode, ObjectState * pState);
	void			_OnExportLight( INode * pNode, ObjectState * pState);
	void			_OnExportCamera( INode * pNode, ObjectState * pState);
	void			_OnExportShape( INode * pNode, ObjectState * pState);
	void			_OnExportHelper( INode * pNode, ObjectState * pState);
	void			_OnExportPhysics( i3tool::exporter::POSTPROCESS_INFO * pInfo);
	void			_OnExportJoint( i3tool::exporter::POSTPROCESS_INFO * pInfo);
	void			_OnExportUnknownNode( INode * pNode, ObjectState * pState);
	INT32			_OnExportTransformSeq( INode * pNode);
	void			_OnExportPhysiqueSkin( INode * pNode, Modifier * pMod, 
											Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, 
											bool bVertexColor, INT32 TexCoordCount);
	void			_OnExportSkin( INode * pNode, Modifier * pMod, 
											Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, 
											bool bVertexColor, INT32 TexCoordCount);
	void			_OnLuxMapMaterial( INode * pNode, i3AttrSet * pAttrSet, Mtl * pMtl);

	i3Node *		_ReplaceToPhysixNode( i3Node * pAttachNode, i3PhysixShapeSet * pShapeSet, int PhysixType, i3::string& sNodeName );

	i3Animation *	_ExtractAnimationFromBone(void);
	i3tool::exporter::SHAPEDESC_INFO * _CreateShapeDesc( INode * pNode, i3PhysixShapeSet * pShapeSet, Matrix3 * pBaseMtx, INT32 geometryType, REAL32 mass, REAL32 density);

	i3tool::exporter::POSTPROCESS_INFO *	_RegisterPostProcess( i3tool::exporter::POSTPROCESS_TYPE type, INode * pINode, i3Node * pNode, MATRIX * pLocalTrans, bool bToHead);
	INT32			_getPostProcessCount(void)					{ return (INT32)m_PostProcessList.size(); }
	i3tool::exporter::POSTPROCESS_INFO *	_getPostProcess( INT32 idx)		{ return m_PostProcessList[idx]; }
	void			_ResetPostProcess(void);
	void			_DoProcess( i3ExpProcessInfo * pInfo);
	void			_DoPostProcess(void);
	void			_FindPostProcessByProxy( const char * pszProxy, i3::vector<i3tool::exporter::POSTPROCESS_INFO*>& List);

	i3Node *		_FindAttachBoneByName( char * pszName);
	bool			_IsReferencedForProxy( INode * pNode);

	bool			_IsValidNode( INode * pNode);
	bool			_IsValidTransform( INode * pNode);
	bool			_IsPhysixNode( INode * pNode);

	void			LinkNode(void);

	void			_AddTriToMeshGroup( MtlBase * pMtl, INT32 idxFace);
	i3tool::exporter::MESH_GROUP_INFO *	_AddMeshGroup( MtlBase * pMtl);
	INT32			_FindMeshGroup( MtlBase * pMtl);
	void			_RemoveAllMeshGroup(void);
	void			_SaveMeshGroup( i3::vector<i3tool::exporter::MESH_GROUP_INFO *>& List);
	void			_RestoreMeshGroup( const i3::vector<i3tool::exporter::MESH_GROUP_INFO *>& List);
	Mtl *			_ResolveMeshGroup( INode * pNode, i3AttrSet * pAttrSet, Mtl * pMtl);

	void			_SetTextureSet( INT32 slot, Mtl * pMtl, int TexMapIndex);
	void			_AddTexture( Bitmap * pBitmap, i3Texture * pTex);
	i3Texture *		_FindTextureByBitmap(const char * pszName, Bitmap * pBitmap);
	void			_RemoveAllTexture(void);

	i3Texture * GetTextureSetting( INode * pNode, StdMat * std, int Channel, I3G_TEXTURE_WRAP * pWrapU, I3G_TEXTURE_WRAP * pWrapV);

public:
	i3Export(void);
	virtual ~i3Export(void);

	HWND			getHWND(void)					{ return m_hwnd; }
	HWND			getDialogHWND(void)				{ return m_hwndDialog; }
	CUIPosData *	getPosData(void)				{ return m_pPosData; }
	HFONT			getDefFont(void)				{ return m_hFont; }
	i3tool::exporter::pack::OPTION_INFO *	getOption(void)				{ return &m_Option; }

	TimeValue		getStartTime(void)				{ return m_timeStart; }
	TimeValue		getEndTime(void)				{ return m_timeEnd; }

	INT32			getNodeStackCount(void)			{ return m_NodeStack.GetCount(); }

	i3::vector<i3::rc_string>*	getExcludeAnimList(void)		{ return &m_ExcludeAnimList;  }

	void			UpdateOption( HWND hwnd, bool bToCtrl = true);
	void			OnCommand( UINT32 id);
	void			Traverse( INode * pRoot, INODE_TRAVERSE_PROC pUserProc, void * pUserData);
	bool			DoExport( char * pszPath, char * pszName, i3Node** outExportRootNode=NULL, bool doClearLog=true);
	void			DoExport2( char * pszPath, char * pszName);
	void			Reset(void);
	void			RefreshExportName(void);

	bool			getUpdateOptionEnable(void)			{ return m_bUpdateOption; }
	void			setUpdateOptionEnable( bool bFlag)	{ m_bUpdateOption = bFlag; }

	bool			getResetAnimExcludeListEnable(void)	{ return m_bResetAnimExcludeList; }
	void			setResetAnimExcludeListEnable( bool bFlag)	{ m_bResetAnimExcludeList = bFlag; }

	void			Log( INode * pNode, char * pszMsg);
	void			LogEx( INode * pNode, char* fmt, ...);
	void			ClearLog(void);

	static i3Texture *		_ConvertBitmap( Bitmap * pBitmap);
	i3Texture *		_CreateTexture( char * pszName, Bitmap * pBitmap);

	void			MatchMaterialNamesWithTexture(void);

	bool			CanBeConvex( INode * pNode, INT32* errVal=NULL);
	INT32			_Rec_CheckCollsionMesh( INode * pNode);
	bool			CheckCollisionMeshes(bool doClearLog=true);

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	//-- Plug-In Implementation Methods
	
	//-----------------------------------------------------------
	// Start() is called at boot time. If the plug-in
	// desires to remain loaded, it returns GUPRESULT_KEEP. If
	// it returns GUPRESULT_NOKEEP, it will be discarded. If it
	// returns GUPRESULT_ABORT MAX will be shut down.
	// #define GUPRESULT_KEEP		0x00
	// #define GUPRESULT_NOKEEP	0x01
	// #define GUPRESULT_ABORT		0x03
	virtual	DWORD			Start(void);
	
	//-------------------------------------------------
	// Stop is called prior to discarding the plug-in
	// for persistent plug-ins (i.e. only those that 
	// returned GUPRESULT_KEEP for Start() above).
	virtual	void			Stop(void);

	//-------------------------------------------------
	// Control is an entry point for external access to
	// GUP plug-ins. For instance, Utility plugins can
	// invoke their GUP plugin counterpart and have 
	// direct access to them.
	//
	
	virtual	DWORD			Control		( DWORD parameter ) { return 0;}

	//-------------------------------------------------
	// Optional Methods for saving the plug-in state
	// within a scene file.

	virtual	IOResult		Save		( ISave *isave );
	virtual	IOResult		Load		( ILoad *iload );

	static bool	_NodeCountProc( INT32 Level, INode * pNode, void * pData);
	static bool _NodeTraverseProc( INT32 Level, INode * pNode, void * pData);


	//
	// implements OptiExpPlugCallBack
	//

	virtual HINSTANCE OEPC_GetOwnerModuleInst();
	virtual void OEPC_ClearLog();
	virtual bool OEPC_DoExport(const char* path, const char* filename, i3Node** rstSceneNode);
	virtual void* OEPC_GetExporter(); // i3Export* g_export;
	virtual bool OEPC_DoExporterOptDialog();

	virtual void OEPC_GetExporterFilePathName(char* path, char* name);
	virtual void OEPC_SetExporterFilePathName(const char* path, const char* name);
	
	virtual i3tool::exporter::pack::OPTION_INFO* OEPC_GetExporterOpt();
	virtual void OEPC_Log(char* msg);	

	virtual INT32 OEPC_SetPhysXSettingToNode(INT32 dynType, bool includeOnly=false, 
		const char* includeNode=NULL, const char* excludeNode=NULL);

	virtual void OEPC_GetAniTimeRange(INT32& startTime, INT32& endTime);
	virtual void OEPC_SetAniTimeRange(INT32 startTime, INT32 endTime);

	virtual void OEPC_Changei3DxResourceClassMode(bool toMem);

	virtual bool OEPC_CheckDupplicatedName(void** rst);
	virtual bool OEPC_CheckCollsionMesh(DWORD logFlag=0);

	virtual void OEPC_SetLoggingToFile(bool logging, bool* prevStatus=NULL);
	virtual bool OEPC_GetPhysXCookingLibInfo(DWORD* ver=NULL);
};

extern i3Export *		g_pExport;

#endif
