#if !defined( __I3_PSP_DATA_COMPILER_H)
#define __I3_PSP_DATA_COMPILER_H

#include "i3Base.h"
#include "i3Scene.h"

// 24bits 이상의 Image를 16bits Image Format으로 변환한다. 
// 이때, Alpha Channel의 여부에 따라 4:4:4:4, 또는 5:6:5 형식으로 변환한다.
#define		I3PSP_CA_FORCE_16BITS_IMAGE			0x00000001	

// Image를 PSP Graphic Engine의 Native Swizzled Image 형식으로 변환한다.
#define		I3PSP_CA_SWIZZLED_IMAGE				0x00000002

// Vertex Array의 Format을 PSP의 Native Align에 맞추어 변환한다.
// Loading 후, 형 변환이 발생하지 않기 때문에 Loading이 빠르다.
#define		I3PSP_CA_NATIVE_VERTEX_FORMAT		0x00000004

// Node Flag에 I3_NODEFLAG_STATIC이 포함되어 있다면,
// 해당 Node를 포함한 하위 Node들을 Traverse해,
// Rendering에 직접 적용할 수 있는 Display List를
// 미리 작성해 둔다.
// 단, Runtime 시에 Animation이 되거나, Render State가
// 변경될 수 있는 경우에는 사용하지 않아야 한다.
// 일단 Display List로 변환된 후에는 Render State의 변경 및
// Model View Matrix를 변경하는 것이 불가능하기 때문이다.
#define		I3PSP_CA_COMPILE_STATIC_GEOMETRY	0x00000008

// 기본 16bits 크기의 Index Array의 크기를 더 작은 bits로 줄인다.
// 물론, 적용 가능한 경우에만 적용한다.
#define		I3PSP_CA_OPTIMIZE_INDEX_ARRAY		0x00000010

// BoundBox 정보를 미리 포함하도록 하여, Display List로
// 변환된 후에도 Culling이 가능하도록 한다.
// 만약, 이것을 설정하지 않은 상태에서 Display List를
// 작성한다면, Compile된 모든 Scene Graph가 한꺼번에 Rendering된다.
#define		I3PSP_CA_ADD_CLIPPING_BOUND			0x00000020

#define		I3PSP_CA_ROTATE_AXIS				0x00000040

typedef		void (*I3LOGPROC)( char * pszModule, char * pszMsg, void * pUserData);

class i3DataCompilerPSP : public i3ElementBase
{
	I3_CLASS_DEFINE( i3DataCompilerPSP);

protected:
	i3Node *		m_pScene;
	i3Texture *		m_pTexture;

	i3RenderContext *	m_pGfx;
	i3SceneTracer *	m_pTracer;

	UINT32			m_Option;

	UINT32			m_ImageCount;
	UINT32			m_VertexArrayCount;
	UINT32			m_IndexArrayCount;

	UINT32			m_OrgImageSize;
	UINT32			m_NewImageSize;
	
	UINT32			m_OrgVertexArraySize;
	UINT32			m_NewVertexArraySize;

	UINT32			m_OrgIndexArraySize;
	UINT32			m_NewIndexArraySize;

	BOOL			m_bConvertPos;
	I3VF_TYPES		m_PosType;

	BOOL			m_bConvertNormal;
	I3VF_TYPES		m_NormalType;

	BOOL			m_bConvertColor;
	I3VF_TYPES		m_ColorType;

	BOOL			m_bConvertWeight;
	I3VF_TYPES		m_WeightType;

	BOOL			m_bConvertTexCoord;
	I3VF_TYPES		m_TexCoordType;

	INT32			m_TargetPlatform;

	I3LOGPROC		m_pUserLogProc;
	void *			m_pUserData;

	char			m_szFileName[MAX_PATH];

protected:
	void			RetreiveStatistics(void);
	void			Log( const char * lpszText, ...);

	i3Texture *		CompileDither16Bits( i3Texture * pTex);
	void			CompileDither16Bits( i3List * pList);
	void			CompileIndexArray( i3List * pList);
	void			CompileStaticGeometry(void);

	void			Rec_FindStaticNode( i3Node * pNode);

public:
	i3DataCompilerPSP(void);
	virtual ~i3DataCompilerPSP(void);

	void		SetSceneGraph( i3Node * pNode);
	i3Node *	GetSceneGraph(void)						{ return m_pScene; }

	void		SetTexture( i3Texture * pTex);
	i3Texture *	GetTexture(void)						{ return m_pTexture; }

	void		SetRenderContext( i3RenderContext * pGfx)	{ m_pGfx = pGfx; }

	void		SetOption( UINT32 opt)					{ m_Option = opt; }
	UINT32		GetOption(void)							{ return m_Option; }
	void		AddOption( UINT32 opt)					{ m_Option |= opt; }
	void		RemoveOption( UINT32 opt)				{ m_Option &= ~opt; }
	BOOL		IsOption( UINT32 opt)					{ return (m_Option & opt) == opt; }

	void		SetPositionConvertEnable( BOOL bFlag)	{ m_bConvertPos = bFlag; }
	BOOL		GetPositionConvertEnable(void)			{ return m_bConvertPos; }

	void		SetPositionType( I3VF_TYPES type)		{ m_PosType = type; }
	I3VF_TYPES	GetPositionType(void)					{ return m_PosType; }

	void		SetNormalConvertEnable( BOOL bFlag)		{ m_bConvertNormal = bFlag; }
	BOOL		GetNormalConvertEnable(void)			{ return m_bConvertNormal; }

	void		SetNormalType( I3VF_TYPES type)			{ m_NormalType = type; }
	I3VF_TYPES	GetNormalType(void)						{ return m_NormalType; }

	void		SetColorConvertEnable( BOOL bFlag)		{ m_bConvertColor = bFlag; }
	BOOL		GetColorConvertEnable(void)				{ return m_bConvertColor; }

	void		SetColorType( I3VF_TYPES type)			{ m_ColorType = type; }
	I3VF_TYPES	GetColorType(void)						{ return m_ColorType; }

	void		SetWeightConvertEnable( BOOL bFlag)		{ m_bConvertWeight = bFlag; }
	BOOL		GetWeightConvertEnable(void)			{ return m_bConvertWeight; }

	void		SetWeightType( I3VF_TYPES type)			{ m_WeightType = type; }
	I3VF_TYPES	GetWeightType(void)						{ return m_WeightType; }

	void		SetTextureCoordConvertEnable( BOOL bFlag)	{ m_bConvertTexCoord = bFlag; }
	BOOL		GetTextureCoordConvertEnable(void)			{ return m_bConvertTexCoord; }

	void		SetTextureCoordType( I3VF_TYPES type)	{ m_TexCoordType = type; }
	I3VF_TYPES	GetTextureCoordType(void)				{ return m_TexCoordType; }

	void		SetTargetPlatform( INT32 platform)			{ m_TargetPlatform = platform; }
	INT32		GetTargetPlatform(void)						{ return m_TargetPlatform; }

	void		SetLogProc( I3LOGPROC pProc, void * pUserData)
	{
		m_pUserLogProc = pProc;
		m_pUserData = pUserData;
	}

	BOOL		Load( const char * pszFile);
	BOOL		Save( const char * pszFile);

	void		Compile(void);
};

#endif