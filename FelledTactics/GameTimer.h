#pragma once
#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <Windows.h>

static class GameTimer
{
public:
	GameTimer(void);
	~GameTimer(void);
	
	static void		Reset();
	static void		Init();
	static void		Start();
	static void		Stop();
	static void		Tick();
	static float	GetGameTime();
	static float	GetDeltaTime();


private:
	static double mSecondsPerCount;
	static double mDeltaTime;
	static _int64 mPrevTime;
	static _int64 mCurrTime;
	static _int64 mBaseTime;
	static _int64 mPausedTime;
	static _int64 mStopTime;
	static bool	  mStopped;
};
#endif

