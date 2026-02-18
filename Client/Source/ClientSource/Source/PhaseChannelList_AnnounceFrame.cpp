#include "pch.h"
#include "PhaseChannelList_AnnounceFrame.h"

#include "GuiNotifyReceiver.h"
#include "PhaseChannelList.h"
#include "WebControl.h"


CPhaseChannelList_AnnounceFrame::CPhaseChannelList_AnnounceFrame()
{
	m_pAnnounceWin = NULL;
	m_pAnnouceCaption = NULL;
	m_pDummyAnnounceShort = NULL;
	m_pDummyAnnounceLong = NULL;

	i3mem::FillZero(m_pAnnounce, sizeof(m_pAnnounce));

	m_pImageFrame	= NULL;
	m_pScrollFrame	= NULL;
	m_pScrollList	= NULL;

	m_announceImageHeight = 0;

	i3mem::FillZero(m_pAnnounceTexture, sizeof(m_pAnnounceTexture));
	i3mem::FillZero(m_bAnnounceScroll, sizeof(m_bAnnounceScroll));

	m_rAnnounceWinX = 0.0f;

	// 공지사항
	m_nNoticeIndex = -1;
	m_fLocalNoticeTime = 0.0f;
	m_bEnableNoticeText = TRUE;
	m_bClickedNoticeCaption = FALSE;
	m_fAnnounceFilpElapsedTime = 0;
	m_fAnnounceFlipTime = 0;
	m_nCurrentSelectedIndex = 0;
	
	m_hWebAnnounce = NULL;
}

CPhaseChannelList_AnnounceFrame::~CPhaseChannelList_AnnounceFrame()
{
	for(INT32 i = 0; i < MAX_ANNOUNCE_BUTTON; i++)
	{
		I3_SAFE_RELEASE(m_pAnnounceTexture[i]);
	}
	CloseAnnouce();
}


void CPhaseChannelList_AnnounceFrame::SetEnable( BOOL Enable )
{
	m_pAnnounceWin->SetEnable(Enable);

	if ( m_hWebAnnounce )
	{
		int cmdShow = (Enable ? SW_SHOW : SW_HIDE);
		ShowWindow(m_hWebAnnounce, cmdShow);
	}
}


BOOL	CPhaseChannelList_AnnounceFrame::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "ssAnnounceWin", m_pAnnounceWin, -1);
	LinkControlEx(plist, "ssAnnounceCaption", m_pAnnouceCaption, -1);

	for(INT32 i = 0; i < MAX_ANNOUNCE_BUTTON; i++)
	{
		char name[MAX_STRING_COUNT];

		sprintf_s(name, "ssAnnounce%d", i + 1);
		LinkControlEx(plist, name, m_pAnnounce[i], GCI_SSP_B_ANNOUNCE_1 + i);
	}

	LinkControlEx(plist, "ssDummyImageShort", m_pDummyAnnounceShort, -1);
	LinkControlEx(plist, "ssDummyImageLong", m_pDummyAnnounceLong, -1);

	LinkControlEx(plist, "ssImageFrame", m_pImageFrame, -1);
	LinkControlEx(plist, "ssScrollFrame", m_pScrollFrame, -1);
	LinkControlEx(plist, "ssAnnounceList", m_pScrollList, GCI_SSP_L_ANNOUNCE_LIST);

	return TRUE;
}

void	CPhaseChannelList_AnnounceFrame::OnInitControl(void)
{
	m_rAnnounceWinX = m_pAnnounceWin->getPositionX();
	INIT_WIN_CAPTION(m_pAnnouceCaption, GAME_STRING("STR_TBL_GUI_SERVER_NOTICE_LIST"));/*공지사항*/

	if ( false == g_pConfigEx->GetMisc().webAnnounce.IsActive() )
	{

		// 이미지 공지 읽어오기
		INT32 announceCount = g_pFramework->GetAnnounceCount();
		I3ASSERT(announceCount <= MAX_ANNOUNCE_BUTTON);

		if (0 < announceCount)
		{
			for(INT32 i = 0; i < announceCount; i++)
			{
				const char* pName = g_pFramework->GetAnnounceName(i);
				const char* pPath = g_pFramework->GetAnnouncePath(i);
				BOOL scroll = g_pFramework->IsAnnounceScroll(i);

				// 공지사항용 인덱스를 저장합니다.
				if( i3String::CompareCase( pName, "알림") == 0)
				{
					m_nNoticeIndex = i;
				}

				INIT_CAPTION_TAB(m_pAnnounce[i], pName);
				m_pAnnounceTexture[i] = g_pFramework->GetResourceManager()->LoadTexture(pPath);
				m_bAnnounceScroll[i] = scroll;
			}

			// 사용하지 않는 버튼 비활성화
			for(INT32 i = announceCount; i < MAX_ANNOUNCE_BUTTON; i++)
			{
				m_pAnnounce[i]->SetEnable(FALSE);
			}

			// 최초버튼 선택
			OnAnnounceButton(0);
		}
		else
		{
			// 공지설정 없음
			m_pAnnounceWin->SetEnable(FALSE);
		}

		m_pScrollList->CreateTextEx(1,  GetDefaultFontName(), FONT_COUNT_256);
		m_pScrollList->SetSelColor(0, 0, 0, 0);
		m_pScrollList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	}
	else
	{
		for ( int i = 0 ; i < MAX_ANNOUNCE_BUTTON ; ++i )
			m_pAnnounce[i]->SetEnable(FALSE);
		m_pImageFrame->SetEnable(FALSE);
		m_pScrollFrame->SetEnable(FALSE);
		m_pScrollList->SetEnable(FALSE);
		m_pDummyAnnounceShort->SetEnable(FALSE);
		m_pDummyAnnounceLong->SetEnable(FALSE);
	}
}


