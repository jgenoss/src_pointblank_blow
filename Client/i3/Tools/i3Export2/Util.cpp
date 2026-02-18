#include "stdafx.h"
#include "Util.h"
#include "decomp.h"
#include "iiksys.h"

#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"

TCHAR *GetString(int id) 
{
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

REAL32 Time2Sec( TimeValue t)
{
	return (REAL32) t / GetTicksPerFrame() / GetFrameRate();
}

INT32 Time2Frame( TimeValue t)
{
	return (INT32)(t / GetTicksPerFrame());
}

// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
bool TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0);
}

void	Convert( MATRIX * pDest, Matrix3 * pSrc)
{
	Point3 v;

	v = pSrc->GetRow( 0);
	i3Matrix::SetRight( pDest, v.x, v.y, v.z, 0.0f);

	v = pSrc->GetRow( 1);
	i3Matrix::SetUp( pDest, v.x, v.y, v.z, 0.0f);

	v = pSrc->GetRow( 2);
	i3Matrix::SetAt( pDest, v.x, v.y, v.z, 0.0f);

	v = pSrc->GetRow( 3);
	i3Matrix::SetPos( pDest, v.x, v.y, v.z, 1.0f);
}

void	Convert( Matrix3 * pDest, MATRIX * pSrc)
{
	VEC3D * pVec;
	
	pVec = i3Matrix::GetRight( pSrc);
	pDest->SetRow( 0, Point3( pVec->x, pVec->y, pVec->z));

	pVec = i3Matrix::GetUp( pSrc);
	pDest->SetRow( 1, Point3( pVec->x, pVec->y, pVec->z));

	pVec = i3Matrix::GetAt( pSrc);
	pDest->SetRow( 2, Point3( pVec->x, pVec->y, pVec->z));

	pVec = i3Matrix::GetPos( pSrc);
	pDest->SetRow( 3, Point3( pVec->x, pVec->y, pVec->z));
}

void	Convert( VEC3D * pVec, Point3 pt)
{
	i3Vector::Set( pVec, pt.x, pt.y, pt.z);
}

void	Convert( VEC2D * pVec, UVVert uv)
{
	i3Vector::Set( pVec, uv.x, uv.y);
}

void _dumpMatrix( Matrix3 * pMtx)
{
	Point3 vec;

	I3TRACE( "---\n");

	vec = pMtx->GetRow(0);
	I3TRACE( "%f %f %f\n", vec.x, vec.y, vec.z);
	vec = pMtx->GetRow(1);
	I3TRACE( "%f %f %f\n", vec.x, vec.y, vec.z);
	vec = pMtx->GetRow(2);
	I3TRACE( "%f %f %f\n", vec.x, vec.y, vec.z);
	vec = pMtx->GetRow(3);
	I3TRACE( "%f %f %f\n", vec.x, vec.y, vec.z);
	I3TRACE( "---\n");
}

static char s_szTemp[4096];

void _dumpMatrix2( const char * pszLabel, Matrix3 * pMtx)
{
	Point3 vec0, vec1, vec2, vec3;

	vec0 = pMtx->GetRow(0);
	vec1 = pMtx->GetRow(1);
	vec2 = pMtx->GetRow(2);
	vec3 = pMtx->GetRow(3);

	sprintf( s_szTemp, "%s\r\n%f, %f, %f\r\n%f, %f, %f\r\n%f, %f, %f\r\n%f, %f, %f", pszLabel,
		vec0.x, vec0.y, vec0.z,
		vec1.x, vec1.y, vec1.z,
		vec2.x, vec2.y, vec2.z,
		vec3.x, vec3.y, vec3.z);

	DebugBox( NULL, s_szTemp);
}

