#pragma once


// 전방 선언
class	i3XFileParser;
class	i3XFileLexer;

class	i3XFileTemplate;
class	i3XFileMesh;
class	i3XFileMeshNormals;
class	i3XFileMeshTextureCoords;
class	i3XFileMaterial;
class	i3XFileMeshMaterialList;
class	i3XFileTextureFilename;
class	i3XFileFrame;
class	i3XFileFrameTransformMatrix;
class	i3XFileXSkinMeshHeader;
class	i3XFileSkinWeights;
class	i3XFileAnimationSet;
class	i3XFileAnimation;
class	i3XFileAnimationKey;


class	i3XFileTemplate : public i3ElementBase
{
	I3_CLASS_DEFINE( i3XFileTemplate);

public:
	i3XFileTemplate();
	virtual	~i3XFileTemplate();

public:
	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent) = 0;
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	char *				GetName()		{return m_Name;}
	i3XFileTemplate *	GetParent()		{return m_pParent;}

protected:
	void	FirstParse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	BOOL	LastParse(i3XFileLexer * pLexer);
	BOOL	RefObjectApply(i3XFileLexer * pLexer);

protected:
	i3XFileParser *		m_pParser;
	i3XFileTemplate *	m_pParent;

	char	m_Name[MAX_PATH];
	i3List	m_ChildList;
};



class	i3XFileMesh : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileMesh);

public:
	i3XFileMesh();
	virtual	~i3XFileMesh();

public:
	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

public:
	i3Vector3Array *	GetVertexePosArr()	{return m_pVertexPosArr;}
	i3IndexArray *		GetIndexArr()		{return m_pIndexArr;}
	i3AttrSet *			GetAttrSet()		{return m_pAttrSet;}

	BOOL		SkinWeightsToVectexWeightsArray();
	BOOL		Build();

	// 파일에 노말 정보가 없을때 노말을 계산한다.
	// 성공하면 노말이 담긴 i3Vector3Array (Ref: 0)반환한다. 실패는 NULL
	i3Vector3Array *	ComputeNormal();


	i3XFileSkinWeights *	GetSkinWeight(INT32 Idx)	{return (i3XFileSkinWeights*)m_SkinWeightList.GetItem(Idx);}

private:
	struct	VertexWeight
	{
		REAL32	Weight[4];
		INT32	NumUsedWeight;
		UINT8	Indices[4];
		INT32	NumUsedIndex;
	};

private:
	i3AttrSet *			m_pAttrSet;

	i3Vector3Array *	m_pVertexPosArr;
	i3IndexArray *		m_pIndexArr;

	i3XFileMeshNormals *	m_pNormals;
	i3XFileMeshTextureCoords *	m_pTexCoords;

	i3XFileXSkinMeshHeader *	m_pXSkinMeshHeader;
	i3List						m_SkinWeightList;

	VertexWeight*		m_pVertexWeights;
};



class	i3XFileMeshNormals : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileMeshNormals);

public:
	i3XFileMeshNormals();
	virtual	~i3XFileMeshNormals();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	i3Vector3Array *	GetVertexNormalArr()	{return m_pVertexNormalArr;}

protected:
	i3Vector3Array *	m_pVertexNormalArr;
};



class	i3XFileMeshTextureCoords : public i3XFileTemplate
{
	I3_ABSTRACT_CLASS_DEFINE( i3XFileMeshTextureCoords);

public:
	i3XFileMeshTextureCoords();
	virtual	~i3XFileMeshTextureCoords();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	i3Vector2Array *	GetVertexTexArr()	{return m_pVertexTexArr;}

protected:
	i3Vector2Array *	m_pVertexTexArr;
};



class	i3XFileMaterial : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileMaterial);

public:
	i3XFileMaterial();
	virtual	~i3XFileMaterial();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	i3MaterialAttr *			GetMaterialAttr()	{return m_pMtrlAttr;}
	i3XFileTextureFilename *	GetTextureFilename()	{return m_pTextureFileName;}

protected:
	i3MaterialAttr *			m_pMtrlAttr;
	i3XFileTextureFilename *	m_pTextureFileName;
};



class	i3XFileMeshMaterialList : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileMeshMaterialList);

public:
	i3XFileMeshMaterialList();
	virtual	~i3XFileMeshMaterialList();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);

protected:
};



class	i3XFileTextureFilename : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileTextureFilename);

public:
	i3XFileTextureFilename();
	virtual	~i3XFileTextureFilename();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	i3TextureBindAttr *	GetTextureBindAttr()	{return m_pTexBindAttr;}

protected:
	i3TextureBindAttr *	m_pTexBindAttr;
	char				m_FullPathName[MAX_PATH];
};



class	i3XFileFrame : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileFrame);

public:
	i3XFileFrame();
	virtual	~i3XFileFrame();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	i3Bone *	GetBone()	{return m_pBone;}

protected:
	i3Bone *	m_pBone;
};



class	i3XFileFrameTransformMatrix : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileFrameTransformMatrix);

public:
	i3XFileFrameTransformMatrix();
	virtual	~i3XFileFrameTransformMatrix();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	MATRIX *	GetMatrix()	{return &m_Mat;}

protected:
	MATRIX	m_Mat;		// local matrix
};



class	i3XFileXSkinMeshHeader : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileXSkinMeshHeader);

