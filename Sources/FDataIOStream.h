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
	virtual void	Rewind();
	virtual bool	AtEnd();
	virtual void	FastFwd();
	virtual unsigned int  GetPosition() const;
	virtual void    SetPosition(unsigned int position);
	virtual void    Truncate();
	virtual void	Flush();
	virtual unsigned int	GetEOF() const;
	virtual unsigned int			GetSizeLeft() const;
	virtual void	CopyToMem(void* mem);
	
	virtual unsigned int	WriteString(const wchar *szName);
	virtual unsigned int	WriteString(const char *szName);
	virtual unsigned int	WriteString(const std::string& szName);
	virtual unsigned int	WriteString(const std::wstring& szName);
	virtual unsigned int	WriteString(int count, std::string values[]);
	virtual unsigned int	WriteString(int count, std::wstring values[]);
	
	virtual unsigned int	ReadString(char *szName);
	virtual unsigned int	ReadString(wchar *szName);
	virtual unsigned int	ReadString(std::string& szName);
	virtual unsigned int	ReadString(std::wstring& szName);
	virtual unsigned int	ReadString(int count, std::string values[]);
	virtual unsigned int	ReadString(int count, std::wstring values[]);
	
	virtual char *			ReadString();		// allocates memory 
	virtual wchar *		ReadWideString();	// allocates memory 

	virtual void		Read(char *);
	virtual void		Read(byte *);
	virtual void		Read(int count, char values[]);
	virtual void		Read(int count, byte values[]);
	virtual void		Read(bool *);
	virtual void		Read(int count, bool values[]);
	virtual void		Read(short	*s);
	virtual void		Read(unsigned short	*s);
	virtual void		Read(int count, short values[]);
	virtual void		Read(int count, unsigned short values[]);
	virtual void		Read(int* i);
	virtual void		Read(unsigned int* i);
	virtual void 		Read(int count, int values[]);
	virtual void 		Read(int count, unsigned int values[]);

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

	virtual void		Write( char value);
	virtual void		Write(byte value);
	virtual void		Write(int count, const  char values[]);
	virtual void		Write(int count, const  byte values[]);

	virtual void		Write(bool value);
	virtual void		Write(int count, const bool values[]);

	virtual void		Write(short value);
	virtual void		Write(unsigned short value);
	virtual void		Write(int count, const short values[]);
	virtual void		Write(int count, const unsigned short values[]);

	virtual void		Write(int value);
	virtual void		Write(unsigned int value);
	virtual void 		Write(int count, const int values[]);
	virtual void		Write(int count, const unsigned int values[]);

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
