#if !defined( __I3_SCENE_UTIL_H)
#define __I3_SCENE_UTIL_H

#include "i3SceneDef.h"
#include "i3Node.h"
#include "i3RenderAttr.h"

class NxActor;
class NxShape;

enum I3SG_TRAVERSAL_RESULT
{
	I3SG_TRAVERSAL_STOP = 0,
	I3SG_TRAVERSAL_CONTINUE,
	I3SG_TRAVERSAL_SKIP_NODE,
	
};

typedef I3SG_TRAVERSAL_RESULT (* I3_SG_TRAVERSE_PROC)( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

namespace i3Scene
{
	I3_EXPORT_SCENE 
	i3Node *		FindNodeByType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void			FindNodeByType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3Node*>& List, bool bRecursive = true);


	I3_EXPORT_SCENE 
	i3Node *		FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void			FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3Node*>& List, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void		FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, I3_SG_TRAVERSE_PROC pProc, void * pData = nullptr, bool bRecursive = true);


	I3_EXPORT_SCENE 
	i3Node *		FindNodeByName( i3Node * pRoot, const char * pszName, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void			FindNodeByName( i3Node * pRoot, const char * pszName, i3::vector<i3Node*>& List, bool bRecursive = true);
	
	I3_EXPORT_SCENE 
	i3RenderAttr *	FindAttrByType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void			FindAttrByType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List, bool bRecursive = true);

	I3_EXPORT_SCENE
	void			FindAttrByType_LOD(i3Node * pRoot, UINT32 LODIndex, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List);
	
	I3_EXPORT_SCENE
	UINT32			FindMaxLODCount(i3Node* pRoot);
	
	I3_EXPORT_SCENE
	void			FindAttrByType_NonLOD(i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List);
	
	I3_EXPORT_SCENE
	void			SortAttrFromEnableAttrToBindAttr( i3Node * pRoot );

	I3_EXPORT_SCENE 
	i3RenderAttr *	FindAttrByExactType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);

	I3_EXPORT_SCENE 
	void			FindAttrByExactType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List, bool bRecursive = true);

	I3_EXPORT_SCENE
	i3RenderAttr *	FindLastAttr( i3Node * pNode, i3ClassMeta * pMeta);

	I3_EXPORT_SCENE
	void			Traverse( i3Node * pRoot, I3_SG_TRAVERSE_PROC pUserProc, void * pUserData, i3MatrixStack * pStack = nullptr, UINT32 disableFlag = 0);

	I3_EXPORT_SCENE 
	i3Node *		CreateClone( i3Node * pRoot, I3_COPY_METHOD method);

	I3_EXPORT_SCENE 
	void			RemoveRenderAttr( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive = true);

	I3_EXPORT_SCENE
	void			SetMaterial( i3Node * pRoot, COLORREAL * pDiffuse, COLORREAL * pSpecular = nullptr, COLORREAL * pEmissive = nullptr, REAL32 shininess = -1.0f, bool bRecursive = true);
	I3_EXPORT_SCENE 
	void			InitColliderSet( i3Node * pRoot, INT32 GrpIdx, UINT32 grpMask, INT32 Style = 0, i3EventReceiver * pReceiver = nullptr);

	I3_EXPORT_SCENE
	void			MoveChildren( i3Node * pDest, i3Node * pSrc);

	// Physix
	I3_EXPORT_SCENE
	void			ModifyDynamicState( i3Node * pRoot, UINT32 addState, UINT32 removeState);

	I3_EXPORT_SCENE
	void			SetDynamicVelocity(  i3Node * pRoot, char * pszName, VEC3D * pFBLin, VEC3D * pFBAng, VEC3D * pLinear = nullptr, VEC3D * pAngular = nullptr );

	I3_EXPORT_SCENE
	void			SetDynamicDamping( i3Node * pRoot, REAL32 fLinear, REAL32 fAngular );

	I3_EXPORT_SCENE
	void			SetSleeping( i3Node * pRoot, bool bEnable );

#if defined (I3_PHYSX)
	I3_EXPORT_SCENE
	void			SetActorPairFlag( i3Node * pRoot, NxActor * pActor, UINT32 nFlags );

	I3_EXPORT_SCENE
	void			SetShapePairFlag( i3Node * pRoot, NxShape * pShape, UINT32 nFlags );
#endif

	I3_EXPORT_SCENE 
	i3Node *	BuildSphereMesh( REAL32 radius = 1.0f, bool bTri = true, bool bLine = true, UINT32 horzdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = nullptr, COLORREAL * pAmbientColor = nullptr, COLORREAL * pDiffuse = nullptr);
	
	I3_EXPORT_SCENE
	i3Node *	BuildAxisMesh( INT32 hdiv = 20, REAL32 Length = 1.0f, REAL32 rad = 0.05f, bool bTri = true, bool bLine = true, COLORREAL * pAxisColor = nullptr, COLORREAL * pConeColor = nullptr);
	
	I3_EXPORT_SCENE 
	i3Node *	BuildBoxMesh( REAL32 size = 1.0f, bool bTri = true, bool bLine = true, COLORREAL * pTriColor = nullptr, COLORREAL * pLineColor = nullptr);
};

#endif

