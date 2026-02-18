#pragma once

#include "i3Base/itl/function/function.h"

struct LocaleString;
struct LocaleStringSet;

struct LocaleStringDiff_Visitor
{
	virtual void Modified( const LocaleString* lhs, const LocaleString* rhs ) {}
	virtual void Added( const LocaleString* rhs ) {}
	virtual void Removed( const LocaleString* lhs) {}
	virtual ~LocaleStringDiff_Visitor() {}
};

class LocaleStringDiff_Accept
{
public:
	typedef i3::function<void (LocaleStringDiff_Visitor*)> function_type;

	LocaleStringDiff_Accept() {}
	
	void visit(LocaleStringDiff_Visitor* v);
	~LocaleStringDiff_Accept() {}
	
	void move_function(function_type& f);

private:
	
	typedef function_type		fun_t;
	
	fun_t			m_visit_mem_fun;
};

typedef i3::shared_ptr<LocaleStringDiff_Accept>				LocaleStringDiff_AcceptPtr;


struct LocaleStringDiffSet
{
	i3::vector<LocaleStringDiff_AcceptPtr>	m_list;
};

bool FindLocaleStringDiff( const LocaleStringSet& lhs, const LocaleStringSet& rhs, 
						  LocaleStringDiffSet& out);

void VisitLocaleStringDiff( const LocaleStringDiffSet& in, LocaleStringDiff_Visitor* v);


struct LocaleString
{
	i3::rc_wstring	m_key;
	i3::rc_wstring	m_val;
};

typedef i3::shared_ptr<LocaleString>	LocaleStringPtr;

struct LocaleStringSet
{
	i3::vector<LocaleStringPtr>	m_list;
};

typedef i3::shared_ptr<LocaleStringSet>	LocaleStringSetPtr;

bool LoadLocaleStringForDiff(const i3::wstring& root_path, const i3::const_char_range& rel_path, 
						  LocaleStringSet& out);

