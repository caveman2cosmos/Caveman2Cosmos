#pragma once

#ifndef CVINFO_UTIL_H
#define CVINFO_UTIL_H

#include "CvGameCoreDLL.h"
//#include "CvGlobals.h"
//#include "CvInfos.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

struct CvInfoUtil
{
	//============
	// Base class
	//============

	struct Var
	{
		virtual ~Var() {}
		virtual void initVar() {}
		virtual void uninitVar() {}
		virtual void checkSum(uint32_t& iSum) const {}
		virtual void readXml(CvXMLLoadUtility&) {}
		virtual void copyNonDefaults(const Var&) {}

	private:
		//const wchar_t* m_tag;
		//CvWString m_tag;
	};

	//====================
	// IDValueMap wrapper
	//====================

	template <typename IDValueMap_T>
	struct IDValueMapVar : public Var
	{
		IDValueMapVar(IDValueMap_T& map, const wchar_t* tag)
			: m_var(&map)
			, m_tag(tag)
		{}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_var);
		}

		virtual void readXml(CvXMLLoadUtility& pXML)
		{
			m_var->read(&pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const Var& pOther)
		{
			m_var->copyNonDefaults(*static_cast<const IDValueMapVar&>(pOther).m_var);
		}

	private:
		IDValueMap_T* m_var;
		const CvWString m_tag;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addVar(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_DataMembers.push_back(IDValueMapVar<IDValueMap<T1, T2, default_> >(map, tag));
		return *this;
	}

	//============================================
	// IDValueMap with delayed resolution wrapper
	//============================================

	template <typename IDValueMap_T>
	struct IDValueMapDelayedResolutionVar : public Var
	{
		IDValueMapDelayedResolutionVar(IDValueMap_T& map, const wchar_t* tag)
			: m_var(&map)
			, m_tag(tag)
		{}

		virtual void uninitVar()
		{
			m_var->removeDelayedResolution();
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_var);
		}

		virtual void readXml(CvXMLLoadUtility& pXML)
		{
			m_var->readWithDelayedResolution(&pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const Var& pOther)
		{
			m_var->copyNonDefaultsDelayedResolution(*static_cast<const IDValueMapVar&>(pOther).m_var);
		}

	private:
		IDValueMap_T* m_var;
		const CvWString m_tag;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addDelayedResolutionVar(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_DataMembers.push_back(IDValueMapDelayedResolutionVar<IDValueMap<T1, T2, default_> >(map, tag));
		return *this;
	}

	//===========
	// Iteration
	//===========

	typedef std::vector<Var>::iterator        iterator;
	typedef std::vector<Var>::const_iterator  const_iterator;

	iterator begin() { return m_DataMembers.begin(); }
	iterator end()   { return m_DataMembers.end(); }

	const_iterator begin() const { return m_DataMembers.begin(); }
	const_iterator end() const   { return m_DataMembers.end(); }

	//========
	// Helper
	//========

	void copyNonDefaults(const CvInfoUtil& pOther)
	{
		for (uint32_t i = 0, num = m_DataMembers.size(); i < num; i++)
		{
			m_DataMembers[i].copyNonDefaults(pOther.m_DataMembers[i]);
		}
	}

private:
	std::vector<Var> m_DataMembers;
};

#endif