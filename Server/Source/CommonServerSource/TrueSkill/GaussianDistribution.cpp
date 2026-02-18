#include "pch.h"
#include "GaussianDistribution.h"

#define _USE_MATH_DEFINES
#include <math.h>

double CGaussianDistribution::Coefficients[COEFFICIENT_CNT] = {
	-1.3026537197817094, 6.4196979235649026e-1,	1.9476473204185836e-2, -9.561514786808631e-3, -9.46595344482036e-4,
	3.66839497852761e-4, 4.2523324806907e-5, -2.0278578112534e-5, -1.624290004647e-6, 1.303655835580e-6,
	1.5626441722e-8, -8.5238095915e-8, 6.529054439e-9, 5.059343495e-9, -9.91364156e-10,
	-2.27365122e-10, 9.6467911e-11, 2.394038e-12, -6.886027e-12, 8.94487e-13,
	3.13092e-13, -1.12708e-13, 3.81e-16, 7.106e-15, -1.523e-15,	-9.4e-17, 1.21e-16, -2.8e-17
};

CGaussianDistribution::CGaussianDistribution()
{
}


CGaussianDistribution::~CGaussianDistribution()
{
}

double CGaussianDistribution::CumulativeTo(double x)
{
	double result = ErrorFunctionCumulativeTo(INVERSE_SQRT_2 * x);
	return result * 0.5;
}

double CGaussianDistribution::InverseCumulativeTo(double x, double mean, double standard_deviation)
{
	return mean - sqrt(2.0) * standard_deviation * InverseErrorFunctionCumulativeTo(2.0 * x);
}

double CGaussianDistribution::At(double x, double mean, double standard_deviation)
{
	double multiplier = 1.0 / (standard_deviation * sqrt(2.0 * M_PI));
	double exp_part = exp((-1.0 * pow(x - mean, 2.0)) / (2.0 * (standard_deviation * standard_deviation)));

	return multiplier * exp_part;
}

double CGaussianDistribution::ErrorFunctionCumulativeTo(double x)
{
	double z = abs(x);

	double t = 2.0 / (2.0 + z);
	double ty = 4.0 * t - 2.0;

	double d = 0.0;
	double dd = 0.0;
	for( int i = COEFFICIENT_CNT - 1; i > 0; --i )
	{
		double tmp = d;
		d = ty * d + Coefficients[i];
		dd = tmp;
	}

	double ans = t * exp(-z * z + 0.5 * (Coefficients[0] + ty * d) - dd);
	return 0.0 <= x ? ans : (2.0 - ans);
}

double CGaussianDistribution::InverseErrorFunctionCumulativeTo(double p)
{
	if( 2.0 <= p )
		return - 100.0;

	if( p <= 0.0 )
		return 100;

	double pp = (p < 1.0) ? p : 2.0 - p;
	double t = sqrt(-2.0 * log(pp / 2.0));
	double x = -0.70711 * ((2.30753 + t * 0.27061) / (1.0 + t * (0.99229 + t * 0.04481)) - t);

	for( int i = 0; i < 2; ++i )
	{
		double error = ErrorFunctionCumulativeTo(x) - pp;
		x += error / (1.12837916709551257 * exp(-(x * x)) - x * error);
	}

	return p < 1.0 ? x : -x;
}

#undef _USE_MATH_DEFINES
