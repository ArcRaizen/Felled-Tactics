#pragma once
#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#include <cstddef>
#include <memory>

class ReferenceCounterBase
{
private:
	ReferenceCounterBase(ReferenceCounterBase&);
	ReferenceCounterBase& operator=(const ReferenceCounterBase&);

public:
	// Default Constructor
	ReferenceCounterBase() : strongCount(1), weakCount(1) { }
	virtual ~ReferenceCounterBase() {}

	// Dispose is called when strongCount drops to 0
	// to release the resources managed by *this
	virtual void Dispose() = 0;

	// Destroy is called when weakCount drops to 0
	void Destroy() { delete this; }
	
	// Increment StrongCount
	void AddStrongReference() 
	{ 
		++strongCount; 
		if(strongCount > 4)
		{
			int x = 5;
			x++;
		}
	}

	// Increment StrongCount only if another StrongReference exists already
	// Used when creating a SmartPointer from a WeakPointer
	// Return whether or not another StrongReference exists already
	bool AddStrongLock() 
	{ 
		if(strongCount == 0) 
			return false; 

		++strongCount;
		return true;
	}

	// Decrement StrongCount
	// Delete pointer if no StrongReferences remain
	void Release()
	{
		if(--strongCount == 0)
		{
			Dispose();
			WeakRelease();		// why exactly?
			// If no WeakReferences exist, this makes weakCount 0 so *this is deleted
			// If 1 WeakReference exists, this makes weakCount 1, so *this isn't deleted
				// until that WeakReference is
		}
	}

	// Increment WeakCount
	void AddWeakReference() { ++weakCount; }

	// Decrement WeakCount
	// Delete *this when no WeakPointers exist anymore
	// (There cannot be a StrongPointer and this happens since the 1st StrongPointer counts as a WeakPointer)
	void WeakRelease() { if(--weakCount == 0) Destroy(); }
	
	// Number of StrongReferences
	int UseCount() const { return strongCount; }

private:
	unsigned int strongCount;	// #strong
	unsigned int weakCount;		// #weak + (#strong != 0)
};

// This class exists to simplify having to make all other
// class templated. Instead, use ReferenceCounterBase* = new ReferenceCounter<T>
// to only worry about templates at creation and no other classes need to be templated
template <typename T>
class ReferenceCounter : public ReferenceCounterBase
{
private:
	ReferenceCounter(const ReferenceCounter&);
	ReferenceCounter& operator=(const ReferenceCounter&);

public:
	explicit ReferenceCounter(T* t_) : t(t_) {}
	
	// Delete pointer managed by a SmartPointer
	// This crazy way of doing it was copied from Boost's shared_ptr
	void Dispose()
	{
		typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
		(void)sizeof(type_must_be_complete);
		delete t;
	}

	// Also copied from Boost's shared_ptr
	// I can only assume they did this for a reason...
	void* operator new(std::size_t) { return std::allocator<ReferenceCounter<T>>().allocate(1, static_cast<ReferenceCounter<T>*>(0)); }
	void operator delete(void* p) { std::allocator<ReferenceCounter<T>>().deallocate(static_cast<ReferenceCounter<T>*>(p), 1); }

private:
	T* t;
};
#endif