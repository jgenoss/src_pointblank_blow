#pragma once

#include "CustomUGCtrl.h"
#include "ugctrl/ugctelps.h"

namespace ui
{
	class ShaderList;

	struct IShaderListPrintable
	{
		virtual ~IShaderListPrintable() {}
		virtual void	OnPrintShaderList(CUGCtrl* ctrl, const std::string& stri3EngineAbsPath) = 0;
	};

	class ShaderSyncDlg;

	class ShaderList final
	{
	public:
		ShaderList();
		virtual ~ShaderList();

		void AdjustLayout(const RECT& rt);
		void CreateGridCtrl(ShaderSyncDlg* owner, const int id, CustomUGCtrlCallbackPtr cb);
		void OnShowShaderList(const std::string& stri3EngineAbsPath);

	private:
		CustomUGCtrl m_ctrl;
		CUGEllipsisType m_ellipType;

		CPen m_penIvoryBlack;
		CPen m_penIvoryBlackLight;
		CPen m_penOrange;

		long m_currSelectionRow = -1;

		friend struct FileShaderListCallback;
		friend struct MemoryShaderListCallback;
	};
}