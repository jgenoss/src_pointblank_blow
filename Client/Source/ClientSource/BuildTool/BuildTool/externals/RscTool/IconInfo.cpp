#include "stdafx.h"
#include "IconInfo.h"


int GetFolderItemIconIndex()
{
	//	SHFILEINFO sfi;
	//	SHGetFileInfo(L"?", FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi), 
	//		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_LINKOVERLAY);

	SHSTOCKICONINFO ssii;
	ssii.cbSize = sizeof(SHSTOCKICONINFO);

	SHGetStockIconInfo(SIID_FOLDER, SHGSI_SYSICONINDEX | SHGSI_SMALLICON, &ssii);		// 이 함수는 비스타이상에서만 지원..

	return ssii.iSysImageIndex;
}

int GetFolderItemIconIndex_LinkOverlay()
{
	SHSTOCKICONINFO ssii;
	ssii.cbSize = sizeof(SHSTOCKICONINFO);

	SHGetStockIconInfo(SIID_FOLDER, SHGSI_SYSICONINDEX | SHGSI_SMALLICON | SHGSI_LINKOVERLAY, &ssii);	// 이 함수는 비스타이상에서만 지원..

	return ssii.iSysImageIndex;
}

int GetFolderItemIconIndex_Open()
{
	SHSTOCKICONINFO ssii;
	ssii.cbSize = sizeof(SHSTOCKICONINFO);

	SHGetStockIconInfo(SIID_FOLDER, SHGSI_SYSICONINDEX | SHGSI_SMALLICON | SHGSI_SELECTED, &ssii);		// 이 함수는 비스타이상에서만 지원..

	return ssii.iSysImageIndex;

}