//If the material is null or is on the unsupported list, we do not support it.
bool IsSupported(Mtl *mat)
{
	if (mat == NULL) 
	{
		return false;
	}

	static const TSTR unsupportedMaterialTypeNames[] =
	{
		TSTR("Top/Bottom"),
		TSTR("Morpher"),
		TSTR("Composite")
	};
	const int numUnsupportedMaterials = sizeof(unsupportedMaterialTypeNames)/sizeof(TSTR);
	
	TSTR matClassName;
	mat->GetClassName(matClassName);
	for (int i = 0; i < numUnsupportedMaterials; i++)
	{
		if (matClassName == unsupportedMaterialTypeNames[i])
		{
			return false;
		}
	}
	return true;
}

bool IsSupported(Texmap *map)
{
	if (!map) {
		return false;
	}

	static const TSTR unsupportedTexmapNames[] =
	{
		TSTR("RGB Tint"),
		TSTR("Output"),
		TSTR("Vertex Color"),
	};
	const int numUnSupportedTexmap = sizeof(unsupportedTexmapNames)/sizeof(TSTR);
	
	TSTR className;
	map->GetClassName(className);
	for (int i = 0; i < numUnSupportedTexmap; i++)
	{
		if (className == unsupportedTexmapNames[i])
		{
			return false;
		}
	}

	// Recurse on stripped texmap
	/*
	Texmap* stripped = StripTexmap(map);
	if (stripped && (stripped != map))
	{
		return IsSupported( stripped );
	}
	*/

	return true;
}

Point3 GetVertexNormal( Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPECIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) 
	{
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) 
		{
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) 
			{
				if (rv->ern[i].getSmGroup() & smGroup) 
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else 
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}

bool IsEqual( REAL32 f1, REAL32 f2)
{
	if( fabs( f1 - f2) < 0.000005f)
		return true;

	return false;
}

bool IsIdentity( Matrix3 * pMat)
{
	bool b1, b2, b3, b4;
	Point3 v;

	v = pMat->GetRow( 0);
	b1 =	IsEqual( v.x, 1.0f) && IsEqual( v.y, 0.0f) && IsEqual( v.z, 0.0f);

	v = pMat->GetRow( 1);
	b2 =	IsEqual( v.x, 0.0f) && IsEqual( v.y, 1.0f) && IsEqual( v.z, 0.0f);

	v = pMat->GetRow( 2);
	b3 =	IsEqual( v.x, 0.0f) && IsEqual( v.y, 0.0f) && IsEqual( v.z, 1.0f);

	v = pMat->GetRow( 3);
	b4 =	IsEqual( v.x, 0.0f) && IsEqual( v.y, 0.0f) && IsEqual( v.z, 0.0f);
	
	return b1 && b2 && b3 && b4;
}


void GetPivotMatrix(INode* node, Matrix3 * pPivot, TimeValue time) 
{
	Point3 pivotTranslation = node->GetObjOffsetPos();
	ScaleValue  pivotScale = node->GetObjOffsetScale();
	Quat pivotRotation = node->GetObjOffsetRot();
		  
	pPivot->IdentityMatrix();
	pPivot->PreTranslate( pivotTranslation);
	PreRotateMatrix( *pPivot, pivotRotation);
	ApplyScaling( *pPivot, pivotScale);
}

