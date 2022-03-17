// CvTaggedSaveFormatWrapper.cpp

#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"

#ifdef _DEBUG
//#define	DETAILED_TRACE	1
#endif

#ifdef DETAILED_TRACE
#define DEBUG_TRACE(x)			OutputDebugString(x);
#define DEBUG_TRACE2(x,y)		OutputDebugString(CvString::format(x,y).c_str());
#define DEBUG_TRACE3(x,y,z)		OutputDebugString(CvString::format(x,y,z).c_str());
#define DEBUG_TRACE4(w,x,y,z)	OutputDebugString(CvString::format(w,x,y,z).c_str());
#else
#define DEBUG_TRACE(x)			;
#define DEBUG_TRACE2(x,y)		;
#define DEBUG_TRACE3(x,y,z)		;
#define DEBUG_TRACE4(w,x,y,z)	;
#endif

#ifdef TEMP_DEBUGGING_SUPPORT
class StreamWrapper : public FDataStreamBase
{
public:
	StreamWrapper(FDataStreamBase* wrapped)
	{
		m_wrapped = wrapped;
		m_lenRead = 0;
	}

	virtual void	Rewind() { m_wrapped->Rewind(); }
	virtual bool	AtEnd() { return m_wrapped->AtEnd(); }
	virtual void	FastFwd() { m_wrapped->FastFwd(); }
	virtual uint32_t  GetPosition() const { return m_lenRead; }
	virtual void    SetPosition(uint32_t position) { m_wrapped->SetPosition(position); }
	virtual void    Truncate() { m_wrapped->Truncate(); }
	virtual void	Flush() { m_wrapped->Flush(); }
	virtual uint32_t	GetEOF() const { return m_wrapped->GetEOF(); }
	virtual uint32_t			GetSizeLeft() const { return m_wrapped->GetSizeLeft(); }
	virtual void	CopyToMem(void* mem) { m_wrapped->CopyToMem(mem); }

	virtual uint32_t	WriteString(const wchar_t* szName) { return m_wrapped->WriteString(szName); }
	virtual uint32_t	WriteString(const char* szName) { return m_wrapped->WriteString(szName); }
	virtual uint32_t	WriteString(const std::string& szName) { return m_wrapped->WriteString(szName); }
	virtual uint32_t	WriteString(const std::wstring& szName) { return m_wrapped->WriteString(szName); }
	virtual uint32_t	WriteString(int count, std::string values[]) { return m_wrapped->WriteString(count,values); }
	virtual uint32_t	WriteString(int count, std::wstring values[]) { return m_wrapped->WriteString(count,values); }

	virtual uint32_t	ReadString(char* szName){ return m_wrapped->ReadString(szName); m_lenRead += strlen(szName); }
	virtual uint32_t	ReadString(wchar_t* szName) { return m_wrapped->ReadString(szName); m_lenRead += wcslen(szName); }
	virtual uint32_t	ReadString(std::string& szName) { return m_wrapped->ReadString(szName); m_lenRead += szName.length(); }
	virtual uint32_t	ReadString(std::wstring& szName) { return m_wrapped->ReadString(szName); m_lenRead += 2*szName.length(); }
	virtual uint32_t	ReadString(int count, std::string values[]) { return m_wrapped->ReadString(count,values); m_lenRead += count*values[0].length(); }
	virtual uint32_t	ReadString(int count, std::wstring values[]) { return m_wrapped->ReadString(count,values); m_lenRead += 2*count*values[0].length(); }

	virtual char*		ReadString() { char* result = m_wrapped->ReadString(); m_lenRead += (result == NULL ? 0 : strlen(result)); return result; }
	virtual wchar_t*	ReadWideString() { wchar_t* result = m_wrapped->ReadWideString(); m_lenRead += (result == NULL ? 0 : 2*wcslen(result)); return result; }
	virtual void		Read(char *arg) { m_wrapped->Read(arg); m_lenRead++;}
	virtual void		Read(byte *arg) { m_wrapped->Read(arg); m_lenRead++;}
	virtual void		Read(int count, char values[]){ m_wrapped->Read(count, values); m_lenRead += count;}
	virtual void		Read(int count, byte values[]) { m_wrapped->Read(count, values); m_lenRead += count;}
	virtual void		Read(bool *arg) { m_wrapped->Read(arg); m_lenRead++;}
	virtual void		Read(int count, bool values[]) { m_wrapped->Read(count, values); m_lenRead += count;}
	virtual void		Read(short* s) { m_wrapped->Read(s); m_lenRead += 2;}
	virtual void		Read(uint16_t* s)  { m_wrapped->Read(s); m_lenRead += 2;}
	virtual void		Read(int count, short values[]){ m_wrapped->Read(count, values); m_lenRead += count*2;}
	virtual void		Read(int count, uint16_t values[]) { m_wrapped->Read(count, values); m_lenRead += count*2;}
	virtual void		Read(int* i){ m_wrapped->Read(i); m_lenRead += 4;}
	virtual void		Read(uint32_t* i) { m_wrapped->Read(i); m_lenRead += 4;}
	virtual void 		Read(int count, int values[]) { m_wrapped->Read(count, values); m_lenRead += 4*count;}
	virtual void 		Read(int count, uint32_t values[]) { m_wrapped->Read(count, values); m_lenRead += 4*count; }

	virtual void		Read(long* l) { m_wrapped->Read(l); m_lenRead += 4;}
	virtual void		Read(unsigned long* l)  { m_wrapped->Read(l); m_lenRead += 4;}
	virtual void 		Read(int count, long values[]) { m_wrapped->Read(count, values); m_lenRead += 4*count;}
	virtual void 		Read(int count, unsigned long values[])  { m_wrapped->Read(count, values); m_lenRead += 4*count;}

	virtual void		Read(float* value) { m_wrapped->Read(value); m_lenRead += sizeof(float);}
	virtual void		Read(int count, float values[]) { m_wrapped->Read(count, values); m_lenRead += count*sizeof(float);}

	virtual void		Read(double* value) { m_wrapped->Read(value); m_lenRead += sizeof(double);}
	virtual void		Read(int count, double values[]) { m_wrapped->Read(count, values); m_lenRead += count*sizeof(double);}

	virtual void		Write( char value) { m_wrapped->Write(value); }
	virtual void		Write(byte value) { m_wrapped->Write(value); }
	virtual void		Write(int count, const  char values[]) { m_wrapped->Write(count, values); }
	virtual void		Write(int count, const  byte values[]) { m_wrapped->Write(count, values); }

	virtual void		Write(bool value) { m_wrapped->Write(value); }
	virtual void		Write(int count, const bool values[]) { m_wrapped->Write(count, values); }

	virtual void		Write(short value) { m_wrapped->Write(value); }
	virtual void		Write(uint16_t value){ m_wrapped->Write(value); }
	virtual void		Write(int count, const short values[]) { m_wrapped->Write(count, values); }
	virtual void		Write(int count, const uint16_t values[]) { m_wrapped->Write(count, values); }

	virtual void		Write(int value) { m_wrapped->Write(value); }
	virtual void		Write(uint32_t value) { m_wrapped->Write(value); }
	virtual void 		Write(int count, const int values[]) { m_wrapped->Write(count, values); }
	virtual void		Write(int count, const uint32_t values[])  { m_wrapped->Write(count, values); }

	virtual void		Write(long value) { m_wrapped->Write(value); }
	virtual void		Write(unsigned long  value) { m_wrapped->Write(value); }
	virtual void 		Write(int count, const long values[]) { m_wrapped->Write(count, values); }
	virtual void		Write(int count, const unsigned long values[]) { m_wrapped->Write(count, values); }

	virtual void		Write(float value) { m_wrapped->Write(value); }
	virtual void		Write(int count, const float values[]) { m_wrapped->Write(count, values); }

	virtual void		Write(double value) { m_wrapped->Write(value); }
	virtual void		Write(int count, const double values[]) { m_wrapped->Write(count, values); }

	FDataStreamBase* m_wrapped;
	int m_lenRead;

	static StreamWrapper* g_last;
};

StreamWrapper*	StreamWrapper::g_last = NULL;
#endif

static CvTaggedSaveFormatWrapper* pSingleton = NULL;
static int usageSeq = 0;

#define VAR 2	//	Dummy value to allow definition of array bounds interpretted at runtime

//	Everything is saved as an (id, value) pair.  These can be of broadly two types:
//		id definitions		Define an id->element name mapping
//		value definitions	Defines the value of an id'd element
//	Writing the entries for the dictionary of id->elements names as we go
//	means we only have to save a name once and thereafter can use the corresponding
//	id, saving lots of storage.  An id definiton is signified by an entry with a
//	reserved id of its own
#define SAVE_ELEMENT_ID_DICTIONARY_ENTRY (-1)	//	Special id value for id->name mapping elements (payload is a mapping element)
#define SAVE_ELEMENT_ID_OBJECT_DELIMITER (-2)	//	Special id for object delimiter elements
#define SAVE_ELEMENT_ID_CLASS_MAP		 (-3)	//	Special id for class mapping table elements

#pragma pack(push)
#pragma pack(1)
//	Mapping entry for a new id dictionary term
typedef struct id_mapping_entry
{
	int				escapeId;	//	Always SAVE_ELEMENT_ID_DICTIONARY_ENTRY
	int				id;
	SaveValueType	type;
	uint8_t			nameLen;
	char			name[VAR];
} id_mapping_entry;
#define ID_MAPPING_ENTRY_LEN(nameLen) (sizeof(id_mapping_entry) + ((int)sizeof(char))*(nameLen-VAR))

