#pragma once

#include "PropertyEditBox.h"

#define PROPERTY_LINE_HSIZE	17

////////////////////////////////////////////////////////////////////////////
// On Command List
#define IDC_EDIT				WM_USER + 1
#define IDC_DORPDOWNBUTTON		WM_USER + 2
#define IDC_DIALOGBUTTON		WM_USER + 3
#define IDC_LISTBOX				WM_USER + 4

////////////////////////////////////////////////////////////////////////////
//For Node Attr Mask 
#define I3_NA_MASK			0xF000
#define I3_NODE_MASK		0x1000
#define I3_ATTR_MASK		0x0000

////////////////////////////////////////////////////////////////////////////
//For Attr 
#define I3_ENABLE_ATTR					0x0001
#define I3_MATERIAL_ATTR				0x0002
#define I3_COLORATTR_ATTR				0x0003
#define I3_ALPHAFUNC_ATTR				0X0004
#define I3_BLENDMODE_ATTR				0X0005
#define I3_FACECULLMODE_ATTR			0X0006
#define I3_FOGCOLOR_ATTR				0X0007
#define I3_FOGDENSITY_ATTR				0X0008	
#define I3_FOGMODEATTR_ATTR				0X0009
#define I3_FOGNEARFAR_ATTR				0X000A
#define I3_GEOMETRY_ATTR				0X000B
#define I3_LIGHT_ATTR					0X000C
#define I3_TEXTUREFILTER_ATTR			0X000D
#define I3_TEXTUREWRAP_ATTR				0X000E
#define I3_ZFUNC_ATTR					0X000F
#define I3_SHADEMODE_ATTR				0X0010
#define I3_RENDERMODE_ATTR				0X0020
#define I3_TEXTUREBIND_ATTR				0X0030

//New Attr 2004,  8, 26
#define I3_TEXTURE_MATRIX_ATTR			0X0040
#define I3_TEXTURE_COORDSOURCE_ATTR		0X0050
#define I3_TEXTURE_FUNCTION_ATTR		0X0060
#define I3_PROJECT_MATRIX_ATTR			0X0070
#define I3_VIEW_MATRIX_ATTR				0X0080

//2005.01.05 raja
#define I3_STENCIL_FUNC_ATTR			0X0090
#define	I3_STENCIL_OP_ATTR				0X00A0
#define	I3_MODELVIEW_MATRIX_ATTR		0X00B0
#define I3_VERTEXBLEND_MATRIXLIST_ATTR  0X00C0
//end raja



/*
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
#define I3__ATTR
*/
////////////////////////////////////////////////////////////////////////////
//For Node
#define I3_GEOMETRY_NODE	0x1001	
#define I3_ATTRSET_NODE		0x1002	
#define I3_CAMERA_NODE		0x1003
#define I3_JOINT_NODE		0x1004
#define I3_NODE_NODE		0x1005
#define I3_TRANSFORM_NODE	0x1006
#define	I3_PARTICLE_NODE	0x1007
#define I3_COLLIDERSET_NODE	0x1008
#define I3_SCENEOBJECT_NODE	0x1009
#define I3_LIGHTSET_NODE	0x100A
#define I3_SKIN_NODE		0x100B
#define I3_OCTREE_NODE		0x100C
/*
#define I3_
#define I3_
#define I3_
#define I3_
#define I3_
#define I3_
*/

//For other
#define I3_COLLIDER_OTHER		0x2001
#define I3_COLLIDEESPHERE_OTHER	0x2002
#define I3_COLLIDEEMESH_OTHER	0x2003

#include "i3treePropertyBase.h"
#include "Propertyeditbox.h"
#include "PropertyPopupListBox.h"
#include "ImageButton.h"

class PropertyBoard
{
//For Value
private:	
	//For Select Object
	i3ElementBase *			m_pSelObj;

	//For PropertyWnd Size
	INT32					m_Startx; 
	INT32					m_cx; 
	INT32					m_cy; 
	INT32					m_center; 
	i3List					m_PropertyList; 

	//For Select Property
	INT8					m_SelectLine; 

    //For Draw Wnd 
	CPaintDC *				m_pDc; 
	CFont *					m_pFont; 
	HPEN					m_WhitePen, m_GrayPen, m_BlackPen; 	

	//For Edit Propertys 
	CPropertyEditBox		m_EditBox; 
	CPropertyPopupListBox   m_ListBox; 
	CImageButton			m_DropDownButton;			//Property DropDownList Button 
	CImageButton			m_DialogButton;				//Property User Dialog Windows Button 
public:
	HWND					m_Wnd; 

//For Function
private:	
	void DrawBoarder(void); 
	void DrawProperty(void); 
	void DeleteAllPropertys(void); 
	void SetListPropertys(void); 

	void InsertMatrixInfo(MATRIX* pMatrix);
	void ChangeMatrixValue(MATRIX * pMatrix , INT32 pSelLine);


	//For Property Control
	void HideControls(void); 
	void OpenListBox(void); 

	//For Change Property
	void ChangeProperty(void); 
	
	//For Property List
	void SetNode(void); 
	BOOL ChangeNode(void); 
	void SetAttr(void); 
	BOOL ChangeAttr(void); 
	void SetEnableAttr(void); 
	BOOL ChangeEnableAttr(void); 

