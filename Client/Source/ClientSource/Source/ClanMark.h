#ifndef __CLAN_MARK_H
#define __CLAN_MARK_H

/*
담당자 : 백성민
작성일 : 2008.5.22

새로운 클랜 마크 이미지를 저장해야 할 새로운 텍스처가 필요하면 기본적으로 512x512 사이즈의 텍스처를 생성합니다.

하나의 새로운 텍스처에는 작은 마크 이미지(25x25 픽셀)일 경우 모두 20x20개를 저장할 수 있습니다.
하나의 새로운 텍스처에는 중간 마크 이미지(52x52 픽셀)일 경우 모두 9x9개를 저장할 수 있습니다.
하나의 텍스처 공간에 모두 채우고 모자를 경우 또다른 하나의 512x512 사이즈의 새로운 텍스처를 생성하여 담습니다.

한번 특정 클랜 마크가 생성(사용)되면 info list에 저장되며 앞으로 다시 사용할 경우 list에서 정보가 제거되지 않는 이상 
재생성하지 않고 바로 사용됩니다.

작은 클랜 마크와 중간 클랜 마크는 서로 다른 텍스처로 따로 관리됩니다.

한 화면에 보여지는(생성되는) 클랜 마크의 제한 갯수 이상을 생성할 수 없습니다. 제한 갯수 이상 필요할 경우 알고리즘 수정이 필요합니다.

현재 작은 클랜 마크 제한 갯수 = 400개 (CLAN_MARK_SMALL_MAX_COUNT)
현재 중간 클랜 마크 제한 갯수 = 81개 (CLAN_MARK_MID_MAX_COUNT)

mark flag가 0xFFFFFFFF는 비클랜원일 경우이며 클랜 마크는 0xFFFFFFFF가 아니다.

*/



#define CLAN_MARK_SMALL_PIXEL			25		//	작은 이미지 사이즈 (소스 이미지 영역)
#define CLAN_MARK_MID_PIXEL				52		//	중간 이미지 사이즈 (소스 이미지 영역)
#define CLAN_MARK_CREATE_TEX_SIZE		512		//	저정할 조합 끝난 클랜마크 텍스처 서페이스 크기
#define CLAN_MARK_COLUMN_MAX			9		//	마크 이미지 가로 최대 갯수 (소스 이미지 크기 변경시 값변경 필요)
#define CLAN_MARK_ROW_MAX				9		//	마크 이미지 세로 최대 갯수 (소스 이미지 크기 변경시 값변경 필요)

//	리스트에서 관리할 수 있는 최대 마크 제한 갯수
#define CLAN_MARK_SMALL_MAX_COUNT		((CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_SMALL_PIXEL) * (CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_SMALL_PIXEL))
#define CLAN_MARK_MID_MAX_COUNT			((CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_MID_PIXEL) * (CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_MID_PIXEL))

#define CLAN_MARK_FLAG_NONE				CLAN_MARK_DEFAULT	//	클랜 마크가 비클랜원
#define CLAN_MARK_FLAG_DEFAULT			0					//	클랜원이며 기본 클랜 마크
#define IS_CLAN_MEMBER( markflag)		((markflag != CLAN_MARK_FLAG_NONE) ? 1 : 0)

enum CLAN_MARK_SIZE
{
	CLAN_MARK_SMALL = 0,		//	작은 사이즈
	CLAN_MARK_MEDIUM,			//	중간 사이즈

	MAX_CLAN_MARK_SIZE
};

enum CLAN_MARK_LAYER_TYPE
{
	CLAN_MARK_LAYER_FRAME,		//	바탕 이미지
	CLAN_MARK_LAYER_CONTENT,		//	모양 이미지

	MAX_CLAN_MARK_LAYER,
};


struct ClanMarkInfo
{
	CLAN_MARK_SIZE _type = CLAN_MARK_SMALL;
	bool		_used = false;		//	사용 여부
	UINT32		_flag = 0;		//	클랜 마크 플래그	
	UINT16		_texU = 0;
	UINT16		_texV = 0;
	UINT16		_texW = 0;
	UINT16		_texH = 0;
	i3Texture *	_image = nullptr;		//	클랜 마크 텍스처
};

//	모든 인자는 0~255 이상 수치가 나와서는 안된다. komet
#define MAKE_CLANMARK_FLAG(back_image_idx, shape_image_idx, back_color_idx, shape_color_idx)			\
																			(((UINT8)back_image_idx & 0xFF) << 24 | \
																			((UINT8)shape_image_idx & 0xFF) << 16 | \
																			((UINT8)back_color_idx & 0xFF) << 8 | \
																			((UINT8)shape_color_idx & 0xFF))

#define CLANMARK_FRAME_IMAGE_IDX(clanMarkFlag)		((clanMarkFlag >> 24) & 0xFF)
#define CLANMARK_CONTENT_IMAGE_IDX(clanMarkFlag)		((clanMarkFlag >> 16) & 0xFF)
#define CLANMARK_FRAME_COLOR_IDX(clanMarkFlag)		((clanMarkFlag >> 8) & 0xFF)
#define CLANMARK_CONTENT_COLOR_IDX(clanMarkFlag)		((clanMarkFlag) & 0xFF)

