#pragma once

#include "i3NavMeshUtil.h"

class CAIStage;
class CAIStageArray;

class i3NavMeshDataMgr;

class I3_EXPORT_TDK i3NavMeshFile : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshFile, i3ElementBase );

public:
	i3NavMeshFile();
	~i3NavMeshFile();

public:
	bool		LoadNM( const i3::string& _path, i3NavMeshDataMgr* dataMgr );
	bool		SaveNM( const i3::string& _path, const i3NavMeshDataMgr* dataMgr );

	bool		LoadASE( const i3::string& _path, i3NavMeshDataMgr* dataMgr );
	bool		SaveASE( const i3::string& _path, const i3NavMeshDataMgr* dataMgr );

private:
	void		_ASE_AddString( i3::string& szSrc, const i3::string& szAdd, bool bReturn = true, int tabCnt = 0 );
	void		_ASE_AddString( i3::wstring& szSrc, const i3::wstring& szAdd, bool bReturn = true, int tabCnt = 0 );

private:
	CAIStageArray*		m_pNMParser;		// [initialize at constructor]
};