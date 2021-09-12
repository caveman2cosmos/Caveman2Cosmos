#pragma once

#ifndef CvInfoUtil_h__
#define CvInfoUtil_h__

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "IDValueMap.h"


struct CvInfoUtil : bst::noncopyable
{
	///=============================
	/// Variable wrapper base class 
	///=============================

	struct WrappedVar
	{
		//WrappedVar(void* ptr, const wchar_t* tag)
		//	: m_ptr(ptr)
		//	, m_tag(tag)
		//{}

		virtual ~WrappedVar() {}
		//virtual void initVar() {}
		//virtual void uninitVar() {}
		virtual void checkSum(uint32_t&) const = 0;
		virtual void readXml(CvXMLLoadUtility*) = 0;
		virtual void copyNonDefaults(const WrappedVar*)	= 0;

	protected:
		//void* m_ptr;
		//CvWString m_tag;
	};

	///==============
	/// Enum wrapper
	///==============

	template <typename Enum_t>
	struct EnumWrapper : public WrappedVar, bst::noncopyable
	{
		EnumWrapper(Enum_t& var, const wchar_t* tag)
			//: WrappedVar(tag)
			: m_tag(tag)
			, m_ptr(&var)
		{
			DEBUG_LOG("CvInfoUtil.log", "Wrapping enum");
		}

		//void initVar()
		//{
		//	*m_ptr = static_cast<Enum_t>(-1);
		//}

		virtual void checkSum(uint32_t& iSum) const
		{
			DEBUG_LOG("CvInfoUtil.log", "Add checkSum for enum");
			CheckSum(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			DEBUG_LOG("CvInfoUtil.log", "Set XML value for enum");
			if (pXML->TryMoveToXmlFirstChild(m_tag.c_str()))
			{
				CvString szTextVal;
				pXML->GetXmlVal(szTextVal);
				*m_ptr = static_cast<Enum_t>(GC.getInfoTypeForString(szTextVal));
				pXML->MoveToXmlParent();
			}
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			DEBUG_LOG("CvInfoUtil.log", "Copy non defaults for enum");
			if (*m_ptr == -1)
				*m_ptr = *static_cast<const EnumWrapper<Enum_t>*>(pOther)->m_ptr;
		}

	protected:
		Enum_t& value()
		{
			return static_cast<Enum_t&>(*m_ptr);
		}

		Enum_t* m_ptr;
		CvWString m_tag;
	};

	template <typename Enum_t>
	CvInfoUtil& addEnum(Enum_t& var, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new EnumWrapper<Enum_t>(var, tag));
		DEBUG_LOG("CvInfoUtil.log", "Number of wrapped data members: %d", m_wrappedVars.size());
		return *this;
	}

	///====================
	/// Vector wrapper
	///====================

	template <typename T>
	struct VectorWrapper : public WrappedVar, bst::noncopyable
	{
		VectorWrapper(std::vector<T>& vec, const wchar_t* tag)
			//: WrappedVar(tag)
			: m_tag(tag)
			, m_ptr(&vec)
		{
			DEBUG_LOG("CvInfoUtil.log", "Wrapping vector");
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			DEBUG_LOG("CvInfoUtil.log", "Add checkSum for vector");
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			DEBUG_LOG("CvInfoUtil.log", "Set XML values for vector");
			pXML->SetOptionalVector(m_ptr, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			DEBUG_LOG("CvInfoUtil.log", "Copy non defaults for vector");
			foreach_(const T& element, *static_cast<const VectorWrapper<T>*>(pOther)->m_ptr)
				if (element > -1 && !algo::contains(*m_ptr, element))
					m_ptr->push_back(element);
			algo::sort(*m_ptr);
		}

	protected:
		std::vector<T>& value()
		{
			return static_cast<std::vector<T>&>(*m_ptr);
		}

		std::vector<T>* m_ptr;
		CvWString m_tag;
	};

	template <typename T>
	CvInfoUtil& add(std::vector<T>& vec, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new VectorWrapper<T>(vec, tag));
		DEBUG_LOG("CvInfoUtil.log", "Number of wrapped data members: %d", m_wrappedVars.size());
		return *this;
	}

	///====================
	/// IDValueMap wrapper
	///====================

