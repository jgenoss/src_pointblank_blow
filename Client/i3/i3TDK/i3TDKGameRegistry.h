#if !defined( __I3_TDK_GAME_REGISTRY_H)
#define __I3_TDK_GAME_REGISTRY_H

/*
update  : 2006-8-4
make by : komet
*/

#define  REG_FILE_EXT				"i3Reg"

#define	 REG_CONFIG_ROOT			"Config"
#define  REG_OPTIONAL_ROOT			"Optional"			//	i3Registry에 들어가는 Option 이름
#define  REG_LAYER_ROOT				"SgLayer"			//	i3Registry에 들어가는 i3SgLayer의 Root 이름
#define  REG_STAGEINFO_ROOT			"StageInfo"			//	i3Registry에 들어가는 i3StageInfo의 Root 이름



struct RegData
{
	char			_szWorkingDir[256] = { 0 };		//	작업 디렉토리
	char			_szRSCFileName[256] = { 0 };
	i3Framework *	_pi3Framework = nullptr;

} ;


namespace i3TDKGameRegistry
{	
	/*
	The Hierarchy inside i3Registry File

	RegstrySet
		|
	[Root] (RegKey)
		|
		--- ["Optional"] (RegKey)
		|		|
		|		--- Working directory (RegString)
		|
		|
		--- ["SgLayer"] (RegKey)
				|
				--- Layer Name (1) (RegString)
				|
				--- Layer Name (2) (RegString)
				|
				:		:				:
				|
				--- Layer Name (n) (RegString)
	*/

	I3_EXPORT_TDK BOOL		Make( RegData * pRegData);				//	i3Framework의 Registy만 만든다.
	I3_EXPORT_TDK UINT32	Save( const char * pszFileName);
	I3_EXPORT_TDK UINT32	Load( const char * pszFileName);
	I3_EXPORT_TDK void		Destroy( void);

	//	i3Framework의 Registry를 만들고 파일을 같이 생성한다.
	I3_EXPORT_TDK BOOL		Create( const char * pszFileName, RegData * pRegData);
};

#endif //__I3_TDK_GAME_REGISTRY_H