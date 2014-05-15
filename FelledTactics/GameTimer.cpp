#include "stdafx.h"
#include "GameTimer.h"

double GameTimer::mSecondsPerCount = 0.0;
double GameTimer::mDeltaTime = -1.0f;
_int64 GameTimer::mPrevTime = 0;
_int64 GameTimer::mCurrTime = 0;
_int64 GameTimer::mBaseTime = 0;
_int64 GameTimer::mPausedTime = 0;
_int64 GameTimer::mStopTime = 0;
bool   GameTimer::mStopped = false;

GameTimer::GameTimer(void){}
GameTimer::~GameTimer(void){}

void GameTimer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);
	mBaseTime = mCurrTime;
	mPrevTime = mCurrTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Init()
{
	_int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);	// Get the frequency of the performance timer
	mSecondsPerCount = 1.0 / (double)countsPerSec;				// Convert the fractions of a second per count
}

// Updates the Timer
void GameTimer::Tick()
{
	if(mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	// Get the time this frame
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);

	// Compute difference between this and the previous frame
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	// Prepare for the next frame
	mPrevTime = mCurrTime;

	// If we move to another processor, mDeltaTime can be negative
	if(mDeltaTime < 0.0)
		mDeltaTime = 0.0;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if(!mStopped)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);
		mStopTime = mCurrTime;
		mStopped = true;
	}
}

float GameTimer::GetGameTime()
{
	// If Paused, return time between start of game and pause
	if(mStopped)
		return (float)((mStopTime - mBaseTime) * mSecondsPerCount);

	// Otherwise, return full time played without the time paused
	return (((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
}

// Returns how long the previous frame took, in seconds
float GameTimer::GetDeltaTime()
{
	return (float)mDeltaTime;
}