i3Texture * TryLoadTexture( const char * pszPath) 
{
	i3ImageFile file;
	i3Texture * pTex;
	char szName[MAX_PATH], szFull[MAX_PATH];
//	char szTemp[MAX_PATH];
	INT32 i;

	// 지정된 경로로 우선 로딩을 시도한다.
	if( i3System::IsFile( (char *) pszPath))
	{
		pTex = file.Load( pszPath);
		if( pTex != NULL)
		{
			pTex->SetName( pszPath);
			return pTex;
		}
	}

	i3::extract_filename( pszPath, szName);

	Interface* ip = GetCOREInterface();

	// 현재의 MAX 파일이 존재하는 경로에서 찾기를 시도한다.
	{
		char szMaxName[MAX_PATH]; // , * pszTemp;

//		strcpy( szMaxName, ip->GetCurFilePath());
//		pszTemp = i3String::SplitFileName( szMaxName, szTemp, TRUE);
//		*pszTemp = 0;		
		i3::extract_directoryname( (const char*)ip->GetCurFilePath(), szMaxName);

		sprintf( szFull, "%s/%s", szMaxName, szName);
	
		if( i3System::IsFile( szFull))
		{
			pTex = file.Load( szFull);
			if( pTex != NULL)
			{
				pTex->SetName( szFull);
				return pTex;
			}
		}
	}
	
	// MAX에서 Map Directory로 지정된 것들에서 찾는다.
#if (MAX_VERSION_MAJOR >= 12)
	{
		INT32 cnt;
		const MCHAR * pszResPath;

		cnt = ip->GetAssetDirCount( MaxSDK::AssetManagement::kBitmapAsset);

		for( i = 0; i < cnt; i++) 
		{
			pszResPath = ip->GetAssetDir( i, MaxSDK::AssetManagement::kBitmapAsset);

			i3::snprintf( szFull, sizeof(szFull), "%s/%s", pszResPath, szName);

			if( i3System::IsFile( szFull))
			{
				pTex = file.Load( szFull);
				if( pTex != NULL)
				{
					pTex->SetName( szFull);
					return pTex;
				}
			}
		}
	}
#else
	for( i = 0; i < ip->GetMapDirCount(); i++) 
	{
		i3::snprintf( szFull, sizeof(szFull), "%s/%s", ip->GetMapDir( i), szName);

		if( i3System::IsFile( szFull))
		{
			pTex = file.Load( szFull);
			if( pTex != NULL)
			{
				pTex->SetName( szFull);
				return pTex;
			}
		}
	}
#endif

	return NULL;
}

/**
*	returns the skin Modifier if there is one in the stack of the modifier for the given node
*
*	-	returns NULL if there is no bone system attached to the node
*	-	change the value of type to indicate the kind of modifier
*/

Modifier* FindBonesModifier( INode* pNode, BONETYPE * pType)
{
	// by default we assume that there is no skinning modifier
	*pType = NOBONES;

	// gets the first referenced pObj
	Object *pObj = pNode->GetObjectRef();

	if (pObj) 
	{
		// for each derived pObj
		while( (pObj != NULL) && (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID))
		{
			// we have a derived object
			// otherwise it is the base pObj with no modifier
			IDerivedObject* derivedObject = (IDerivedObject*) pObj;

			// gets the number of modifier from the current derived pObj
			int countModifier = derivedObject->NumModifiers();

			// for each of the modifier
			for( int index=0; index < countModifier ;index++)
			{
				// gets the modifier
				Modifier* modifier = derivedObject->GetModifier(index);

				Class_ID classID = modifier->ClassID();
							
				// checks if it is a character studio modifier
				if (classID == PHYSIQUE_CLASSID)
				{
					*pType = PHYSIQUE;
					return modifier;
				}

				// checks if it is max bone modifier
				if(classID == SKIN_CLASSID)
				{
					*pType = BONES_3DS;
					return modifier;
				}

				// this modifier is not a known bones modifier
			}

			// gets the next referenced pObj
			pObj = derivedObject->GetObjRef();
		}
	}

	// we have found no skinning modifier
	return NULL;
}

/**
* returns true only if the node is a biped bone
*/
bool IsBiped(INode* node) 
{
	Control* c = node->GetTMController();

	if (!c) return false;
	if ((BIPSLAVE_CONTROL_CLASS_ID == c->ClassID() ) ||
	(BIPBODY_CONTROL_CLASS_ID == c->ClassID()) )  {
			return true;
	}
		return false;
}

/**
* return true only if the node is a character studio footprint
*/
bool IsFootPrint(INode* node)
{
	Control* c = node->GetTMController();
	if (!c) return false;
	if (FOOTPRINT_CLASS_ID == c->ClassID() )  
	{
		return true;
	}

	return false;
}

