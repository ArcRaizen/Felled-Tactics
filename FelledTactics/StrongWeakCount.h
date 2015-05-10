#pragma once
#include "ReferenceCounter.h"

class WeakCount;

class StrongCount
{
public:
	// Default Constructor
	StrongCount() : rc(nullptr) { }

	// Constructor when creating a non-nullptr
	// Will now track the number of references to 't'
	template <typename T> StrongCount(T* t) : rc(nullptr)
	{
		rc = new ReferenceCounter<T>(t);
	}

	// Copy Constructor
	// A SmartPointer was created to track a pointer already tracked by another SmartPointer
	StrongCount(const StrongCount& sc) : rc(sc.rc)
	{ 
		// A new SmartPointer means a new StrongReference
		if(rc != nullptr) 
			rc->AddStrongReference(); 
	}

	// Forward declare for now (can't define since WeakCount is still undefined)
	// Defined below
	StrongCount(const WeakCount& wc);

	// Destructor
	// A SmartPointer was destroyed. One less StrongReference now
	~StrongCount() 
	{ 
		if(rc != nullptr) 
			rc->Release(); 
	}

	// Copy Constructor
	// Create a new StrongCount from an already existing one
	// Initialize ReferenceCounter and increase StrongReference count
	StrongCount& operator=(const StrongCount& sc)
	{
		ReferenceCounterBase* temp = sc.rc;		// temporary copy

		// Test self-assignment
		if(temp != rc)
		{
			if(temp != nullptr)				// Copying an already existing StrongCount
				temp->AddStrongReference();		// means creating a new StrongReference
			if(rc != nullptr)				// If 'rc' already exists,
				rc->Release();					// we're losing its current StrongReference
			rc = temp;						// Copy
		}

		return *this;
	}

	//  Swap ReferenceCounters of two StrongCounts
	void Swap(StrongCount& sc)
	{
		ReferenceCounterBase* temp = sc.rc;
		sc.rc = rc;
		rc = temp;
	}

	// Number of SmartPointers/StrongReferences
	int UseCount() const { return rc != nullptr ? rc->UseCount() : 0; }
	
	// Is there only one SmartPointers/StrongReferences?
	bool Unique() const { return UseCount() == 1; }

	// Does this really exist?
	bool Empty() const { return rc == nullptr; }

	friend inline bool operator==(const StrongCount& a, const StrongCount& b)
	{
		return a.rc == b.rc;
	}

private:
	ReferenceCounterBase* rc;	// ReferencCounter used to...well...count references
	friend class WeakCount;		// WeakCount needs to be able to access 'rc'
};

class WeakCount
{
public:
	// Default Constructor
	WeakCount() : rc(nullptr) {}

	// Copy Constructor
	// A WeakPointer was created to track a pointer already tracked by a SmartPointer
	// Copy the ReferenceCounter over and add a new WeakReference
	WeakCount(const StrongCount& sc) : rc(sc.rc)
	{
		// A new WeakPointer means a new WeakReference
		if(rc != nullptr)
			rc->AddWeakReference();
	}

	// Copy Constructor
	// A WeakPointer was created to track a pointer already tracked by a WeakPointer
	WeakCount(const WeakCount& wc) : rc(wc.rc)
	{
		if(rc != nullptr)
			rc->AddWeakReference();
	}

	// Destructor
	// A WeakPointer was destroyed. One less WeakReference now
	~WeakCount() 
	{ 
		if(rc != nullptr) 
			rc->WeakRelease(); 
	}

	// Assignment Operator
	// Assign a WeakCount to an already existing ReferenceCounter
	// Initialize ReferenceCounter and increase WeakReference count
	WeakCount& operator=(const StrongCount& sc)
	{
		ReferenceCounterBase* temp = sc.rc;

		if(temp != rc)
		{
			if(temp != nullptr)				// Passed-in ReferenceConter exists, so copying it
				temp->AddWeakReference();		// means we're adding a new WeakReference
			if(rc != nullptr)				// RC is being overwritten, so we are losing its
				rc->WeakRelease();				// weak reference if it already had one
			rc = temp;						// Overwrite/Save rc to pass-in ReferenceCounter
		}

		return *this;
	}
	WeakCount& operator=(const WeakCount& wc)
	{
		ReferenceCounterBase* temp = wc.rc;

		if(temp != rc)
		{
			if(temp != nullptr)				// Passed-in ReferenceConter exists, so copying it
				temp->AddWeakReference();		// means we're adding a new WeakReference
			if(rc != nullptr)				// RC is being overwritten, so we are losing its
				rc->WeakRelease();				// weak reference if it already had one
			rc = temp;						// Overwrite/Save rc to pass-in ReferenceCounter
		}

		return *this;
	}

	//  Swap ReferenceCounter of two WeakCounts
	void Swap(WeakCount& wc)
	{
		ReferenceCounterBase* temp = wc.rc;
		wc.rc = rc;
		rc = temp;
	}

	// Number of SmartPointers/StrongReferences
	int UseCount() const { return rc != nullptr ? rc->UseCount() : 0; }
	
	// Does this really exist?
	bool Empty() const { return rc == nullptr; }

	friend inline bool operator==(const WeakCount& a, const WeakCount& b)
	{
		return a.rc == b.rc;
	}

private:
	ReferenceCounterBase* rc;		// ReferencCounter used to...well...count references
	friend class StrongCount;		// StrongCount needs to be able to access 'rc'
};

// Copy Constructor
// Create a new StrongCount from an already existing WeakCount
// Initialize ReferenceCounter and increase StrongReference count
inline StrongCount::StrongCount(const WeakCount& wc) : rc(wc.rc)
{
	if(rc == nullptr || !rc->AddStrongLock())
		rc = nullptr;		// Set to nullptr if no SmartPointers exist since there's nothing to manage
}