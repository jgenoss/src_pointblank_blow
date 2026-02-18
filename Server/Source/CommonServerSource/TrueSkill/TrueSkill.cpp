#include "pch.h"
#include "TrueSkill.h"

#include "GaussianDistribution.h"

#define COMPENSATE_VALUE		0.5f		// 클랜 포인트가 동일할시 보정 수치

CTrueSkill::CTrueSkill()
{
}


CTrueSkill::~CTrueSkill()
{
}

void CTrueSkill::GetValue(CRating& win_team_rating, CRating& lose_team_rating, int player_count, OUT double& c, OUT double& v, OUT double& w)
{
	c = sqrt(win_team_rating.standard_deviation_ + lose_team_rating.standard_deviation_ + player_count * Square(BETA) );

	double mean_delta = (win_team_rating.mean_ - lose_team_rating.mean_) / c;
	if(  fabs(mean_delta) < COMPENSATE_VALUE )
	{
		mean_delta = COMPENSATE_VALUE;
	}

	double draw_margin = GetDrawMargin(DRAW_PROBABILITY, BETA) / c;

	
	v = VWithinMargin(mean_delta, draw_margin);
	w = WWithinMargin(mean_delta, draw_margin);
}


bool CTrueSkill::CalculateRating(CRating& rating, ROOM_ENDTYPE eResult, double& c, double& v, double& w)
{
	double mean_multiplier = (Square(rating.standard_deviation_) + Square(DYNAMIC_FACTOR)) / c;

	switch( eResult ) 
	{
	case ROOM_ENDTYPE_WIN :
		rating.mean_ += fabs( (mean_multiplier * v) );
		break;
	case ROOM_ENDTYPE_LOSE :
		rating.mean_ -= fabs( (mean_multiplier * v) );
		break;
	}
	//eResult == ROOM_ENDTYPE_WIN ? rating.mean_ += fabs(mean_multiplier * v) : rating.mean_ -= (mean_multiplier * v);

	if( rating.mean_ < CLAN_MIN_POINT ) rating.mean_ = CLAN_MIN_POINT;
	if( rating.mean_ > CLAN_MAX_POINT ) rating.mean_ = CLAN_MAX_POINT;
	

	double winner_variance_with_dynamics = Square(rating.standard_deviation_) + Square(DYNAMIC_FACTOR);
	double winner_std_deviation_multiplier = winner_variance_with_dynamics / Square(c);
	rating.standard_deviation_ = sqrt(winner_variance_with_dynamics * (1.0 - w * winner_std_deviation_multiplier));

	return 0.0 < mean_multiplier * v;
}


void CTrueSkill::CalculateRatings(CRating* winner_ratings, CRating* loser_ratings, int player_count, bool bDraw)
{
	// calculate shared values
	double sum_of_winner_mean = 0.0;
	double sum_of_loser_mean = 0.0;
	double sum_of_winner_deviation = 0.0;
	double sum_of_loser_deviation = 0.0;
	for( int i = 0; i < player_count; ++i )
	{
		sum_of_winner_mean += winner_ratings[i].mean_;
		sum_of_winner_deviation += Square(winner_ratings[i].standard_deviation_);

		sum_of_loser_mean += loser_ratings[i].mean_;
		sum_of_loser_deviation += Square(loser_ratings[i].standard_deviation_);
	}

	double c = sqrt( sum_of_winner_deviation + sum_of_loser_deviation + player_count * Square(BETA) );
	double mean_delta = (sum_of_winner_mean - sum_of_loser_mean) / c;

	double draw_margin = GetDrawMargin(DRAW_PROBABILITY, BETA) / c;

	double v = bDraw ? VWithinMargin(mean_delta, draw_margin) : VExceedMargin(mean_delta, draw_margin);
	double w = bDraw ? WWithinMargin(mean_delta, draw_margin) : WExceedMargin(mean_delta, draw_margin);

	for( int i = 0; i < player_count; ++i )
	{
		// calculate winner rating
		double winner_mean_multiplier = (Square(winner_ratings[i].standard_deviation_) + Square(DYNAMIC_FACTOR)) / c;
		winner_ratings[i].mean_ = winner_ratings[i].mean_ + (winner_mean_multiplier * v);

		double winner_variance_with_dynamics = Square(winner_ratings[i].standard_deviation_) + Square(DYNAMIC_FACTOR);
		double winner_std_deviation_multiplier = winner_variance_with_dynamics / Square(c);
		winner_ratings[i].standard_deviation_ = sqrt(winner_variance_with_dynamics * (1.0 - w * winner_std_deviation_multiplier));

		// calculate loser rating
		double loser_mean_multiplier = Square(loser_ratings[i].standard_deviation_) + Square(DYNAMIC_FACTOR) / c;
		loser_ratings[i].mean_ = loser_ratings[i].mean_ - (loser_mean_multiplier * v);

		double loser_variance_with_dynamics = Square(loser_ratings[i].standard_deviation_) + Square(DYNAMIC_FACTOR);
		double loser_std_deviation_multiplier = loser_variance_with_dynamics / Square(c);
		loser_ratings[i].standard_deviation_ = sqrt(loser_variance_with_dynamics * (1.0 - w * loser_std_deviation_multiplier));
	}
}

