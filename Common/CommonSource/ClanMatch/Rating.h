#pragma once
#define CLAN_BASIC_POINT		1000.f		// 최초 클랜 포인트
#define DEFAULT_DEVIATION		60.f		// CLAN_BASIC_POINT / 3.0
#define DYNAMIC_FACTOR			3.f			// CLAN_BASIC_POINT / 300.0
#define BETA					200.f		// CLAN_BASIC_POINT / 6.0
#define DRAW_PROBABILITY		0.10f		// 0.00 ~ 0.25

#pragma pack( push, 1)

class CRating
{
public:
	void		Initialize()
	{
		mean_ = CLAN_BASIC_POINT;
		standard_deviation_ = DEFAULT_DEVIATION;
	}

	double		GetClanPoint(void) { return mean_; }
	double		GetDeviation(void) { return standard_deviation_; }
	void		IncClanPoint(double IncPoint) { mean_ += IncPoint; }

public:
	CRating() : mean_(CLAN_BASIC_POINT), standard_deviation_(DEFAULT_DEVIATION)									{}
	CRating(double mean, double standard_deviation) : mean_(mean), standard_deviation_(standard_deviation)	{}
	virtual ~CRating()																						{}

	const bool operator <(const CRating& rhs)			{ return mean_ < rhs.mean_; }
	const bool operator >(const CRating& rhs)			{ return mean_ > rhs.mean_; }

	CRating& operator +=(const CRating& rhs)			{ mean_ += rhs.mean_; standard_deviation_ += rhs.standard_deviation_; return *this; }
	CRating& operator -(const CRating& rhs)				{ mean_ = abs(mean_ - rhs.mean_); standard_deviation_ = abs(standard_deviation_ - rhs.standard_deviation_); return *this; }


public:
	double		mean_;
	double		standard_deviation_;
};

#pragma pack( pop)
