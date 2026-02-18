#if !defined( __I3_Level_SCENE_H)
#define __I3_Level_SCENE_H

#include "i3LevelType.h"
#include "i3LevelElement.h"
#include "i3LevelElement3D.h"
#include "i3LevelAxis.h"
#include "i3LevelAxisMove.h"
#include "i3LevelViewport.h"
#include "i3LevelAction.h"
#include "i3LevelTrigger.h"
#include "i3LevelLayer.h"
#include "i3LevelGroup.h"
#include "i3LevelResTexture.h"

#include "i3NavMesh.h"

class I3_EXPORT_TDK i3LevelScene : public i3LevelElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelScene, i3LevelElement);
protected:
	// Persistent Members
	VEC3D				m_StartPos;
	REAL32				m_StartRot = 0.0f;

	REAL32				m_CameraNear = 1.0f;
	REAL32				m_CameraFar = 5000.0f;
	REAL32				m_CameraFOV = I3_DEG2RAD(45.0f);

	i3AttrSet *			m_pRoot = nullptr;

	i3LightAttr	*		m_pLightAttr = nullptr;

	i3FogEnableAttr *	m_pFogEnableAttr = nullptr;
	i3FogColorAttr *	m_pFogColorAttr = nullptr;
	i3FogModeAttr *		m_pFogModeAttr = nullptr;
	i3FogDensityAttr *	m_pFogDensityAttr = nullptr;
	i3FogNearFarAttr *	m_pFogNearFarAttr = nullptr;

	i3::vector<i3LevelLayer*>	m_LayerList;
	i3LevelLayer *		m_pCurLayer = nullptr;

	i3::vector<i3LevelGroup*>	m_GroupList;

	// Volatile Members
	i3::vector<i3LevelElement3D*>	m_ElementList;
	i3::vector<i3LevelTrigger*>		m_TriggerList;

	i3Node *			m_pCameraRoot = nullptr;
	i3LightSet *		m_pLightRoot = nullptr;
	i3AttrSet *			m_pControlRoot = nullptr;
	i3AttrSet *			m_pUnitRoot = nullptr;
	i3AttrSet *			m_pPrimitiveRoot = nullptr;

	i3NavMesh *			m_pNavMeshRoot = nullptr;

	i3SceneGraphContext *	m_pSgContext = nullptr;

	i3Transform *		m_pGrid = nullptr;
	bool				m_bShowGrid = true;

	REAL32				m_Snap_Pos = 0.2f;
	REAL32				m_Snap_Rotate = 1.0f;
	REAL32				m_Snap_Scale = 0.2f;

	static i3LevelResSceneGraph *	m_pAABoxRes;
	static i3LevelResSceneGraph *	m_pSphereRes;
	static i3LevelResSceneGraph *	m_pCameraRes;
	static i3LevelResSceneGraph *	m_pSpawnRes;
	static i3LevelResSceneGraph *	m_pRallyRes;
	static i3LevelResSceneGraph	*	m_pHeroRes; 
	static i3LevelResSceneGraph *	m_pStartPointRes;
	static i3LevelResSceneGraph *	m_pCharaRes;
	static i3LevelResSceneGraph *	m_pPathStart;
	static i3LevelResSceneGraph *	m_pPathMiddle;
	static i3LevelResSceneGraph *	m_pPathEnd;
	static i3LevelResSceneGraph *	m_pRespawnRes;
	static i3LevelResSceneGraph *	m_pPathRes;
	static i3LevelResSceneGraph *	m_pHitMapRes;
	static i3LevelResTexture *		m_pArrowTexRes;

	i3RenderModeAttr *	m_pRenderModeAttr;
	i3LightingEnableAttr *	m_pLightingEnableAttr;

	i3StringTable *		m_pStringTable = nullptr;
	char				m_szStringTablePath[256] = { 0 };

protected:
	void				_AttachToScene( i3LevelRes * pRes, i3LevelElement3D * pElm);

