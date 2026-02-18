#if !defined( _GAME_H__)
#define _GAME_H__

void ExitGame();
bool PreCreateGame();
bool InitGame( void * pPlatformData);
bool DriveGame();
bool CloseGame(void);
void CreateSplashWindow(void);
void DestroySplashWindow(void);
void RemoveResourceManager(void);
bool CreateGlobal(void);
bool CreateGlobal2(void); 
bool CreateUserFont(void);
bool RemoveUserFont(void);
void MakeVersion(void);
bool LoadDesignationValue();
bool LoadMissionCard();

//Init Functions
bool InitFramework(void * pPlatformDat);
bool InitSound(void * pPlatformDat);
extern bool InitManagers();
extern void InitSound(void);
//extern void LoadStageNameFromPef(void);	// revision 29591
UINT32 GetItemIndex( LPCSTR szName);
UINT32 GetItemCountThrowItem(void);

UINT32 GetUITemplateIndex_Chara(LPCSTR szName);
UINT32 GetUITemplateIndex_Item(LPCSTR szName);
UINT32 GetUITemplateIndex_Parts(LPCSTR szName);

void FindDll();

#endif //_GAME_H__