bool IsDummy( Object * pObj)
{
	if( pObj == NULL)
		return false;

	UINT32	partA	=	pObj->ClassID().PartA();

	return partA == DUMMY_CLASS_ID;
}

bool IsBone( Object * pObj)
{
	if( pObj == NULL)
		return false;

	return (pObj->ClassID() == BONE_OBJ_CLASSID) == TRUE;
}

BONE_TYPE isValidBone( INode * pNode, bool bTreatControlAsBone)
{
	::Control * pCtrl = pNode->GetTMController();

	if( pCtrl == NULL)
		return BONE_TYPE_NA;

	if( IsBiped( pNode))
	{
		if( strstr( pNode->GetName(), "Nub") == NULL)
		{
			return BONE_TYPE_SKELETON;
		}
		else
		{
			// Biped 끝에 붙는 Nub이라고 간주.
			return BONE_TYPE_NA;
		}
	}

	if( IsFootPrint( pNode))
	{
		return BONE_TYPE_NA;
	}

	const ulong partA = pCtrl->ClassID().PartA();
	const ulong partB = pCtrl->ClassID().PartB();

	if( bTreatControlAsBone)
	{
		switch( partA)
		{
			case PRS_CONTROL_CLASS_ID :
				return BONE_TYPE_SKELETON;

			default :
				if( pCtrl->ClassID() == IKCONTROL_CLASS_ID)
				{
					return BONE_TYPE_SKELETON;
				}
				break;
		}
	}

	return BONE_TYPE_BONEREF;
}

INode *		FindRootOfBone( INode * pNode)
{
	INode * pPrevNode = pNode;

	while( (pNode->GetParentNode() != NULL) && (pNode->GetTMController() != NULL))
	{
		pPrevNode = pNode;
		pNode = pNode->GetParentNode();
	}

	return pPrevNode;
}

INT32 _findNodeAmongList( INode * pNode, const i3::vector<INode*>& List)
{
	INode * pTemp;

	for(size_t i = 0; i < List.size(); i++)
	{
		pTemp = List[i];

		if( pTemp == pNode)
			return i;
	}

	return -1;
}

INode *		FindSkinRoot(const i3::vector<INode*>& BoneList)
{
	INode * pNode;
	INode * pRoot = NULL;

	// 우선 모든 참조된 Bone들 중에 가장 상위가 되는 Bone을 찾는다.
	for( size_t i = 0; i < BoneList.size(); i++)
	{
		pNode = BoneList[i];

		INT32 idx = _findNodeAmongList( pNode->GetParentNode(), BoneList);

		if( idx == -1)
			pRoot = pNode;
	}

	if( pRoot != NULL)
	{
		// 만약 pRoot로 찾아진 Bone이 Biped라면 최상위의 Biped를 검색해야 한다.
		pNode = pRoot;

		while( IsBiped( pRoot))
		{
			pNode = pRoot;

			pRoot = pRoot->GetParentNode();
		}

		pRoot = pNode;
	}

	return pRoot;
}

void		AllNodeBetween( INode * pRoot, INode * pChild, i3::vector<INode*>& List)
{
	INode * pTemp = pChild;

	while( (pTemp != NULL) && (pTemp != pRoot))
	{
		pTemp = pTemp->GetParentNode();

		if( pTemp != NULL)
		{
			List.push_back( pTemp);
		}
	}
}

void _Rec_GetBoneNode( INode * pNode, i3::vector<INode*>& List)
{
	INT32 i;
	INode * pChild;
	Control * pCtrl = pNode->GetTMController(); 
	
	if( pCtrl != NULL)
	{
		List.push_back( pNode);
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_GetBoneNode( pChild, List);
	}
}