public:
	i3LevelScene(void);
	virtual ~i3LevelScene(void);

	bool			Create(void);

	i3SceneGraphContext *	getSgContext(void)					{ return m_pSgContext; }
	i3Node *		GetRoot(void)								{ return m_pRoot; }
	i3Node *		GetCameraRoot(void)							{ return m_pCameraRoot; }
	i3Node *		GetControlRoot(void)						{ return m_pControlRoot; }
	i3Node *		GetUnitRoot(void)							{ return m_pUnitRoot; }

	i3NavMesh *		GetNavMeshRoot()							{ return m_pNavMeshRoot; }

	bool			IsShowGrid() const
	{
		return m_bShowGrid;
	}

	void			SetGridShow( bool bShow);
	void			SetBound( VEC3D * pMin, VEC3D * pMax);

	INT32			getLayerCount(void)							{ return (INT32)m_LayerList.size(); }
	i3LevelLayer *	getLayer( INT32 idx)						{ return m_LayerList[idx]; }
	void			addLayer( i3LevelLayer * pLayer);
	void			removeLayer( i3LevelLayer * pLayer);
	void			removeAllLayer(void);
	i3LevelLayer *	getCurrentLayer(void)						{ return m_pCurLayer; }
	void			setCurrentLayer( i3LevelLayer * pLayer)		{ m_pCurLayer = pLayer; }
	INT32			FindLayerByName( const char * pszName);
	void			GetUniqueLayerName( const char * pszPrefix, char * pszName);

	INT32			getGroupCount(void)							{ return (INT32)m_GroupList.size(); }
	i3LevelGroup *	getGroup( INT32 idx)						{ return m_GroupList[idx]; }
	void			addGroup( i3LevelGroup * pGroup);
	void			removeGroup( i3LevelGroup * pGroup);
	void			removeAllGroup(void);

	void			SetGroupToSelectedElements( const i3::rc_string& strGroupName);
	void			ReleaseGroupFromSelectedElements(void);

	///////////////////////////////
	// Render State
	void			SetFogEnable( bool bFlag)					{ m_pFogEnableAttr->Set( bFlag); }
	bool			GetFogEnable(void)							{ return m_pFogEnableAttr->Get(); }

	void			SetFogColor( COLOR * pColor)				{ m_pFogColorAttr->Set( pColor); }
	COLOR *			GetFogColor(void)							{ return m_pFogColorAttr->Get(); }

	void			SetFogMode( I3G_FOG_MODE mode)				{ m_pFogModeAttr->Set( mode); }
	I3G_FOG_MODE	GetFogMode(void)							{ return m_pFogModeAttr->Get(); }

	void			SetFogDensity( REAL32 val)					{ m_pFogDensityAttr->Set( val); }
	REAL32			GetFogDensity(void)							{ return m_pFogDensityAttr->Get(); }

	void			SetFogNear( REAL32 fNear)					{ m_pFogNearFarAttr->SetNear( fNear); }
	REAL32			GetFogNear(void)							{ return m_pFogNearFarAttr->GetNear(); }

	void			SetFogFar( REAL32 fFar)						{ m_pFogNearFarAttr->SetFar( fFar); }
	REAL32			GetFogFar(void)								{ return m_pFogNearFarAttr->GetFar(); }

	void			SetCameraNear( REAL32 fNear)				{ m_CameraNear = fNear; }
	REAL32			GetCameraNear(void)							{ return m_CameraNear; }

	void			SetCameraFar( REAL32 fFar)					{ m_CameraFar = fFar; }
	REAL32			GetCameraFar(void)							{ return m_CameraFar; }

	void				SetRenderMode( I3G_RENDER_MODE mode )		{ m_pRenderModeAttr->Set( mode ); }
	I3G_RENDER_MODE		GetRenderMode()								{ return m_pRenderModeAttr->Get(); }

	bool			LoadEffectDescFile( const char *szEffectDesc );

	/////////////////////////////////////////////////
	// Common Resource
	static void				CreateCommonResource(void);
	static void				ReleaseCommonResource(void);
	static i3LevelResSceneGraph *	GetCommonAABoxResource(void)			{ return m_pAABoxRes; }
	static i3LevelResSceneGraph *	GetCommonSphereResource(void)			{ return m_pSphereRes;}
	static i3LevelResSceneGraph *	GetCommonSpawnResource(void)			{ return m_pSpawnRes; }
	static i3LevelResSceneGraph *	GetCommonRallyResource(void)			{ return m_pRallyRes; }
	static i3LevelResSceneGraph *	GetCommonCameraResource(void)			{ return m_pCameraRes;}
	static i3LevelResSceneGraph *	GetCommonHeroResource(void)				{ return m_pHeroRes; }
	static i3LevelResSceneGraph *	GetCommonStartPointResource(void)		{ return m_pStartPointRes; }
	static i3LevelResSceneGraph *	GetCommonCharaResource(void)			{ return m_pCharaRes; }
	static i3LevelResSceneGraph *	GetCommonPathStartResource(void)		{ return m_pPathStart; }
	static i3LevelResSceneGraph *	GetCommonPathMiddleResource(void)		{ return m_pPathMiddle; }
	static i3LevelResSceneGraph *	GetCommonPathEndResource(void)			{ return m_pPathEnd; }
	static i3LevelResSceneGraph *	GetCommonRespawnResource(void)			{ return m_pRespawnRes; }
	static i3LevelResSceneGraph *	GetCommonPathResource(void)				{ return m_pPathRes; }
	static i3LevelResSceneGraph *	GetCommonHitMapResource(void) { return m_pHitMapRes; }
	static i3LevelResTexture *		getCommonArrowTexResource(void)			{ return m_pArrowTexRes; }
	


	///////////////////////////////
	// String
	const char *	GetStringTablePath(){ return m_szStringTablePath; }
	bool			LoadStringTable( const char *szFileName );
	INT32			GetStringCount(void);
	const char *	GetLabel( INT32 idx );
	const WCHAR16 *	GetString( INT32 idx );

	/*
	///////////////////////////////
	// Action
	INT32			AddAction( i3LevelAction * pTrig);
	void			RemoveAction( i3LevelAction * pTrig);
	void			RemoveAction( INT32 idx);
	void			RemoveAllAction(void);
	INT32			FindActionByName( const char * pszName);
	void			GetUniqueActionName( char * pszPrefix, char * pszName);
	INT32			GetActionCount(void)						{ return m_ActionList.GetCount(); }
	i3LevelAction *	GetAction( INT32 idx)						{ return (i3LevelAction *) m_ActionList.Items[idx]; }
	void			UnfreezeAllActions(void);
	void			FreezeAllActions(void);
	*/
	
	///////////////////////////////
	// Trigger
	INT32			AddTrigger( i3LevelTrigger * pTrig);
	void			RemoveTrigger( i3LevelTrigger * pTrig);
	void			RemoveTrigger( INT32 idx);
	void			RemoveAllTrigger(void);
	INT32			FindTriggerByName( const char * pszName);
	void			GetUniqueTriggerName( char * pszPrefix, char * pszName);
	INT32			GetTriggerCount(void)						{ return (INT32)m_TriggerList.size(); }
	i3LevelTrigger *GetTrigger( INT32 idx)						{ return m_TriggerList[idx]; }
	void			UnfreezeAllTriggers(void);
	void			FreezeAllTriggers(void);

	///////////////////////////////
	// Element
	INT32			AddElement( i3LevelElement3D * pElm, const char * pszPrefix, bool bCheckOnce = TRUE);
	void			RemoveElement( i3LevelElement3D * pElm);
	void			RemoveElement( INT32 idx);
	void			RemoveElement( const i3::vector<i3LevelElement3D*>& List);
	void			RemoveAllElement(void);
	void			RemoveSelectedElement(void);
	INT32			FindElementByName( const char * pszName);
	void			FindElementByType( i3::vector<i3LevelElement3D*>& List, i3ClassMeta * pMeta);
	void			FindElementByResource( i3::vector<i3LevelElement3D*>& List, i3LevelRes * pRes);
	void			GetUniqueElementName( const char * pszPrefix, char * pszStr);
	INT32			GetElementIndex( i3LevelElement3D * pElm)		{ size_t idx = i3::vu::index_of(m_ElementList, pElm);  return (idx != m_ElementList.size()) ? INT32(idx) : -1; }
	INT32			GetElementCount(void)						{ return (INT32)m_ElementList.size(); }
	i3LevelElement3D * GetElement( INT32 idx)						{ return m_ElementList[idx]; }
	void			GetPersistElement( i3::vector<i3LevelElement3D*>& List);

	void			GetUniqueGeneratedResName( const char * pszPrefix, const char * pszExt, char * pszPath, INT32 lenPath, char * pszName, INT32 lenName);
	
	void			ReplaceResOfElement( i3LevelRes * pSrc, i3LevelRes * pDest);

	void			CleanUpUnusedResource(void);

	void			SelectElement( i3LevelElement3D * pUnit);
	void			UnselectElement( i3LevelElement3D * pUnit);
	void			UnselectAll(void);
	void			GetSelectedElements( i3::vector<i3LevelElement3D*>& List);

	void			TargetElement( i3LevelElement3D * pElm);
	void			UntargetElement( i3LevelElement3D * pElm);
	void			UntargetAll(void);
	void			GetTargetedElements( i3::vector<i3LevelElement3D*>& List);

	void			UnfreezeAll(void);
	void			FreezeSelection(void);

	void			SaveMatrix(void);
	void			RestoreMatrix(void);

	REAL32			getSnap_Position(void) const			{ return m_Snap_Pos; }
	REAL32			getSnap_Rotation(void) const			{ return m_Snap_Rotate; }
	REAL32			getSnap_Scaling(void) const				{ return m_Snap_Scale; }

	REAL32			getSnappedValue_Position( REAL32 val) const;
	REAL32			getSnappedValue_Rotation( REAL32 rad) const;
	REAL32			getSnappedValue_Scaling( REAL32 val) const;

	void			setSnap_Position( REAL32 snap)			{ m_Snap_Pos = snap; }
	void			setSnap_Rotation( REAL32 snap)			{ m_Snap_Rotate = snap; }
	void			setSnap_Scaling( REAL32 snap)			{ m_Snap_Scale = snap; }

	void			NavMoving( POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap);
	void			Moving( I3LV_SPACE::TYPE space, POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap);
	void			Moving( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap);
	void			Rotating( I3LV_SPACE::TYPE space, VEC3D * pWSStart, VEC3D * pWSEnd, VEC3D * pMask, bool bSnap);
	void			Rotating( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap);
	void			Scaling( I3LV_SPACE::TYPE space, POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap);
	void			Scaling( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap);

	void			GripDrag( INT32 idxGrip, POINT orgPt, POINT newPt, bool bSnap);
	
	void			GetWrappingBoundBox( VEC3D * pMin, VEC3D * pMax);
	bool			GetWrappingBoundBoxForSelect( VEC3D * pMin, VEC3D * pMax);

	i3LevelElement3D *	GetPickedElement( POINT point, bool bSelect, SEL_MODE mode);
	void			GetBandedElements( i3LevelViewport * pView, POINT pt1, POINT pt2, i3::vector<i3LevelElement3D*>& List, bool bSelect, SEL_MODE mode);
	void			Pick( POINT point, PICK_TYPE pickType, SEL_MODE mode);
	void			PickBand( i3LevelViewport * pView, POINT pt1, POINT pt2, PICK_TYPE pickType, SEL_MODE mode);

	void			ToggleRenderMode(void);

	void			SetStartPosition( VEC3D * pVec)				{ i3Vector::Copy( &m_StartPos, pVec); }
	VEC3D *			GetStartPosition(void)						{ return &m_StartPos; }

	void			SetStartRotation( REAL32 theta)				{ m_StartRot = theta; }
	REAL32			GetStartRotation(void)						{ return m_StartRot; }

	void			CopyClipboard( HWND hwnd);
	void			PasteClipboard( HWND hwnd, CPoint * ptScreen);

	void			Update(void);

	// 저장하기 전에 반드시 한번 호출되어야 한다.
	void			PrepareSerialize(void);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

};

#endif