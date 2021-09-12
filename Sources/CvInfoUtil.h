#pragma once

#ifndef CvInfoUtil_h__
#define CvInfoUtil_h__

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "IDValueMap.h"


struct CvInfoUtil
{
	//============
	// Base class
	//============

	struct Var
	{
		Var(const wchar_t* tag)
			: m_tag(tag)
		{}

		virtual ~Var() {}
		virtual void initVar() {}
		virtual void uninitVar() {}
		virtual void checkSum(uint32_t& iSum) const {}
		virtual void readXml(CvXMLLoadUtility*) {}
		virtual void copyNonDefaults(const Var&) {}

	protected:
		const wchar_t* m_tag;
	};

	//========================
	// Info type enum wrapper
	//========================

	template <typename Enum_t>
	struct EnumVar : public Var
	{
		EnumVar(Enum_t& var, const wchar_t* tag)
			: Var(tag)
			, m_ptr(&var)
		{}

		virtual void initVar()
		{
			//*m_ptr = static_cast<Enum_t>(-1);
			*m_ptr = Enum_t(-1);
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			if (pXML->TryMoveToXmlFirstChild(m_tag))
			{
				CvString szTextVal;
				pXML->GetXmlVal(szTextVal);
				*m_ptr = static_cast<Enum_t>(GC.getInfoTypeForString(szTextVal));
				pXML->MoveToXmlParent();
			}
		}

		virtual void copyNonDefaults(const EnumVar<Enum_t>& pOther)
		{
			if (*m_ptr == -1)
				*m_ptr = *pOther.m_ptr;
		}

	protected:
		Enum_t* m_ptr;
	};

	template <typename Enum_t>
	CvInfoUtil& addEnum(Enum_t& var, const wchar_t* tag)
	{
		m_DataMembers.push_back(EnumVar<Enum_t>(var, tag));
		return *this;
	}

	//====================
	// Vector wrapper
	//====================

	template <typename T>
	struct VectorVar : public Var
	{
		VectorVar(std::vector<T>& vec, const wchar_t* tag)
			: Var(tag)
			, m_ptr(&vec)
		{}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->SetOptionalVector(m_ptr, m_tag);
		}

		virtual void copyNonDefaults(const Var& pOther)
		{
			foreach_(const T& element, *static_cast<const VectorVar<T>&>(pOther).m_ptr)
			{
				if (element > -1 && !algo::contains(*m_ptr, element))
				{
					m_ptr->push_back(element);
				}
			}
			algo::sort(*m_ptr);
		}

	protected:
		std::vector<T>* m_ptr;
	};

	template <typename T>
	CvInfoUtil& add(std::vector<T>& vec, const wchar_t* tag)
	{
		m_DataMembers.push_back(VectorVar<T>(vec, tag));
		return *this;
	}

	//====================
	// IDValueMap wrapper
	//====================

	template <typename IDValueMap_T>
	struct IDValueMapVar : public Var
	{
		IDValueMapVar(IDValueMap_T& map, const wchar_t* tag)
			: Var(tag)
			, m_ptr(&map)
		{}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			m_ptr->read(pXML, m_tag);
		}

		virtual void copyNonDefaults(const Var& pOther)
		{
			m_ptr->copyNonDefaults(*static_cast<const IDValueMapVar&>(pOther).m_ptr);
		}

	protected:
		IDValueMap_T* m_ptr;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& add(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
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
			: Var(tag)
			, m_ptr(&map)
		{}

		virtual void uninitVar()
		{
			m_ptr->removeDelayedResolution();
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			m_ptr->readWithDelayedResolution(pXML, m_tag);
		}

		virtual void copyNonDefaults(const Var& pOther)
		{
			m_ptr->copyNonDefaultsDelayedResolution(*static_cast<const IDValueMapVar&>(pOther).m_ptr);
		}

	protected:
		IDValueMap_T* m_ptr;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addDelayedResolution(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
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