struct	CLAN_MARK_IMAGE;


class CClanMarkManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CClanMarkManager, i3ElementBase);

public:
	CClanMarkManager(void);
	virtual ~CClanMarkManager(void);

	void OnRevive( i3RenderContext * pCtx);

	void Create(void);
	
	void GetUserColorTable(INT32 idx, COLOR * pOut);
	void GetLayerColorTable(CLAN_MARK_LAYER_TYPE LayerType, INT32 idx, COLOR * pOut);
	//void GetBackFrameColorTable(INT32 idx, COLOR * pOut);
	//void GetShapeFrameColorTable(INT32 idx, COLOR * pOut);
    
	//////////////////////////////////////////////////////////////////////////////////////////
	//	data list로 정보가 관리됩니다. (클랜 마크 이미지 포함)
	//////////////////////////////////////////////////////////////////////////////////////////

	//	GUI 컨트롤에 원하는 클랜 마크를 생성하여 셋팅합니다.
	//	이미 생성되어 있어 리스트에 관리되는 클랜 마크라면 재생성하지 않고 find하여 셋팅해줍니다.
	bool SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, 
						INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

	bool SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, 
						INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

	//	리스트에 저장되어 있는 클랜 마크 를 찾아 반환합니다. 리스트에 없을 경우 nullptr을 반환합니다.
	ClanMarkInfo * GetClanMarkTex(CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);
	
	//	리스트에 사용되고 있는 클랜 마크 정보 갯수
	INT32 GetInfoCount(CLAN_MARK_SIZE type)				{ I3_BOUNDCHK( type, MAX_CLAN_MARK_SIZE); return m_nInfoListCount[type];		}


	//////////////////////////////////////////////////////////////////////////////////////////
	//	data list로 정보가 관리 되지 않습니다. (독자 사용 가능)
	//////////////////////////////////////////////////////////////////////////////////////////

	//	GUI 컨트롤에 기본 PB 클랜 마크를 셋팅합니다.
	bool SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type);

	bool SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type);
	
	//	원하는 조합이 완성된 클랜 마크 이미지를 pDestTex에 이미지 프로세싱합니다. 이전 위치의 이미지에 덮어씌워 집니다.
	bool MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, 
						 INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, 
						 INT32 nDstTexU = 0, INT32 nDstTexV = 0);
	
	//	조합이 완성된 클랜 마크 이미지를 pOutTex에 이미지 프로세싱합니다. 이전 위치의 이미지에 덮어씌워 집니다.
	bool MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 nDstTexU = 0, INT32 nDstTexV = 0);

	//	조합 일부분 클랜 마크 이미지(배경 또는 모양 이미지)를 pOutTex에 이미지 프로세싱합니다. 이전 위치의 이미지에 덮어씌워 집니다.
	bool MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE STBL_IDX_CLAN_SHAPE_COLOR, 
								INT32 nImageIdx, INT32 nColorIdx, INT32 nDstTexU = 0, INT32 nDstTexV = 0);

	static		INT32		GetMarkBackImageCount();
	static		INT32		GetMarkShapeImageCount();
	static		INT32		GetMarkColorCount();

private:

	bool 	GetMarkTexture(CLAN_MARK_IMAGE & MarkTex, CLAN_MARK_SIZE SizeType, CLAN_MARK_LAYER_TYPE LayerType, INT32 ImageIndex, SIZE32 & Size);

	i3::vector<CLAN_MARK_IMAGE*>		m_ClanMarkTex[MAX_CLAN_MARK_LAYER][MAX_CLAN_MARK_SIZE];	// CLAN_MARK_IMAGE *

	ClanMarkInfo  * m_pClanMarkInfo[MAX_CLAN_MARK_SIZE];

	i3::vector<ClanMarkInfo*> m_pInfoList[MAX_CLAN_MARK_SIZE];

	INT32 m_nInfoListCount[MAX_CLAN_MARK_SIZE];
	
	ClanMarkInfo * m_pLatestClanMarkInfo[MAX_CLAN_MARK_SIZE];

	i3Texture * m_pCombiClanMarkTex[MAX_CLAN_MARK_SIZE];
	i3Texture * m_pDefaultClanMarkTex[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkU[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkV[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkWH[MAX_CLAN_MARK_SIZE];
private:
	ClanMarkInfo * _MakeClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag);

	ClanMarkInfo * _FindClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag);
	ClanMarkInfo * _FindEmptyInfo(CLAN_MARK_SIZE type);
	void _ResetClanMarkInfo(CLAN_MARK_SIZE type);
	void _RebuildClanMarkTexture(void);
public:
	INT32 GetDefaultClanMarkU(INT32 type) const { return m_nDefaultClanMarkU[type]; }
	INT32 GetDefaultClanMarkV(INT32 type) const { return m_nDefaultClanMarkV[type]; }
	INT32 GetDefaultClanMarkWH(INT32 type) const { return m_nDefaultClanMarkWH[type]; }
	i3Texture * GetDefaultClanMarkTex(CLAN_MARK_SIZE type) const { return m_pDefaultClanMarkTex[type]; }
};

#endif