//	Mapping entry for a new id dictionary term
typedef struct id_mapping_entry_maximal
{
	int				escapeId;	//	Always SAVE_ELEMENT_ID_DICTIONARY_ENTRY
	int				id;
	SaveValueType	type;
	uint8_t			nameLen;
	char			name[256];
} id_mapping_entry_maximal;

//	Object delimiter (placed at start and end of each serialized instance)
typedef struct object_delimiter_entry
{
	int				escapeId;	//	Always SAVE_ELEMENT_ID_OBJECT_DELIMITER
	bool			bStart;		//	true for object start, false for end
	uint8_t			nameLen;
	char			name[VAR];
} object_delimiter_entry;
#define OBJECT_DELIMITER_ENTRY_LEN(nameLen) (sizeof(object_delimiter_entry) + ((int)sizeof(char))*(nameLen-VAR))

//	Object delimiter (placed at start and end of each serialized instance)
typedef struct object_delimiter_entry_maximal
{
	int				escapeId;	//	Always SAVE_ELEMENT_ID_OBJECT_DELIMITER
	bool			bStart;		//	true for object start, false for end
	uint8_t			nameLen;
	char			name[256];
} object_delimiter_entry_maximal;

//	Class mapping tables convey the class type string->enum mapping as it
//	existed in the version used to save the game
typedef struct class_mapping_table_entry
{
	int					escapeId;	//	Always SAVE_ELEMENT_ID_CLASS_MAP
	RemappedClassType	classType;
	int					numClasses;
	//	Strings follow using native stream serialization
} class_mapping_table_entry;

//	Value entries for various value types
//
//	Value entry for type char
typedef struct value_entry_char
{
	int id;
	char value;
} value_entry_char;

//	Value entry for type char
typedef struct value_entry_byte
{
	int id;
	uint8_t value;
} value_entry_byte;

//	Value entry for type char array
typedef struct value_entry_char_array
{
	int id;
	int numChars;
	char value[VAR];
} value_entry_char_array;
#define VALUE_ENTRY_CHAR_ARRAY_SIZE_FROM_NUM(numChars)	((int)sizeof(value_entry_char_array)+((int)sizeof(char))*(numChars-VAR))

//	Value entry for type byte array
typedef struct value_entry_byte_array
{
	int id;
	int numBytes;
	uint8_t value[VAR];
} value_entry_byte_array;
#define VALUE_ENTRY_BYTE_ARRAY_SIZE_FROM_NUM(numBytes)	((int)sizeof(value_entry_byte_array)+((int)sizeof(uint8_t))*(numBytes-VAR))

//	Value entry for type bool
typedef struct value_entry_bool
{
	int id;
	bool value;
} value_entry_bool;

//	Value entry for type bool array
typedef struct value_entry_bool_array
{
	int id;
	int numBools;
	bool value[VAR];
} value_entry_bool_array;
#define VALUE_ENTRY_BOOL_ARRAY_SIZE_FROM_NUM(numBools)	((int)sizeof(value_entry_bool_array)+((int)sizeof(bool))*(numBools-VAR))

//	Value entry for type short
typedef struct value_entry_short
{
	int id;
	short value;
} value_entry_short;

//	Value entry for type unsigned short
typedef struct value_entry_unsigned_short
{
	int id;
	uint16_t value;
} value_entry_unsigned_short;

//	Value entry for type short array
typedef struct value_entry_short_array
{
	int id;
	int numShorts;
	short value[VAR];
} value_entry_short_array;
#define VALUE_ENTRY_SHORT_ARRAY_SIZE_FROM_NUM(numShorts)	((int)sizeof(value_entry_short_array)+((int)sizeof(short))*(numShorts-VAR))

//	Value entry for type unsigned short array
typedef struct value_entry_unsigned_short_array
{
	int id;
	int numShorts;
	uint16_t value[VAR];
} value_entry_unsigned_short_array;

//	Value entry for type int
typedef struct value_entry_int
{
	int id;
	int value;
} value_entry_int;

//	Value entry for type unsigned int
typedef struct value_entry_unsigned_int
{
	int id;
	uint32_t value;
} value_entry_unsigned_int;

//	Value entry for type int array
typedef struct value_entry_int_array
{
	int id;
	int numInts;
	int value[VAR];
} value_entry_int_array;
#define VALUE_ENTRY_INT_ARRAY_SIZE_FROM_NUM(numInts)	((int)sizeof(value_entry_int_array)+((int)sizeof(int))*(numInts-VAR))

//	Value entry for type unsigned int array
typedef struct value_entry_unsigned_int_array
{
	int id;
	int numInts;
	uint16_t value[VAR];
} value_entry_unsigned_int_array;

//	Value entry for type long
typedef struct value_entry_long
{
	int id;
	long value;
} value_entry_long;

//	Value entry for type unsigned long
typedef struct value_entry_unsigned_long{
	int id;
	unsigned long value;
} value_entry_unsigned_long;

//	Value entry for type long array
typedef struct value_entry_long_array
{
	int id;
	int numLongs;
	int value[VAR];
} value_entry_long_array;
#define VALUE_ENTRY_LONG_ARRAY_SIZE_FROM_NUM(numLongs)	((int)sizeof(value_entry_long_array)+((int)sizeof(long))*(numLongs-VAR))

//	Value entry for type unsigned long array
typedef struct value_entry_unsigned_long_array
{
	int id;
	int numLongs;
	uint16_t value[VAR];
} value_entry_unsigned_long_array;

//	Value entry for type float
typedef struct value_entry_float
{
	int id;
	float value;
} value_entry_float;

//	Value entry for type float array
typedef struct value_entry_float_array
{
	int id;
	int numFloats;
	int value[VAR];
} value_entry_float_array;
#define VALUE_ENTRY_FLOAT_ARRAY_SIZE_FROM_NUM(numFloats)	((int)sizeof(value_entry_float_array)+((int)sizeof(float))*(numFloats-VAR))

//	Value entry for type double
typedef struct value_entry_double
{
	int id;
	double value;
} value_entry_double;

//	Value entry for type double array
typedef struct value_entry_double_array
{
	int id;
	int numDoubles;
	int value[VAR];
} value_entry_double_array;
#define VALUE_ENTRY_DOUBLE_ARRAY_SIZE_FROM_NUM(numDoubles)	((int)sizeof(value_entry_double_array)+((int)sizeof(double))*(numDoubles-VAR))

//	Value entry for type string
typedef struct value_entry_string
{
	int id;
	//	String follows in native steam format
} value_entry_string;

//	Value entry for type string array
typedef struct value_entry_string_array
{
	int id;
	int numStrings;
	//	Strings follow using native stream serialization
} value_entry_string_array;

//	Value entry for type int
typedef struct value_class_enum
{
	int					id;
	RemappedClassType	classType;
	int					value;
} value_class_enum;

//	Value entry for type int
typedef struct value_class_enum_array
{
	int					id;
	RemappedClassType	classType;
	int					count;
	//	Original values follow in native stream format
} value_class_enum_array;

//	Value entry for type int class array (indexes are class enums)
typedef struct value_entry_class_int_array
{
	int					id;
	RemappedClassType	classType;
	int					numInts;
	int					value[VAR];
} value_entry_class_int_array;
#define VALUE_ENTRY_CLASS_INT_ARRAY_SIZE_FROM_NUM(numInts)	((int)sizeof(value_entry_class_int_array)+((int)sizeof(int))*(numInts-VAR))

//	Value entry for type bool class array (indexes are class enums)
typedef struct value_entry_class_bool_array
{
	int					id;
	RemappedClassType	classType;
	int					numBools;
	bool				value[VAR];
} value_entry_class_bool_array;
#define VALUE_ENTRY_CLASS_BOOL_ARRAY_SIZE_FROM_NUM(numBools)	((int)sizeof(value_entry_class_bool_array)+((int)sizeof(bool))*(numBools-VAR))

//	Value entry for type bool class array (indexes are class enums)
typedef struct value_entry_class_class_array
{
	int					id;
	RemappedClassType	classType;
	RemappedClassType	valueClassType;
	int					numValues;
	int					value[VAR];
} value_entry_class_class_array;
#define VALUE_ENTRY_CLASS_CLASS_ARRAY_SIZE_FROM_NUM(numValues)	((int)sizeof(value_entry_class_class_array)+((int)sizeof(int))*(numValues-VAR))

#pragma pack(pop)

//	Encapsulated dictionary entry
class DictionaryEntry
{
public:
	DictionaryEntry()
		: m_id(0)
		, m_type(SAVE_VALUE_ANY)
	{
	}
	virtual ~DictionaryEntry()
	{
	}

public:
	CvString		m_name;
	int				m_id;
	SaveValueType	m_type;
};

static int lastIdMatch = -1;

CvTaggedSaveFormatWrapper::CvTaggedSaveFormatWrapper()
	: m_stream(NULL)
	, m_useTaggedFormat(false)
	, m_writtenMappingTables(false)
	, m_bReadNextElementHeader(false)
	, m_delimiterIsStart(false)
	, m_nestingDepth(0)
	, m_iNextElementType(0)
	, m_iNextElementNameId(0)
	, m_streamNestingDepth(0)
	, m_inUse(false)
{
	reset(false);
}

CvTaggedSaveFormatWrapper::~CvTaggedSaveFormatWrapper()
{
}

