#if !defined( __I3_SCENE_UTILITY_H)
#define __I3_SCENE_UTILITY_H

#include "i3Scene.h"

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
typedef struct _tagVisibleCollisionMeshInfo
{
	i3PhysixShapeSet *	m_pShapeSet;		// Out
	i3Geometry *		m_pGeo;				// Out
	i3GeometryAttr *	m_pGeoAttr;			// Out
} I3_VISIBLE_COLL_MESH_INFO;
#endif

namespace i3SceneUtil
{
	I3_EXPORT_SGUT i3Node *	BuildLine( i3VertexArray *&pVetexArray, INT32 nLineCountVEC3D, COLORREAL *pColor = NULL );
	I3_EXPORT_SGUT i3Node *	BuildSphereMesh( BOOL bTri = TRUE, BOOL bLine = TRUE, UINT32 horzdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = NULL, COLORREAL * pAmbientColor = NULL, COLORREAL * pDiffuse = NULL);
	I3_EXPORT_SGUT i3Node *	BuildAxisMesh( BOOL bTri = TRUE, BOOL bLine = TRUE, COLORREAL * pAxisColor = NULL, COLORREAL * pConeColor = NULL);
	I3_EXPORT_SGUT i3Node *	BuildBoxMesh( BOOL bTri = TRUE, BOOL bLine = TRUE, COLORREAL * pTriColor = NULL, COLORREAL * pLineColor = NULL);
	I3_EXPORT_SGUT i3Node * BuildCapsuleMesh( REAL32 fRadius, REAL32 fLength, BOOL bTri = TRUE, BOOL bLine = TRUE, UINT32 horzdiv = 20, UINT32 vertdiv = 10, COLORREAL * pLineColor = NULL, COLORREAL * pAmbient = NULL, COLORREAL * pDiffuse = NULL);
	I3_EXPORT_SGUT i3Node *	BuildBoneMesh( REAL32 Length, REAL32 Size, BOOL bLine = FALSE);
	I3_EXPORT_SGUT i3Node *	BuildSelectBoxMesh( COLOR * pColor);
	I3_EXPORT_SGUT i3Node * BuildArc( REAL32 fRadius, REAL32 fAngle, INT32 divcount = 5, BOOL bTri = TRUE, BOOL bLine = TRUE, COLORREAL * pLineColor = NULL, COLORREAL * pAmbient = NULL, COLORREAL * pDiffuse = NULL );
	I3_EXPORT_SGUT i3Node * BuildTriangle( VEC3D * pV1, VEC3D * pV2, VEC3D * pV3, BOOL bTri = TRUE, BOOL bLine = TRUE, COLORREAL * pLineColor = NULL, COLORREAL * pAmbient = NULL, COLORREAL * pDiffuse = NULL );

	I3_EXPORT_SGUT i3Node *	RemoveNodeByFlag( i3Node * pNode, UINT32 mask);

	// Collision
	I3_EXPORT_SGUT i3Node *	BuildRendableCollider( i3Node * pSrc, i3Node* pParent, BOOL bLine, BOOL bNormal, bool bAutoAttr = true, i3List * pCollMeshInfoList = NULL, COLOR * pCol = NULL);
	I3_EXPORT_SGUT i3Node *	BuildCollideeMesh( i3Collidee * pColl, BOOL bSurface, BOOL bLine, BOOL bNormal, BOOL bColor = FALSE);
	I3_EXPORT_SGUT i3Node * BuildVisibleBound( i3Node * pSrc);

	// Navigation Mesh
	I3_EXPORT_SGUT i3Node *	BuildRendableNavigationMesh( i3NavigationMesh * pSrc, BOOL bLine, BOOL bNormal, REAL32 dY );
};

#endif
