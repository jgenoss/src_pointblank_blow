#pragma once
#include "i3GuiEditorFramework.h"
#include "i3GuiToolResManager.h"
#include "GuiEditorProfile.h"
#include "PaneResList.h"

extern i3GuiEditorFramework * g_pFramework;
extern i3GuiRoot			* g_pRoot;

extern i3GuiControlTemplate	*	g_pControlTemplate;
extern i3GuiToolResManager	*	g_pResManager;
extern CGuiEditorProfile	*	g_pProfile;

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