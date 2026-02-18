#pragma once

class i3TextNodeDX2;
class i3Framework;

#include "i3Base/string/i3string.h"

namespace i3 
{
	class occ_statistics_callback;
}


class I3_EXPORT_FRAMEWORK OccDebugInfo
{
public:
	OccDebugInfo();
	~OccDebugInfo();

	bool IsTextEnabled() const { return m_bTextEnabled;  }
	void SetTextEnable(bool bEnable);

	void SetAABBRenderEnable(bool bEnable);
	bool IsAABBRenderEnabled() const;

	void SetWorldQuadRenderEnable(bool bEnable);
	bool IsWorldQuadRenderEnabled() const;

	bool HasWorldOccQuads() const;

	void Create();

	void OnChangeFramework(i3Framework* pFrame);
	void setDebugTextPos(REAL32 x, REAL32 y);
	
	void ClearText();
	i3::occ_statistics_callback*	GetCallback() const { return m_callback;  }
	
		
private:
		
	i3TextNodeDX2*				m_pDebugTextNode = nullptr;
	i3::occ_statistics_callback* m_callback;

	bool						m_bTextEnabled = false;
	bool						m_bAABBRenderEnabled = false;
	bool						m_bWorldQuadRenderEnabled = false;
};
