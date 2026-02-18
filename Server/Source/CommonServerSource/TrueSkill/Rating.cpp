#include "pch.h"
#include "Rating.h"


CRating::CRating()
: mean_(DEFAULT_MEAN)
, standard_deviation_(DEFAULT_DEVIATION)
{
}

CRating::CRating(double mean, double standard_deviation)
: mean_(mean)
, standard_deviation_(standard_deviation)
{
}

CRating::~CRating()
{
}


void CRating::Initialize()
{
	mean_ = DEFAULT_MEAN;
	standard_deviation_ = DEFAULT_DEVIATION;
}
