#pragma once

struct i3VTSubsetRule;
struct i3VTSubset;
class i3VirtualTexture;
struct i3VTFileFormatData;

class I3_EXPORT_FRAMEWORK i3VTSubsetController
{
public:
	i3VTSubsetController(i3VirtualTexture* pVTex);
	~i3VTSubsetController();

	i3VTSubsetRule*	AddSubsetRule(const char * pszPath, const char * pszName, UINT32 limitSz);
	INT32			getSubsetRuleCount() const { return INT32(m_RuleList.size()); }
	i3VTSubsetRule*	getSubsetRule(INT32 idx) const { return m_RuleList[idx]; }

	INT32			getSubsetCount(void) const { return INT32(m_SubsetList.size()); }
	i3VTSubset*		getSubset(INT32 idx) const { return m_SubsetList[idx]; }

	void			RemoveAllSubsetAndFiles();

	bool			LoadAllSubsetInfo();
	bool			SaveAllSubsetInfo();

	i3VTSubsetRule*	FindSubsetRule(const i3::literal_range& rngPath) const;
	void			RemoveSubsetRule(i3VTSubsetRule* pRule);

	INT32			FindOrCreateMatchedSubset(const i3::literal_range& pszPath, UINT32 dataSize);

	i3VirtualTexture*	GetVTex() const { return m_pVTex; }
private:

	i3VirtualTexture*			m_pVTex;			// [initialize at constructor]
	i3::vector<i3VTSubsetRule*> m_RuleList;
	i3::vector<i3VTSubset*>	m_SubsetList;
};

