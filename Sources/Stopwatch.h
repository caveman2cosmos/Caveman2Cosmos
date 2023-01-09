////////////////////////////////////////////////////////////////////////////////
//
// Stopwatch.h  -- A simple stopwatch implementation, based on Windows
//                 high-performance timers.
//                 Can come in handy when measuring elapsed times of
//                 portions of C++ code.
//
// Copyright (C) 2016 by Giovanni Dicanio <giovanni.dicanio@gmail.com>
//
// The MIT License (MIT)
//
// Copyright (c) 2016 Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////


#pragma once

#ifndef Stopwatch_h__
#define Stopwatch_h__

//#include <crtdbg.h>     // For _ASSERTE
#include <Windows.h>    // For high-performance timers


namespace win32
{


	//------------------------------------------------------------------------------
	// Class to measure time intervals, for benchmarking portions of code.
	// It's a convenient wrapper around the Win32 high-resolution timer APIs
	// QueryPerformanceCounter() and QueryPerformanceFrequency().
	//------------------------------------------------------------------------------
	class Stopwatch
	{
	public:
		// Initialize the stopwatch to a safe initial state
		Stopwatch();

		// Clear the stopwatch state
		void Reset();

		// Start measuring time.
		// When finished, call Stop().
		// Can call ElapsedTime() also before calling Stop(): in this case,
		// the elapsed time is measured since the Start() call.
		void Start();

		// Stop measuring time.
		// Call ElapsedMilliseconds() to get the elapsed time from the Start() call.
		void Stop();

		// Return elapsed time interval duration, in milliseconds.
		// Can be called both after Stop() and before it.
		// (Start() must have been called to initiate time interval measurements).
		double ElapsedMilliseconds() const;


		//
		// Ban copy
		//
	private:
		Stopwatch(const Stopwatch&);
		Stopwatch& operator=(const Stopwatch&);


		//
		// *** IMPLEMENTATION ***
		//
	private:
		bool m_running;                 // is the timer running?
		long long m_start;              // start tick count
		long long m_finish;             // end tick count
		const long long m_frequency;    // cached frequency value

		//
		// According to MSDN documentation:
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644905(v=vs.85).aspx
		//
		// The frequency of the performance counter is fixed at system boot and
		// is consistent across all processors.
		// Therefore, the frequency need only be queried upon application
		// initialization, and the result can be cached.
		//

		// Wrapper to Win32 API QueryPerformanceCounter()
		static long long Counter();

		// Wrapper to Win32 API QueryPerformanceFrequency()
		static long long Frequency();

		// Calculate elapsed time in milliseconds,
		// given a start tick and end tick counts.
		double ElapsedMilliseconds(long long start, long long finish) const;
	};



	//
	// Inline implementations
	//


	inline Stopwatch::Stopwatch()
		: m_running(false)
		, m_start(0)
		, m_finish(0)
		, m_frequency(Frequency())
	{}


	inline void Stopwatch::Reset()
	{
		m_finish = m_start = 0;
		m_running = false;
	}


	inline void Stopwatch::Start()
	{
		m_running = true;
		m_finish = 0;

		m_start = Counter();
	}


	inline void Stopwatch::Stop()
	{
		m_finish = Counter();
		m_running = false;
	}


	inline double Stopwatch::ElapsedMilliseconds() const
	{
		if (m_running)
		{
			const long long current = Counter();
			return ElapsedMilliseconds(m_start, current);
		}

		return ElapsedMilliseconds(m_start, m_finish);
	}


	inline long long Stopwatch::Counter()
	{
		LARGE_INTEGER li;
		::QueryPerformanceCounter(&li);
		return li.QuadPart;
	}


	inline long long Stopwatch::Frequency()
	{
		LARGE_INTEGER li;
		::QueryPerformanceFrequency(&li);
		return li.QuadPart;
	}


	inline double Stopwatch::ElapsedMilliseconds(long long start, long long finish) const
	{
		//_ASSERTE(start >= 0);
		//_ASSERTE(finish >= 0);
		//_ASSERTE(start <= finish);

		return ((finish - start) * 1000.0) / m_frequency;
	}


} // namespace win32

#endif // Stopwatch_h__
