#pragma once



class i3RegKey;
struct i3SGTexChangeInfo;


class I3_EXPORT_SCENE i3SGTexChangeManager : public i3::shared_ginst<i3SGTexChangeManager>
{
public:
	~i3SGTexChangeManager();
	
	bool						LoadRegistryFile(const char* strFilename);									// 콘피그 설정때 로딩 함수만 호출 잘하면 된다..
	const i3SGTexChangeInfo*	FindTexChangeInfo(const char* i3sFilePath) const;								// 일단은 Weapon부터 시작하는 경로/파일명..

	static i3::rc_string		FindTargetTexturePath( const i3SGTexChangeInfo* info, const char* src_texture_path);
	static i3::rc_string		FindLODTargetTexturePath(const i3SGTexChangeInfo* info, INT32 lod_idx, const char* src_texture_path);
	static INT32				GetNumLOD(const i3SGTexChangeInfo* info);


	static i3::rc_string		GetRefi3sFilename(const i3SGTexChangeInfo* info);	
	static i3::rc_string		GetTgti3sFilename( const i3SGTexChangeInfo* info);

	static void					GatherAllTargetTexturePaths(const i3SGTexChangeInfo* info, i3::vector<i3::rc_string>& out_texture_paths);

	void						Clear();
private:
	void						FillTexturePairs(const i3RegKey* tgt_key, i3SGTexChangeInfo* pInfo);

	i3::unordered_map<i3::rc_string, i3SGTexChangeInfo*>	m_clonemap;
};
