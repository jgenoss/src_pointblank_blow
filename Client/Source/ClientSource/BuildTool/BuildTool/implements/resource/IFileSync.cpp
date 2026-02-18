#include "stdafx.h"
#include "IFileSync.h"

namespace rsc
{
	IFileSync::IFileSync() : m_callback(std::make_shared<FileSyncCallback>())
	{}

	IFileSync::~IFileSync() 
	{}
}