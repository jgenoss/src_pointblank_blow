#include "pch.h"

#include "ProgressBar.h"
#include "UIMath.h"
#include "GameGUI.h"

/*****************************//*****************************/
//iColorProgressBar
iColorProgressBar::iColorProgressBar() : m_ProgressBar(0)
{
}

iColorProgressBar::~iColorProgressBar()
{

}

void iColorProgressBar::SetControl(i3UIScene * pScene, const char* progressbar)
{
	i3GameNode* node1 = pScene->FindChildByName( progressbar );
	if( !node1 )
	{
		I3PRINTLOG(I3LOG_FATAL, "progressbar Control 이 없음.. i3Uie 파일 확인");
	}
	m_ProgressBar = (i3UIProgressBar*)node1;
}

void iColorProgressBar::SetControl(i3UIControl* progressbar)
{
	m_ProgressBar = (i3UIProgressBar*)progressbar;
}

const COLOR* iColorProgressBar::find_color(int spot)
{
	//m_list는 올림차순(작은->큰)
	list::iterator it = m_list.begin(), before;
	list::iterator end = m_list.end();
	before = it++;

	if( spot <= before->split_spot ) return &before->col;
	
	while( it != end )
	{
		if( before->split_spot < spot && spot <= it->split_spot )
			return &it->col;

		before = it;
		++it;
	}

	return &before->col;
}

void iColorProgressBar::SetEnable(bool enable)
{
	m_ProgressBar->EnableCtrl( enable );
}

void iColorProgressBar::AddMaxColorSpot(int r, int g, int b)
{
	m_list.insert( spot(INT_MAX,r,g,b) );
}

void iColorProgressBar::AddColorSpot(int split_spot, int r, int g, int b)
{
	m_list.insert( spot(split_spot,r,g,b) );
}



/*****************************//*****************************/
//IntColorProgressBar
IntColorProgressBar::IntColorProgressBar() : iColorProgressBar()
{
	m_Min = m_Max = 0;
	m_CurrNum = INT_MIN;
}

void IntColorProgressBar::SetRange(int min, int max)
{
	m_Min = min; 
	m_Max = max;
}

void IntColorProgressBar::SetNumber(int num)
{
	SetNumber(num, m_Min, m_Max);
}

void IntColorProgressBar::SetNumber(int num, int min, int max)
{
	if(num == m_CurrNum) return;

	m_CurrNum = num;
	REAL32 factor = uiMath::inverse_interpolate( num, min, max);
	m_ProgressBar->setProgress(factor);
	m_ProgressBar->setColor( const_cast<COLOR*>(find_color(num)) );
}



/*****************************//*****************************/
//RealColorProgressBar
RealColorProgressBar::RealColorProgressBar() : iColorProgressBar()
{
	m_Min = 0;
	m_Max = FLOAT_FACTOR;
	m_CurrNum = INT_MIN;
}

void RealColorProgressBar::SetRange(REAL32 min, REAL32 max)
{
	m_Min = (int)(min * FLOAT_FACTOR); 
	m_Max = (int)(max * FLOAT_FACTOR);
}

void RealColorProgressBar::SetNumber(REAL32 num)
{
	int tmp_num = (int)(num * FLOAT_FACTOR);

	if(tmp_num == m_CurrNum) return;

	m_CurrNum = tmp_num;
	REAL32 factor = uiMath::inverse_interpolate( tmp_num, m_Min, m_Max);
	m_ProgressBar->setProgress(factor);

	int spot = (int)(factor * FLOAT_INT);
	m_ProgressBar->setColor( const_cast<COLOR*>(iColorProgressBar::find_color(spot)) );
}

void RealColorProgressBar::SetNumber(REAL32 num, REAL32 min, REAL32 max)
{
	int tmp_num = (int)(num * FLOAT_FACTOR);
	int tmp_min = (int)(min * FLOAT_FACTOR);
	int tmp_max = (int)(max * FLOAT_FACTOR);

	if(tmp_num == m_CurrNum) return;

	m_CurrNum = tmp_num;
	REAL32 factor = uiMath::inverse_interpolate( tmp_num, tmp_min, tmp_max);
	m_ProgressBar->setProgress(factor);

	int spot = (int)(factor * FLOAT_INT);
	m_ProgressBar->setColor( const_cast<COLOR*>(iColorProgressBar::find_color(spot)) );
}