Matrix3 Uniform_Matrix( Matrix3 orig_cur_mat)
{
    AffineParts   parts;  
    Matrix3       mat;   
    
    ///Remove  scaling  from orig_cur_mat
    //1) Decompose original and get decomposition info
    decomp_affine(orig_cur_mat, &parts);
    
    //2) construct 3x3 rotation from quaternion parts.q
    parts.q.MakeMatrix(mat);
    
    //3) construct position row from translation parts.t  
    mat.SetRow(3,  parts.t);
    
    return(mat);
}


void ScaleBiped(INode* node, BOOL scale)
{
	return;
	
	if( node == NULL)
		return;

	//If we have a biped strip the non-uniform scaling
	if( IsBiped(node)) 
	{
		// gets the node transform controller
		Control* c = node->GetTMController();
		
        // Get the Biped Export Interface from the controller 
        IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);
		
		if (BipIface) 
		{
			// Remove the non uniform scale
			// BipIface->RemoveNonUniformScale( scale);

			/*
			if( scale)
			{
				BipIface->BeginFigureMode( 1);
			}
			else
			{
				BipIface->EndFigureMode(1);
			}
			*/

			/*
			Control* iMaster = (Control *) c->GetInterface(I_MASTER);
			if (iMaster) 
			{
				iMaster->NotifyDependents(FOREVER, PART_TM, REFMSG_CHANGE);

				// Release the interface
				c->ReleaseInterface(I_MASTER,iMaster);
			}
			*/

			// Release the interface
			c->ReleaseInterface(I_BIPINTERFACE,BipIface);
		}

	}
}

bool IsAnimation( INode * pNode)
{
	Interface * ip = GetCOREInterface();
	TimeValue s, e, step, time;
	Interval intv;
	Matrix3 prevTM, nowTM, parentTM;

	intv = ip->GetAnimRange();

	s = intv.Start();
	e = intv.End();
	step = GetTicksPerFrame();
  
	for( time = s; time <= e; time += step)
	{
		parentTM = __PARENT_TM( pNode, time);
		nowTM = __NODE_TM( pNode, time);

		nowTM = nowTM * Inverse( parentTM);

		if( time > s)
		{
			if( !(prevTM == nowTM))
				return true;
		}

		prevTM = nowTM;
	}

	return false;
}

void	_Tab( INT32 count)
{
	INT32 i;

	for( i = 0; i < count; i++)
	{
		I3TRACE("\t");
	}
}

void	_DumpMaterial( INT32 level, MtlBase * pMtl)
{
	INT32 i;
	Mtl * pMaterial;
	static char s_TexmapLabel[][64] =
	{
		"Ambient",
		"Diffuse",
		"Specular",
		"ShininessNs",
		"Shininess Strength",
		"Self-Illumination",
		"Opacity",
		"Filter Color",
		"Bump",
		"Reflection",
		"Refraction",
		"Displacement",
	};

	_Tab( level);
	

	ulong partA = pMtl->ClassID().PartA();
	ulong partB = pMtl->ClassID().PartB();

	switch( partA)
	{
		case DMTL_CLASS_ID :		I3TRACE( "StdMtl ");	break;
		case DMTL2_CLASS_ID :		I3TRACE( "StdMtl2 ");	break;
		case MULTI_CLASS_ID :		I3TRACE( "MultiMtl ");	break;
		case DOUBLESIDED_CLASS_ID :	I3TRACE( "DoubleSided Mtl ");	break;
		case MIXMAT_CLASS_ID :		I3TRACE( "Blend Mtl ");	break;

		case BMTEX_CLASS_ID :		
			I3TRACE( "Bitmap Tex ");
			{
				BitmapTex * pTex = (BitmapTex *) pMtl;

				I3TRACE( "\"%s\"", pTex->GetMapName());
			}
			break;

		default :
			I3TRACE( "Unknown Mtl[%x, %x] ", partA, partB);
			break;
	}

	I3TRACE( "\n");

	for( i = 0; i <= ID_DP; i++)
	{
		Texmap * pTexmap = pMtl->GetSubTexmap( i);

		if( pTexmap != NULL)
		{
			_Tab( level);

			I3TRACE( "[%s] ", s_TexmapLabel[i]);

			_DumpMaterial( level + 1, pTexmap);
		}
	}

	if( pMtl->SuperClassID() == MATERIAL_CLASS_ID)
	{
		pMaterial = (Mtl *) pMtl;

		for( i = 0; i < pMaterial->NumSubMtls(); i++)
		{
			if( pMaterial->GetSubMtl( i) != NULL)
			{
				_DumpMaterial( level + 1, pMaterial->GetSubMtl( i));
			}
		}
	}
}

