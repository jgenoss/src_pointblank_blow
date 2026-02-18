#if !defined( __I3_VIRTUAL_TEXTURE_DEFINE_H)
#define __I3_VIRTUAL_TEXTURE_DEFINE_H

#define		I3VT_VTEX_SIZE		32768
#define		I3VT_TILE_SIZE		128

#define		I3VT_TILE_COUNTX    (I3VT_VTEX_SIZE / I3VT_TILE_SIZE)			// 256
#define		I3VT_TILE_COUNTY	(I3VT_VTEX_SIZE / I3VT_TILE_SIZE)			// 256
#define		I3VT_TILE_COUNTXY	(I3VT_TILE_COUNTX * I3VT_TILE_COUNTY)

typedef		INT32			I3VT_HTILE;
typedef		INT32			I3VT_HVTILE;
typedef		INT32			I3VT_HRTILE;

#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_ncopy.h"

struct I3VT_WEAK_TEX
{
	class i3Texture*			m_pRealTex = nullptr;
	mutable unsigned long		m_weakCount = 0;
};

I3VT_WEAK_TEX*	i3VTWeakTex_Create(i3Texture* pRealTex);
void			i3VTWeakTex_AddRef(const I3VT_WEAK_TEX* weak_tex);
void			i3VTWeakTex_WeakRelease(I3VT_WEAK_TEX*& weak_tex);
void			i3VTWeakTex_StrongRelease(I3VT_WEAK_TEX*& weak_tex);
//////////////////////////////////////////////////////
//
// Virtual Texture 내부에서 포함된 원본 Texture 정보를 관리하는 용도.

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

// 실제로 쓰는 멤버는 7개이고, 나머지는 전혀 쓰이지 않고 있었다...(아예 포맷 변경된 케이스에서는 7개 포함 전혀 참고하지 않도록 조절할 것임)

struct ALIGN4 I3VT_TEXINFO_FILE
{
	char		m_szName[256] = { 0 };							// 구조체 전체가 파일저장되므로 변경불가능 (2012.11.20.수빈)
	UINT128		m_CODE = { 0, 0 };								// m_CODE같은건 정말 쓰잘데기없고 불편함...
	INT32		m_Width = 0;
	INT32		m_Height = 0;
	INT32		m_X = 0;
	INT32		m_Y = 0;

	// Runtime member
	INT32		m_pTex = 0;
	INT32		m_bLoaded = 0;
	INT32		m_RenderPass = 0;
	UINT64		m_AddedDate = 0;
	UINT32		m_LastUsed = 0;

	BOOL		m_bBlockAutoTrashTex = FALSE;
	UINT32		pad = 0;
};

//
// 신버전 불필요한 건 다 빼버린다.. i3RefTex는 더이상쓰지 않으며, i3VTexList에서 모든걸 쥐고 흔들도록 한다...

struct ALIGN4 I3VT_TEXINFO_FILE_VTEXLIST
{
										// 문자열의 경우 따로 스트링 목록에서 갖고온다. 공유가 필요없기 때문에 그냥 선형으로 처리가능..
	INT32		m_Width = 0;				// m_CODE를 빼버린다..호환성을 위해서는 실시간 계산해서 저장하는쪽으로다...
	INT32		m_Height = 0;
	INT32		m_X = 0;
	INT32		m_Y = 0;
	BOOL		m_bBlockAutoTrashTex = FALSE;
	UINT32		pad[3] = { 0 };					// 걍 32바이트에 맞춘다...패딩이 아깝다...하지 말까...
};


struct ALIGN4 I3VT_TEXINFO
{
	i3::rc_string	m_strName;	

	UINT128		m_CODE = { 0, 0 };
	INT32		m_Width = 0;
	INT32		m_Height = 0;
	INT32		m_X = 0;
	INT32		m_Y = 0;

	// Runtime member
	I3VT_WEAK_TEX*	m_pWeakTex = nullptr;
	INT32		m_bLoaded = 0;
	INT32		m_RenderPass = 0;
	UINT64		m_AddedDate = 0;
	UINT32		m_LastUsed = 0;
	bool		m_bBlockAutoTrashTex = false;
	bool		m_bBlankTexture = false;
	
	void		CopyFromFile( const I3VT_TEXINFO_FILE * pSrc)
	{
		m_strName = pSrc->m_szName;
		m_CODE = pSrc->m_CODE;
		m_Width = pSrc->m_Width;
		m_Height = pSrc->m_Height;
		m_X = pSrc->m_X;
		m_Y = pSrc->m_Y;
		
		m_bBlockAutoTrashTex = pSrc->m_bBlockAutoTrashTex?true:false;
	}

};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

//////////////////////////////////////////////////////
//
// Tile 정보를 관리하기 위한 구조체.
#define			INVALID_OFFSET			0


#define	I3VT_TILE_STATE_LOADED_RAM			0x00000001
#define I3VT_TILE_STATE_LOADED_VRAM			0x00000002
#define I3VT_TILE_STATE_LOADING				0x80000000

#if defined( I3_COMPILER_VC)
#pragma pack(push,16)
#endif