	//For Node Proeprty List
	void SetGeometryList(void);
	void ChangeGeometryList(void);

	void SetOcTreeList(void);
	void ChangeOcTreeList(void);

	void SetAttrSetList(void);
	void ChangeAttrSetList(void);

	void SetCameraList(void);
	void ChangeCameraList(void);

	void SetJointList(void);
	void ChangeJointList(void);

	void SetNodeList(void);
	void ChangeNodeList(void);

	void SetTransformList(void);
	void ChangeTransformList(void);

	void SetParticleList(void);	
	void ChangeParticleList(void);

	void SetColliderSetList(void); 
	void ChangeColliderSetList(void); 

	void SetSceneObjectList(void); 
	void ChangeSceneObjectList(void); 

	void SetLightSetList(void); 
	void ChangeLightSetList(void); 

	void SetSkinList(void); 
	void ChangeSkinList(void); 

	void SetBodyList(void);
	void ChangeBodyList(void);

	void SetBoneRefList(void);
	void ChangeBoneRefList(void);

	void SetLODList(void);
	void ChangeLODList(void);

	void SetTimeSequenceList(void);

	//For Attr Property List
	void SetMaterialAttrList(void); 
	void ChangeMaterialAttrList(void); 

	void SetColorAttrList(void); 
	void ChangeColorAttrList(void); 

	void SetAlphaFuncAttrList(void); 
	void ChangeAlphaFuncAttrList(void); 

	void SetBlendModeAttrList(void); 
	void ChangeBlendModeAttrList(void); 

	void SetFaceCullModeAttrList(void); 
	void ChangeFaceCullModeAttrList(void); 

	void SetFogColorAttrList(void); 	
	void ChangeFogColorAttrList(void);      

	void SetFogDensityAttrList(void); 
	void ChangeFogDensityAttrList(void); 

	void SetFogModeAttrList(void); 
	void ChangeFogModeAttrList(void); 

	void SetFogNearFarAttrList(void); 
	void ChangeFogNearFarAttrList(void); 
	
	void SetGeometryAttrList(void); 
	void ChangeGeometryAttrList(void); 

	void SetLightAttrList(void); 
	void ChangeLightAttrList(void); 

	void SetTextureFilterAttrList(void); 
	void ChangeTextureFilterAttrList(void); 

	void SetTextureWrapAttrList(void); 
	void ChangeTextureWrapAttrList(void); 

	void SetZfuncAttrList(void); 
	void ChangeZfuncAttrList(void); 

	void SetShadeModeList(void); 
	void ChangeShadeModeList(void); 

	void SetRenderModeList(void); 
	void ChangeRenderModeList(void); 

	void SetTextureBindList(void); 
	void ChangeTextureBindList(void); 

	//New Insert 2004 8 26
	void SetTextureMatrixAttrList(void); 
	void ChangeTextureMatrixAttrList(void);

	void SetTextureCoordsourceAttrList(void); 
	void ChangeTextureCoordsourceAttrList(void);

	void SetTextureFuctionAttrList(void); 
	void ChangeTextureFuctionAttrList(void);

	void SetTextureEnableAttr(void); 
	void ChangeTextureEnableAttr(void);

	void SetProjectMatrixAttrList(void); 
	void ChangeProjectMatrixAttrList(void); 

	void SetViewMatrixAttrList(void); 
	void ChangeViewMatrixAttrList(void);

	//2005.01.05 raja
	void SetStencilFuncAttrList(void);
	void ChangeStencilFuncAttrList(void);

	void SetStencilOPAttrList(void);
	void ChangeStencilOPAttrList(void);

	void SetModelViewMatrixAttrList(void);
	void ChangeModelViewMatrixAttrList(void);

	void	SetNormalMapBindAttr(void);
	void	ChangeNormalMapBindAttr(void);

	void	SetSpecularMapBindAttr(void);
	void	ChangeSpecularMapBindAttr(void);

	void	SetLuxMapBindAttr(void);
	void	ChangeLuxMapBindAttr(void);

	void	SetFresnelAttr(void);
	void	ChangeFresnelAttr(void);

	/*
	void SetBillboardInfo(UINT index);
	void ChangeBillboardInfo(UINT index);
	void SetBillboardAttrList(void);
	void ChangeBillboardAttrList(void);

	void SetSpriteInfo(UINT index);
	void ChangeSpriteInfo(UINT index);
	void SetSpriteAttrList(void);
	void ChangeSpriteAttrList(void);
	//*/
	//end raja

	//Other Property 
	void SetCollider(void); 
	void ChangeCollider(void); 

	void SetCollideeMesh(void);
	void ChangeCollideeMesh(void); 

	void SetCollideeSphere(void); 
	void ChangeCollideeSphere(void); 

	//For Property Update
	CpropertyBase * GetProperty(void); 

public:
	PropertyBoard(); 
	~PropertyBoard(); 

	void CreateControl(CWnd * dlg); 
	void SetBoard(i3ElementBase * pObject); 
	void Draw(CPaintDC *pDC, INT32 StartX, INT32 cx, INT32 cy,  INT32 center); 
	BOOL Ok(); 
	BOOL Cancel(); 
	BOOL ActControl(WPARAM wParam, LPARAM lParam); 

	void MouseDown(CPoint point); 
};


