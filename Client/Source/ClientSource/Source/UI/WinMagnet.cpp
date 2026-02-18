#include "pch.h"

#include "WinMagnet.h"

#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "Batmanager.h"

namespace detail
{
	enum { Vertical, Horizen, Max };
	typedef i3::pair< LONG, LONG > SideLine;
	i3::vector< LONG > g_line_list[Max]; //스크린 화면 좌표 값 리스트

	//모니터 정보 얻는 것
	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);

	void Init();
	void CalcMoveMagnet(LPWINDOWPOS pWindowPos);
	SideLine GetNextSideLine(INT32 dir, INT32 pos); //현재 위치 포지션에 대해서 dir 방향에 따른 가장 가까운 값(좌,우)을 반환한다.

	INT32 g_offset = 20;
	bool g_is_init = false;
}

void WinMagnet::CreateProc(INT32 offset)
{
	if( BaTMaN::use_batman == false ) return;

	detail::g_offset = offset;
	detail::Init();
}

void WinMagnet::WindowPosChangingProc(LPARAM lParam)
{
	if( BaTMaN::use_batman == false ) return;

	if( detail::g_is_init == false )
	{
		WinMagnet::CreateProc();
		detail::g_is_init = true;
	}

	detail::CalcMoveMagnet( (LPWINDOWPOS)lParam );
}

void WinMagnet::SetOffset(INT32 offset)
{
	detail::g_offset = offset;
}

namespace detail
{
	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData)
	{
		MONITORINFOEX mi;
		mi.cbSize=sizeof(MONITORINFOEX);

		::GetMonitorInfo(hMonitor,&mi);

		detail::g_line_list[Horizen].push_back(mi.rcWork.left);
		detail::g_line_list[Horizen].push_back(mi.rcWork.right);

		detail::g_line_list[Vertical].push_back(mi.rcWork.top);
		detail::g_line_list[Vertical].push_back(mi.rcWork.bottom);

		return true;
	}

	void Init()
	{
		for(int i=0;i<detail::Max;++i)
		{
			detail::g_line_list[i].clear();
		}

		::EnumDisplayMonitors(NULL,NULL, detail::MonitorEnumProc,0);

		for(int i=0;i<detail::Max;++i)
		{
			i3::sort( detail::g_line_list[i].begin(), detail::g_line_list[i].end() );
			i3::vector< LONG >::iterator it  = std::unique( detail::g_line_list[i].begin(), detail::g_line_list[i].end() );
			detail::g_line_list[i].resize( it - detail::g_line_list[i].begin() );			
		}
	}

	SideLine GetNextSideLine(INT32 dir, INT pos)
	{
		const i3::vector< LONG > & list = g_line_list[dir];

		i3::vector< LONG >::const_iterator it = list.begin(), before;
		i3::vector< LONG >::const_iterator end = list.end();
		before = it;

		if( pos < *it )	
			return SideLine( *it, LONG_MIN );

		while( it != end )
		{
			if( *it >= pos && *before <= pos )
			{
				return SideLine( *it, *before );
			}
			before = it;
			++it;
		}

		return SideLine( LONG_MAX, *before );
	}

	void CalcMoveMagnet(LPWINDOWPOS pWindowPos)
	{
		detail::SideLine side_line;
		LONG line;

		line = pWindowPos->x;
		side_line = detail::GetNextSideLine(detail::Horizen, line );
		if( abs(line - side_line.second) < detail::g_offset )
			pWindowPos->x = side_line.second;

		line = pWindowPos->x + pWindowPos->cx;
		side_line = detail::GetNextSideLine(detail::Horizen, line );
		if( abs(line - side_line.first) < detail::g_offset )
			pWindowPos->x = side_line.first - pWindowPos->cx;

		line = pWindowPos->y;
		side_line = detail::GetNextSideLine(detail::Vertical, line );
		if( abs(line - side_line.second) < detail::g_offset )
			pWindowPos->y = side_line.second;

		line = pWindowPos->y + pWindowPos->cy;
		side_line = detail::GetNextSideLine(detail::Vertical, line );
		if( abs(line - side_line.first) < detail::g_offset )
			pWindowPos->y = side_line.first - pWindowPos->cy;
	}
}
