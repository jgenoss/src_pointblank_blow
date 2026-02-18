#if !defined( __UTIL_H)
#define __UTIL_H

TCHAR *GetString(int id);
REAL32 Time2Sec( TimeValue t);
INT32	Time2Frame( TimeValue t);
bool TMNegParity( Matrix3 &m);

void	Convert( MATRIX * pDest, Matrix3 * pSrc);
void	Convert( Matrix3 * pDest, MATRIX * pSrc);
void	Convert( VEC3D * pVec, Point3 pt);
void	Copy( MATRIX * pDest, Matrix3 * pSrc);
void	Copy( VEC3D * pVec, Point3 pt);
void	Convert( VEC2D * pVec, UVVert uv);
void	_dumpMatrix( Matrix3 * pMtx);
void	_dumpMatrix2( const char * pszLabel, Matrix3 * pMtx);

bool IsSupported(Mtl *mat);
bool IsSupported(Texmap *map);
bool IsEqual( REAL32 f1, REAL32 f2);
bool IsIdentity( Matrix3 * pMat);

Point3 GetVertexNormal( Mesh* mesh, int faceNo, RVertex* rv);
void GetPivotMatrix(INode* node, Matrix3 *, TimeValue time);

i3Texture * TryLoadTexture( const char * pszPath) ;

enum BONETYPE 
{
	BONES_3DS,
	PHYSIQUE,
	NOBONES
};

enum BONE_TYPE
{
	BONE_TYPE_NA = -1,
	BONE_TYPE_SKELETON,
	BONE_TYPE_BONEREF,
};

Modifier* FindBonesModifier( INode* pNode, BONETYPE * pType);

bool IsBiped(INode* node);
bool IsDummy( Object * pObj);
bool IsBone( Object * pObj);
bool IsFootPrint(INode* node);
BONE_TYPE isValidBone( INode * pNode, bool bTreatControlAsBone);
bool IsLeafOfBiped( INode * pNode);

bool	GetProxyProperty( INode * pNode);

INode *		FindRootOfBone( INode * pNode);
INode *		FindNodeByName( const char * pszName);
void _Rec_GetBoneNode( INode * pNode, i3::vector<INode*>& List);
Matrix3 Uniform_Matrix( Matrix3 orig_cur_mat);

INode *		FindSkinRoot( const i3::vector<INode*>& BoneList);
void		AllNodeBetween( INode * pRoot, INode * pChild, i3::vector<INode*>& List);

void ScaleBiped(INode* node, BOOL scale);
bool IsAnimation( INode * pNode);

void	_DumpMaterial( INT32 level, MtlBase * pMtl);

void	NormalizeTexCoord( VEC2D * pUV);

void		_i3Space( Matrix3 * pMtx);
void		_i3Space( Point3 * pPt);
void		_i3Space( VEC3D * pPt);
void		_i3Space( NxVec3 * pPt);
Matrix3		__NODE_TM( INode * pNode, TimeValue tm);
Matrix3		__PARENT_TM( INode * pNode, TimeValue tm);
Matrix3		__OBJECT_TM( INode * pNode, TimeValue tm);
bool		ExtractTransform( INode * pNode, Matrix3 * pMtx, TimeValue time);
bool		ExtractTransform( INode * pNode, MATRIX * pMtx, TimeValue time);

bool _str2vec( char * pszStr, VEC3D * pVec);

void enumModifier( INode* pNode);

#endif
