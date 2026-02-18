#include "pch.h"

#if 지워질_것들

#include "GameButton.h"
#include "GlobalVariables.h"

//I3_CONCRETE_CLASS_INSTANCE( CGameButton, i3GuiButton);

I3_CLASS_INSTANCE( CGameButton);

CGameButton::CGameButton(void)
{

}

CGameButton::~CGameButton( void)
{

}

void CGameButton::OnInitAfterLoad()
{
	i3GuiButton::OnInitAfterLoad();

	/*if( getEventData( I3GUI_CONTROL_STATE_CLICKED) == NULL)
	{
		I3GUI_EVENT_DATA * pNewEventData = I3GUI_EVENT_DATA::new_object_ref();
		pNewEventData->Init();

		i3SoundNode * pNode = i3SoundNode::new_object();
		i3SoundPlayInfo * pNewInfo = i3SoundPlayInfo::new_object_ref();

		i3GameResSound * pRes = (i3GameResSound *) g_pFramework->FindResourceByOnlyName( "UI_Button_Click.wav");

		I3ASSERT( pRes != NULL);

		i3SoundPlayInfo * pSndResInfo = pRes->getSound();
		if( pSndResInfo != NULL)
		{
			pSndResInfo->CopyTo( pNewInfo, I3_COPY_INSTANCE);
		}

		pNode->setSoundPlayInfo( pNewInfo);

		pNewEventData->m_pSoundNode		= pNode;
		pNewEventData->m_nEventStyle	= I3GUI_EVENT_SOUND;

		SetEventData( I3GUI_CONTROL_STATE_CLICKED, pNewEventData);
	}*/
}
#endif