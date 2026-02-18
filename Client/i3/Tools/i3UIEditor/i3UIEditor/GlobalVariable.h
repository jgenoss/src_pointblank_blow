#pragma once
#include "i3UIEditorFramework.h"
#include "i3UIToolResManager.h"
#include "UIEditorProfile.h"
#include "PaneResList.h"
#include "i3UIControlTemplate.h"

extern i3GuiEditorFramework *	g_pFramework;

extern i3UIControlTemplate	*	g_pControlTemplate;
extern i3UIToolResManager	*	g_pResManager;
extern CUIEditorProfile	*		g_pProfile;

extern CPaneResList			*	g_pResList;

#define I3_TDK_USRWND_MAINFRAME		0x00000001
#define I3_TDK_USRWND_CTRLSHAPE		0x00000002
#define I3_TDK_USRWND_SCENEGRAPH	0x00000004
#define I3_TDK_USRWND_GAMEGRAPH		0x00000008
#define I3_TDK_USRWND_RESLIST		0x00000010
#define I3_TDK_USRWND_PREVIEW		0x00000020
#define I3_TDK_USRWND_LAYOUT		0x00000040
#define I3_TDK_USRWND_EVENTNACTION	0x00000080
#define I3_TDK_USRWND_PROPERTY		0x00000100
#define I3_TDK_USRWND_UVEDIT		0x00000200

void DeleteUnusedObjects();