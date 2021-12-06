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
	CvInfoUtil(CvInfoBase* info)
	{
		info->getDataMembers(*this);
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

	void checkSum(uint32_t& iSum) const
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

		virtual void sendVarToPython(const char*) const {}

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

		void sendVarToPython(const char* file) const
		{
			Cy::call(file, "handleInteger", Cy::Args()
				<< ref()
			);
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

		void readXml(CvXMLLoadUtility* pXML)
		{
			if (pXML->TryMoveToXmlFirstChild(m_tag.c_str()))
			{
				CvString szTextVal;
				pXML->GetXmlVal(szTextVal);
				ref() = static_cast<Enum_t>(GC.getInfoTypeForString(szTextVal));
				pXML->MoveToXmlParent();
			}
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			if (ref() == -1)
				ref() = static_cast<const EnumWrapper*>(source)->ref();
		}

		void sendVarToPython(const char* file) const
		{
			Cy::call(file, "handleEnum", Cy::Args()
				<< static_cast<int>(ref())
			);
		}

		Enum_t& ref() const { return *static_cast<Enum_t*>(m_ptr); }
	};

	template <typename Enum_t>
	CvInfoUtil& addEnum(Enum_t& var, const wchar_t* tag)
	{
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

		void checkSum(uint32_t& iSum) const
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

	///===============
	/// Array wrapper
	///===============

	template <typename T>
	struct ArrayWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		ArrayWrapper(T*& var, const wchar_t* tag, size_t size, T defaultValue)
			: WrappedVar(static_cast<void*>(&var), tag)
			, m_size(size)
			, m_default(defaultValue)
		{}

		void initVar()
		{
			ref() = NULL;
		}

		void uninitVar()
		{
			SAFE_DELETE_ARRAY(ref());
		}

		void checkSum(uint32_t& iSum) const
		{
			CheckSum(iSum, ref(), m_size);
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			pXML->SetVariableListTagPair(&ref(), m_tag.c_str(), m_size, m_default);
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			if (const T*& sourceArray = static_cast<const ArrayWrapper*>(source)->ref())
			{
				for (uint32_t i = 0; i < m_size; i++)
				{
					const T& sourceElement = sourceArray[i];
					if (sourceElement != m_default)
					{
						T*& array = ref();
						if (array == NULL)
							CvXMLLoadUtility::InitList(&array, m_size, m_default);

						T& element = array[i];
						if (element == m_default)
							element = sourceElement;
					}
				}
			}
		}

		void sendVarToPython(const char* file) const
		{
			/*
			if (const T*& array = ref())
			{
				PyObject* list = PyList_New(static_cast<int>(m_size));
				FAssert(list != NULL);
				for (uint32_t i = 0; i < m_size; i++)
				{
					PyObject* item = PyInt_FromLong(static_cast<long>(array[i]));
					FAssert(item != NULL);
					PyList_SetItem(list, i, item);
				}
				Cy::Args args = Cy::Args();
				args.args().add(list);
				Cy::call(file, "handleArray", args);

				//Cy::call(file, "handleArray", Cy::Args()
				//	<< Cy::Array<T>(array, m_size)
				//);
			}
			*/
		}

		T*& ref()				{ return *static_cast<T**>(m_ptr); }
		const T*& ref() const	{ return *static_cast<const T**>(m_ptr); }

	private:
		const size_t m_size;
		const T m_default;
	};

	CvInfoUtil& add(int*& var, const wchar_t* tag, int size, int defaultValue = 0)
	{
		m_wrappedVars.push_back(new ArrayWrapper<int>(var, tag, size, defaultValue));
		return *this;
	}

	///==================
	/// 2D array wrapper
	///==================

	template <typename T>
	struct DoubleArrayWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		DoubleArrayWrapper(T** var, const wchar_t* tag, const wchar_t* firstChildTag, const wchar_t* secondChildTag, int size1, int size2, T defaultValue)
			: WrappedVar(static_cast<void*>(&var), tag)
			, m_size1(size1)
			, m_size2(size2)
			, m_default(defaultValue)
			, m_firstChildTag(firstChildTag)
			, m_secondChildTag(secondChildTag)
		{}

		void initVar()
		{
			ref() = NULL;
		}

		void uninitVar()
		{
			SAFE_DELETE_ARRAY2(ref(), m_size1);
		}

		void checkSum(uint32_t& iSum) const
		{
			if (const T**& ptrArray = ref())
				for (int i = 0; i < m_size1; i++)
					CheckSum(iSum, ptrArray[i], m_size2);
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			if (pXML->TryMoveToXmlFirstChild(m_tag.c_str()))
			{
				const int iNumChildren = pXML->GetXmlChildrenNumber();

				if (pXML->TryMoveToXmlFirstChild())
				{
					T**& ptrArray = ref();

					CvString szTextVal;
					for (int i = 0; i < iNumChildren; i++)
					{
						pXML->GetChildXmlValByName(szTextVal, m_firstChildTag.c_str());
						const int eType = pXML->GetInfoClass(szTextVal);
						if (eType > -1)
						{
							if (ptrArray == NULL)
								CvXMLLoadUtility::InitPointerList(&ptrArray, m_size1);

							if (pXML->TryMoveToXmlFirstChild(m_secondChildTag.c_str()))
							{
								pXML->SetCommerce(&ptrArray[eType]);
								pXML->MoveToXmlParent();
							}
						}
						if (!pXML->TryMoveToXmlNextSibling())
							break;
					}
					pXML->MoveToXmlParent();
				}
				pXML->MoveToXmlParent();
			}
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			if (const T**& sourcePtrArray = static_cast<const DoubleArrayWrapper*>(source)->ref())
			{
				for (int i = 0; i < m_size1; i++)
				{
					if (const T*& sourceInnerArray = sourcePtrArray[i])
					{
						for (int j = 0; j < m_size2; j++)
						{
							const T& sourceElement = sourceInnerArray[j];
							if (sourceElement != m_default)
							{
								T**& ptrArray = ref();
								if (ptrArray == NULL)
									CvXMLLoadUtility::InitPointerList(&ptrArray, m_size1);

								T*& InnerArray = ptrArray[i];
								if (InnerArray == NULL)
									CvXMLLoadUtility::InitList(&InnerArray, m_size2, m_default);

								T& element = InnerArray[j];
								if (element == m_default)
									element = sourceElement;
							}
						}
					}
				}
			}
		}

		T**& ref()				{ return *static_cast<T***>(m_ptr); }
		const T**& ref() const	{ return *static_cast<const T***>(m_ptr); }

	private:
		const int m_size1;
		const int m_size2;
		const T m_default;
		const std::wstring m_firstChildTag;
		const std::wstring m_secondChildTag;
	};

	CvInfoUtil& add(int** var, const wchar_t* rootTag, const wchar_t* firstChildTag, const wchar_t* secondChildTag, int size1, int size2, int defaultValue = 0)
	{
		m_wrappedVars.push_back(new DoubleArrayWrapper<int>(var, rootTag, firstChildTag, secondChildTag, size1, size2, defaultValue));
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
				if (element > -1 && !algo::contains(ref(), element))
					ref().push_back(element);
			algo::sort(ref());
		}

		void sendVarToPython(const char* file) const
		{
			//Cy::call(file, "handleVector", Cy::Args()
			//	<< ref()
			//);
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

		void readXml(CvXMLLoadUtility* pXML)
		{
			ref().read(pXML, m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaults(static_cast<const IDValueMapWrapper*>(source)->ref());
		}

		IDValueMap_T& ref() const { return *static_cast<IDValueMap_T*>(m_ptr); }
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
		friend struct CvInfoUtil;

	protected:
		IDValueMapWithDelayedResolutionWrapper(IDValueMap_T& var, const wchar_t* tag)
			: WrappedVar(static_cast<void*>(&var), tag)
		{}

		void uninitVar()
		{
			ref().removeDelayedResolution();
		}

		void checkSum(uint32_t& iSum) const
		{
			CheckSumC(iSum, ref());
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			ref().readWithDelayedResolution(pXML, m_tag.c_str());
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaultDelayedResolution(static_cast<const IDValueMapWithDelayedResolutionWrapper*>(source)->ref());
		}

		IDValueMap_T& ref() const { return *static_cast<IDValueMap_T*>(m_ptr); }
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& addDelayedResolution(IDValueMap<T1, T2, default_>& map, const wchar_t* rootTag)
	{
		m_wrappedVars.push_back(new IDValueMapWithDelayedResolutionWrapper<IDValueMap<T1, T2, default_> >(map, rootTag));
		return *this;
	}

	///=====================================
	/// IDValueMap of paired arrays wrapper
	///=====================================

	template <typename IDValueMap_T>
	struct IDValueMapOfPairedArrayWrapper : WrappedVar
	{
		friend struct CvInfoUtil;

	protected:
		IDValueMapOfPairedArrayWrapper(IDValueMap_T& var, const wchar_t* rootTag, const wchar_t* firstChildTag, const wchar_t* secondChildTag, int pairedArraySize)
			: WrappedVar(static_cast<void*>(&var), rootTag)
			, m_pairedArraySize(pairedArraySize)
			, m_firstChildTag(firstChildTag)
			, m_secondChildTag(secondChildTag)
		{}

		void uninitVar()
		{
			//foreach_(const IDValueMap_T::pair_t& pair, ref())
			//{
			//	SAFE_DELETE_ARRAY(pair.second);
			//}
		}

		void checkSum(uint32_t& iSum) const
		{
			foreach_(const IDValueMap_T::pair_t& pair, ref())
			{
				CheckSum(iSum, pair.first);
				CheckSum(iSum, pair.second, m_pairedArraySize);
			}
		}

		void readXml(CvXMLLoadUtility* pXML)
		{
			ref().readPairedArrays(pXML, m_tag.c_str(), m_firstChildTag.c_str(), m_secondChildTag.c_str(), m_pairedArraySize);
		}

		void copyNonDefaults(const WrappedVar* source)
		{
			ref().copyNonDefaultPairedArrays(static_cast<const IDValueMapOfPairedArrayWrapper*>(source)->ref());
		}

		void sendVarToPython(const char* file)
		{
		}

		IDValueMap_T& ref() const { return *static_cast<IDValueMap_T*>(m_ptr); }

	private:
		const int m_pairedArraySize;
		const std::wstring m_firstChildTag;
		const std::wstring m_secondChildTag;
	};

	template <typename T1, typename T2, T2 default_>
	CvInfoUtil& add(IDValueMap<T1, T2*, default_>& map, const wchar_t* rootTag, const wchar_t* firstChildTag, const wchar_t* secondChildTag, int pairedArraySize)
	{
		m_wrappedVars.push_back(new IDValueMapOfPairedArrayWrapper<IDValueMap<T1, T2*, default_> >(map, rootTag, firstChildTag, secondChildTag, pairedArraySize));
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
	void sendDataMembersToPython(const std::string file) const
	{
		foreach_(const WrappedVar* wrapper, m_wrappedVars)
			wrapper->sendVarToPython(file.c_str());
	}

	///========================================================
	/// Wrapped pointers to the data members of an info object
	///========================================================

	std::vector<WrappedVar*> m_wrappedVars;
};

#endif