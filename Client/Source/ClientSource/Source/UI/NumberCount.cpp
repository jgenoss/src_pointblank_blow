#include "pch.h"

#include "NumberCount.h"
#include "UIMath.h"
#include <functional>

/*****************************//*****************************/

Number::Number() : m_ImgBox(0), m_Enable(0)
{
}

Number::~Number()
{
}

void Number::SetControl(i3UIScene * pScene, const char* name)
{
	i3GameNode* node = pScene->FindChildByName( name );

	if( !node )
	{
		I3PRINTLOG(I3LOG_FATAL, "ImageBox Control 이 없음.. i3Uie 파일 확인");
	}

	m_ImgBox = (i3UIButtonImageSet*)node;
}

void Number::SetControl(i3UIButtonImageSet* ctrl)
{
	m_ImgBox = ctrl;
}

void Number::Enable(bool enable)
{
	m_ImgBox->EnableCtrl(enable);
}

bool Number::IsEnable() const
{
	return (m_ImgBox->isEnable() == true) ? true : false;
}

void Number::SetNumber(INT32 num)
{
	m_ImgBox->SetShapeIdx(num);
}

INT32 Number::GetNumber() const
{
	return m_ImgBox->GetShapeIdx();
}



/*****************************//*****************************/
NumberCount::NumberCount() : m_is_show_zero(1), m_is_show_front_zero(0), m_is_center_pos(0), m_Round_ctrl(0)
{
	m_CurrNum = INT_MIN;
	m_range.mini = INT_MAX;
	m_range.maxi = INT_MIN;
	m_max_width = INT_MIN;
	m_initial_offset = m_width_offset = 0;
}

NumberCount::~NumberCount()
{
	struct Fn	{ void operator()(Number*p) { I3_SAFE_DELETE( p); }	};
	for_each( m_list.begin(), m_list.end(), Fn() );
	m_list.clear();

	I3_SAFE_DELETE( m_Round_ctrl);
}

void NumberCount::get_initial_position()
{
	m_range.mini = INT_MAX;
	m_range.maxi = INT_MIN;
	m_max_width = INT_MIN;

	size_t size = m_list.size();
	for(size_t i=0; i<size;++i)
	{
		Number * pctrl = m_list[i];
		i3::pack::RECT rect;
		pctrl->GetControl()->getRect(&rect);

		if(m_max_width < rect.Width() -1 ) m_max_width = (INT32)rect.Width() -1;

		if(m_range.mini > rect.left )	m_range.mini = (INT32)rect.left;
		if(m_range.maxi < rect.right)	m_range.maxi = (INT32)rect.right;
	}
}

void NumberCount::re_position()
{
	size_t enable_cnt = 0;

	size_t size = m_list.size();
	for(size_t i=0; i<size;++i)
	{
		if( m_list[i]->IsEnable() )
			++enable_cnt;
	}

	INT32 hor = m_range.maxi - m_range.mini;
	INT32 w = (m_max_width * enable_cnt) + (m_width_offset * (enable_cnt-1));
	INT32 offset = (hor - w) / 2;

	INT32 x = m_range.mini + offset + m_initial_offset;

	for(size_t i=enable_cnt-1; (signed)i>=0;--i)
	{
		i3UIButtonImageSet* pBtn = m_list[i]->GetControl();
		VEC3D* pos = pBtn->getPos();
		pBtn->setPos((REAL32)x, pos->y);

		x += (m_width_offset+m_max_width);
	}
}

void NumberCount::ClearNumber()
{
	struct Fn	{ void operator()(Number*p) { I3_SAFE_DELETE( p); }	};
	for_each( m_list.begin(), m_list.end(), Fn() );

	m_list.clear();
}

void NumberCount::SetVisible(bool visible)
{
	size_t size = m_list.size();
	for(size_t i=0; i<size;++i)
	{
		m_list[i]->Enable(visible);
	}
}

void NumberCount::EnableDynamicCenter()
{ 
	m_is_center_pos = true;
	get_initial_position();
}

void NumberCount::SetWidthOffset(INT32 offset)
{ 
	m_width_offset = offset;
	re_position();
}

void NumberCount::InitialPosOffset(INT32 x)
{ 
	m_initial_offset = x;
	re_position();
}

void NumberCount::AddNumberControl(i3UIScene * pScene, const char* name)
{
	Number * pctrl = new Number;
	MEMDUMP_NEW( pctrl, sizeof(Number));

	pctrl->SetControl(pScene, name);
	m_list.push_back( pctrl );
}

void NumberCount::AddNumberControl(i3UIButtonImageSet* ctrl)
{
	Number * pctrl = new Number;
	MEMDUMP_NEW( pctrl, sizeof(Number));

	pctrl->SetControl(ctrl);
	m_list.push_back( pctrl );
}

void NumberCount::RemoveNumberControl(size_t idx)
{	
	Number* remove_ctrl = DetchNumberControl(idx);
	I3_SAFE_DELETE( remove_ctrl);
}

Number* NumberCount::DetchNumberControl(size_t idx)
{
	Number* ret = m_list[idx];

	i3::vector< Number* >::iterator it = m_list.begin();
	std::advance(it, idx);
	m_list.erase(it);

	if( m_is_center_pos )
		get_initial_position();

	return ret;
}

