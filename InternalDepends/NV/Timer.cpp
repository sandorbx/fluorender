/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2018 Scientific Computing and Imaging Institute,
University of Utah.


Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
// ----------------------------------------------------------------------------
// 
// Content:
//      Timer class
//
// Description:
//      A high precision timer with built in filtering (averaging) 
//      capabilities.
//
// Author: Frank Jargstorff (03/08/04)
//
// Note:
//      Copyright (C) 2004 NVIDIA Crop. 
//
// ----------------------------------------------------------------------------


//
// Includes
//

#include "Timer.h"
#include "../compatibility.h"

#include <assert.h>


//
// Namespaces
//

using namespace nv;


//
// Global consts
//

#ifdef _WIN32
const LARGE_INTEGER gcLargeIntZero = { {0, 0} };
#endif
// ----------------------------------------------------------------------------
// Timer class
//

// 
// Construction and destruction
//

// Default constructor
//
Timer::Timer(unsigned int nBoxFilterSize) :
#ifdef _WIN32
	_nStartCount(gcLargeIntZero),
	_nStopCount(gcLargeIntZero),
	_nFrequency(gcLargeIntZero),
#else
	_nStartCount(0),
	_nStopCount(0),
	_nFrequency(0),
#endif
	_nLastPeriod(0.0),
	_nSum(0.0),
	_nTotal(0.0),
	_nCount(0),
	_nBoxFilterSize(nBoxFilterSize),
	_iFilterPosition(0),
	_aIntervals(0),
	_bClockRuns(false)
{
#ifdef _WIN32
	QueryPerformanceFrequency(&_nFrequency);
#else
	_nFrequency = 1000;
#endif
	// create array to store timing results
	_aIntervals = new double[_nBoxFilterSize];

	// initialize inverals with 0
	for (unsigned int iInterval = 0; iInterval < _nBoxFilterSize; ++iInterval)
		_aIntervals[iInterval] = 0.0;
}

// Destructor
//
Timer::~Timer()
{
	delete[] _aIntervals;
}

//
// Public methods
//

// start
//
void
Timer::start()
{
	if (_bClockRuns) return;

#ifdef _WIN32
	QueryPerformanceCounter(&_nStartCount);
#else
	timeval t;
	gettimeofday(&t, NULL);
	_nStartCount = t.tv_sec * 1000;
	_nStartCount += t.tv_usec / 1000;
#endif
	_bClockRuns = true;
}

// stop
//
void
Timer::stop()
{
	if (!_bClockRuns) return;

#ifdef _WIN32
	QueryPerformanceCounter(&_nStopCount);
	_nLastPeriod = static_cast<double>(_nStopCount.QuadPart - _nStartCount.QuadPart)
		/ static_cast<double>(_nFrequency.QuadPart);
#else
	timeval t;
	gettimeofday(&t, NULL);
	_nStopCount = t.tv_sec * 1000;
	_nStopCount += t.tv_usec / 1000;
	_nLastPeriod = static_cast<double>(_nStopCount - _nStartCount)
		/ static_cast<double>(_nFrequency);
#endif
	_nSum -= _aIntervals[_iFilterPosition];
	_nSum += _nLastPeriod;
	_aIntervals[_iFilterPosition] = _nLastPeriod;
	_iFilterPosition++;
	_iFilterPosition %= _nBoxFilterSize;
}

// sample
//
void
Timer::sample()
{
	if (!_bClockRuns) return;

#ifdef _WIN32
	LARGE_INTEGER nCurrentCount;
	QueryPerformanceCounter(&nCurrentCount);
	_nLastPeriod = static_cast<double>(nCurrentCount.QuadPart - _nStartCount.QuadPart)
		/ static_cast<double>(_nFrequency.QuadPart);
	_nStartCount = nCurrentCount;
#else
	unsigned long long nCurrentCount;
	timeval t;
	gettimeofday(&t, NULL);
	nCurrentCount = t.tv_sec * 1000;
	nCurrentCount += t.tv_usec / 1000;
	_nLastPeriod = static_cast<double>(nCurrentCount - _nStartCount)
		/ static_cast<double>(_nFrequency);
	_nStartCount = nCurrentCount;
#endif
	_nSum -= _aIntervals[_iFilterPosition];
	_nSum += _nLastPeriod;
	_aIntervals[_iFilterPosition] = _nLastPeriod;
	_iFilterPosition++;
	_iFilterPosition %= _nBoxFilterSize;

	//total
	_nCount++;
	if (_nCount >= _nBoxFilterSize)
		_nTotal += _nLastPeriod;
}

// time
//
// Description:
//      Time interval in ms
//
double
Timer::time()
const
{
	return _nLastPeriod;
}

// average
//
// Description:
//      Average time interval of the last events in ms.
//          Box filter size determines how many events get
//      tracked. If not at least filter-size events were timed
//      the result is undetermined.
//
double
Timer::average()
const
{
	return _nSum / _nBoxFilterSize;
}

unsigned long long
Timer::count() const
{
	if (_nCount >= _nBoxFilterSize)
		return _nCount - _nBoxFilterSize;
	else
		return 0;
}

double
Timer::total_time() const
{
	return _nTotal;
}

double
Timer::total_fps() const
{
	if (_nCount >= _nBoxFilterSize)
		return static_cast<double>(_nCount) / _nTotal;
	else
		return 0;
}