double CTrueSkill::CalculateMatchQuality(CRating& team1_rating, CRating& team2_rating, int player_count)
{
	double beta_squared = Square(BETA);

// 	double sum_of_team1_mean = 0.0;
// 	double sum_of_team2_mean = 0.0;
// 	double sum_of_team1_deviation = 0.0;
// 	double sum_of_team2_deviation = 0.0;
// 	for( int i = 0; i < player_count; ++i )
// 	{
// 		sum_of_team1_mean += team1_rating[i].mean_;
// 		sum_of_team1_deviation += Square(team1_rating[i].standard_deviation_);
// 
// 		sum_of_team2_mean += team2_rating[i].mean_;
// 		sum_of_team2_deviation += Square(team2_rating[i].standard_deviation_);
// 	}

	double total_player_beta_squared = player_count * 2 * beta_squared;
//	double sqrt_part = sqrt(total_player_beta_squared / (total_player_beta_squared + sum_of_team1_deviation + sum_of_team2_deviation));
	double sqrt_part = sqrt(total_player_beta_squared / (total_player_beta_squared + team1_rating.standard_deviation_ + team2_rating.standard_deviation_));
//	double exp_part = exp((-1.0 * Square(sum_of_team1_mean - sum_of_team2_mean)) / 2.0 * total_player_beta_squared);
	double exp_part = exp((-1.0 * Square(team1_rating.mean_ - team2_rating.mean_)) / 2.0 * total_player_beta_squared);

	return exp_part * sqrt_part;
}

double CTrueSkill::VExceedMargin(double mean_delta, double draw_margin)
{
	double denominator = CGaussianDistribution::CumulativeTo(mean_delta - draw_margin);
	if( denominator < 2.222758749e-162 )
		return -mean_delta + draw_margin;

	return CGaussianDistribution::At(mean_delta - draw_margin, 0.0, 1.0) / denominator;
}

double CTrueSkill::VWithinMargin(double mean_delta, double draw_margin)
{
	double abs_mean_delta = abs(mean_delta);
	double denominator = CGaussianDistribution::CumulativeTo(draw_margin - abs_mean_delta) - CGaussianDistribution::CumulativeTo(-draw_margin - abs_mean_delta);
	if( denominator < 2.222758749e-162 )
	{
		if( mean_delta < 0.0 )
			return -mean_delta - draw_margin;

		return -mean_delta + draw_margin;
	}

	double numerator = CGaussianDistribution::At(-draw_margin - abs_mean_delta, 0.0, 1.0) - CGaussianDistribution::At(draw_margin - abs_mean_delta, 0.0, 1.0);
	if( mean_delta < 0.0 )
		return -numerator / denominator;

	return numerator / denominator;
}

double CTrueSkill::WExceedMargin(double mean_delta, double draw_margin)
{
	double denominator = CGaussianDistribution::CumulativeTo(mean_delta - draw_margin);
	if( denominator < 2.222758749e-162 )
	{
		if( mean_delta < 0.0 )
			return 1.0;

		return 0.0;
	}

	double vWin = VExceedMargin(mean_delta, draw_margin);

	return vWin * (vWin + mean_delta - draw_margin);
}

double CTrueSkill::WWithinMargin(double mean_delta, double draw_margin)
{
	double abs_mean_delta = abs(mean_delta);
	double denominator = CGaussianDistribution::CumulativeTo(draw_margin - abs_mean_delta) - CGaussianDistribution::CumulativeTo(-draw_margin - abs_mean_delta);
	if( denominator < 2.222758749e-162 )
		return 1.0;

	double vt = VWithinMargin(abs_mean_delta, draw_margin);
	return vt * vt + ((draw_margin - abs_mean_delta) * CGaussianDistribution::At(draw_margin - abs_mean_delta, 0.0, 1.0) - (-draw_margin - abs_mean_delta) * 
		CGaussianDistribution::At(-draw_margin - mean_delta, 0.0, 1.0)) / denominator;
}

double CTrueSkill::GetDrawMargin(double draw_probability, double beta)
{
	return CGaussianDistribution::InverseCumulativeTo(0.5 * (draw_probability + 1.0), 0.0, 1.0) * sqrt(1.0 + 1.0) * beta;
}
