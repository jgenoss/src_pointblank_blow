#pragma once

#define INVERSE_SQRT_2		-0.707106781186547524400844362104
#define COEFFICIENT_CNT		28


class CGaussianDistribution
{
public:
	CGaussianDistribution();
	~CGaussianDistribution();


public:
	static double		CumulativeTo(double x);
	static double		InverseCumulativeTo(double x, double mean, double standard_deviation);
	static double		At(double x, double mean, double standard_deviation);


private:
	static double		ErrorFunctionCumulativeTo(double x);
	static double		InverseErrorFunctionCumulativeTo(double p);

private:
	static double		Coefficients[COEFFICIENT_CNT];
};

