#pragma once
#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#ifdef _DEBUG
	#define CONSOLEOUTPUT(s)					\
	{											\
		std::wostringstream os_;				\
		os_ << s;								\
		OutputDebugStringW(os_.str().c_str());	\
	}

	//#define LOG_SMARTPOINTER_OUTPUT
#endif

#pragma region Operator->* Overload
// Code Source: http://www.aristeia.com/Papers/DDJ_Oct_1999.pdf
template <typename ObjectType, typename ReturnType, typename MemFuncPtrType>
class PMFC
{
public:
	typedef std::pair<ObjectType*, MemFuncPtrType> CallInfo;

	// Constructor
	PMFC(const CallInfo& info) : callInfo(info) {}

	// 0 parameter function
	ReturnType operator()() const { return (callInfo.first->*callInfo.second)(); }

	// 1 parameter function
	template <typename Param1Type>
	ReturnType operator()(Param1Type p1) const { return (callInfo.first->*callInfo.second)(p1); }

private:
	CallInfo callInfo;
};
#pragma endregion

// A custom-written Smart Pointer class.
// Any class "T" that make this class a friend of it can use SmartPointer<T>'s in place of T*.
// These SmartPointer<T>'s will track how many copies of each T* exist and will delete the memory when all copies are gone automatically.
// Classes utilizing this class should use private/protected constructors and static "create" functions for each constructor that return a SmartPointer<T> object.

// Proper syntax to max this class a friend is: "template <typename T> friend class SmartPointer;"
// Suggest you use typedef to rename "SmartPointer<T>" to something simpler.

template <typename T> class SmartPointer
{
public:
	// Default Constructor to allow the pointer to be NULL
	SmartPointer()								
	{ 
		t = nullptr; 
	}

	// Constructor when creating a non-nullptr.
	// Essentially casting a T* into a SmartPtr<T>
	// Initialize the pointer and increase count if 't' is being copied
	SmartPointer(T* u) : t(u)					
	{ 
		if(u != nullptr) 
			++t->pointerCount; 

#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t == nullptr){}
		else
		CONSOLEOUTPUT("Constructor: " << typeid(t).name() << " " << t->pointerCount << "\n");
#endif
	}

	// Constructor when casting a non-nullptr up an inheritance line
	// Essentially creating a SmartPtr<T> from a SmartPtr<Y> where Y inherits from T
	template<typename Y>
	SmartPointer(const SmartPointer<Y> &y)
	{
		(void)static_cast<T*>(static_cast<Y*>(0));
		t = static_cast<T*>(y.GetPointer());
		assert(t != nullptr);
		++t->pointerCount;
	}

	// Destructor
	// Decrement count and delete pointer when no copies remain
	~SmartPointer()
	{ 
		if(t != nullptr && --t->pointerCount == 0)
		{
#ifdef LOG_SMARTPOINTER_OUTPUT
			if(t == nullptr){}
			else
			CONSOLEOUTPUT("Destructor: " << typeid(t).name() << " " << "DESTROYED!!!!" << "\n");
#endif

			delete t; 
		}
#ifdef LOG_SMARTPOINTER_OUTPUT
		else
		{
			if(t == nullptr){}
			else
			CONSOLEOUTPUT("Destructor: " << typeid(t).name() << " " << t->pointerCount << "\n");
		}
#endif
	}

	// Copy Constructor
	// Initialize the pointer and increase count
	SmartPointer(const SmartPointer<T>& u) : t(u.t)	
	{ 
		if(t != nullptr)			// assert this because if 't' is null
			++t->pointerCount;		// then 'u.t' is also null and we can't copy that

#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t == nullptr){}
		else
		CONSOLEOUTPUT("Copy Constructor: " << typeid(t).name() << " " << t->pointerCount << "\n");
#endif
	}

	// Assignment Operator
	// Set pointer equal to another pointer, delete original data if it was the last copy of itself
	SmartPointer<T>& operator=(const SmartPointer<T>& u)
	{
		// Increase count if 'u.t' already exists (isn't null)
		if(u.t != nullptr)
			++u.t->pointerCount;

		// If pointer isn't null and is the last/only copy, it is now being overwritten
		// Delete pointer before we overwrite it
		if(t != nullptr && --t->pointerCount == 0)
			delete t;

		// Set/Overwrite pointer equal to one passed in
		t = u.t;

#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t == nullptr){}
		else
		CONSOLEOUTPUT("Assignment Operator: " << typeid(t).name() << " " << t->pointerCount << "\n");
#endif

		return *this;
	}

	// Get the actual pointer managed by this SmartPointer
	// Necessary to pass the pointer to Lua scripts
	T* GetPointer() { return t; }
	T* GetPointer() const { return t; }

	// ~~~~ Operators ~~~~
	bool operator==(const T* u)	{ return t == u; }
	bool operator!=(const T* u) { return t != u; }

	// Member Access
	T* operator->()	
	{ 
		assert(t != nullptr); 
		return t; 
	}

	T* operator->() const
	{
		assert(t != nullptr);
		return t;
	}

	// Dereference
	T& operator*()
	{ 
		assert(t != nullptr); 
		return *t; 
	}

#pragma region Operator->* Overload
	// Code Source: http://www.aristeia.com/Papers/DDJ_Oct_1999.pdf
	// 0 Parameter Non-Const Function
	template <typename ReturnType>
	const PMFC<T, ReturnType, ReturnType(T::*)()> operator->*(ReturnType(T::*pmf)()) const
	{
		return std::make_pair(t, pmf);
	}

	// 1 Parameter Non-Cosnt Function
	template <typename ReturnType, typename Param1Type>
	const PMFC<T, ReturnType, ReturnType(T::*)(Param1Type)> operator->*(ReturnType(T::*pmf)(Param1Type)) const
	{
		return std::make_pair(t, pmf);
	}

	// 0 Parameter Const Function
	template <typename ReturnType>
	const PMFC<T, ReturnType, ReturnType(T::*)() const> operator->*(ReturnType(T::*pmf)() const) const
	{
		return std::make_pair(t, pmf);
	}

	// 1 Parameter Non-Const Function
	template <typename ReturnType, typename Param1Type>
	const PMFC<T, ReturnType, ReturnType(T::*)(Param1Type) const> operator->*(ReturnType(T::*pmf)(Param1Type) const) const
	{
		return std::make_pair(t, pmf);
	}

#pragma endregion

private:
	T* t;		// Pointer we are controlling
};
#endif