void	NormalizeTexCoord( VEC2D * pUV)
{
	REAL32 _x, _y;

	_x = i3Vector::GetX( pUV);
	_y = i3Vector::GetY( pUV);

	// 음수는 양수로 바꾼다.
	if( _x < 0.0f)
	{
		REAL32 fDe;

		fDe = (REAL32)((INT32) i3Math::abs(_x));

		i3Vector::SetX( pUV, (fDe + 1.0f) + _x);
	}

	if( _y < 0.0f)
	{
		REAL32 fDe;

		fDe = (REAL32)((INT32) i3Math::abs(_y));

		i3Vector::SetY( pUV, (fDe + 1.0f) + _y);
	}
}

void		_i3Space( Matrix3 * pMtx)
{
	Matrix3 rot;
	Point3	r, u, a, t;

	r.Set( -1.0f,	0.0f,	0.0f);
	u.Set( 0.0f,	0.0f,	1.0f);
	a.Set( 0.0f,	1.0f,	0.0f);
	t.Set( 0.0f,	0.0f,	0.0f);

	rot.Set( r, u, a, t);

	*pMtx = *pMtx * rot;
}

void	_i3Space( Point3 * pPt)
{
	REAL32 tmp;

	tmp = pPt->y;

	pPt->y = -pPt->z;
	pPt->z = tmp;
}

void	_i3Space( VEC3D * pPt)
{
	REAL32 tmp;

	tmp = pPt->y;

	pPt->y = -pPt->z;
	pPt->z = tmp;
}

void	_i3Space( NxVec3 * pPt)
{
	REAL32 tmp;

	tmp = pPt->y;

	pPt->y = -pPt->z;
	pPt->z = tmp;
}

inline void _TreatForBone( INode * pNode, Matrix3 * pMtx)
{
	if( IsBone( pNode->GetObjectRef()))
	{
		Matrix3 rot;

		rot.SetRow( 0,	Point3( 0.0f,	1.0f,	0.0f));
		rot.SetRow( 1,	Point3( 0.0f,	0.0f,	1.0f));
		rot.SetRow( 2,	Point3( 1.0f,	0.0f,	0.0f));
		rot.SetRow(	3,	Point3( 0.0f,	0.0f,	0.0f));

		*pMtx = rot * *pMtx;
	}
}

Matrix3		__NODE_TM( INode * pNode, TimeValue tm)
{
	Matrix3 mtx;

	mtx = pNode->GetNodeTM( tm);

	_TreatForBone( pNode, &mtx);

	return mtx;
}

Matrix3		__PARENT_TM( INode * pNode, TimeValue tm)
{
	Matrix3 mtx;
	INode * pParent = pNode->GetParentNode();

	if( pParent == NULL)
	{
		mtx.IdentityMatrix();
		return mtx;
	}

	mtx = pParent->GetNodeTM( tm);

	_TreatForBone( pParent, &mtx);

	return mtx;
}

Matrix3		__OBJECT_TM( INode * pNode, TimeValue tm)
{
	Matrix3 mtx;

	mtx = pNode->GetObjectTM( tm);

	_TreatForBone( pNode, &mtx);

	return mtx;
}

