#if !defined( __UI_Video_UTIL_H__)
#define __UI_Video_UTIL_H__

class VideoUtil
{
	i3::vector<VID_INFO *> m_VidList;
	i3::vector<I3G_AA_INFO*> m_AAList;
	i3::vector<INT32> m_HzList;

public:
	VideoUtil() {}
	~VideoUtil();

	//윈도우 모드일 경우엔 screen 크기보다 작은 해상도만 지원한다.
	//전체화면일 경우엔 모든 해상도가 가능하다
	void init_video_info(INT32 bits, bool win_mode);
	int find_video_info_idx(INT32 width, INT32 height)const;
	int find_max_video_info_idx(INT32* width, INT32* height)const;
	VID_INFO* get_video_info(UINT32 inx)	{ if( inx < m_VidList.size()) { return m_VidList[inx];} return nullptr;}
	i3::wstring get_video_info_string() const;

	void init_AA_Info(INT32 bits, bool win_mode, bool all);
	size_t size_AA_Info(bool include_eight_anti) const;
	int find_AA_info_idx(INT32 width, INT32 height) const;
	I3G_AA_INFO* find_AA_info(INT32 idx) { return m_AAList[idx]; }

	void init_Hz_Info(INT32 bits, INT32 s_width, INT32 s_height);
	int find_Hz_info_idx(INT32 hz) const;
	INT32 get_Hz_info(UINT32 inx) { if (inx < m_HzList.size()) { return m_HzList[inx]; } return 0; }
	i3::wstring get_Hz_info_string() const;

	void init_VCRes_info();


private:
	void add_video_info(I3G_DISPLAY_MODE_INFO * pInfo, INT32 width, INT32 height/*, bool win_mode*/);

	void remove_video_info();
	void remove_AA_info();
	void remove_Hz_info();
	void removeAllVCResInfo();

public:
	static void ApplyGammaState(REAL32 gamma);
	static void ApplyVideoCaptureResolution(int level);
	static void ApplyVideoEnv();
	static void ApplySpecularQuality(INT32 SpecularQuality);
	static void ApplyShadowQuality(INT32 ShadowQuality);
	static void	ApplyNormalMap( bool bEnable);

	struct VideoContext
	{
		bool m_bFullScreen;		bool m_bVSync;		bool m_bTriLinearFilter;		bool m_b16BitsDepth;
		INT32 m_nScreenWidth;	INT32 m_nScreenHeight;	INT32 m_nScreenHz;		INT32 m_ScreenDepth;
		INT32 m_AA;	INT32 m_AAQ;
	};
	static void ApplyVideo(const VideoContext & cxt);

	enum {MAX_QUALITY = 2,
		PASS = 0,
		LOW_RISK,
		HIGH_RISK,
	};

	struct EnableContext
	{
		INT32 specular_quality;
		bool normal_map;
		bool shadow_quality;
	};
	static EnableContext m_enable_context;

	static void check_enable(EnableContext & cxt);
	static const EnableContext& get_ability_context();

	static UINT check_shadow_ability(INT32 type);
	static UINT check_texture_ability(INT32 type);
	static UINT check_dynamiclight_ability(bool dynamic_light, bool normal_map);
	static UINT check_normalmap_ability(bool enable);
};

#endif