void
CvTaggedSaveFormatWrapper::reset(bool useTaggedFormat)
{
	usageSeq++;
	lastIdMatch = -1;
	m_idDictionary.clear();
	m_reverseIdDictionary.clear();

	for(int i = 0; i < NUM_REMAPPED_TYPES; i++)
	{
		m_enumMaps[i].clear();
	}

	m_useTaggedFormat = useTaggedFormat;
	m_stream = NULL;
	m_writtenMappingTables = false;
	m_nestingDepth = 0;
	m_streamNestingDepth = 0;
	m_bReadNextElementHeader = false;
	m_requestUseTaggedFormat = false;

	m_warnings.clear();
}

//	Get the singleton instance
CvTaggedSaveFormatWrapper&
CvTaggedSaveFormatWrapper::getSaveFormatWrapper()
{
	if ( NULL == pSingleton )
	{
		pSingleton = new CvTaggedSaveFormatWrapper();
	}

	return *pSingleton;
}


//	Attach the wrapper to a stream
void
CvTaggedSaveFormatWrapper::AttachToStream(FDataStreamBase* pStream)
{
#ifdef TEMP_DEBUGGING_SUPPORT
	if ( StreamWrapper::g_last != NULL )
	{
		m_stream = StreamWrapper::g_last;
		StreamWrapper::g_last->m_wrapped = pStream;
	}
	else
	{
		StreamWrapper::g_last = new StreamWrapper(pStream);;
		m_stream = StreamWrapper::g_last;
	}
#else
	m_stream = pStream;
#endif
}

void
CvTaggedSaveFormatWrapper::WriteObjectDelimiter(const char* name, int& idHint, int& idSeq, bool bStart)
{
	PROFILE_FUNC();

	object_delimiter_entry	entry;

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Object %s: %s\n",bStart ? "start" : "end",name)

		entry.escapeId = SAVE_ELEMENT_ID_OBJECT_DELIMITER;
		entry.bStart = bStart;
		entry.nameLen = (name == NULL ? 0 : strlen(name));

		m_stream->Write((int)(sizeof(entry) - sizeof(entry.name)), (const uint8_t*)&entry);
		if ( name != NULL )
		{
			m_stream->Write(entry.nameLen, (const uint8_t*)name);
		}
	}
}

