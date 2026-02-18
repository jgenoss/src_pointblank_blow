#if !defined( __I3_VIRTUAL_TEXTURE_H)
#define __I3_VIRTUAL_TEXTURE_H

#include "i3VirtualTextureDefine.h"
#include "i3VirtualTextureCache.h"
#include "i3Base/itl/range/literal_range.h"

#if defined(I3_COMPILER_VC)&& defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가합니다. 2017.02.14. soon9
#endif

namespace i3
{
	namespace pack
	{
		struct ALIGN4 VIRTUALTEXTURE_TILE_1
		{
			INT32		m_State = 0;			// I3VT_VTILE_STATE

			I3VT_HRTILE	m_idxRTile = -1;			// RAM Cache에서의 할당 Index (-1)
			I3VT_HVTILE	m_idxVTile = -1;			// VRAM Cache에서의 할당 Index (-1)

			INT32		m_idxRefTex = 0;		// Volatile data. 저장은 되지만, 저장된 값이 의미는 없음. 로딩될 때 재설정됨.
			UINT64		m_Offset = 0;			// 파일에 저장된 위치.
		};

		struct ALIGN4 VIRTUALTEXTURE_TILE_2
		{
			INT32		m_idxRefTex = 0;
			INT32		m_idxFile = 0;
			UINT64		m_Offset = 0;
		};
	}
}

#if defined(I3_COMPILER_VC)&& defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

struct ALIGN4	I3VT_TILE
{
protected:
	INT32		m_State = 0;			// I3VT_VTILE_STATE
	UINT32		m_LastFrameID = 0xFFFFFFFF;

	I3VT_HRTILE	m_idxRTile = -1;			// RAM Cache에서의 할당 Index (-1)
	I3VT_HVTILE	m_idxVTile = -1;			// VRAM Cache에서의 할당 Index (-1)

	I3VT_TEXINFO* m_pVTTexInfo = nullptr;	// 그냥 포인터로 접근...이쪽이 훨씬 낫다고 결론 지음..

	INT32		m_idxFile = 0;
	UINT64		m_FileOffset = 0;		// 파일에 저장된 위치.

public:
	/////////////////////////////////////////////////
	INT32		getState(void)				{ return m_State; }
	void		setState( INT32 state)		{ m_State = state; }
	void		addState( INT32 state)		{ m_State |= state; }
	void		removeState( INT32 state)	{ m_State &= ~state; }
	bool		isState( INT32 state)		{ return (m_State & state) == state; }
	bool		isLoaded_RAM(void)			{ return isState( I3VT_TILE_STATE_LOADED_RAM); }
	bool		isLoaded_VRAM(void)			{ return isState( I3VT_TILE_STATE_LOADED_VRAM); }
	bool		isLoading(void)				{ return isState( I3VT_TILE_STATE_LOADING); }

	UINT32		getLastFrameID(void)		{ return m_LastFrameID; }
	void		setLastFrameID( UINT32 id)	{ m_LastFrameID = id; }

	/////////////////////////////////////////////////

	I3VT_TEXINFO* getTexInfo() const { return m_pVTTexInfo;  }
	void		  setTexInfo(I3VT_TEXINFO* info) { m_pVTTexInfo = info;  }		// 댕글링이 나지 않도록 필요시 잘 리셋해두어야 한다.
	
	INT32		getSubset(void)				{ return m_idxFile; }
	void		setSubset( INT32 idx)		{ m_idxFile = idx; }

	/////////////////////////////////////////////////
	UINT64		getFileOffset(void)			{ return m_FileOffset; }
	void		setFileOffset(UINT64 off)	{ m_FileOffset = off; }
	bool		hasData(void)				{ return m_FileOffset != INVALID_OFFSET; }

	/////////////////////////////////////////////////
	I3VT_HRTILE	getRTile(void)				{ return m_idxRTile; }
	void		setRTile( I3VT_HRTILE hndl)
	{
		I3ASSERT( hndl != -1);
		m_idxRTile = hndl;
		addState( I3VT_TILE_STATE_LOADED_RAM);
	}

	void		clearRTile(void)
	{
		m_idxRTile = -1;
		removeState( I3VT_TILE_STATE_LOADED_RAM);
	}

	/////////////////////////////////////////////////
	I3VT_HVTILE	getVTile(void)				{ return m_idxVTile; }
	void		setVTile( I3VT_HVTILE hndl)
	{
		I3ASSERT( hndl != -1);
		m_idxVTile = hndl;
		addState( I3VT_TILE_STATE_LOADED_VRAM);
	}