namespace i3
{
	namespace pack
	{
		// 이것은 구버전...신버전은 이거 쓰지 말것...  이름 바꿀까..
		struct ALIGN16	VIRTUALTEXTURE_FILE_HEADER
		{
			UINT8		m_ID[4] = { 'V', 'T', 'V', '2' };
			UINT32		m_Format = 0;
			INT32		m_MipmapLevel = 0;
			INT32		m_Width = 0;
			INT32		m_Height = 0;

			UINT64		m_TileTableOffset = 0;
			UINT64		m_DataBaseOffset = 0;

			UINT32		pad[128] = { 0 };
		};

		// 이게 신버전 .. 불필요한건 다 빼버린다...TILE같은 경우 하나도 저장을 안한다...저장하지 말것...걍...하지마...
		struct ALIGN16 VIRTUALTEXTURE_FILE_HEADER_VTEXLIST
		{
			UINT8		m_ID[4] = { 'V', 'T', 'L', '1' };		// 4	 // 버추얼텍스쳐 목록 1번버전..
			UINT32		m_Format = 0;		// 4
			INT32		m_MipmapLevel = 0;	// 4
			INT32		m_Width = 0;		// 4
			INT32		m_Height = 0;		// 4

		//	INT32		m_numTexture;	//
		//	INT32		m_stringDataSize;
			UINT32		pad[3] = { 0 };			// 걍 32바이트 토탈이 되도록...모자라면 ID보고 그냥 헤더를 교체할것...
		};
	}
}

//struct ALIGN4 I3VT_TEXTURE_NAME_LENGTH
//{
//	INT32		m_strLength;	// 파일 옵셋 아니고 걍 길이값이다. m_numName만큼 OFFSET배열을 굴려가며, 카운트만큼 세서 스트링리스트에 적재한다..
//};
								// 문자열 데이터 시작위치는 걍 64바이트 정렬 수준으로만 맞춘다..

//
// I3VT_TEXINFO_FILE_VTEXLIST 시작 옵셋의 경우, 적당이 64바이트 정렬만 맞추고 바로 시작하게끔 하자...(2048수준이 필요하지 않을것 같으며..
//																							옵셋도 별도 저장하지 않는다..)




//
// 2개 모두 파일 입출력에 쓰이고 있기 때문에 제거가 불가능함
// 파일 보관 이외의 부분에 대해서는 구조체를 따로 마련한다..
//


struct i3VTSubsetRule : i3::class_common_pool<i3VTSubsetRule>
{
	i3::rc_string	m_strPath;
	i3::rc_string	m_strName;
	UINT32			m_LimitSize = 0;
};


struct i3VTSubset : i3::class_common_pool<i3VTSubset>
{
	i3::rc_string	m_strPath;
	INT32			m_idxRule = 0;
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

//////////////////////////////////////////////////////
//
enum I3VT_REQSTATE
{
	I3VT_REQSTATE_NONE = 0,
	I3VT_REQSTATE_READY,
	I3VT_REQSTATE_LOADING,
	I3VT_REQSTATE_FINISHED,
};

class i3VirtualTexture;
class i3VirtualTextureCache;

struct I3VT_IOHANDLE
{
	INT32					m_idxSubset = 0;
	__HANDLE_TYPE			m_hFile = nullptr;
	UINT64					m_LastDataOffset = 0;
#if defined( I3_DEBUG)
	char					m_szPath[MAX_PATH] = { 0 };		// 나중에 i3::rc_string으로 교체..(그때는 MemAlloc때문에 신경써야함)
#endif
};

typedef void	(*I3VT_COMPLETE_PROC)( void * pUserData, INT32 key, i3Texture * pTex);

//////////////////////////////////////////////////////
//
struct ALIGN4	I3VT_IOREQ
{
	I3VT_REQSTATE			m_State = I3VT_REQSTATE::I3VT_REQSTATE_NONE;
	bool					m_bRead = false;
	bool					m_bReadEnd = false;
	I3VT_IOHANDLE *			m_pVTex = nullptr;
	UINT64					m_Offset = 0;
	UINT32					m_Size = 0;

	i3VirtualTextureCache *	m_pCache = nullptr;
	INT32					m_Key = 0;
	char *					m_pBuffer = nullptr;
};


typedef		INT32				I3VT_REQ_HNDL;

#define		MAX_VTQUEUE			16//1024

struct i3VirtualTextureCtx
{
	INT32				m_Width = 0;
	INT32				m_Height = 0;
	I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT::I3G_IMAGE_FORMAT_NONE;
	INT32				m_MipmapLevel = 0;
	REAL32				m_revWidth = 0.0f;
	REAL32				m_revHeight = 0.0f;

	INT32				m_CacheWidth = 0;
	INT32				m_CacheHeight = 0;

	INT32				m_TileSize = 0;
	INT32				m_cntTileX = 0;
	INT32				m_cntTileY = 0;
	INT32				m_cntTileTotal = 0;
	INT32				m_DataSizePerTile = 0;

	i3VirtualTexture *	m_pVTex = nullptr;

	bool				m_bLegacyMode = false;
};

//#define VTX_DEBUG

#if defined( VTX_DEBUG)
#define	VTX_LOG				I3TRACE
#else
#define VTX_LOG				__noop
#endif

#define			_ALIGN_( v, unit)				((((v) + (unit - 1)) / unit) * unit)

extern bool		g_bUIDumpLog;

#define			UILOG							if( g_bUIDumpLog)	I3TRACE

#endif