void NumberCount::AttachNumberControl(size_t idx, Number* ctrl)
{
	i3::vector< Number* >::iterator it = m_list.begin();
	std::advance(it, idx);
	m_list.insert(it, 1, ctrl);

	if( m_is_center_pos )
		get_initial_position();
}

namespace
{
	void partition(i3::vector<INT32> & list, INT32 num)
	{
		if( num < 0 ) return; //음수는 안 된다.

		while(num)
		{
			int mod = num %10;
			list.push_back( mod );
			num = static_cast<INT32>(num * 0.1f);
		}
	}
	INT32 merge(const i3::vector<INT32> & list)
	{
		INT32 ret = 0;
		size_t size = list.size();
		for(size_t i=size-1; (INT32)i>=0; --i)
		{
			if( list[i] != 0 ) 
				ret = (ret *10) + list[i];	
		}
		return ret;
	}
}

void NumberCount::SetNumber(INT32 num)
{
	if( m_CurrNum == num ) return;
	m_CurrNum = num;

	if(num == 0)
	{
		size_t size = m_list.size();
		for(size_t i=0;i<size;++i)
		{
			if( m_is_show_zero )
			{
				if( m_is_show_front_zero || i == 0 )
				{
					m_list[i]->Enable(true);
					m_list[i]->SetNumber( 0 );
				}
				else if( !m_is_show_front_zero )
				{
					m_list[i]->Enable(false);
				}
			}
			else
			{
				m_list[i]->Enable(false);
			}
		}
		
		if(m_is_show_zero && m_is_center_pos) re_position();
		return;
	}

	i3::vector<INT32> list;
	partition(list, num);
	size_t max = list.size();

	size_t size = m_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( i < max )
		{
			m_list[i]->Enable(true);
			m_list[i]->SetNumber( list[i] );
		}
		else
		{
			if(m_is_show_front_zero)
			{
				m_list[i]->Enable(true);
				m_list[i]->SetNumber( 0 );
			}
			else
			{
				m_list[i]->Enable(false);
			}
		}
	}

	if(m_is_show_zero && m_is_center_pos) re_position();
}

INT32 NumberCount::GetNumber()
{
	i3::vector<INT32> list;

	size_t size = m_list.size();
	for(size_t i=0;i<size;++i)
	{
		list[i] = m_list[i]->GetNumber();
	}

	return merge(list);
}

void NumberCount::ShowRound()
{
	i3::vector< Number* >::iterator it = m_list.begin();
	m_Round_ctrl = *it;
	m_list.erase(it);

	enum { ROUND_IMG_IDX = 10, };

	m_Round_ctrl->SetNumber(ROUND_IMG_IDX);
	m_Round_ctrl->Enable(true);

	if( m_is_center_pos )
		get_initial_position();
}

void NumberCount::HideRound()
{
	if( m_Round_ctrl == 0 ) return;

	AttachNumberControl(0, m_Round_ctrl);
	m_Round_ctrl = 0;
}

void NumberCount::SetAlpha(UINT8 alpha)
{
	struct Fn : std::binary_function<Number*, UINT8, void>
	{
		void operator()(Number* p, UINT8 a) const
		{
			COLOR* col = p->GetControl()->getColor(); 
			col->a = a;
			p->GetControl()->setColor(col);
		}
	};

	std::for_each(m_list.begin(), m_list.end(), std::bind2nd( Fn(), alpha) );
}

void NumberCount::SetColor(UINT8 r, UINT8 g, UINT8 b)
{
	size_t size = m_list.size();
	for(size_t i=0;i<size;++i)
	{
		COLOR* col = m_list[i]->GetControl()->getColor(); 
		col->r = r; col->g = g; col->b = b;
		col->a = 255;
		m_list[i]->GetControl()->setColor(col);
	}
}

const VEC3D* NumberCount::GetNumberPos() const
{
	if (m_list.empty()) 
		return nullptr;

	return m_list[0]->GetControl()->getAccumulatedPos();
}

REAL32 NumberCount::GetNumberHeight() const
{
	if (m_list.empty())
		return 0.0f;

	return m_list[0]->GetControl()->getHeight();
}

/*****************************//*****************************/
ColorNumberCount::ColorNumberCount() : NumberCount()
{
	m_Min = m_Max = 0;
}

ColorNumberCount::~ColorNumberCount()
{
}

const COLOR* ColorNumberCount::find_color(int spot)
{
	//m_list는 올림차순(작은->큰)
	list::iterator it = m_color_list.begin(), before;
	list::iterator end = m_color_list.end();
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

void ColorNumberCount::AddMaxColorSpot(int r, int g, int b)
{
	m_color_list.insert( spot(INT_MAX,r,g,b) );
}

void ColorNumberCount::AddColorSpot(int split_spot, int r, int g, int b)
{
	m_color_list.insert( spot(split_spot,r,g,b) );
}

void ColorNumberCount::SetRange(int min, int max)
{
	m_Min = min; 
	m_Max = max;
}

void ColorNumberCount::SetNumber(int num)
{
	SetNumber(num, m_Min, m_Max);
}

void ColorNumberCount::SetNumber(int num, int min, int max)
{
	//if(num == m_CurrNum) return;

	NumberCount::SetNumber(num);

	const COLOR* color = find_color(num);
	NumberCount::SetColor(color->r, color->g, color->b);
}
