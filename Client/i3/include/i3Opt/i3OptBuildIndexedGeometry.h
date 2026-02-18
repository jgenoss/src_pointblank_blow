//*******************************************************************
//	Vertex array <-> Indexed array 상호 변환시켜줍니다.
//	
//	by KOMET 2006.01.19
//*******************************************************************
#if !defined ( __I3_OPT_BUILD_INDEXED_GEOMETRY_H)
#define __I3_OPT_BUILD_INDEXED_GEOMETRY_H

#include "i3OptPCH.h"
#include "i3SceneOptimizer.h"
#include "i3OptBuildError.h"

#define	MAX_TEXTURE		8		//	8장까지 멀티 텍스처 지원
#define MAX_BLEND_INDEX	4

#define DUMP_TYPE_ALL			0x0001
#define DUMP_TYPE_POSITION		0x0002
#define DUMP_TYPE_NORMAL		0x0004
#define DUMP_TYPE_COLOR			0x0008
#define DUMP_TYPE_TEXCOORD		0x0010
#define DUMP_TYPE_BLENDINDEX	0x0020
#define DUMP_TYPE_BLENDWEIGHT	0x0040

struct BuildIndexInfo
{
	bool		_bAutoSwitching = false;		//	none index <-> build index 자동 switching 여부 (default: false)
	bool		_bBuildIndex = false;			//	IndexArray를 생성할지 여부 ( default: false) 

	void SetDefault( void)
	{
		_bAutoSwitching = false;
		_bBuildIndex = true;
	}

} ;


struct OPT_VERTICES
{
	i3Vector3Array 		vPositionArray;					//	Position
	i3Vector3Array 		vNormalArray;					//	Normal
	i3Vector4Array 		vColorArray;					//	Vertex Color
	i3Vector3Array 		vTangentArray;					//	Tangent
	i3Vector3Array 		vBiNormalArray;					//	BiNormal
	i3Vector3Array 		vTexcoordArray[ MAX_TEXTURE];	//	Texture Coord
	i3Vector4Array		vBlendWeightArray;				//	Blend Weight	(4개까지)
	i3::vector<UINT32>	nBlendIndexArray[ MAX_BLEND_INDEX];		//	Blend Index		(cast : UINT32)
	
	i3::vector<UINT16>	Indices;						//	Vertex Index	(cast : UINT16)	//i3리스트라면 UINT16일 필요가 없는데, 왜 이렇게 하는지 나중에 살펴볼것..

	I3G_PRIMITIVE		nPrimType = I3G_PRIMITIVE::I3G_PRIM_NONE;
	UINT32				nPrimCount = 0;
	UINT32				nTriCount = 0;
	i3::vector<UINT32>  nPrimLength;
	
	UINT32				nVertexCount = 0;
	UINT32				nIndexCount = 0;
	UINT32				nTexSetCount = 0;
	UINT32				nTexcoordCount = 0;
	UINT32				nBlendIndexCount = 0;
	UINT32				nBlendWeightCount = 0;

	bool				bHasPosition = false;
	bool				bHasNormal = false;
	bool				bHasColor = false;
	bool				bHasTangent = false;
	bool				bHasBiNormal = false;
	bool				bHasTexcoord = false;
	bool				bHasBlendIndex = false;
	bool				bHasBlendWeight = false;
	bool				bIsPrimLength = false;

} ;

class I3_EXPORT_OPT i3OptBuildIndexedGeometry : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildIndexedGeometry, i3SceneOptimizer);

protected:
	BuildIndexInfo		m_BuildInfo;
	OPT_VERTICES		m_OptData;		//	Optimize Buffer

	OPT_ERROR			m_Error;

public:
	i3OptBuildIndexedGeometry(void);
	virtual ~i3OptBuildIndexedGeometry(void);

	virtual bool OnNode( i3Node * pNode, i3Node * pParent); 
	
	virtual void SetProperty( char * pszFieldName, char * pszValue);

