#pragma once

//	AIAndy: An implementation of FDataStreamBase for manual caching, based on fstream

#ifndef		FDATAIOSTREAM_H
#define		FDATAIOSTREAM_H

class FDataIOStream : public FDataStreamBase
{
public:
	void OpenRead(CvString szFile);
	void OpenWrite(CvString szFile);
	void Close();
	bool IsOpen();
	virtual void		Rewind();
	virtual bool		AtEnd();
	virtual void		FastFwd();
	virtual uint32_t	GetPosition() const;
	virtual void		SetPosition(uint32_t position);
	virtual void		Truncate();
	virtual void		Flush();
	virtual uint32_t	GetEOF() const;
	virtual uint32_t	GetSizeLeft() const;
	virtual void		CopyToMem(void* mem);

	virtual uint32_t	WriteString(const wchar_t* szName);
	virtual uint32_t	WriteString(const char* szName);
	virtual uint32_t	WriteString(const std::string& szName);
	virtual uint32_t	WriteString(const std::wstring& szName);
	virtual uint32_t	WriteString(int count, std::string values[]);
	virtual uint32_t	WriteString(int count, std::wstring values[]);

	virtual uint32_t	ReadString(char* szName);
	virtual uint32_t	ReadString(wchar_t* szName);
	virtual uint32_t	ReadString(std::string& szName);
	virtual uint32_t	ReadString(std::wstring& szName);
	virtual uint32_t	ReadString(int count, std::string values[]);
	virtual uint32_t	ReadString(int count, std::wstring values[]);

	virtual char*		ReadString();		// allocates memory
	virtual wchar_t*	ReadWideString();	// allocates memory

	virtual void		Read(char *);
	virtual void		Read(uint8_t*);
	virtual void		Read(int count, char values[]);
	virtual void		Read(int count, uint8_t values[]);
	virtual void		Read(bool *);
	virtual void		Read(int count, bool values[]);
	virtual void		Read(short* s);
	virtual void		Read(uint16_t* s);
	virtual void		Read(int count, short values[]);
	virtual void		Read(int count, uint16_t values[]);
	virtual void		Read(int* i);
	virtual void		Read(uint32_t* i);
	virtual void 		Read(int count, int values[]);
	virtual void 		Read(int count, uint32_t values[]);

	virtual void		Read(long* l);
	virtual void		Read(unsigned long* l);
	virtual void 		Read(int count, long values[]);
	virtual void 		Read(int count, unsigned long values[]);

	virtual void		Read(float* value);
	virtual void		Read(int count, float values[]);

	virtual void		Read(double* value);
	virtual void		Read(int count, double values[]);

	virtual bool		TryRead(int* i);
	virtual bool		TryRead(unsigned long* l);

	virtual void		Write(char value);
	virtual void		Write(uint8_t value);
	virtual void		Write(int count, const char values[]);
	virtual void		Write(int count, const uint8_t values[]);

	virtual void		Write(bool value);
	virtual void		Write(int count, const bool values[]);

	virtual void		Write(short value);
	virtual void		Write(uint16_t value);
	virtual void		Write(int count, const short values[]);
	virtual void		Write(int count, const uint16_t values[]);

	virtual void		Write(int value);
	virtual void		Write(uint32_t value);
	virtual void 		Write(int count, const int values[]);
	virtual void		Write(int count, const uint32_t values[]);

	virtual void		Write(long value);
	virtual void		Write(unsigned long  value);
	virtual void 		Write(int count, const long values[]);
	virtual void		Write(int count, const unsigned long values[]);

	virtual void		Write(float value);
	virtual void		Write(int count, const float values[]);

	virtual void		Write(double value);
	virtual void		Write(int count, const double values[]);
protected:
	std::fstream stream;
};

#endif	//FDATASTREAMBASE_H
