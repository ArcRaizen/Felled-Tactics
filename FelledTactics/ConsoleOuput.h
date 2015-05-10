#pragma once
#ifndef CONSOLEOUTPUT_H
#define CONSOLEOUTPUT_H

#ifdef _DEBUG
	#define CONSOLEOUTPUT(s)					\
	{											\
		std::wostringstream os_;				\
		os_ << s;								\
		OutputDebugStringW(os_.str().c_str());	\
	}
#endif
#endif