bool _str2vec( char * pszStr, VEC3D * pVec)
{
	i3LineLexer lexer;
	char szToken[256], sep;

	lexer.SetMasterString( pszStr);

	if( lexer.GetNextToken( szToken, &sep, "[") <= 0)
		return false;

	if( sep != '[')	return false;

	if( lexer.GetNextToken( szToken, &sep, ",") <= 0)
		return false;

	pVec->x = (REAL32) atof( szToken);

	if( lexer.GetNextToken( szToken, &sep, ",") <= 0)
		return false;

	pVec->y = (REAL32) atof( szToken);

	if( lexer.GetNextToken( szToken, &sep, "]") <= 0)
		return false;

	pVec->z = (REAL32) atof( szToken);

	return true;
}

void enumModifier( INode* pNode)
{
	// gets the first referenced pObj
	Object *pObj = pNode->GetObjectRef();

	if( pObj == NULL)
		return;

	// for each derived pObj
	while( (pObj != NULL) && (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID))
	{
		// we have a derived object
		// otherwise it is the base pObj with no modifier
		IDerivedObject* derivedObject = (IDerivedObject*) pObj;

		// gets the number of modifier from the current derived pObj
		int countModifier = derivedObject->NumModifiers();

		// for each of the modifier
		for( int index=0; index < countModifier ;index++)
		{
			// gets the modifier
			Modifier* modifier = derivedObject->GetModifier(index);

			Class_ID classID = modifier->ClassID();

			UINT32	partA	=	classID.PartA();
			UINT32	partB	=	classID.PartB();
			
			I3TRACE( "MODIFIER[%s] (0x$08X-0x%08X) ---------\n", modifier->GetName(), partA, partB);
			I3TRACE( " PARAM Block : %p\n", modifier->GetParamBlock());

		}

		// gets the next referenced pObj
		pObj = derivedObject->GetObjRef();
	}
}

static INode *		_FindNodeByName( INode * pNode, const char * pszName)
{
	INT32 i;
	INode * pChild;

	if( i3::generic_is_iequal( pNode->GetName(), pszName) )
	{
		return pNode;
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = _FindNodeByName( pNode->GetChildNode( i), pszName);
		if( pChild != NULL)
			return pChild;
	}

	return NULL;
}

INode * FindNodeByName( const char * pszName)
{
	Interface * ip = GetCOREInterface();

	return _FindNodeByName( ip->GetRootNode(), pszName);
}

bool IsLeafOfBiped( INode * pNode)
{
	while( pNode != NULL)
	{
		if( IsBiped( pNode))
			return true;

		pNode = pNode->GetParentNode();
	}

	return false;
}

bool ExtractTransform( INode * pNode, Matrix3 * pMtx, TimeValue time)
{
	Matrix3 parentMtx;
	Matrix3 offmtx;
	Matrix3 selfMtx;

	// Transform이 필요한지 확인.

	ScaleBiped( pNode->GetParentNode(), TRUE);
	ScaleBiped( pNode, TRUE);

	parentMtx = __PARENT_TM( pNode, time);
	selfMtx = __NODE_TM( pNode, time);

	ScaleBiped( pNode->GetParentNode(), FALSE);
	ScaleBiped( pNode, FALSE);

	if( parentMtx == selfMtx)
	{
		// 부모와 동일한 TM을 갖고 있다는 것은
		// 자신의 Transform 값이 없다는 뜻이기 때문에..
		pMtx->IdentityMatrix();
		return false;
	}

	*pMtx = selfMtx * Inverse( parentMtx);

	return true;
}


bool ExtractTransform( INode * pNode, MATRIX * pMtx, TimeValue time)
{
	Matrix3 tmp;
	bool rv;

	rv = ExtractTransform( pNode, &tmp, time);

	::Convert( pMtx, &tmp);

	return rv;
}

bool	GetProxyProperty( INode * pNode)
{
	TSTR str;

	if( pNode->GetUserPropString( TSTR( "Proxy_Geometry"), str) == FALSE)
		return false;

	if( str.data() == NULL)
		return false;

	if( str.data()[0] == 0)
		return false;

	return true;
}
