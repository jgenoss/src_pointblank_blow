#if !defined(_I3REG_KEY_EX_H__)
#define _I3REG_KEY_EX_H__


class I3_EXPORT_SCENE i3RegKeyEx :	public i3RegKey
{
private:
	INT32 m_iNumber = 0;
public:
	void SetNumber(INT32 iNum) { m_iNumber = iNum; }
	INT32 GetNumber() { return m_iNumber; }
};

#endif // _I3REG_KEY_EX_H__