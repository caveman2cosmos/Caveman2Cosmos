#pragma once

#ifndef BuildingInfo_h__
#define BuildingInfo_h__

class BuildingInfo
{
public:
	BuildingInfo()
		: m_bNoLimit(false)
	{}

	~BuildingInfo() {}

	bool isNoLimit() const	{ return m_bNoLimit; }
	void setNoLimit(bool b)	{ m_bNoLimit = b; }

private:
	bool m_bNoLimit;
};

#endif // BuildingInfo_h__