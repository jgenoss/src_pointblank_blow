#if !defined( __UI_LB_NumberCount_H__)
#define __UI_LB_NumberCount_H__

/*	i3UIButtonImageSet를 이용해서 number를 표시한다.
 **/
class Number
{
	i3UIButtonImageSet* m_ImgBox;
	bool m_Enable;
	
public:
	Number();
	~Number();

	void SetControl(i3UIScene * pScene, const char* name);
	void SetControl(i3UIButtonImageSet* ctrl);
	i3UIButtonImageSet* GetControl()	{ return m_ImgBox; }

	void SetNumber(INT32 num);
	INT32 GetNumber() const;

	void Enable(bool enable);
	bool IsEnable() const;
};

/*	i3UIButtonImageSet를 이용해서 number를 표시한다.
 *		AddNumberControl에 따라서 자릿수를 나타낸다. 첫번째가 1의 자리, 두번째가 10의 자리이다.
 *		중점을 위치로 자동 위치 조정 된다.
 *		EnableShowFrontZero 함수 : //001 -> 1 로 표시 유무
 *		EnableShowZero 함수 : 0 을 표시 유무
  **/
class NumberCount
{
public:
	NumberCount();
	virtual ~NumberCount();

	void ClearNumber();

	void SetVisible(bool visible);

	void AddNumberControl(i3UIScene * pScene, const char* name);
	void AddNumberControl(i3UIButtonImageSet* ctrl);

	void RemoveNumberControl(size_t idx);
	Number* DetchNumberControl(size_t idx);
	void AttachNumberControl(size_t idx, Number* ctrl);

	void SetNumber(INT32 num);
	INT32 GetNumber();
	void ResetDisplay() { m_CurrNum = INT_MIN; }

	void EnableShowFrontZero(bool show) { m_is_show_front_zero = show; }	//1 -> 001 로 표시 유무
	void EnableShowZero(bool show) { m_is_show_zero = show; }	//0(zero) 을 표시 유무
	
	void EnableDynamicCenter(); //center를 중심으로 number 가 보여진다.
	void DisableDynamicCenter();

	void SetWidthOffset(INT32 offset); //Control 사이 넓이.
	void InitialPosOffset(INT32 x); //모든 Control을 이동한다.

	void ShowRound();
	void HideRound();

	void SetColor(UINT8 r, UINT8 g, UINT8 b);
	void SetAlpha(UINT8 alpha); //0(투명) ~255(불투명) 값

	const VEC3D* GetNumberPos() const;
	REAL32 GetNumberHeight() const;

private:
	void get_initial_position();
	void re_position();

private:
	i3::vector< Number* > m_list;	//0인덱스가 1자리
	Number* m_Round_ctrl;
	INT32 m_CurrNum;
	bool m_is_show_zero;	//0 표시 유무
	bool m_is_show_front_zero;	//01, 1 표시 유무
	bool m_is_center_pos;

	struct context
	{
		INT32 mini;
		INT32 maxi;
	};
	context m_range;
	INT32 m_max_width;
	INT32 m_initial_offset, m_width_offset;
};

#include <set>

class ColorNumberCount : public NumberCount
{
public:
	ColorNumberCount();
	virtual ~ColorNumberCount();

public:
	void SetRange(int min, int max);
	void SetNumber(int num);		//SetRange 이 함수가 선행 되어야 한다.
	void SetNumber(int num, int min, int max);

	void AddMaxColorSpot(int r, int g, int b); //최대값 색상
	void AddColorSpot(int split_spot, int r, int g, int b);	//split_spot 부터(포함) 해당 색상으로 변경(아래쪽)

private:
	const COLOR* find_color(int spot);

private:
	struct spot
	{
		spot(int s, int r, int g, int b) : split_spot(s) { col.a = 255; col.r = (UINT8)r; col.g = (UINT8)g; col.b = (UINT8)b; }
		int split_spot; COLOR col;
	};

	struct comp
	{
		bool operator()( const spot& lhs, const spot & rhs ) const 
		{ return lhs.split_spot < rhs.split_spot; }
	};

	typedef std::set< spot, comp > list;
	list m_color_list;

private:
	INT32 m_Min, m_Max, m_CurrNum;
};

#endif