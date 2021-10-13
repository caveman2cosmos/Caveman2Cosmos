#pragma once

//	$Revision: #4 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

/*
*
* FILE:    FDataStreamBase.h
* DATE:    7/15/2004
* AUTHOR: Mustafa Thamer
* PURPOSE: Base Classe of Stream classes for file, null and mem streams.
*
*		Easily read and write data to those kinds of streams using the same baseclass interface.
*		FStrings not used since this is a public dll header.
*
*/

#ifndef		FDATASTREAMBASE_H
#define		FDATASTREAMBASE_H

//
// Stream abstract base class
//
class FDataStreamBase
{
public:
	virtual void		Rewind() = 0;
	virtual bool		AtEnd() = 0;
	virtual void		FastFwd() = 0;
	virtual uint32_t	GetPosition() const =  0;
	virtual void		SetPosition(uint32_t position) = 0;
	virtual void		Truncate() = 0;
	virtual void		Flush() = 0;
	virtual uint32_t	GetEOF() const = 0;
	virtual uint32_t	GetSizeLeft() const = 0;
	virtual void		CopyToMem(void* mem) = 0;

	virtual uint32_t	WriteString(const wchar_t* szName) = 0;
	virtual uint32_t	WriteString(const char* szName) = 0;
	virtual uint32_t	WriteString(const std::string& szName) = 0;
	virtual uint32_t	WriteString(const std::wstring& szName) = 0;
	virtual uint32_t	WriteString(int count, std::string values[]) = 0;
	virtual uint32_t	WriteString(int count, std::wstring values[]) = 0;

	virtual uint32_t	ReadString(char* szName) = 0;
	virtual uint32_t	ReadString(wchar_t* szName) = 0;
	virtual uint32_t	ReadString(std::string& szName) = 0;
	virtual uint32_t	ReadString(std::wstring& szName) = 0;
	virtual uint32_t	ReadString(int count, std::string values[]) = 0;
	virtual uint32_t	ReadString(int count, std::wstring values[]) = 0;

	virtual char*		ReadString() = 0;		// allocates memory
	virtual wchar_t*	ReadWideString() = 0;	// allocates memory

	virtual void		Read(char *) = 0;
	virtual void		Read(uint8_t*) = 0;
	virtual void		Read(int count, char values[]) = 0;
	virtual void		Read(int count, uint8_t values[]) = 0;
	virtual void		Read(bool *) = 0;
	virtual void		Read(int count, bool values[]) = 0;
	virtual void		Read(short	*s) = 0;
	virtual void		Read(uint16_t* s)  = 0;
	virtual void		Read(int count, short values[]) = 0;
	virtual void		Read(int count, uint16_t values[]) = 0;
	virtual void		Read(int* i) = 0;
	virtual void		Read(uint32_t* i) = 0;
	virtual void 		Read(int count, int values[]) = 0;
	virtual void 		Read(int count, uint32_t values[]) = 0;

	virtual void		Read(long* l) = 0;
	virtual void		Read(unsigned long* l)  = 0;
	virtual void 		Read(int count, long values[]) = 0;
	virtual void 		Read(int count, unsigned long values[])  = 0;

	virtual void		Read(float* value) = 0;
	virtual void		Read(int count, float values[]) = 0;

	virtual void		Read(double* value) = 0;
	virtual void		Read(int count, double values[]) = 0;

	virtual void		Write(char value) = 0;
	virtual void		Write(uint8_t value) = 0;
	virtual void		Write(int count, const char values[]) = 0;
	virtual void		Write(int count, const uint8_t values[]) = 0;

	virtual void		Write(bool value) = 0;
	virtual void		Write(int count, const bool values[]) = 0;

	virtual void		Write(short value) = 0;
	virtual void		Write(uint16_t value) = 0;
	virtual void		Write(int count, const short values[]) = 0;
	virtual void		Write(int count, const uint16_t values[])  = 0;

	virtual void		Write(int value) = 0;
	virtual void		Write(uint32_t value)  = 0;
	virtual void 		Write(int count, const int values[]) = 0;
	virtual void		Write(int count, const uint32_t values[])  = 0;

	virtual void		Write(long value) = 0;
	virtual void		Write(unsigned long  value)  = 0;
	virtual void 		Write(int count, const long values[]) = 0;
	virtual void		Write(int count, const unsigned long values[])  = 0;

	virtual void		Write(float value) = 0;
	virtual void		Write(int count, const float values[]) = 0;

	virtual void		Write(double value) = 0;
	virtual void		Write(int count, const double values[]) = 0;
};

#endif	//FDATASTREAMBASE_H