	void		clearVTile(void)
	{
		m_idxVTile = -1;
		m_LastFrameID = 0;
		removeState( I3VT_TILE_STATE_LOADED_VRAM);
	}
};

//
// 64k x 64k 크기의 Texture.
//
// Rect 영역을 지정하여 Validate할 수 있다.
//	- Validate 요청이 떨어진 영역의 Tile은 i3VirtualTextureIO에 의해 Loading된다.
//	- 

// 호환성을 위해서만 존재하던 클래스의 멤버변수는 이 쪽으로 옮겨둔다...
// 서브셋목록과 룰은 여전이 툴에서 쓰일수 있어서 신규포맷이라 할지라도 툴상에서 작동은 해야한다..
struct i3VirtualTexture_LegacyData
{

	UINT64					m_BaseOffset = 0;
};

class i3VTFileFormat;

//
// i3VirtualTextureSystem이 i3VirtualTextureIO가 아닌 i3VirtualTexture를 소유하고, 
// i3VirtualTexture 내부에 i3VirtualTextureIO클래스객체를 멤버로 삼는게 더 나아보여서 그쪽으로 가다를 잡아보도록 한다...
//
struct hash_TexInfo_string
{
	size_t operator()(const I3VT_TEXINFO* info) const
	{
		return i3::istring_hash_value(info->m_strName.c_str(), info->m_strName.size());
	}

	size_t operator()(const i3::literal_range&  str) const
	{
		return i3::istring_hash_value(str.c_str(), str.size());
	}
};

struct equal_to_TexInfo_string
{

	bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
	{
		return i3::generic_is_iequal(lhs->m_strName, rhs->m_strName);
	}

	bool operator()(const I3VT_TEXINFO* lhs, const i3::literal_range& rhs) const
	{
		return i3::generic_is_iequal(lhs->m_strName, rhs);
	}

	bool operator()(const i3::literal_range& lhs, const I3VT_TEXINFO* rhs) const
	{
		return i3::generic_is_iequal(lhs, rhs->m_strName);
	}
};


// i3VirtualTexture에서 일반 접근함수로 접근이 어려운 private을 모아놓음...

typedef i3::unordered_set< I3VT_TEXINFO*,
	hash_TexInfo_string, equal_to_TexInfo_string>	I3VT_TEXINFO_INDEX_SET;

struct i3VirtualTextureData
{
	I3VT_TILE *				m_pTiles = nullptr;

	i3::vector<I3VT_TEXINFO*>		m_linearTexInfoList;			// 그냥 배열처럼 사용.(일반적으로 파일문자열의 케이스 둔감 소팅을 기대할것)
	
	I3VT_TEXINFO_INDEX_SET			m_str_refTexIdx_set;	// 대소문자 안가리기가 필요함..I3VT_TEXINFO의 정수인덱스 매핑은 이제 하지 말것..툴에서의 실시간 변경처리시 상당이 피곤하다.

	i3VirtualTexture_LegacyData m_LegacyData;			// 폐기 예정..

};



class I3_EXPORT_FRAMEWORK i3VirtualTexture : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3VirtualTexture, i3GfxResource);

	friend class i3VirtualTextureCache;

private:
	i3VirtualTextureIO *	m_pIO;				// i3VirtualTextureSystem(전역클래스)에서 이쪽으로 변경...[initialize at constructor]
	UINT32					m_FrameID = 10;
	i3VirtualTextureCtx		m_Ctx;
	i3VirtualTextureData	m_Data;
		
	i3VTFileFormat*			m_pFileFormat;				// [initialize at constructor]
	i3VirtualTextureCache *	m_pCache = nullptr;
	
