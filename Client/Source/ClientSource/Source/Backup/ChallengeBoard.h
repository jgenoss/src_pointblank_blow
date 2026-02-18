#ifndef _ChallengeBoard_h
#define _ChallengeBoard_h

#if 지워질_것들

#include "GuiNotifyReceiver.h"

enum ChallengeBoardFrame
{
	CBF_BOARD_TOP = 0,
	CBF_BOARD_MIDDLE,
	CBF_BOARD_BOTTOM,

	CBF_MAX
};

class CChallengeBoard: public i3GameObjBase
{
	I3_CLASS_DEFINE(CChallengeBoard, i3GameObjBase);
public:
	const static UINT32 MaxBoardString = MAX_STRING_COUNT;	// 문서상 20 char 이하

public:
	CChallengeBoard();
	virtual ~CChallengeBoard();

	virtual BOOL Create(i3Node* pParentNode = NULL,BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void Change(void); 
	
	void OnActivate(void);

	//BOOL IsComplate(UINT32 idxChallenge);
	//INT32 GetChallengeCount(UINT32 idxChallenge);
	
	//UINT32 GetIngCount(void) const	{ return m_IngChallegeCount;	}

private:
	void _InitTextureCoord(INT32 challengIdx,ChallengeBulletType bulletType= CBT_NONE);
	void _InitSpriteRect(INT32 challengIdx);

private:	
	i3Texture	*	m_pTexture;						// HUD texture
	i3Sprite2D	*	m_pSpriteBoard;					// 임무 틀
	i3Sprite2D	*	m_pSprite;						// 임무 틀
	i3TextNodeDX2*	m_pText[MAX_CHALLENGE_COUNT];	// 상단 라인
	
	//Create할때 값을 넣어서 사용합니다. 
	//UINT8			m_IngChallengeClass[MAX_CHALLENGE_COUNT];	//임무 클래스 
	//UINT32			m_IngChallegeVale[MAX_CHALLENGE_COUNT];		//임무 
	//UINT32			m_IngChallegeCount; 						//하고 있는 임무 전체 카운트
};

#endif

#endif //_ChallengeBoard_h
