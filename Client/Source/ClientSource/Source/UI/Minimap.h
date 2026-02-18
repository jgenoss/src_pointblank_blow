#ifndef __MINIMAP_H__
#define __MINIMAP_H__

namespace minimap_new
{
	struct RatioWorldToImage
	{
		REAL32 w;
		REAL32 h;
		REAL32 AdjustWidth;
		REAL32 AdjustHeight;
	};

	struct MapImageInfo
	{
		REAL32	w, h;
		REAL32	posCenterX, posCenterY;
		REAL32  bakedRatioW, bakedRatioH;

		RatioWorldToImage  ratio;
	};

	struct TextureInfo
	{
		REAL32	w, h;
		INT32	totalCount;
		char	name[MAX_PATH];

		MapImageInfo	mapImageInfo;
	};

	struct FrameWindowInfo
	{
		REAL32	w, h;
		REAL32	halfW, halfH;
	};


	class Minimap
	{
	public:
		static Minimap* GetInstance() 
		{
			if (m_pInstance == 0)
			{
				m_pInstance = new Minimap;
				MEMDUMP_NEW( m_pInstance, sizeof( Minimap));
			}
			return m_pInstance;
		}

		static void Destroy()
		{
			I3_SAFE_DELETE(m_pInstance);
		}

		virtual ~Minimap();

		bool				Create(i3UIFrameWnd * pFrame);

		void				Update(REAL32 deltaSeconds);

		i3UIFrameWnd*		GetFrame() const		{ return m_pFrame;    }
		const FrameWindowInfo&	GetFrameInfo() const	{ return m_FrameInfo; }
		const TextureInfo&		GetTexInfo() const		{ return m_TexInfo;   }


	private:
		Minimap();

		bool	_LoadMapInfo();
		bool	_LoadMapTextures();
		void	_CreateMinimapImageBox();
		void	_ChangeMapBG();
		INT32	_GetMyCurFloor() const;
		VEC3D	_GetWorldMapSize();
		void	_GetMinimapImageRect(REAL32& x, REAL32& y, REAL32& w, REAL32& h) const;

	private:
		TextureInfo				m_TexInfo;
		FrameWindowInfo			m_FrameInfo;
		i3::vector<i3Texture*>	m_pTextures;
		static Minimap*			m_pInstance;
		i3UIFrameWnd*			m_pFrame;
		i3UIImageBoxEx*			m_pImageBox;
		i3Texture*				m_pCurTex;
	};
}

#endif
