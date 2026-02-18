#pragma once

namespace Cheatkey
{
	void TrimEmptySpace(i3::string& str);

	class Tokenizer
	{
	public:
		Tokenizer(const i3::string& tgt, const i3::string& sep, const i3::string& subsep = "");
		
		bool HasToken();
		i3::string NextToken();

	private:
		const i3::string& m_target;
		i3::string m_sep;
		i3::string m_subsep;
		size_t m_idx;
		size_t m_pos;
	};

	void Tokenize(const i3::string& text, i3::vector<i3::string>& tokens, const i3::string& sep, const i3::string& subsep = "");
}