public:
	i3XFileXSkinMeshHeader();
	virtual	~i3XFileXSkinMeshHeader();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	UINT32		GetMaxSkinWeightsPerVertex()	{return m_MaxSkinWeightsPerVertex;}
	UINT32		GetMaxSkinWeightsPerFace()		{return m_MaxSkinWeightsPerFace;}
	UINT32		GetNumBones()					{return m_NumBones;}

protected:
	UINT32		m_MaxSkinWeightsPerVertex; 
    UINT32		m_MaxSkinWeightsPerFace;
	UINT32		m_NumBones;
};



class	i3XFileSkinWeights : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileSkinWeights);

public:
	i3XFileSkinWeights();
	virtual	~i3XFileSkinWeights();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	const char *	GetTransformNodeName()		{return m_TransNodeName;}
	INT32			GetFrameIndex()				{return m_FrameIndex;}
	void			SetFrameIndex(INT32 Index)	{m_FrameIndex = Index;}
	UINT32			GetNumWeight()				{return m_NumWeight;}
	UINT32*			GetVertexIndices()			{return m_pVertexIndices;}
	REAL32*			GetWeights()				{return m_pWeights;}
	MATRIX *		GetOffset()					{return &m_Offset;}

	UINT32			GetVertexIndex(INT32 Index)			{return m_pVertexIndices[Index];}
	REAL32			GetWeight(INT32 Index)					{return m_pWeights[Index];}

protected:
	char	m_TransNodeName[MAX_PATH];
	INT32	m_FrameIndex;
	UINT32	m_NumWeight;
	UINT32*	m_pVertexIndices;	// 동적배열, 개수:nWeight
	REAL32*	m_pWeights;			// 동적배열, 개수:nWeight
	MATRIX	m_Offset;
};



class	i3XFileAnimationSet : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileAnimationSet);

public:
	i3XFileAnimationSet();
	virtual	~i3XFileAnimationSet();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	BOOL			Build(i3StringList * pBoneNameStringList);
	i3XFileAnimation *	GetAnimTrackByName(char * pName);

	i3Animation *	GetAnimation()	{return m_pAnimation;}

protected:
	i3List	m_AnimationTrackList;
	i3Animation *	m_pAnimation;
};



class	i3XFileAnimation : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileAnimation);

public:
	i3XFileAnimation();
	virtual	~i3XFileAnimation();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	BOOL	Build();
	char *					GetRefFrameName()	{return m_pRefFrame->GetName();}
	i3TransformSequence *	GetTransformSeq()	{return m_pTransSeq;}

protected:
	i3XFileFrame *	m_pRefFrame;
	i3TransformSequence *	m_pTransSeq;
	i3List			m_AnimationKeyList;	// 다수의 키들을 가질 수 있음.
										// Matrix키 하나 또는 Rotation, Scale, Postion의 조합으로
										// 가질 수 있음.
};


enum	XFILE_ANIM_KEY_TYPE
{
	XFILE_ANIM_KEY_ROTATION	= 0,
	XFILE_ANIM_KEY_SCALE	= 1,
	XFILE_ANIM_KEY_POSITION	= 2,
	XFILE_ANIM_KEY_MATRIX	= 4,

	XFILE_ANIM_KEY_INVALD,
};


struct	TimedFloatKeys
{
	~TimedFloatKeys();
	BOOL	Parse(i3XFileLexer * pLexer);

	REAL32		Time;
	UINT32		NumValue;
	REAL32 *	pValues;
};

class	i3XFileAnimationKey : public i3XFileTemplate
{
	I3_CLASS_DEFINE( i3XFileAnimationKey);

public:
	i3XFileAnimationKey();
	virtual	~i3XFileAnimationKey();

	virtual	BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent);
	virtual	BOOL	ApplyChild(i3XFileTemplate* pChild);

	XFILE_ANIM_KEY_TYPE	GetKeyType()		{return m_KeyType;}
	UINT32				GetNumKeyItem()		{return m_NumKey;}
	REAL32				GetKeyInterval()	{return m_pTimedKeys[1].Time - m_pTimedKeys[0].Time;}
	REAL32				GetDuration()		{return m_pTimedKeys[m_NumKey-1].Time;}


	REAL32			GetTime(INT32 Idx)			{return m_pTimedKeys[Idx].Time;}
	REAL32 *		GetKeyValue(INT32 Idx)		{return m_pTimedKeys[Idx].pValues;}
	MATRIX *		GetKeyMatrix(INT32 Idx)		{return (MATRIX *)m_pTimedKeys[Idx].pValues;}
	VEC3D *			GetKeyPossition(INT32 Idx)	{return (VEC3D *)m_pTimedKeys[Idx].pValues;}
	QUATERNION *	GetKeyRotation(INT32 Idx)	{return (QUATERNION *)m_pTimedKeys[Idx].pValues;}
	VEC3D *			GetKeyScale(INT32 Idx)		{return (VEC3D *)m_pTimedKeys[Idx].pValues;}


	static	REAL32	GetTimeRate()				{return s_TimeRate;}
	static	REAL32	SetTimeRate(REAL32 Rate)	{s_TimeRate = Rate;}

protected:
	XFILE_ANIM_KEY_TYPE	m_KeyType;
	UINT32				m_NumKey;
	TimedFloatKeys *	m_pTimedKeys;

	static	REAL32	s_TimeRate;
};


