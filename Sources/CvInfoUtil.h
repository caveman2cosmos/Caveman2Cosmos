#pragma once

#ifndef CvInfoUtil_h__
#define CvInfoUtil_h__

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "IDValueMap.h"

struct WrappedVar;


struct CvInfoUtil : bst::noncopyable
{
	template <class CvInfoClass_T>
	CvInfoUtil(CvInfoClass_T* info)
	{
		info->wrapDataMembers(*this);
	}

	~CvInfoUtil()
	{
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			delete wrapper;
		m_wrappedVars.clear();
	}

	void initDataMembers()
	{
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->initVar();
	}

	void uninitDataMembers()
	{
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->uninitVar();
	}

	void checkSum(uint32_t& iSum)
	{
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			wrapper->checkSum(iSum);
	}

	void readXml(CvXMLLoadUtility* pXML)
	{
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->readXml(pXML);
	}

	void copyNonDefaults(const CvInfoUtil otherUtil)
	{
		for (uint32_t i = 0, num = m_wrappedVars.size(); i < num; i++)
			m_wrappedVars[i]->copyNonDefaults(otherUtil.m_wrappedVars[i]);
	}

	///=============================
	/// Variable wrapper base class
	///=============================

	struct WrappedVar : bst::noncopyable
	{
		explicit WrappedVar(const wchar_t* tag)
			: m_tag(tag)
		{}

		virtual ~WrappedVar() {}
		virtual void initVar() {}
		virtual void uninitVar() {}
		virtual void checkSum(uint32_t&) const = 0;
		virtual void readXml(CvXMLLoadUtility*) = 0;
		virtual void copyNonDefaults(const WrappedVar*)	= 0;
		virtual void sendVarToPython(const char*) = 0;

	protected:
		const std::wstring m_tag;
	};

	///=================
	/// Integer wrapper
	///=================

	template <typename T>
	struct IntWrapper : WrappedVar
	{
		IntWrapper(T& var, const wchar_t* tag, T defaultValue)
			: WrappedVar(tag)
			, m_ptr(&var)
			, m_default(defaultValue)
		{}

		void initVar()
		{
			*m_ptr = m_default;
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->GetOptionalChildXmlValByName(m_ptr, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			if (*m_ptr == m_default)
				*m_ptr = *static_cast<const IntWrapper*>(pOther)->m_ptr;
		}

		void sendVarToPython(const char* file)
		{
			Cy::call(file, "handleInteger", Cy::Args()
				<< *m_ptr
			);
		}

	protected:
		T* m_ptr;
		const T m_default;
	};

	CvInfoUtil& add(int& var, const wchar_t* tag, int defaultValue = 0)
	{
		m_wrappedVars.push_back(new IntWrapper<int>(var, tag, defaultValue));
		return *this;
	}

	CvInfoUtil& add(bool& var, const wchar_t* tag, bool defaultValue = false)
	{
		m_wrappedVars.push_back(new IntWrapper<bool>(var, tag, defaultValue));
		return *this;
	}

	///==============
	/// Enum wrapper
	///==============

	template <typename Enum_t>
	struct EnumWrapper : WrappedVar
	{
		EnumWrapper(Enum_t& var, const wchar_t* tag)
			: WrappedVar(tag)
			, m_ptr(&var)
		{}

		void initVar()
		{
			*m_ptr = static_cast<Enum_t>(-1);
		}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
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
			if (*m_ptr == -1)
				*m_ptr = *static_cast<const EnumWrapper*>(pOther)->m_ptr;
		}

		void sendVarToPython(const char* file)
		{
			Cy::call(file, "handleEnum", Cy::Args()
				<< static_cast<int>(*m_ptr)
			);
		}

	protected:
		Enum_t* m_ptr;
	};

	template <typename Enum_t>
	CvInfoUtil& addEnum(Enum_t& var, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new EnumWrapper<Enum_t>(var, tag));
		return *this;
	}

	///====================
	/// Vector wrapper
	///====================

	template <typename T>
	struct VectorWrapper : WrappedVar
	{
		VectorWrapper(std::vector<T>& var, const wchar_t* tag)
			: WrappedVar(tag)
			, m_ptr(&var)
		{}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->SetOptionalVector(m_ptr, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			foreach_(const T& element, *static_cast<const VectorWrapper*>(pOther)->m_ptr)
				if (element > -1 && !algo::contains(*m_ptr, element))
					m_ptr->push_back(element);
			algo::sort(*m_ptr);
		}

		void sendVarToPython(const char* file)
		{
			Cy::call(file, "handleVector", Cy::Args()
				<< static_cast<const std::vector<int>&>(*m_ptr)
			);
		}

	protected:
		std::vector<T>* m_ptr;
	};

	template <typename T>
	CvInfoUtil& add(std::vector<T>& vec, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new VectorWrapper<T>(vec, tag));
		return *this;
	}

	///====================
	/// IDValueMap wrapper
	///====================

	template <typename IDValueMap_T>
	struct IDValueMapWrapper : WrappedVar
	{
		IDValueMapWrapper(IDValueMap_T& var, const wchar_t* tag)
			: WrappedVar(tag)
			, m_ptr(&var)
		{}

		virtual void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, *m_ptr);
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			m_ptr->read(pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			m_ptr->copyNonDefaults(*static_cast<const IDValueMapWrapper*>(pOther)->m_ptr);
		}

		void sendVarToPython(const char* file)
		{
		}

	protected:
		IDValueMap_T* m_ptr;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& add(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new IDValueMapWrapper<IDValueMap<T1, T2, default_> >(map, tag));
		return *this;
	}

	///============================================
	/// IDValueMap with delayed resolution wrapper
	///============================================

	template <typename IDValueMap_T>
	struct IDValueMapWithDelayedResolutionWrapper : WrappedVar
	{
		IDValueMapWithDelayedResolutionWrapper(IDValueMap_T& var, const wchar_t* tag)
			: WrappedVar(tag)
			, m_ptr(&var)
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
			m_ptr->readWithDelayedResolution(pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* pOther)
		{
			m_ptr->copyNonDefaultsDelayedResolution(*static_cast<const IDValueMapWithDelayedResolutionWrapper*>(pOther)->m_ptr);
		}

		void sendVarToPython(const char* file)
		{
		}

	protected:
		IDValueMap_T* m_ptr;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addDelayedResolution(IDValueMap<T1, T2, default_>& map, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new IDValueMapWithDelayedResolutionWrapper<IDValueMap<T1, T2, default_> >(map, tag));
		return *this;
	}

	///==================
	/// Python interface
	///==================

	static void publishPythonInterface()
	{
		python::class_<CvInfoUtil, boost::noncopyable>("CvInfoUtil", python::init<CvInfoBase*>())
			.def("sendDataMembersToPython", &CvInfoUtil::sendDataMembersToPython)
		;
	}

private:
	void sendDataMembersToPython(const std::string file)
	{
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->sendVarToPython(file.c_str());
	}

	///========================================================
	/// Wrapped pointers to the data members of an info object
	///========================================================

	std::vector<WrappedVar*> m_wrappedVars;
};

#endif