void	CPhaseChannelList_AnnounceFrame::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	if( m_nNoticeIndex != -1) // 공지사항이 존재함
	{
		if(g_pFramework->getNoticeActivation() && !m_bClickedNoticeCaption)
		{
			m_fLocalNoticeTime += rDeltaSeconds;

			if( m_fLocalNoticeTime > 0.5f)
			{
				m_fLocalNoticeTime = 0.0f;

				m_pAnnounce[m_nNoticeIndex]->SetCaptionEnable( m_bEnableNoticeText);
				m_bEnableNoticeText = !m_bEnableNoticeText;
			}
		}
		else
		{
			m_pAnnounce[m_nNoticeIndex]->SetCaptionEnable( TRUE);
		}
	}

	// 공지 순환
	if( m_fAnnounceFlipTime > 0.f && g_pFramework->GetAnnounceCount() > 1)
	{
		if( m_fAnnounceFlipTime <= m_fAnnounceFilpElapsedTime)
		{
			m_nCurrentSelectedIndex++;

			if( m_nCurrentSelectedIndex >= g_pFramework->GetAnnounceCount())
				m_nCurrentSelectedIndex = 0;

			OnAnnounceButton(m_nCurrentSelectedIndex);

			m_fAnnounceFilpElapsedTime = 0.f;

			// 유저의 클릭으로 일정시간이 지났을 경우 다시 원래 순환 시간으로 설정
			if( m_fAnnounceFlipTime == g_pConfigEx->GetMisc().AnnounceWaitingTimeAfterClicked)
				m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceFlipTime;
		}
		else
		{
			m_fAnnounceFilpElapsedTime += rDeltaSeconds;
		}
	}	
}

void	CPhaseChannelList_AnnounceFrame::OnWheelUpdate(INT32 scrollby)
{
	if (m_pAnnounceWin->isEnable() && m_pScrollFrame->isEnable())
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pScrollList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pScrollList->MoveNext();
			}
		}
	}

}

void	CPhaseChannelList_AnnounceFrame::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
    {
	case GCI_SSP_B_ANNOUNCE_1:
	case GCI_SSP_B_ANNOUNCE_2:
	case GCI_SSP_B_ANNOUNCE_3:
	case GCI_SSP_B_ANNOUNCE_4:
	case GCI_SSP_B_ANNOUNCE_5:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			// 이미지 공지 클릭할 경우
			m_nCurrentSelectedIndex = pNotify->m_nID - GCI_SSP_B_ANNOUNCE_1;
			m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceWaitingTimeAfterClicked;
			m_fAnnounceFilpElapsedTime = 0.f;

			OnAnnounceButton(m_nCurrentSelectedIndex);
		}
		break;

	}
}

BOOL	CPhaseChannelList_AnnounceFrame::OnSliding(RT_REAL32 rDeltaSeconds, CPhaseChannelList * pOwner)
{
	m_pAnnounceWin->setPositionX(m_rAnnounceWinX - pOwner->GetSlideX());
	return TRUE;
}

void CPhaseChannelList_AnnounceFrame::OnEntranceStart(void)
{
	m_nCurrentSelectedIndex = 0;
	m_fAnnounceFilpElapsedTime = 0.f;

	m_fAnnounceFlipTime = g_pConfigEx->GetMisc().AnnounceFlipTime;

	OnAnnounceButton(m_nCurrentSelectedIndex);
}

