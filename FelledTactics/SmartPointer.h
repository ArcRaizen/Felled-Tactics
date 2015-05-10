#pragma once
#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#include "StrongWeakCount.h"
#include <algorithm>
template <typename T> class WeakPointer;


//#define LOG_SMARTPOINTER_OUTPUT
#ifdef LOG_SMARTPOITER_OUTPUT
	#ifndef CONSOLEOUTPUT_H
	#include "ConsoleOuput.h"
	#endif
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
	SmartPointer() : t(nullptr), sc() {	}

	// Constructor when creating a non-nullptr.
	// Essentially casting a T* into a SmartPtr<T>
	// Initialize the pointer and increase count if 't' is being copied
	SmartPointer(T* u) : t(u), sc(u)		
	{ 
#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t != nullptr)
			CONSOLEOUTPUT("Constructor: " << typeid(t).name() << " " << sc.UseCount() << "\n");
#endif
	}

	// Constructor when casting a non-nullptr up an inheritance line
	// Essentially creating a SmartPointer<T> from a SmartPointer<Y>/WeakPointer<Y> where Y inherits from T
	template <typename Y> SmartPointer(const WeakPointer<Y>& y) : sc(y.wc) { t = y.t; }
	template <typename Y> SmartPointer(const SmartPointer<Y>& y) : sc(y.sc) { t = y.t; }
/*	template <typename Y> SmartPointer(const SmartPointer<Y>& y)
	{
		(void)static_cast<T*>(static_cast<Y*>(0));
		t = static_cast<T*>(y.GetPointer());
		assert(t != nullptr);
		sc = y.sc;
	}*/

	// Copy Constructor
	// Initialize the pointer and increase count
	SmartPointer(const SmartPointer& u) : t(u.t), sc(u.sc)	
	{ 
#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t != nullptr)
			CONSOLEOUTPUT("Constructor: " << typeid(t).name() << " " << sc.UseCount() << "\n");
#endif
	}

	// Destructor
	// Decrement count and delete pointer when no copies remain
	~SmartPointer()
	{ 
		// Since StrongCount is a non-pointer object, it is destroyed automatically
		// and it will delete 't' if no SmartPointers/StrongReferences remain
#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t != nullptr)
			CONSOLEOUTPUT("Constructor: " << typeid(t).name() << " " << sc.UseCount() << "\n");
#endif
	}

	// Assignment Operator
	// Set pointer equal to another pointer, delete original data if it was the last copy of itself
	SmartPointer<T>& operator=(const SmartPointer<T>& u)
	{
		// Create temporary local SmartPointer from 'u:' SmartPointer<T>(u)
		// Swap the contents of this SmartPointer with *this
		// Temporary local SmartPointer is deleted when its scope is left (original *this is deleted)
		// *this now holds the contents of 'u'
		SmartPointer<T>(u).Swap(*this);
#ifdef LOG_SMARTPOINTER_OUTPUT
		if(t != nullptr)
			CONSOLEOUTPUT("Constructor: " << typeid(t).name() << " " << sc.UseCount() << "\n");
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

	// Number of SmartPointers/StrongReferences
	int UseCount() const { return (rc == nullptr ? 0 : rc->UseCount()); }

	// Is this the only SmartPointer managing 't'?
	bool Unique() const { return sc.Unique(); }

	// Reset this SmartPointer so it's not managing anything
	// 't' and 'sc' will be deleted appropriately 
	void Reset() { SmartPointer<T>().Swap(*this); }

	// Swap managed pointer and ReferenceCounter with another SmartPointer
	// Can be used to delete current 't' and 'sc' by swapping *this with a local SmartPointer
		// e.g. SmartPointer<T>().Swap(*this)
	void Swap(SmartPointer<T>& other) 
	{ 
		std::swap(t, other.t); 
		sc.Swap(other.sc); 
	}

private:
	T* t;				// Pointer we are managing
	StrongCount sc;		// Reference counter to track 't'
	template <typename Y> friend class SmartPointer;
	template <typename Y> friend class WeakPointer;
};
#endif