#pragma once


#define DEFAULT_RATING			1000.0
#define DYNAMIC_FACTOR			DEFAULT_RATING / 300.0
#define BETA					DEFAULT_RATING / 6.0
#define DRAW_PROBABILITY		0.10

class CRating
{
public:
	void		Initialize();

public:
	CRating();
	CRating(double mean, double standard_deviation);
	virtual ~CRating();

	const bool operator <(const CRating& rhs)			{ return mean_ < rhs.mean_; }
	const bool operator >(const CRating& rhs)			{ return mean_ > rhs.mean_; }

	CRating& operator +=(const CRating& rhs)			{ mean_ += rhs.mean_; standard_deviation_ += rhs.standard_deviation_; return *this; }
	CRating& operator -(const CRating& rhs)				{ mean_ = abs(mean_ - rhs.mean_); standard_deviation_ = abs(standard_deviation_ - rhs.standard_deviation_); return *this; }


public:
	double		mean_;
	double		standard_deviation_;
};