public:
	i3VirtualTexture(void);
	virtual ~i3VirtualTexture(void);

	i3VirtualTextureIO*	GetIO() const { return m_pIO;  }
	UINT32				getFrameID(void) const { return m_FrameID;  }
	void				incFrameID(void) { ++m_FrameID; }
	//
	// Virtual Texture를 생성합니다.
	// Virtual Texture의 크기는 항상 64k x 64k로 고정된다.

	bool			PrepareForRendering( INT32 cx, INT32 cy, bool bLegacyMode);

	bool			isReadyToWork(void) const; 

	i3VirtualTextureCtx *		getContext() { return &m_Ctx; }
	i3VirtualTextureData*		getData() { return &m_Data; }

	INT32			getWidth(void)							{ return m_Ctx.m_Width; }
	INT32			getHeight(void)							{ return m_Ctx.m_Height; }
	I3G_IMAGE_FORMAT	getFormat(void)						{ return m_Ctx.m_Format; }

	i3VirtualTextureCache *		getCache(void)				{ return m_pCache; }

	i3Texture *					getCacheTexture(void)
	{
		I3ASSERT( m_pCache != nullptr);

		return m_pCache->getCacheTexture();
	}

	i3Texture *					getRedirectTexture(void)
	{
		I3ASSERT( m_pCache != nullptr);

		return m_pCache->getRedirectTexture();
	}

	I3VT_TILE *		getTileBySerial( INT32 idx)
	{
		return m_Data.m_pTiles + idx;
	}

	I3VT_HTILE			getTileIndex( I3VT_TILE * pTile)
	{
		return pTile - m_Data.m_pTiles;
	}

	inline I3VT_HTILE	getTileIndex( INT32 xidx, INT32 yidx)
	{
		return (yidx * m_Ctx.m_cntTileX) + xidx;
	}


	inline INT32	getTileX( INT32 idx)
	{
		return (idx % m_Ctx.m_cntTileX);
	}

	inline INT32	getTileY( INT32 idx)
	{
		return (idx / m_Ctx.m_cntTileX);
	}

	inline INT32	getTexelX( INT32 idx)
	{
		return (idx % m_Ctx.m_cntTileX) * m_Ctx.m_TileSize;
	}

	inline INT32	getTexelY( INT32 idx)
	{
		return (idx / m_Ctx.m_cntTileX) * m_Ctx.m_TileSize;
	}

	inline INT32	getTexelBy2D( INT32 xyidx)
	{
		return (xyidx * m_Ctx.m_TileSize);
	}

	// Reference Texture 관련
	void			RemoveRefTexture( i3Texture * pTex);
	

	I3VT_TEXINFO*	FindRefTexture( const char * pszPath) const;

	INT32			FindRefTexture( INT32 px, INT32 py);
	INT32			getRefTextureCount(void) const			{ return (INT32)m_Data.m_linearTexInfoList.size(); }
	I3VT_TEXINFO *	getRefTexture( INT32 idx) const			{ I3_BOUNDCHK( idx, (INT32)m_Data.m_linearTexInfoList.size()); return m_Data.m_linearTexInfoList[idx]; }

	VEC4D *			getRedirectShaderParam(void)			{ return m_pCache->getRedirectShaderParam(); }
	VEC4D *			getCacheShaderParam(void)				{ return m_pCache->getCacheShaderParam(); }

	
	i3VTFileFormat*	GetFileFormat() const { return m_pFileFormat;  }



	bool			ExtractTexture( i3Texture * pTex, INT32 x, INT32 y, INT32 cx, INT32 cy);
	void			DeleteWrongTexture( void);
	void			ChangeFormat( I3G_IMAGE_FORMAT fmt);

	INT32			getNumberOfTiles(void);
	void			LoadATile( INT32 idx, char * pBuf);

	void			ReqLoad( I3VT_HTILE idx, I3VT_HRTILE idxRTile);

	//////////////////////////////////////////////////
	// 외부 Interface
	bool			Validate( VEC4D * pRect, VEC4D * pRange, bool bMustBeVRAM)
	{
		return m_pCache->Validate( pRect, pRange, bMustBeVRAM);
	}

	void			UpdateIndirectTexture(void)					{ m_pCache->UpdateIndirectTexture(); }
	void			Transfer_RAM_VRAM( REAL32 tmLimit)			{ m_pCache->ProcessTrans( tmLimit); }

	// Legacy Mode 전용 함수
	bool			Validate_Legacy( I3VT_TEXINFO * pTex, VEC4D * pRect);
	I3VT_TEXINFO*	GetRelatedRefTexture( REAL32 x, REAL32 y);
	void			FlushVRAM(void);
	void			SortUsedTextureList( i3::vector<INT32>& TexIdxList);

	void			DumpVirtualTexture(void)					{ m_pCache->Dump(); }
	void			DumpTiles( const char * pszDir);
	void			SaveVRAMCache( const char * pszPath);
	void			ExtractAllContainedTextures( const char * pszPath);
	void			Verify(void);
	void			VerifyRef2Tile(void);

};

#endif
