#pragma once
#ifndef WEAKPOINTER_H
#define WEAKPOINTER_H

#include "StrongWeakCount.h"
#include "SmartPointer.h"

template <typename T> 
class WeakPointer
{
public:
	// Default Constructor
	WeakPointer() : t(nullptr), wc() {}

	// Copy Constructor
	WeakPointer(const WeakPointer& wp) : t(wp.t), wc(wp.wc) {}
	template <typename Y> 
	WeakPointer(const WeakPointer<Y>& wp) : t(wp.Lock().GetPointer()), wc(wp.wc) { }
	template <typename Y> 
	WeakPointer(const SmartPointer<Y>& sp) : t(sp.t), wc(sp.sc) { }

	// Assignment Operator
	WeakPointer& operator=(const WeakPointer& wp)
	{
		t = wp.t;
		wc = wp.wc;
		return *this;
	}
	template <typename Y>
	WeakPointer& operator=(const WeakPointer<Y>& wp)
	{
		t = wp.Lock().GetPointer();
		wc = wp.wc;
		return *this;
	}
	template <typename Y>
	WeakPointer& operator=(const SmartPointer<Y>& sp)
	{
		t = sp.t;
		wc = sp.sc;
		return *this;
	}

	// Create a SmartPointer to access the pointer tracked by this WeakPointer
	SmartPointer<T> Lock() const { return SmartPointer<T>(*this); }

	// Number of SmartPointers/StrongReferences
	int UseCount() const { return wc.UseCount(); }

	// Has the pointer tracked by this been deleted?
	bool Expired() const { return wc.UseCount() == 0; }

	// Does this really exist?
	bool Empty() const { return wc.Empty(); }

	// Reset this WeakPointer so its not pointing to anything
	// Essentially sets 't' to null (without deleting it) and deletes wc
	void Reset() { WeakPointer<T>().Swap(*this); }

	// Swap managed pointer and reference counter with another WeakPointer
	void Swap(WeakPointer<T>& other) 
	{ 
		std::swap(t, other.t); 
		wc.Swap(other.wc); 
	}

private:
	T* t;				// Pointer we are managing
	WeakCount wc;		// Reference counter to track 't'
	template <typename Y> friend class WeakPointer;
	template <typename Y> friend class SmartPointer;
};
#endif