	template <typename IDValueMap_T>
	struct IDValueMapWrapper : public WrappedVar, bst::noncopyable
	{
		IDValueMapWrapper(IDValueMap_T& map, const wchar_t* tag)
			//: WrappedVar(tag)
			: m_tag(tag)
			, m_ptr(&map)
		{
			DEBUG_LOG("CvInfoUtil.log", "Wrapping vector");
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			DEBUG_LOG("CvInfoUtil.log", "Add checkSum for IDValueMap");
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			DEBUG_LOG("CvInfoUtil.log", "Set XML values for IDValueMap");
			m_ptr->read(pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			DEBUG_LOG("CvInfoUtil.log", "Copy non defaults for IDValueMap");
			m_ptr->copyNonDefaults(*static_cast<const IDValueMapWrapper*>(pOther)->m_ptr);
		}

	protected:
		IDValueMap_T& value()
		{
			return static_cast<IDValueMap_T&>(*m_ptr);
		}

		IDValueMap_T* m_ptr;
		CvWString m_tag;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& add(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new IDValueMapWrapper<IDValueMap<T1, T2, default_> >(map, tag));
		DEBUG_LOG("CvInfoUtil.log", "Number of wrapped data members: %d", m_wrappedVars.size());
		return *this;
	}

	///============================================
	/// IDValueMap with delayed resolution wrapper
	///============================================

	template <typename IDValueMap_T>
	struct IDValueMapWithDelayedResolutionWrapper : public WrappedVar, bst::noncopyable
	{
		IDValueMapWithDelayedResolutionWrapper(IDValueMap_T& map, const wchar_t* tag)
			//: WrappedVar(tag)
			: m_tag(tag)
			, m_ptr(&map)
		{
			DEBUG_LOG("CvInfoUtil.log", "Wrapping IDValueMap with delayed resolution");
		}

		virtual void uninitVar()
		{
			DEBUG_LOG("CvInfoUtil.log", "Uninit IDValueMap with delayed resolution");
			m_ptr->removeDelayedResolution();
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			DEBUG_LOG("CvInfoUtil.log", "Add checkSum for IDValueMap with delayed resolution");
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			DEBUG_LOG("CvInfoUtil.log", "Set XML delayed resolution values for IDValueMap");
			m_ptr->readWithDelayedResolution(pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			DEBUG_LOG("CvInfoUtil.log", "Copy non defaults with delayed resolution for IDValueMap");
			m_ptr->copyNonDefaultsDelayedResolution(*static_cast<const IDValueMapWithDelayedResolutionWrapper*>(pOther)->m_ptr);
		}

	protected:
		IDValueMap_T& value()
		{
			return static_cast<IDValueMap_T&>(*m_ptr);
		}

		IDValueMap_T* m_ptr;
		CvWString m_tag;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addDelayedResolution(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new IDValueMapWithDelayedResolutionWrapper<IDValueMap<T1, T2, default_> >(map, tag));
		DEBUG_LOG("CvInfoUtil.log", "Number of wrapped data members: %d", m_wrappedVars.size());
		return *this;
	}

	///============
	/// Destructor
	///============

	CvInfoUtil()
	{
		DEBUG_LOG("CvInfoUtil.log", "Util created, initial size: %d", m_wrappedVars.size());
		FAssert(m_wrappedVars.empty());
		reset();
	}

	~CvInfoUtil()
	{
		reset();
		DEBUG_LOG("CvInfoUtil.log", "Util destroyed, size: %d", m_wrappedVars.size());
	}

	void reset()
	{
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			delete wrapper;
		m_wrappedVars.clear();
	}

	///===================================
	/// Boost range iterator requirements
	///===================================

	typedef std::vector<WrappedVar*>::iterator        iterator;
	typedef std::vector<WrappedVar*>::const_iterator  const_iterator;

	iterator begin() { return m_wrappedVars.begin(); }
	iterator end()   { return m_wrappedVars.end(); }

	const_iterator begin() const { return m_wrappedVars.begin(); }
	const_iterator end() const   { return m_wrappedVars.end(); }

	///=========
	/// Helpers
	///=========

	bool empty() const
	{
		return m_wrappedVars.empty();
	}

	void checkSum(uint32_t& iSum) const
	{
		//algo::for_each(m_wrappedVars, bind(WrappedVar::checkSum, _1, iSum));
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			wrapper->checkSum(iSum);
	}

	void readXml(CvXMLLoadUtility* pXML)
	{
		//algo::for_each(m_wrappedVars, bind(WrappedVar::readXml, _1, pXML));
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->readXml(pXML);
	}

	void copyNonDefaults(const CvInfoUtil& pOther)
	{
		size_t num = m_wrappedVars.size(), otherNum = pOther.m_wrappedVars.size();
		FAssert(num == otherNum);
		num = std::min(num, otherNum);
		for (uint32_t i = 0; i < num; i++)
		{
			m_wrappedVars[i]->copyNonDefaults(pOther.m_wrappedVars[i]);
		}
	}

private:
	std::vector<WrappedVar*> m_wrappedVars;
};

#endif