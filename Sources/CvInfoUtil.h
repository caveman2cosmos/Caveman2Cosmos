#pragma once

#ifndef CvInfoUtil_h__
#define CvInfoUtil_h__

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInfoClassTraits.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"
#include "IDValueMap.h"

struct WrappedVar;


struct CvInfoUtil
	: private bst::noncopyable
{
	template <class CvInfoClass_T>
	CvInfoUtil(const CvInfoClass_T* info)
		: m_eInfoClass(InfoClassTraits<CvInfoClass_T>::InfoClassEnum)
	{
		const_cast<CvInfoClass_T*>(info)->getDataMembers(*this);
	}

	~CvInfoUtil()
	{
		PROFILE_EXTRA_FUNC();
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			delete wrapper;
	}

	void initDataMembers()
	{
		PROFILE_EXTRA_FUNC();
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->initVar();
	}

	void uninitDataMembers()
	{
		PROFILE_EXTRA_FUNC();
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->uninitVar();
	}

	void checkSum(uint32_t& iSum) const
	{
		PROFILE_EXTRA_FUNC();
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			wrapper->checkSum(iSum);
	}

	void readXml(CvXMLLoadUtility* pXML)
	{
		PROFILE_EXTRA_FUNC();
		foreach_(WrappedVar* wrapper, m_wrappedVars)
			wrapper->readXml(pXML);
	}

	void copyNonDefaults(const CvInfoUtil otherUtil)
	{
		PROFILE_EXTRA_FUNC();
		for (uint32_t i = 0, num = m_wrappedVars.size(); i < num; i++)
			m_wrappedVars[i]->copyNonDefaults(otherUtil.m_wrappedVars[i]);
	}

	///=============================
	/// Variable wrapper base class
	///=============================

	struct WrappedVar : bst::noncopyable
	{
		friend struct CvInfoUtil;

	protected:
		WrappedVar(void* var, const wchar_t* tag)
			: m_ptr(var)
			, m_tag(tag)
		{}

		virtual ~WrappedVar() {}

		virtual void initVar() {}
		virtual void uninitVar() {}
		//virtual void copyVar() {}

		//virtual void read(FDataStreamBase*) {}
		//virtual void write(FDataStreamBase*) {}

		virtual void checkSum(uint32_t&) const = 0;
		virtual void readXml(CvXMLLoadUtility*) = 0;
		virtual void copyNonDefaults(const WrappedVar*)	= 0;

		void* m_ptr;
		const std::wstring m_tag;
	};

	///=================
	/// Integer wrapper
	///=================

	template <typename T>
	struct IntWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		IntWrapper(T& var, const wchar_t* tag, T defaultValue)
			: WrappedVar(static_cast<void*>(&var), tag)
			, m_default(defaultValue)
		{}

		void initVar()
		{
			ref() = m_default;
		}

		void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, ref());
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->GetOptionalChildXmlValByName(&ref(), m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			if (ref() == m_default)
				ref() = static_cast<const IntWrapper*>(source)->ref();
		}

		T& ref() const { return *static_cast<T*>(m_ptr); }

	private:
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
		friend struct CvInfoUtil;

	protected:
		EnumWrapper(Enum_t& var, const wchar_t* tag)
			: WrappedVar(static_cast<void*>(&var), tag)
		{}

		void initVar()
		{
			ref() = static_cast<Enum_t>(-1);
		}

		void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, ref());
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			CvXMLLoadUtility::SetOptionalInfoType<NO_DELAYED_RESOLUTION>(pXML, ref(), m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* source)
		{
			if (ref() == -1)
				ref() = static_cast<const EnumWrapper*>(source)->ref();
		}

		Enum_t& ref() const { return *static_cast<Enum_t*>(m_ptr); }
	};

	///======================================
	/// Enum with delayed resolution wrapper
	///======================================

	template <typename Enum_t>
	struct EnumWithDelayedResolutionWrapper : EnumWrapper<Enum_t>
	{
		friend struct CvInfoUtil;

	protected:
		EnumWithDelayedResolutionWrapper(Enum_t& var, const wchar_t* tag)
			: EnumWrapper<Enum_t>(var, tag)
		{}

		void uninitVar()
		{
			GC.removeDelayedResolution(reinterpret_cast<int*>(&ref()));
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			CvXMLLoadUtility::SetOptionalInfoType<USE_DELAYED_RESOLUTION>(pXML, ref(), m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			GC.copyNonDefaultDelayedResolution(reinterpret_cast<int*>(&ref()), reinterpret_cast<int*>(&(static_cast<const EnumWithDelayedResolutionWrapper*>(source)->ref())));
		}
	};

	template <typename Enum_t>
	CvInfoUtil& addEnum(Enum_t& var, const wchar_t* tag)
	{
		if (GC.isDelayedResolutionRequired(m_eInfoClass, InfoClassTraits<Enum_t>::InfoClassEnum))
			m_wrappedVars.push_back(new EnumWithDelayedResolutionWrapper<Enum_t>(var, tag));
		else
			m_wrappedVars.push_back(new EnumWrapper<Enum_t>(var, tag));
		return *this;
	}

	///================
	/// String wrapper
	///================

	struct StringWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		StringWrapper(CvString& var, const wchar_t* tag)
			: WrappedVar(static_cast<void*>(&var), tag)
		{}

		void checkSum(uint32_t&) const
		{
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->GetOptionalChildXmlValByName(ref(), m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			if (ref().empty() || ref() == CvString::format("").c_str())
				ref() = static_cast<const StringWrapper*>(source)->ref();
		}

		CvString& ref() const { return *static_cast<CvString*>(m_ptr); }
	};

	CvInfoUtil& add(CvString& var, const wchar_t* tag)
	{
		m_wrappedVars.push_back(new StringWrapper(var, tag));
		return *this;
	}

	///================
	/// Vector wrapper
	///================

	template <typename T>
	struct VectorWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		VectorWrapper(std::vector<T>& var, const wchar_t* tag)
			: WrappedVar(static_cast<void*>(&var), tag)
		{}

		void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, ref());
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->SetOptionalVector(&ref(), m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			foreach_(const T& element, static_cast<const VectorWrapper*>(source)->ref())
				if (element > -1 && algo::none_of_equal(ref(), element))
					ref().push_back(element);
			algo::sort(ref());
		}

		std::vector<T>& ref() const { return *static_cast<std::vector<T>*>(m_ptr); }
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
		friend struct CvInfoUtil;

	protected:
		IDValueMapWrapper(IDValueMap_T& var, const wchar_t* tag)
			: WrappedVar(static_cast<void*>(&var), tag)
		{}

		void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, ref());
		}

		virtual void readXml(CvXMLLoadUtility* pXML)
		{
			ref().read(pXML, m_tag.c_str());
		}

		virtual void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaults(static_cast<const IDValueMapWrapper*>(source)->ref());
		}

		IDValueMap_T& ref() const { return *static_cast<IDValueMap_T*>(m_ptr); }
	};

	///============================================
	/// IDValueMap with delayed resolution wrapper
	///============================================

	template <typename IDValueMap_T>
	struct IDValueMapWithDelayedResolutionWrapper
		: public IDValueMapWrapper<IDValueMap_T>
	{
		friend struct CvInfoUtil;

	protected:
		IDValueMapWithDelayedResolutionWrapper(IDValueMap_T& var, const wchar_t* tag)
			: IDValueMapWrapper<IDValueMap_T>(var, tag)
		{}

		void uninitVar()
		{
			ref().removeDelayedResolution();
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			ref().readWithDelayedResolution(pXML, m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaultDelayedResolution(static_cast<const IDValueMapWithDelayedResolutionWrapper*>(source)->ref());
		}
	};

	template <typename T1, int default_>
	CvInfoUtil& add(IDValueMap<T1, int, default_>& map, const wchar_t* rootTag)
	{
		if (GC.isDelayedResolutionRequired(m_eInfoClass, InfoClassTraits<T1>::InfoClassEnum))
			m_wrappedVars.push_back(new IDValueMapWithDelayedResolutionWrapper<IDValueMap<T1, int, default_> >(map, rootTag));
		else
			m_wrappedVars.push_back(new IDValueMapWrapper<IDValueMap<T1, int, default_> >(map, rootTag));
		return *this;
	}

	///=====================================
	/// IDValueMap of paired arrays wrapper
	///=====================================

	template <typename IDValueMap_T, DelayedResolutionTypes delayedRes_>
	struct IDValueMapOfPairedArrayWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		IDValueMapOfPairedArrayWrapper(IDValueMap_T& var, const wchar_t* rootTag, const wchar_t* firstChildTag, const wchar_t* secondChildTag)
			: WrappedVar(static_cast<void*>(&var), rootTag)
			, m_firstChildTag(firstChildTag)
			, m_secondChildTag(secondChildTag)
		{}

		void uninitVar()
		{
			if (delayedRes_ == USE_DELAYED_RESOLUTION)
				ref().removeDelayedResolution();
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			ref()._readPairedArrays<delayedRes_>(pXML, m_tag.c_str(), m_firstChildTag.c_str(), m_secondChildTag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaultPairedArrays(static_cast<const IDValueMapOfPairedArrayWrapper*>(source)->ref());
		}

		void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, ref());
		}

		IDValueMap_T& ref() const { return *static_cast<IDValueMap_T*>(m_ptr); }

	private:
		const std::wstring m_firstChildTag;
		const std::wstring m_secondChildTag;
	};

	template <typename T1, size_t arraySize_, int default_>
	CvInfoUtil& add(IDValueMap<T1, bst::array<int, arraySize_>, default_>& map, const wchar_t* rootTag, const wchar_t* firstChildTag, const wchar_t* secondChildTag)
	{
		if (GC.isDelayedResolutionRequired(m_eInfoClass, InfoClassTraits<T1>::InfoClassEnum))
			m_wrappedVars.push_back(new IDValueMapOfPairedArrayWrapper<IDValueMap<T1, bst::array<int, arraySize_>, default_>, USE_DELAYED_RESOLUTION>(map, rootTag, firstChildTag, secondChildTag));
		else
			m_wrappedVars.push_back(new IDValueMapOfPairedArrayWrapper<IDValueMap<T1, bst::array<int, arraySize_>, default_>, NO_DELAYED_RESOLUTION>(map, rootTag, firstChildTag, secondChildTag));
		return *this;
	}

private:
	///========================================================
	/// Wrapped pointers to the data members of an info object
	///========================================================

	const InfoClassTypes m_eInfoClass;
	std::vector<WrappedVar*> m_wrappedVars;
};

#endif