void
CvTaggedSaveFormatWrapper::WriteClassMappingTable(RemappedClassType classType)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	class_mapping_table_entry	entry;

	DEBUG_TRACE2("Output class mapping table for class type %d\n", (int)classType)

	entry.escapeId = SAVE_ELEMENT_ID_CLASS_MAP;
	entry.classType = classType;

	switch(classType)
	{
	case REMAPPED_CLASS_TYPE_BUILDINGS:
		entry.numClasses = GC.getNumBuildingInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvBuildingInfo& info = GC.getBuildingInfo((BuildingTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_UNITS:
		entry.numClasses = GC.getNumUnitInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvUnitInfo& info = GC.getUnitInfo((UnitTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_PROJECTS:
		entry.numClasses = GC.getNumProjectInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvProjectInfo& info = GC.getProjectInfo((ProjectTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_BONUSES:
		entry.numClasses = GC.getNumBonusInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvBonusInfo& info = GC.getBonusInfo((BonusTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_SPECIALISTS:
		entry.numClasses = GC.getNumSpecialistInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvSpecialistInfo& info = GC.getSpecialistInfo((SpecialistTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_IMPROVEMENTS:
		entry.numClasses = GC.getNumImprovementInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvImprovementInfo& info = GC.getImprovementInfo((ImprovementTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_RELIGIONS:
		entry.numClasses = GC.getNumReligionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvReligionInfo& info = GC.getReligionInfo((ReligionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_COMBATINFOS:
		entry.numClasses = GC.getNumUnitCombatInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvUnitCombatInfo& info = GC.getUnitCombatInfo((UnitCombatTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_PROMOTIONLINES:
		entry.numClasses = GC.getNumPromotionLineInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvPromotionLineInfo& info = GC.getPromotionLineInfo((PromotionLineTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_MAPCATEGORIES:
		entry.numClasses = GC.getNumMapCategoryInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvMapCategoryInfo& info = GC.getMapCategoryInfo((MapCategoryTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_IDEACLASSES:
		entry.numClasses = GC.getNumIdeaClassInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvIdeaClassInfo& info = GC.getIdeaClassInfo((IdeaClassTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_IDEAS:
		entry.numClasses = GC.getNumIdeaInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvIdeaInfo& info = GC.getIdeaInfo((IdeaTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	//case REMAPPED_CLASS_TYPE_TRAITOPTIONEDITS:
	//	entry.numClasses = GC.getNumTraitOptionEditsInfos();
	//	m_stream->Write(sizeof(class_mapping_table_entry), (byte*)&entry);
	//	for(int i = 0; i < entry.numClasses; i++)
	//	{
	//		CvInfoBase& info = GC.getTraitOptionEditsInfo((TraitOptionEditsTypes)i);

	//		DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
	//		m_stream->WriteString(info.getType());
	//	}
	//	break;
	case REMAPPED_CLASS_TYPE_PROMOTIONS:
		entry.numClasses = GC.getNumPromotionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvPromotionInfo& info = GC.getPromotionInfo((PromotionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_CORPORATIONS:
		entry.numClasses = GC.getNumCorporationInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvCorporationInfo& info = GC.getCorporationInfo((CorporationTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_TECHS:
		entry.numClasses = GC.getNumTechInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvTechInfo& info = GC.getTechInfo((TechTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_CIVICS:
		entry.numClasses = GC.getNumCivicInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvCivicInfo& info = GC.getCivicInfo((CivicTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_VOTES:
		entry.numClasses = GC.getNumVoteInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvVoteInfo& info = GC.getVoteInfo((VoteTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_VOTE_SOURCES:
		entry.numClasses = GC.getNumVoteSourceInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvVoteSourceInfo& info = GC.getVoteSourceInfo((VoteSourceTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_SPECIAL_UNITS:
		entry.numClasses = GC.getNumSpecialUnitInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvSpecialUnitInfo& info = GC.getSpecialUnitInfo((SpecialUnitTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS:
		entry.numClasses = GC.getNumSpecialBuildingInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvSpecialBuildingInfo& info = GC.getSpecialBuildingInfo((SpecialBuildingTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_UPKEEPS:
		entry.numClasses = GC.getNumUpkeepInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvUpkeepInfo& info = GC.getUpkeepInfo((UpkeepTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_HURRIES:
		entry.numClasses = GC.getNumHurryInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvHurryInfo& info = GC.getHurryInfo((HurryTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_FEATURES:
		entry.numClasses = GC.getNumFeatureInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvFeatureInfo& info = GC.getFeatureInfo((FeatureTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_CIVIC_OPTIONS:
		entry.numClasses = GC.getNumCivicOptionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvCivicOptionInfo& info = GC.getCivicOptionInfo((CivicOptionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_BUILDS:
		entry.numClasses = GC.getNumBuildInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvBuildInfo& info = GC.getBuildInfo((BuildTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_TERRAINS:
		entry.numClasses = GC.getNumTerrainInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvTerrainInfo& info = GC.getTerrainInfo((TerrainTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_ROUTES:
		entry.numClasses = GC.getNumRouteInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvRouteInfo& info = GC.getRouteInfo((RouteTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_VICTORIES:
		entry.numClasses = GC.getNumVictoryInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvVictoryInfo& info = GC.getVictoryInfo((VictoryTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_LEADERHEADS:
		entry.numClasses = GC.getNumLeaderHeadInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvLeaderHeadInfo& info = GC.getLeaderHeadInfo((LeaderHeadTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_CIVILIZATIONS:
		entry.numClasses = GC.getNumCivilizationInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvCivilizationInfo& info = GC.getCivilizationInfo((CivilizationTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_GAMEOPTIONS:
		entry.numClasses = GC.getNumGameOptionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvGameOptionInfo& info = GC.getGameOptionInfo((GameOptionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_MPOPTIONS:
		entry.numClasses = GC.getNumMPOptionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvMPOptionInfo& info = GC.getMPOptionInfo((MultiplayerOptionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_UNITAIS:
		entry.numClasses = NUM_UNITAI_TYPES;
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvInfoBase& info = GC.getUnitAIInfo((UnitAITypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_EVENTS:
		entry.numClasses = GC.getNumEventInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvEventInfo& info = GC.getEventInfo((EventTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_EVENT_TRIGGERS:
		entry.numClasses = GC.getNumEventTriggerInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvEventTriggerInfo& info = GC.getEventTriggerInfo((EventTriggerTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_GAMESPEEDS:
		entry.numClasses = GC.getNumGameSpeedInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvGameSpeedInfo& info = GC.getGameSpeedInfo((GameSpeedTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_PROPERTIES:
		entry.numClasses = GC.getNumPropertyInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvPropertyInfo& info = GC.getPropertyInfo((PropertyTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_TRAITS:
		entry.numClasses = GC.getNumTraitInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvTraitInfo& info = GC.getTraitInfo((TraitTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_INVISIBLES:
		entry.numClasses = GC.getNumInvisibleInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for(int i = 0; i < entry.numClasses; i++)
		{
			const CvInvisibleInfo& info = GC.getInvisibleInfo((InvisibleTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_MISSIONS:
		entry.numClasses = GC.getNumMissionInfos();
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for (int i = 0; i < entry.numClasses; i++)
		{
			const CvMissionInfo& info = GC.getMissionInfo((MissionTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_YIELDS:
		entry.numClasses = NUM_YIELD_TYPES;
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for (int i = 0; i < entry.numClasses; i++)
		{
			const CvYieldInfo& info = GC.getYieldInfo((YieldTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_COMMERCES:
		entry.numClasses = NUM_COMMERCE_TYPES;
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for (int i = 0; i < entry.numClasses; i++)
		{
			const CvCommerceInfo& info = GC.getCommerceInfo((CommerceTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	case REMAPPED_CLASS_TYPE_DOMAINS:
		entry.numClasses = NUM_DOMAIN_TYPES;
		m_stream->Write(sizeof(class_mapping_table_entry), (uint8_t*)&entry);
		for (int i = 0; i < entry.numClasses; i++)
		{
			const CvInfoBase& info = GC.getDomainInfo((DomainTypes)i);

			DEBUG_TRACE3("\t%d : %s\n", i, info.getType())
			m_stream->WriteString(info.getType());
		}
		break;
	default:
		FErrorMsg("Unexpected RemappedClassType");
		break;
	}
}

void
CvTaggedSaveFormatWrapper::WriteClassMappingTables()
{
	PROFILE_FUNC();

	//	Write out mapping tables for all mappable enum types
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_BUILDINGS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_UNITS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_PROJECTS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_BONUSES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_SPECIALISTS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_IMPROVEMENTS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_RELIGIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_COMBATINFOS);
	//TB Promotion Line Mod begin
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_PROMOTIONLINES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_MAPCATEGORIES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_IDEACLASSES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_IDEAS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_TRAITS);
	//WriteClassMappingTable(REMAPPED_CLASS_TYPE_TRAITOPTIONEDITS);
	//TB Promotion Line Mod end
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_PROMOTIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_CORPORATIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_TECHS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_CIVICS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_VOTES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_VOTE_SOURCES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_SPECIAL_UNITS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_UPKEEPS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_HURRIES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_FEATURES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_CIVIC_OPTIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_BUILDS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_TERRAINS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_ROUTES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_VICTORIES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_LEADERHEADS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_CIVILIZATIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_GAMEOPTIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_MPOPTIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_UNITAIS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_EVENTS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_EVENT_TRIGGERS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_GAMESPEEDS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_PROPERTIES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_INVISIBLES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_MISSIONS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_YIELDS);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_COMMERCES);
	WriteClassMappingTable(REMAPPED_CLASS_TYPE_DOMAINS);
}

//	How many members of a given class type were present at save time?
int
CvTaggedSaveFormatWrapper::getNumClassEnumValues(RemappedClassType classType)
{
	PROFILE_FUNC();

	if ( m_useTaggedFormat )
	{
		return m_enumMaps[classType].size();
	}
	else
	{
		int result = 0;

		switch(classType)
		{
		case REMAPPED_CLASS_TYPE_BUILDINGS:
			result = GC.getNumBuildingInfos();
			break;
		case REMAPPED_CLASS_TYPE_UNITS:
			result = GC.getNumUnitInfos();
			break;
		case REMAPPED_CLASS_TYPE_PROJECTS:
			result = GC.getNumProjectInfos();
			break;
		case REMAPPED_CLASS_TYPE_BONUSES:
			result = GC.getNumBonusInfos();
			break;
		case REMAPPED_CLASS_TYPE_SPECIALISTS:
			result = GC.getNumSpecialistInfos();
			break;
		case REMAPPED_CLASS_TYPE_IMPROVEMENTS:
			result = GC.getNumImprovementInfos();
			break;
		case REMAPPED_CLASS_TYPE_RELIGIONS:
			result = GC.getNumReligionInfos();
			break;
		case REMAPPED_CLASS_TYPE_COMBATINFOS:
			result = GC.getNumUnitCombatInfos();
			break;
		case REMAPPED_CLASS_TYPE_PROMOTIONLINES:
			result = GC.getNumPromotionLineInfos();
			break;
		case REMAPPED_CLASS_TYPE_MAPCATEGORIES:
			result = GC.getNumMapCategoryInfos();
			break;
		case REMAPPED_CLASS_TYPE_IDEACLASSES:
			result = GC.getNumIdeaClassInfos();
			break;
		case REMAPPED_CLASS_TYPE_IDEAS:
			result = GC.getNumIdeaInfos();
			break;
		//case REMAPPED_CLASS_TYPE_TRAITOPTIONEDITS:
		//	result = GC.getNumTraitOptionEditsInfos();
		//	break;
		case REMAPPED_CLASS_TYPE_PROMOTIONS:
			result = GC.getNumPromotionInfos();
			break;
		case REMAPPED_CLASS_TYPE_CORPORATIONS:
			result = GC.getNumCorporationInfos();
			break;
		case REMAPPED_CLASS_TYPE_TECHS:
			result = GC.getNumTechInfos();
			break;
		case REMAPPED_CLASS_TYPE_CIVICS:
			result = GC.getNumCivicInfos();
			break;
		case REMAPPED_CLASS_TYPE_VOTES:
			result = GC.getNumVoteInfos();
			break;
		case REMAPPED_CLASS_TYPE_VOTE_SOURCES:
			result = GC.getNumVoteSourceInfos();
			break;
		case REMAPPED_CLASS_TYPE_SPECIAL_UNITS:
			result = GC.getNumSpecialUnitInfos();
			break;
		case REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS:
			result = GC.getNumSpecialBuildingInfos();
			break;
		case REMAPPED_CLASS_TYPE_UPKEEPS:
			result = GC.getNumUpkeepInfos();
			break;
		case REMAPPED_CLASS_TYPE_HURRIES:
			result = GC.getNumHurryInfos();
			break;
		case REMAPPED_CLASS_TYPE_FEATURES:
			result = GC.getNumFeatureInfos();
			break;
		case REMAPPED_CLASS_TYPE_CIVIC_OPTIONS:
			result = GC.getNumCivicOptionInfos();
			break;
		case REMAPPED_CLASS_TYPE_BUILDS:
			result = GC.getNumBuildInfos();
			break;
		case REMAPPED_CLASS_TYPE_TERRAINS:
			result = GC.getNumTerrainInfos();
			break;
		case REMAPPED_CLASS_TYPE_ROUTES:
			result = GC.getNumRouteInfos();
			break;
		case REMAPPED_CLASS_TYPE_VICTORIES:
			result = GC.getNumVictoryInfos();
			break;
		case REMAPPED_CLASS_TYPE_UNITAIS:
			result = NUM_UNITAI_TYPES;
			break;
		case REMAPPED_CLASS_TYPE_EVENTS:
			result = GC.getNumEventInfos();
			break;
		case REMAPPED_CLASS_TYPE_EVENT_TRIGGERS:
			result = GC.getNumEventTriggerInfos();
			break;
		case REMAPPED_CLASS_TYPE_GAMESPEEDS:
			result = GC.getNumGameSpeedInfos();
			break;
		case REMAPPED_CLASS_TYPE_PROPERTIES:
			result = GC.getNumPropertyInfos();
			break;
		case REMAPPED_CLASS_TYPE_TRAITS:
			result = GC.getNumTraitInfos();
			break;
		case REMAPPED_CLASS_TYPE_INVISIBLES:
			result = GC.getNumInvisibleInfos();
			break;
		case REMAPPED_CLASS_TYPE_MISSIONS:
			result = GC.getNumMissionInfos();
			break;
		case REMAPPED_CLASS_TYPE_YIELDS:
			result = NUM_YIELD_TYPES;
			break;
		case REMAPPED_CLASS_TYPE_COMMERCES:
			result = NUM_COMMERCE_TYPES;
			break;
		case REMAPPED_CLASS_TYPE_DOMAINS:
			result = NUM_DOMAIN_TYPES;
			break;
		default:
			FErrorMsg("Unexpected RemappedClassType");
			break;
		}

		return result;
	}
}

//	Translate explicitly from an old enum value to the current
int
CvTaggedSaveFormatWrapper::getNewClassEnumValue(RemappedClassType classType, int oldValue, bool allowMissing)
{
	PROFILE_FUNC();

	if ( m_useTaggedFormat )
	{
		if ( oldValue == -1 )
		{
			return -1;
		}
		else
		{
			std::vector<EnumInfo>& mapVector = m_enumMaps[classType];

			if ( oldValue < (int)mapVector.size() )
			{
				EnumInfo& info = mapVector[oldValue];

				if ( info.m_id == -1 && !info.m_lookedUp )
				{
					info.m_id = GC.getInfoTypeForString(info.m_szType, true);

					if ( info.m_id == -1 && !allowMissing )
					{
						//	Instantiated object uses class no longer defined - game is not save compatible
						HandleIncompatibleSave(CvString::format("Save format is not compatible due to missing class %s", info.m_szType.c_str()).c_str());
					}

					info.m_lookedUp = true;
				}

				return info.m_id;
			}
			else
			{
				return -1;
			}
		}
	}
	else
	{
		return oldValue;
	}
}

void
CvTaggedSaveFormatWrapper::WriteStartObject(const char* name, int& idHint, int& idSeq)
{
	PROFILE_FUNC();

	//	If we're about to output our first object spit out the mapping tables first
	if ( !m_writtenMappingTables && m_useTaggedFormat )
	{
		WriteClassMappingTables();

		m_writtenMappingTables = true;
		m_inUse = true;
	}

	WriteObjectDelimiter(name, idHint, idSeq, true);

	m_nestingDepth++;
}

void
CvTaggedSaveFormatWrapper::WriteEndObject()
{
	int dummy;

	PROFILE_FUNC();

	FAssert(m_nestingDepth > 0);

	m_nestingDepth--;

	WriteObjectDelimiter(NULL, dummy, dummy, false);
}

//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::WriteClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_class_enum entry;

		DEBUG_TRACE4("Write class enum for class %d, name %s: %d\n", classType, name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM, true);
		entry.classType = classType;
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::WriteClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType classType, short value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_class_enum entry;

		DEBUG_TRACE4("Write class enum for class %d, name %s: %d\n", classType, name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM, true);
		entry.classType = classType;
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

//	The following are for arrays whose index is a class enum value
void
CvTaggedSaveFormatWrapper::WriteClassArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, const int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_class_int_array entry;

		DEBUG_TRACE4("Write int class array for class %d, name %s, count=%d\n", classType, name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_INT_ARRAY, true);
		entry.classType = classType;
		entry.numInts = count;

		m_stream->Write(VALUE_ENTRY_CLASS_INT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

//	The following are for arrays whose index is a class enum value
void
CvTaggedSaveFormatWrapper::WriteClassArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, const bool values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_class_bool_array entry;

		DEBUG_TRACE4("Write bool class array for class %d, name %s, count=%d\n", classType, name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_BOOL_ARRAY, true);
		entry.classType = classType;
		entry.numBools = count;

		m_stream->Write(VALUE_ENTRY_CLASS_BOOL_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

//	The following are for arrays whose index is a class enum value and value is another class enum value
void
CvTaggedSaveFormatWrapper::WriteClassArrayOfClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType indexClassType, RemappedClassType valueClassType, int count, const int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_class_class_array entry;

		DEBUG_TRACE4("Write bool class array for class %d, name %s, count=%d\n", valueClassType, name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_BOOL_ARRAY, true);
		entry.classType = indexClassType;
		entry.valueClassType = valueClassType;
		entry.numValues = count;

		m_stream->Write(VALUE_ENTRY_CLASS_CLASS_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

//	Array whose values are class enum entities
void
CvTaggedSaveFormatWrapper::WriteClassEnumArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, short values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_class_enum_array entry;

		DEBUG_TRACE4("Write class enum array for class %d, name %s, count %d\n", classType, name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM_ARRAY, true);
		entry.classType = classType;
		entry.count = count;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

//	Array whose values are class enum entities
void
CvTaggedSaveFormatWrapper::WriteClassEnumArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_class_enum_array entry;

		DEBUG_TRACE4("Write class enum array for class %d, name %s, count %d\n", classType, name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM_ARRAY, true);
		entry.classType = classType;
		entry.count = count;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


//	The following methods are direct replacements for the write calls
//	in the underlying FDataStreamBase
void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, char value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_char entry;

		DEBUG_TRACE3("Write char %s: %c\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CHAR, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, uint8_t value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_byte entry;

		DEBUG_TRACE3("Write byte %s: %d\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_BYTE, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const  char values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_char_array entry;

		DEBUG_TRACE3("Write char array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_CHAR_ARRAY, true);
		entry.numChars = count;

		m_stream->Write(VALUE_ENTRY_CHAR_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const uint8_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_byte_array entry;

		DEBUG_TRACE3("Write byte array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_BYTE_ARRAY, true);
		entry.numBytes = count;

		m_stream->Write(VALUE_ENTRY_BYTE_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, bool value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_bool entry;

		DEBUG_TRACE3("Write bool %s: %d\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_BOOL, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const bool values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_bool_array entry;

		DEBUG_TRACE3("Write bool array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_BOOL_ARRAY, true);
		entry.numBools = count;

		m_stream->Write(VALUE_ENTRY_BOOL_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, short value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_short entry;

		DEBUG_TRACE3("Write short %s: %d\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_SHORT, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, uint16_t value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_short entry;

		DEBUG_TRACE3("Write unsigned short %s: %u\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_SHORT, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const short values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_short_array entry;

		DEBUG_TRACE3("Write short array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_SHORT_ARRAY, true);
		entry.numShorts = count;

		m_stream->Write(VALUE_ENTRY_SHORT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const uint16_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_short_array entry;

		DEBUG_TRACE3("Write unsigned short array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_SHORT_ARRAY, true);
		entry.numShorts = count;

		m_stream->Write(VALUE_ENTRY_SHORT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_int entry;

		DEBUG_TRACE3("Write int %s: %d\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_INT, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, uint32_t value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_int entry;

		DEBUG_TRACE3("Write unsigned int %s: %u\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_INT, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_int_array entry;

		DEBUG_TRACE3("Write int array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_INT_ARRAY, true);
		entry.numInts = count;

		m_stream->Write(VALUE_ENTRY_INT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const uint32_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_int_array entry;

		DEBUG_TRACE3("Write usigned int array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_INT_ARRAY, true);
		entry.numInts = count;

		m_stream->Write(VALUE_ENTRY_INT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, long value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_long entry;

		DEBUG_TRACE3("Write long %s: %ld\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_LONG, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, unsigned long  value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_long entry;

		DEBUG_TRACE3("Write unsigned long %s: %lu\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_LONG, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const long values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_long_array entry;

		DEBUG_TRACE3("Write long array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_LONG_ARRAY, true);
		entry.numLongs = count;

		m_stream->Write(VALUE_ENTRY_LONG_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const unsigned long values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_unsigned_long_array entry;

		DEBUG_TRACE3("Write unsigned long array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_LONG_ARRAY, true);
		entry.numLongs = count;

		m_stream->Write(VALUE_ENTRY_LONG_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, float value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_float entry;

		DEBUG_TRACE3("Write float %s: %f\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_FLOAT, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const float values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_float_array entry;

		DEBUG_TRACE3("Write float array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_FLOAT_ARRAY, true);
		entry.numFloats = count;

		m_stream->Write(VALUE_ENTRY_FLOAT_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, double value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_double entry;

		DEBUG_TRACE3("Write double %s: %f\n", name, value)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_DOUBLE, true);
		entry.value = value;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
	}
	else
	{
		m_stream->Write(value);
	}
}

void
CvTaggedSaveFormatWrapper::Write(const char* name, int& idHint, int& idSeq, int count, const double values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_double_array entry;

		DEBUG_TRACE3("Write double array %s, count=%d\n", name, count)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_DOUBLE_ARRAY, true);
		entry.numDoubles = count;

		m_stream->Write(VALUE_ENTRY_DOUBLE_ARRAY_SIZE_FROM_NUM(0), (const uint8_t*)&entry);
		m_stream->Write(count, values);
	}
	else
	{
		m_stream->Write(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, const wchar_t* szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string entry;

		DEBUG_TRACE3("Write string %s: %S\n", name, szName)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_WSTRING, true);
		m_stream->Write(sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(szName);
	}
	else
	{
		m_stream->WriteString(szName);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, const char *szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string entry;

		DEBUG_TRACE3("Write string %s: %s\n", name, szName)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_STRING, true);
		m_stream->Write(sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(szName);
	}
	else
	{
		m_stream->WriteString(szName);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, const std::string& szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string entry;

		DEBUG_TRACE3("Write string %s: %s\n", name, szName)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_STD_STRING, true);
		m_stream->Write(sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(szName);
	}
	else
	{
		m_stream->WriteString(szName);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, const std::wstring& szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string entry;

		DEBUG_TRACE3("Write string %s: %s\n", name, szName)

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_STD_WSTRING, true);
		m_stream->Write(sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(szName);
	}
	else
	{
		m_stream->WriteString(szName);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, int count, std::string values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string_array	entry;

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_STRING_ARRAY, true);
		entry.numStrings = count;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(count, values);
	}
	else
	{
		m_stream->WriteString(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::WriteString(const char* name, int& idHint, int& idSeq, int count, std::wstring values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		value_entry_string_array	entry;

		entry.id = getId(name, idHint, idSeq, SAVE_VALUE_TYPE_WSTRING_ARRAY, true);
		entry.numStrings = count;

		m_stream->Write((int)sizeof(entry), (const uint8_t*)&entry);
		m_stream->WriteString(count, values);
	}
	else
	{
		m_stream->WriteString(count, values);
	}
}

int
CvTaggedSaveFormatWrapper::getId(const char* name, int& idHint, int& idSeq, SaveValueType type, bool addToDictionary)
{
	PROFILE_FUNC();

	FAssertMsg(name, "name cannot be null");

	if ( idSeq == usageSeq )
	{
		FAssert((int)m_idDictionary.size() > idHint);

		return idHint;
	}

	int id;
	CvString	normalizedName = NormalizeName(name);

	stdext::hash_map<std::string,DictionaryEntry>::const_iterator itr = m_reverseIdDictionary.find(normalizedName);

	if ( itr == m_reverseIdDictionary.end() )
	{
		PROFILE("getId.New");

		DictionaryEntry newDictionaryEntry;

		id = m_idDictionary.size();

		newDictionaryEntry.m_id = id;
		newDictionaryEntry.m_name = normalizedName;
		newDictionaryEntry.m_type = type;

		m_idDictionary.push_back(newDictionaryEntry);
		m_reverseIdDictionary[normalizedName] = newDictionaryEntry;

		if ( addToDictionary )
		{
			id_mapping_entry_maximal	newEntry;

			FAssert(strlen(name) <= 255);

			newEntry.escapeId = SAVE_ELEMENT_ID_DICTIONARY_ENTRY;
			newEntry.id = id;
			newEntry.type = type;
			newEntry.nameLen = std::min(255,(int)normalizedName.length());
			memcpy(newEntry.name, normalizedName.c_str(), newEntry.nameLen);

			m_stream->Write(ID_MAPPING_ENTRY_LEN(newEntry.nameLen), (const uint8_t*)&newEntry);
		}
	}
	else
	{
		FAssert(itr->second.m_type == type);

		id = itr->second.m_id;
	}

	idHint = id;
	idSeq = usageSeq;

	return id;
}

void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, char **szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read string, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_STRING) )
		{
			*szName = m_stream->ReadString();
		}
	}
	else
	{
		*szName = m_stream->ReadString();
	}
}

void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, wchar_t **szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read string, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_WSTRING) )
		{
			*szName = m_stream->ReadWideString();
		}
	}
	else
	{
		*szName = m_stream->ReadWideString();
	}
}


void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, std::string& szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read string, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_STD_STRING) )
		{
			m_stream->ReadString(szName);
		}
	}
	else
	{
		m_stream->ReadString(szName);
	}
}


void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, std::wstring& szName)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read string, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_STD_WSTRING) )
		{
			m_stream->ReadString(szName);
		}
	}
	else
	{
		m_stream->ReadString(szName);
	}

	size_t index;

	//	Fix a screw up in a build that caused civ names to be corrupted when minior civs were
	//	created.  This is more than cosemtic because the resulting 'name' contains format
	//	specifiers (%s) which leads to crashes in use when the parameters don't match
	if ( (index = szName.find(L"Stateof %s")) != std::string::npos )
	{
		szName = szName.substr(0,index) + std::wstring(L"State of ") + szName.substr(index + 10);
	}
}


void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, int count, std::string values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read string array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_STRING_ARRAY) )
		{
			int numStrings;

			m_stream->Read(&numStrings);

			if ( numStrings != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->ReadString(count, values);
		}
	}
	else
	{
		m_stream->ReadString(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::ReadString(const char* name, int& idHint, int& idSeq, int count, std::wstring values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read wstring array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_WSTRING_ARRAY) )
		{
			int numStrings;

			m_stream->Read(&numStrings);

			if ( numStrings != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->ReadString(count, values);
		}
	}
	else
	{
		m_stream->ReadString(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, char *pChar)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read char, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CHAR) )
		{
			m_stream->Read(pChar);
		}
	}
	else
	{
		m_stream->Read(pChar);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, uint8_t* pByte)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read byte, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_BYTE) )
		{
			m_stream->Read(pByte);
		}
	}
	else
	{
		m_stream->Read(pByte);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, char values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read char array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CHAR_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, uint8_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read byte array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_BYTE_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, bool *pBool)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read bool, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_BOOL) )
		{
			m_stream->Read(pBool);
		}
	}
	else
	{
		m_stream->Read(pBool);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, bool values[], bool bAllowTruncation)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read bool array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_BOOL_ARRAY) )
		{
			int num;

			m_stream->Read(&num);
			m_stream->Read(std::min(num,count), values);

			//	Allow reading LESS than the desired number.  This allows for things like new player options
			if ( num > count )
			{
				//	Also allow less, throwing the extra away - use this option CAREFULLY!
				if ( bAllowTruncation )
				{
					bool*	tempBuffer = new bool[num-count];

					m_stream->Read(num-count, tempBuffer);
				}
				else
				{
					//	Incompatible save
					HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
				}
			}
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, short	*s)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read short, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_SHORT) )
		{
			m_stream->Read(s);
		}
	}
	else
	{
		m_stream->Read(s);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, uint16_t* s)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read unsigned short, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_SHORT) )
		{
			m_stream->Read(s);
		}
	}
	else
	{
		m_stream->Read(s);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, short values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read short array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_SHORT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, uint16_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read unsigned short array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_SHORT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int* i)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read int, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_INT) )
		{
			m_stream->Read(i);
		}
	}
	else
	{
		m_stream->Read(i);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, uint32_t* i)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read unsigned int, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_INT) )
		{
			m_stream->Read(i);
		}
	}
	else
	{
		m_stream->Read(i);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read int array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_INT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				DEBUG_TRACE4("Read int array, name %s, count=%d, num=%d\n", name, count, num)

				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, uint32_t values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read unsigned int array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_INT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, long* l)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read long, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_LONG) )
		{
			m_stream->Read(l);
		}
	}
	else
	{
		m_stream->Read(l);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, unsigned long* l)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read unsigned long, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_LONG) )
		{
			m_stream->Read(l);
		}
	}
	else
	{
		m_stream->Read(l);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, long values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read long array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_LONG_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, unsigned long values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read unsigned long array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_UNSIGNED_LONG_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}



void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, float* value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read float, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_FLOAT) )
		{
			m_stream->Read(value);
		}
	}
	else
	{
		m_stream->Read(value);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, float values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read float array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_FLOAT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}



void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, double* value)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE2("Read double, name %s\n", name)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_DOUBLE) )
		{
			m_stream->Read(value);
		}
	}
	else
	{
		m_stream->Read(value);
	}
}


void
CvTaggedSaveFormatWrapper::Read(const char* name, int& idHint, int& idSeq, int count, double values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read double array, name %s, count=%d\n", name, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_DOUBLE_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(count, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}


//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::ReadClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int* value, bool allowMissing)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read class enum, name %s, classType=%d\n", name, classType)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM) )
		{
			value_class_enum	entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.value);

			if ( entry.value == -1 )
			{
				*value = -1;
			}
			else
			{
				*value = getNewClassEnumValue(classType, entry.value, allowMissing);
			}
		}
	}
	else
	{
		m_stream->Read(value);
	}
}

//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::ReadClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType classType, short* value, bool allowMissing)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE3("Read class enum, name %s, classType=%d\n", name, classType)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM) )
		{
			value_class_enum entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.value);

			if ( entry.value == -1 )
			{
				*value = -1;
			}
			else
			{
				*value = (short)getNewClassEnumValue(classType, entry.value, allowMissing);
			}
		}
	}
	else
	{
		m_stream->Read(value);
	}
}

//	The following are for arrays whose index is a class enum value
//	The 'allowRawArray' is provided for backward compatibility where a
//	array used to be saved as an uninterpretted array but is now handled
//	as a class array (if the save has a raw array ids are ASSUMED to be
//	unchanged and the actual saved element count must be <= class enum array size)
void
CvTaggedSaveFormatWrapper::ReadClassArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, int values[], bool allowMissing, bool allowRawArray)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE4("Read class array, name %s, classType=%d, count=%d\n", name, classType, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_INT_ARRAY) )
		{
			value_entry_class_int_array	entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)&entry.classType);
			m_stream->Read(&entry.numInts);

			bst::scoped_array<int> arrayBuffer(new int[entry.numInts]);

			FAssert (classType == entry.classType);

			m_stream->Read(entry.numInts, &arrayBuffer[0]);

			std::vector<EnumInfo>& mapVector = m_enumMaps[classType];

			for (int i = 0; i < entry.numInts; i++)
			{
				EnumInfo& info = mapVector[i];

				if (info.m_id == -1 && !info.m_lookedUp)
				{
					info.m_id = GC.getInfoTypeForString(info.m_szType, true);

					//	If some objects are missing be tolerant provided their value was 0, -1, MIN_INT (assumed likely defaults
					//	for most int array entries).  Need to do something like this because these arrays generally
					//	represent values about every possible member of an entity type, so even if they are not
					//	actually instantiated they will be present (but are ignorable if we are right about the 0/-1/MIN_INT
					//	defaulting which is the 'risky' part - should perhaps take an extra argument to specify the
					//	not-referenced default)
					int currentValue = arrayBuffer[i];
					if (info.m_id == -1 && currentValue != 0 && currentValue != -1 && currentValue != MIN_INT)
					{
						//	Instantiated object uses class no longer defined - game is not save compatible
						HandleRecoverableIncompatibleSave(CvString::format("Current assets are missing in-use class %s - any instances will have been removed", info.m_szType.c_str()).c_str());
					}

					info.m_lookedUp = true;
				}

				if (info.m_id != -1)
				{
					FAssert(info.m_id < count);

					values[info.m_id] = arrayBuffer[i];
				}
			}
		}
		else if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_INT_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num > count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(num, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::ReadClassArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, bool values[], bool allowMissing, bool allowRawArray)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE4("Read class array, name %s, classType=%d, count=%d\n", name, classType, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_BOOL_ARRAY) )
		{
			value_entry_class_bool_array	entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.numBools);

			FAssert (classType == entry.classType);

			bst::scoped_array<bool> arrayBuffer(new bool[entry.numBools]);

			m_stream->Read(entry.numBools, arrayBuffer.get());

			std::vector<EnumInfo>& mapVector = m_enumMaps[classType];

			for (int i = 0; i < entry.numBools; i++)
			{
				EnumInfo& info = mapVector[i];

				if (info.m_id == -1 && !info.m_lookedUp)
				{
					info.m_id = GC.getInfoTypeForString(info.m_szType, true);

					//	If some obehjcts are mising be tolerant provided their value was false (assumed default
					//	for most bool array entries).  Need to do something like this because these arrays generally
					//	represent values about every possible member of an entity type, so even if they are not
					//	actually instantiated they will be present (but are ignorable if we are right about the 0
					//	defaulting which is the 'risky' part - should perhaps take an extra argument to specify the
					//	not-referenced default)
					if (info.m_id == -1 && arrayBuffer[i] && !allowMissing)
					{
						//	Instantiated object uses class no longer defined - game is not save compatible
						HandleIncompatibleSave(CvString::format("Save format is not compatible due to missing class %s", info.m_szType.c_str()).c_str());
					}

					info.m_lookedUp = true;
				}

				if (info.m_id != -1)
				{
					FAssert(info.m_id < count);

					values[info.m_id] = arrayBuffer[i];
				}
			}
		}
		else if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_BOOL_ARRAY) )
		{
			int num;

			m_stream->Read(&num);

			if ( num > count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}
			m_stream->Read(num, values);
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

//	The following are for arrays whose index is a class enum value and value is another class enum value
void
CvTaggedSaveFormatWrapper::ReadClassArrayOfClassEnum(const char* name, int& idHint, int& idSeq, RemappedClassType indexClassType, RemappedClassType valueClassType, int count, int values[])
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE4("Read class array, name %s, classType=%d, count=%d\n", name, valueClassType, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_BOOL_ARRAY) )
		{
			value_entry_class_class_array	entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)&entry.classType);
			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)&entry.valueClassType);
			m_stream->Read(&entry.numValues);

			FAssert ( indexClassType == entry.classType && valueClassType == entry.valueClassType );

			bst::scoped_array<int> arrayBuffer(new int[entry.numValues]);

			m_stream->Read(entry.numValues, arrayBuffer.get());

			std::vector<EnumInfo>& mapVector = m_enumMaps[indexClassType];

			for(int i = 0; i < entry.numValues; i++)
			{
				EnumInfo& info = mapVector[i];

				if ( info.m_id == -1 && !info.m_lookedUp )
				{
					info.m_id = GC.getInfoTypeForString(info.m_szType, true);
					info.m_lookedUp = true;
				}

				//	If some objects are missing be tolerant as the implication is that the
				//	lookup indircting through this array will never happen
				if ( info.m_id != -1 )
				{
					FAssert( info.m_id < count );

					values[info.m_id] = getNewClassEnumValue(valueClassType, arrayBuffer[i]);
				}
			}
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::ReadClassEnumArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, short values[], bool allowMissing)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE4("Read class enum array, name %s, classType=%d, count=%d\n", name, classType, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM_ARRAY) )
		{
			value_class_enum_array entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.count);

			if ( entry.count != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}

			m_stream->Read(count, values);

			//std::vector<EnumInfo>& mapVector = m_enumMaps[classType];

			for(int i = 0; i < count; i++)
			{
				if ( values[i] != -1 )
				{
					values[i] = (short)getNewClassEnumValue(classType, values[i], allowMissing);
				}
			}
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

//	The following methods must be used for entities that correspond to class enums
void
CvTaggedSaveFormatWrapper::ReadClassEnumArray(const char* name, int& idHint, int& idSeq, RemappedClassType classType, int count, int values[], bool allowMissing)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		DEBUG_TRACE4("Read class enum array, name %s, classType=%d, count=%d\n", name, classType, count)

		if ( Expect(name, idHint, idSeq, SAVE_VALUE_TYPE_CLASS_ENUM_ARRAY) )
		{
			value_class_enum_array	entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.count);

			if ( entry.count != count )
			{
				//	Incompatible save
				HandleIncompatibleSave(CvString::format("Save format is not compatible (%s)", name).c_str());
			}

			m_stream->Read(count, values);

			//std::vector<EnumInfo>& mapVector = m_enumMaps[classType];

			for(int i = 0; i < count; i++)
			{
				if ( values[i] != -1 )
				{
					values[i] = getNewClassEnumValue(classType, values[i], allowMissing);
				}
			}
		}
	}
	else
	{
		m_stream->Read(count, values);
	}
}

void
CvTaggedSaveFormatWrapper::ReadStartObject(const char* name, int& idHint, int& idSeq)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	m_inUse = true;
	if ( m_useTaggedFormat )
	{
		m_nestingDepth++;

		OutputDebugString(CvString::format("ReadStartObject for %s, depth now %d\n", name, m_nestingDepth).c_str());

		Expect(name, idHint, idSeq, (SaveValueType)SAVE_ELEMENT_ID_OBJECT_DELIMITER);
	}
}

void
CvTaggedSaveFormatWrapper::ReadEndObject()
{
	int dummy;

	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		FAssert(m_nestingDepth > 0);

		m_nestingDepth--;

		//OutputDebugString(CvString::format("ReadStartEnd, depth now %d\n", m_nestingDepth).c_str());

		//	Consume until the stream also exits this object
		while( !Expect(NULL, dummy, dummy, (SaveValueType)SAVE_ELEMENT_ID_OBJECT_DELIMITER) )
		{
			SkipElement();
		}
	}
}

//	Read next element from the stream, with info about what the load
//	expects.  Various things can happen:
//		1) The element read matches what is expected - return the data
//		2) The element read is not a match - assume that extra fields have been added
//		   which we cannot fill in (they are left as their presumed defalts)
//		3) An object start delimiter is the next thing in the stream - same as case (2)
//		4) An object end delimiter is the next thing in the stream - error incompatible save
bool
CvTaggedSaveFormatWrapper::Expect(const char* name, int& idHint, int& idSeq, SaveValueType type)
{
	PROFILE_FUNC();

#ifdef TEMP_DEBUGGING_SUPPORT
	char buffer[200];

	sprintf(buffer, "Stream read offset %u, expecting %s\n", m_stream->GetPosition(), name);
	OutputDebugString(buffer);
#endif
	while ( !m_bReadNextElementHeader )
	{
		m_stream->Read(&m_iNextElementNameId);

		//	Dictionary entries and class mapping tables can appear at any point
		//	we process them silently as they occur
		switch(m_iNextElementNameId)
		{
		case SAVE_ELEMENT_ID_DICTIONARY_ENTRY:
			ReadDictionaryElement();
			break;
		case SAVE_ELEMENT_ID_OBJECT_DELIMITER:
			ReadObjectDelimiter();
			m_iNextElementType = SAVE_ELEMENT_ID_OBJECT_DELIMITER;
			m_bReadNextElementHeader = true;
			break;
		case SAVE_ELEMENT_ID_CLASS_MAP:
			ReadClassMap();
			break;
		default:
			m_bReadNextElementHeader = true;

			FAssert(m_idDictionary.size() > (uint32_t)m_iNextElementNameId);

			m_iNextElementType = m_idDictionary[m_iNextElementNameId].m_type;
			break;
		}
	}

	if ( m_iNextElementType == SAVE_ELEMENT_ID_OBJECT_DELIMITER )
	{
		if ( type == SAVE_ELEMENT_ID_OBJECT_DELIMITER )
		{
			if ( m_delimiterIsStart )
			{
				m_streamNestingDepth++;
			}
			else
			{
				FAssert(m_streamNestingDepth > 0);

				m_streamNestingDepth--;
			}

			m_bReadNextElementHeader = false;
			return true;
		}
		else
		{
			//	Non matching start element for a nested object.  This implies the version we are running
			//	now has added fields to the serialisation of the current object (prior to the
			//	nested element).  These are skipped and it is assumed that their default initialization suffices
			//	to provide default semantics
			return false;
		}
	}
	else if ( (m_iNextElementType == type || type == SAVE_VALUE_ANY) && m_nestingDepth == m_streamNestingDepth )
	{
		CvString	normalizedName = NormalizeName(name);

		if ( m_idDictionary[m_iNextElementNameId].m_name.compare(normalizedName) == 0 )
		{
			//	Match
			m_bReadNextElementHeader = false;
			return true;
		}
		else
		{
			//	Non matching leaf element.  This implies the version we are running
			//	now has added fields to the serialisation of the current object.  These
			//	are skipped and it is assumed that their default initialization suffices
			//	to provide default semantics
			return false;
		}
	}
	else
	{
		//	Expected position is out of step with stream.  Assuming this IS a comatible
		//	save this will mean that the (presumed newer) current version is expecting
		//	extra elements.  Thes will continue to be skipped until synchronisation is re-acquired
		return false;
	}
}

void
CvTaggedSaveFormatWrapper::ConsumeBytes(int numBytes)
{
	PROFILE_FUNC();

	uint8_t	buffer[512];
	int		readSize;

	while(numBytes > 0)
	{
		readSize = std::min(numBytes, (int)sizeof(buffer));

		m_stream->Read(readSize, buffer);
		numBytes -= readSize;
	}
}

void
CvTaggedSaveFormatWrapper::SkipElement(const char* name, int& idHint, int& idSeq, SaveValueType saveType)
{
	PROFILE_FUNC();

	FAssert(m_stream != NULL);

	if ( m_useTaggedFormat )
	{
		if ( Expect(name, idHint, idSeq, saveType) )
		{
			SkipElement();
		}
	}
}

//	Consume an unwanted element from the stream
void
CvTaggedSaveFormatWrapper::SkipElement()
{
	PROFILE_FUNC();

	int arraySize;

	switch(m_iNextElementType)
	{
	case SAVE_ELEMENT_ID_OBJECT_DELIMITER:
		//	Already been read but now process as consumed in the stream
		if ( m_delimiterIsStart )
		{
			m_streamNestingDepth++;
		}
		else
		{
			FAssert(m_streamNestingDepth > 0);

			m_streamNestingDepth--;
		}

		break;
	case SAVE_VALUE_TYPE_CHAR:
		ConsumeBytes(sizeof(value_entry_char)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_BYTE:
		ConsumeBytes(sizeof(value_entry_byte)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_CHAR_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(char)*arraySize);
		break;
	case SAVE_VALUE_TYPE_BYTE_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(uint8_t)*arraySize);
		break;
	case SAVE_VALUE_TYPE_BOOL:
		ConsumeBytes(sizeof(value_entry_bool)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_BOOL_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(bool)*arraySize);
		break;
	case SAVE_VALUE_TYPE_SHORT:
		ConsumeBytes(sizeof(value_entry_short)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_SHORT:
		ConsumeBytes(sizeof(value_entry_unsigned_short)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_SHORT_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(short)*arraySize);
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_SHORT_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(uint16_t)*arraySize);
		break;
	case SAVE_VALUE_TYPE_INT:
		ConsumeBytes(sizeof(value_entry_int)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_INT:
		ConsumeBytes(sizeof(value_entry_unsigned_int)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_INT_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(int)*arraySize);
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_INT_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(uint32_t)*arraySize);
		break;
	case SAVE_VALUE_TYPE_LONG:
		ConsumeBytes(sizeof(value_entry_long)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_LONG:
		ConsumeBytes(sizeof(value_entry_unsigned_long)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_LONG_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(long)*arraySize);
		break;
	case SAVE_VALUE_TYPE_UNSIGNED_LONG_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(unsigned long)*arraySize);
		break;
	case SAVE_VALUE_TYPE_FLOAT:
		ConsumeBytes(sizeof(value_entry_float)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_FLOAT_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(float)*arraySize);
		break;
	case SAVE_VALUE_TYPE_DOUBLE:
		ConsumeBytes(sizeof(value_entry_double)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_DOUBLE_ARRAY:
		m_stream->Read(&arraySize);
		ConsumeBytes(sizeof(double)*arraySize);
		break;
	case SAVE_VALUE_TYPE_WSTRING:
		{
			wchar_t* dummy = m_stream->ReadWideString();

			SAFE_DELETE(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_STRING:
		{
			char* dummy = m_stream->ReadString();

			SAFE_DELETE(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_STD_STRING:
		{
			CvString dummy;
			m_stream->ReadString(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_STD_WSTRING:
		{
			CvWString dummy;
			m_stream->ReadString(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_WSTRING_ARRAY:
		m_stream->Read(&arraySize);
		for(int i = 0; i < arraySize; i++)
		{
			CvWString dummy;
			m_stream->ReadString(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_STRING_ARRAY:
		m_stream->Read(&arraySize);
		for(int i = 0; i < arraySize; i++)
		{
			CvString dummy;
			m_stream->ReadString(dummy);
		}
		break;
	case SAVE_VALUE_TYPE_CLASS_ENUM:
		ConsumeBytes(sizeof(value_class_enum)-sizeof(int));
		break;
	case SAVE_VALUE_TYPE_CLASS_INT_ARRAY:
		{
			value_entry_class_int_array entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)&entry.classType);
			m_stream->Read(&entry.numInts);

			ConsumeBytes(sizeof(int)*entry.numInts);
		}
		break;
	case SAVE_VALUE_TYPE_CLASS_BOOL_ARRAY:
		{
			value_entry_class_bool_array entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.numBools);

			ConsumeBytes(sizeof(bool)*entry.numBools);
		}
		break;
	case SAVE_VALUE_TYPE_CLASS_ENUM_ARRAY:
		{
			value_class_enum_array entry;

			m_stream->Read(sizeof(RemappedClassType), (uint8_t*)& entry.classType);
			m_stream->Read(&entry.count);

			ConsumeBytes(sizeof(int)*entry.count);
		}
		break;
	default:
		//	Invalid save stream
		HandleIncompatibleSave("Save format is not compatible (SkipElement)");
	}

	m_bReadNextElementHeader = false;
}

void
CvTaggedSaveFormatWrapper::ReadDictionaryElement()
{
	int dummy = -1;

	PROFILE_FUNC();

	id_mapping_entry_maximal	newEntry;

	m_stream->Read(sizeof(id_mapping_entry_maximal) - sizeof(int) - sizeof(newEntry.name), ((uint8_t*)&newEntry) + sizeof(int));
	m_stream->Read(newEntry.nameLen, (uint8_t*)&newEntry.name);
	newEntry.name[newEntry.nameLen] = '\0';

#ifdef TEMP_DEBUGGING_SUPPORT
	char buffer[300];
	sprintf(buffer, "Read dictionary entry with id %d: %s\n", newEntry.id, newEntry.name);
	OutputDebugString(buffer);
#endif
	//	Re-normalize the name from the dictionary element so as to be compatible (in most cases) across
	//	fixes to name normalization!
	//int id = getId(NormalizeName(newEntry.name),newEntry.type, false);
	int id = getId(newEntry.name,dummy,dummy,newEntry.type, false);

	//	Dictionary is written to the save stream in the order it's needed so should
	//	match the regenerated id order - confirm this
	FAssert(id == newEntry.id);
}

void
CvTaggedSaveFormatWrapper::ReadClassMap()
{
	PROFILE_FUNC();

	class_mapping_table_entry entry;

	m_stream->Read(sizeof(class_mapping_table_entry) - sizeof(int), ((uint8_t*)&entry) + sizeof(int));

	FAssert(entry.classType < NUM_REMAPPED_TYPES);

	for(int i = 0; i < entry.numClasses; i++)
	{
		EnumInfo	mappingInfo;

		mappingInfo.m_id = -1;	//	Not yet known - mapped on demand when we first see an instantiation
		mappingInfo.m_lookedUp = false;
		m_stream->ReadString(mappingInfo.m_szType);

		m_enumMaps[entry.classType].push_back(mappingInfo);
	}
}

void
CvTaggedSaveFormatWrapper::ReadObjectDelimiter()
{
	PROFILE_FUNC();

	object_delimiter_entry_maximal entry;

	m_stream->Read(&entry.bStart);
	m_stream->Read(&entry.nameLen);
	m_stream->Read(entry.nameLen, (uint8_t*)&entry.name);

	FAssert(entry.nameLen <= 255);
	entry.name[entry.nameLen] = '\0';

	m_delimiterIsStart = entry.bStart;
	m_delimiterName = entry.name;
}

//	Names will be fully qualified variable names.  On read they may include address-of (&)
//	operators.  They may also be nexted structures including array accessors ([...]).  All
//	of these should be stripped to arrive at a canonical form which is comparable across
//	the save & reload
char*
CvTaggedSaveFormatWrapper::NormalizeName(const char* name)
{
	//	Single threaded so use a static buffer to provide the canonicalized form.
	//	Doing this to avoid construiction of CvStrings since this routine is called
	//	millions of times per load/save
	static char	normalizationBuffer[1024];
	char* result = (char*)name;

	PROFILE_FUNC();

	//	Strip leading casts (e.g. - m_thingy on save should match (int*)&m_thingy on load)
	char*	ptr;
	char*	ptr2;
	if ( (ptr = (char*)strstr(name,"::(")) != NULL )
	{
		if ( (ptr2 = strchr(ptr+3,')')) != NULL )
		{
			memcpy(normalizationBuffer, name, ptr-name+2);
			strcpy(normalizationBuffer + (ptr-name+2), ptr2+1);

			result = normalizationBuffer;
		}
	}

	//	Strip address-of operators (e.g. - m_thingy on save should match &m_thingy on load)
	if ((ptr = strstr(result, "::&")) != NULL)
	{
		if (result == name)
		{
			memcpy(normalizationBuffer, name, ptr-name+2);
			strcpy(normalizationBuffer + (ptr-name+2), ptr+3);

			result = normalizationBuffer;
		}
		else
		{
			memmove(ptr + 2, ptr + 3, strlen(ptr + 3) + 1);
		}
	}

	//	Strip array accessors.  Do this crudely but cheaply - we just assume the
	//	expression is legal ([ comes before ]) and not nested (no arrays of arrays)
	//	e.g. - we want m_thingy[iI] on save to match m_thingy[iJ] on load (don't require the same
	//	indexer name)
	if ( (ptr = strchr(result, '[')) != NULL )
	{
		if ( result == name )
		{
			strcpy(normalizationBuffer, name);

			RemoveArrayIndices(normalizationBuffer, NULL);

			result = normalizationBuffer;
		}
		else
		{
			RemoveArrayIndices(ptr, ptr);
		}
	}

	return result;
}

void
CvTaggedSaveFormatWrapper::RemoveArrayIndices(char* str, char* brace)
{
	char*	ptr = (brace != NULL ? brace : strchr(str,'['));

	if( ptr != NULL )
	{
		char* ptr2 = strchr(ptr,']');
		FAssert ( ptr2 != NULL );

		strcpy(ptr+1,ptr2);
		RemoveArrayIndices(ptr2,NULL);
	}
}

void
CvTaggedSaveFormatWrapper::HandleIncompatibleSave(const char* reason)
{
	::MessageBox(NULL, reason, "Unreadable Save Game!", MB_OK);

	_THROW(std::invalid_argument, reason);
}

void
CvTaggedSaveFormatWrapper::HandleRecoverableIncompatibleSave(const char* reason)
{
	m_warnings.push_back(CvWString(reason));
}

//	Methods to allow warnings and errors associated with the load/save to be reported
void
CvTaggedSaveFormatWrapper::warning(const char* msg)
{
	HandleRecoverableIncompatibleSave(msg);
}

void
CvTaggedSaveFormatWrapper::error(const char* msg)	//	Will terminate the app with a thrown exceptionm after displaying the message
{
	HandleIncompatibleSave(msg);
}

void
CvTaggedSaveFormatWrapper::close()
{
	if ( m_inUse )
	{
		reset(false);

		m_inUse = false;
		m_useTaggedFormat = false;
	}
}