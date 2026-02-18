#pragma once


// TrueSkill¢â Ranking System
// This implementation reference from Microsoft Research
// http://research.microsoft.com/en-us/projects/trueskill/details.aspx

class CTrueSkill
{
public:
	CTrueSkill();
	~CTrueSkill();


public:
	static void				GetValue(CRating& win_team_rating, CRating& lose_team_rating, int player_count, OUT double& c, OUT double& v, OUT double& w);
	static bool				CalculateRating(CRating& rating, ROOM_ENDTYPE eResult, double& c, double& v, double& w);

public:
	static void				CalculateRatings(CRating* winner_ratings, CRating* loser_ratings, int player_count, bool bDraw);
	static double			CalculateMatchQuality(CRating& team1_rating, CRating& team2_rating, int player_count);


private:
	static double			GetDrawMargin(double draw_probability, double beta);
	static double			VExceedMargin(double mean_delta, double draw_margin);
	static double			VWithinMargin(double mean_delta, double draw_margin);
	static double			WExceedMargin(double mean_delta, double draw_margin);
	static double			WWithinMargin(double mean_delta, double draw_margin);

	static double			Square(double value)									{ return value * value; }
};