void CPhaseChannelList_AnnounceFrame::OnEntranceEnd( CPhaseChannelList * pOwner )
{
	if ( g_pConfigEx->GetMisc().webAnnounce.IsActive() )
	{
		REAL32 widthRate = i3GuiRoot::getGuiResolutionWidth();
		REAL32 heightRate = i3GuiRoot::getGuiResolutionHeight();

		INT32 msgBoxOffsetX	= static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetX) * widthRate);
		INT32 msgBoxOffsetY	= static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetY) * heightRate);
		((CStageReady*)(pOwner->getParent()))->SetExitMsgBoxOffset(msgBoxOffsetX, msgBoxOffsetY);


		int	x		= static_cast<int>(g_pConfigEx->GetMisc().webAnnounce.posX * widthRate);
		int	y		= static_cast<int>(g_pConfigEx->GetMisc().webAnnounce.posY * heightRate);
		int	width	= static_cast<int>(g_pConfigEx->GetMisc().webAnnounce.sizeWidth * widthRate);
		int	height	= static_cast<int>(g_pConfigEx->GetMisc().webAnnounce.sizeHeight * heightRate);

		m_hWebAnnounce = CreateWebControl(g_pConfigEx->GetMisc().webAnnounce.URL.c_str(),
			WS_CLIPCHILDREN | WS_CHILD, x, y, width, height, g_pViewer->GetWindowHandle(), NULL);
		::ShowWindow(m_hWebAnnounce,SW_SHOW);
	}
}

void CPhaseChannelList_AnnounceFrame::OnExitStart( CPhaseChannelList * pOwner )
{
	((CStageReady*)(pOwner->getParent()))->SetExitMsgBoxOffset(0, 0);
	CloseAnnouce();
}

void CPhaseChannelList_AnnounceFrame::OnExitEnd(void)
{
	
}

void	CPhaseChannelList_AnnounceFrame::OnUpdateAnnounceImage(REAL32 rDeltaSeconds)
{
	if (m_pAnnounceWin->isEnable() && m_pScrollFrame->isEnable())
	{
		REAL32 scrollValue = m_pScrollList->getCurValue();
		INT32 y = (INT32)((m_announceImageHeight - 478) * scrollValue / 100.0f);
		y = MAX(0, y);

		m_pDummyAnnounceLong->SetShapeUV(GUI_SHAPE_OFFSET, 0, y, 359, 478);
	}
}


void	CPhaseChannelList_AnnounceFrame::ShowAnnounceShort(i3Texture* shortTexture)
{
	m_pScrollFrame->SetEnable(FALSE);
	m_pScrollList->SetEnable(FALSE);
	m_pDummyAnnounceLong->SetEnable(FALSE);

	m_pDummyAnnounceShort->ChangeFirstShape(shortTexture, 0, 0, 380, 478);
	m_pDummyAnnounceShort->SetEnable(TRUE);
}


void	CPhaseChannelList_AnnounceFrame::ShowAnnounceLong(i3Texture* longTexture,INT32 height)
{
	m_pDummyAnnounceShort->SetEnable(FALSE);

	// 478 px 이상 10px 마다 스크롤 1칸씩 가능.
	height = MAX(478, height);
	INT32 scrollCount = (height - 478) / 10;

	GameGUI::Reset(m_pScrollList, scrollCount);
	m_pScrollList->SetCurValue(0.0f);

	m_pScrollFrame->SetEnable(TRUE);
	m_pScrollList->SetEnable(TRUE);
	m_pDummyAnnounceLong->ChangeFirstShape(longTexture, 0, 0, 359, 478);
	m_pDummyAnnounceLong->SetEnable(TRUE);

	m_announceImageHeight = height;
}


void	CPhaseChannelList_AnnounceFrame::OnAnnounceButton(INT32 announceIdx)
{
	for(INT32 i = 0; i < MAX_ANNOUNCE_BUTTON; i++)
	{
		m_pAnnounce[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}

	m_pAnnounce[announceIdx]->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	if (m_bAnnounceScroll[announceIdx])
	{
		INT32 height = m_pAnnounceTexture[announceIdx]->GetHeight();
		ShowAnnounceLong(m_pAnnounceTexture[announceIdx], height);
	}
	else
	{
		ShowAnnounceShort(m_pAnnounceTexture[announceIdx]);
	}

	if( m_nNoticeIndex != -1)
	{
		if( announceIdx == m_nNoticeIndex) m_bClickedNoticeCaption = TRUE;
	}
}

void CPhaseChannelList_AnnounceFrame::CloseAnnouce()
{
	if ( m_hWebAnnounce )
	{
		DestroyWindow(m_hWebAnnounce);
		m_hWebAnnounce = NULL;
	}
}