public:
	bool	Convert( i3Node * pNode);		//	None Index <-> Use Index swicthing	

	bool	ConvertVertex2Indexed( i3GeometryAttr * pGeoAttr);	//	None Index -> Use Index
	bool	ConvertIndexed2Vertex( i3GeometryAttr * pGeoAttr);	//	Use Index -> None Index

	//	 빌드 정보 설정 ( Convert 함수를 호출하기 전에 설정해야 한다.)
	void	SetAutoSwitching( bool bVal)	{   m_BuildInfo._bAutoSwitching = bVal;				}
	void	SetBuildIndex( bool	bVal)		{	m_BuildInfo._bBuildIndex = bVal;				}

	void	DumpVertexComponents( i3VertexArray * pVA, UINT32 nDumpType, UINT32 count = -1);
		
	OPT_ERROR * GetError(void)		{	return &m_Error;								}

protected:	

	void	SetResult( I3OPT_RESULT nResult, char * pMsg = nullptr)	{	m_Error.SetResult( nResult, pMsg);				}
	
	bool	BuildOptimize( i3GeometryAttr * pGeoAttr);		//	Vertex를 최적화한다. (Index array가 생성된다.)
	bool	RemapIndices( i3GeometryAttr * pGeoAttr);		//	Index를 바탕으로 Vertex를 재배치한다. (Index array가 생성되어 있어야 한다.)
	
	bool	BuildOptimizeToOpt( i3GeometryAttr * pGeoAttr, bool bBuildIndex = true);
	bool	RenewVerticesFromOpt( i3VertexArray * pOutVA, i3IndexArray * pOutIA, i3VertexFormat * pFormat);	
	bool	RemapIndicesToOpt( i3GeometryAttr * pGeoAttr);
	bool	FindUniqueVertex( i3VertexArray * pLockedVA, UINT32 nSrcIdx);	//	Unique Vertex인지 비교하여 m_OptData에 정보 저장
	
	bool	IsUniqueVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx);	//	Optimize Data에 Vertex를 비교
	INT32	CampareVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx);	//	Optimize Data에 Vertex를 검색하여 저장된 index를 반환, 없으면 -1을 반환

	INT32	ComparePositionFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareNormalFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareColorFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareTangentFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareBiNormalFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareTextureCoordFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareBlendIndexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	INT32	CompareBlendWeightFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);
	
	INT32	ComparePositionInOpt( UINT32 nIdx1, UINT32 nIdx2);
	INT32	CompareNormalInOpt( UINT32 nIdx1, UINT32 nIdx2);
	INT32	CompareColorInOpt( UINT32 nIdx1, UINT32 nIdx2);
	INT32	CompareTangentInOpt( UINT32 nIdx1, UINT32 nIdx2);
	INT32	CompareBiNormalInOpt( UINT32 nIdx1, UINT32 nIdx2);
	INT32	CompareTextureCoordInOpt( UINT32 nIdx1, UINT32 nIdx2);

	void	InitOpt( i3GeometryAttr * pGeoAttr, OPT_VERTICES * pOpt);
	bool	AddIndex( i3VertexArray * pLockedVA, UINT32 nSrcIdx);		//	인덱스에 번호 추가	
	bool	BuildTriangleFromIndex( i3VertexArray * pLockedVA, i3IndexArray * pLockedVI, UINT32 nIdx, INT32 idxOut);	//	IndexArray를 바탕으로 Triagle Vertex를 만들어 Opt로 복사
	bool	BuildStripFromIndex( i3VertexArray * pLockedVA, i3IndexArray * pLockedVI, UINT32 nIdx);		//	IndexArray를 바탕으로 Strip Vertex를 만들어 Opt로 복사
	bool	CopyVertexToOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);		//	i3VertexArray의 원하는 Vertex Data를 Optimize Buffer로 복사
	bool	CopyVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx);		//	Optimize Buffer의 원하는 Vertex Data를 i3VertexArray로 복사
	
};

#endif	//	__I3_OPT_BUILD_INDEXED_GEOMETRY_H
