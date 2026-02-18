#if !defined ( I3_GAME_EDITOR_PROFILE_H)
#define I3_GAME_EDITOR_PROFILE_H

struct GameEditorProfile
{
	char _szWorkDir[256] = { 0 };		//	작업 디렉토리
	char _szRSCGenFile[256] = { 0 };	//	RSCGen File
	
	void SetWorkDir( const char * pszPath)	{	i3::safe_string_copy( _szWorkDir, pszPath, 256 );		}
	const char * GetWorkDir( void)			{	return _szWorkDir;							}

	void SetRSCFile( const char * pszPath)	{	i3::safe_string_copy( _szRSCGenFile, pszPath, 256 );	}
	const char * GetRSCFile( void)			{	return _szRSCGenFile;						}

	void OnLoad( void);
	void OnSave( void);

} ;



#endif //I3_GAME_EDITOR_PROFILE_H

