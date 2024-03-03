#pragma once

//shared_ptr을 사용하려면 RefCountable처럼 레퍼런스 카운트 가능한 클래스여야 가능하다는 뜻이다.

//class RefCountable
//{
//
//public:
//
//	RefCountable() : RefCount(1)
//	{
//	}
//
//	virtual ~RefCountable()
//	{
//	}
//
//	int32 GetRefCount() 
//	{
//		return RefCount; 
//	}
//
//	int32 AddRef() 
//	{
//		return ++RefCount;
//	}
//
//	int32 ReleaseRef()
//	{
//		int32 nRefCount = --RefCount;
//		if (nRefCount == 0)
//		{
//			delete this;
//		}
//	
//		return nRefCount;
//	}
//
//
//protected:
//
//	Atomic<int32> RefCount;
//
//};
//
//template<typename T>
//class TSharedPtr
//{
//public:
//	TSharedPtr()
//	{
//	}
//
//	TSharedPtr(T* ptr) 
//	{
//	
//	}
//
//	//복사
//	TSharedPtr(const TSharedPtr& rhs)
//	{
//		Set(rhs.Ptr);
//	}
//
//	//이동
//	TSharedPtr(TSharedPtr&& rhs)
//	{
//		Ptr = rhs.Ptr;
//		rhs.Ptr = nullptr;
//	}
//
//	template<typename U>
//	TSharedPtr(const TSharedPtr<U>& rhs)
//	{
//		Set(static_cast<T*>(rhs.Ptr));
//	}
//
//	~TSharedPtr()
//	{
//		Release();
//	}
//
//public:
//
//	//복사 연산
//	TSharedPtr& operator=(const TSharedPtr& rhs)
//	{
//		if (Ptr != rhs.Ptr)
//		{
//			Release();
//			Set(rhs.Ptr);
//		}
//
//		return *this;
//	}
//
//	//이동
//	TSharedPtr& operator=(TSharedPtr&& rhs)
//	{
//		Release();
//
//		Ptr = rhs.Ptr;
//		rhs.Ptr = nullptr;
//
//		return *this;
//	}
//
//	bool operator==(const TSharedPtr& rhs) const
//	{
//		return Ptr == rhs.Ptr;
//	}
//	bool operator==(T* ptr) const
//	{
//		return Ptr == Ptr;
//	}
//
//	bool operator!=(const TSharedPtr& rhs) const
//	{
//		return Ptr != rhs.Ptr;
//	}
//	bool operator!=(T* ptr) const
//	{
//		return Ptr != Ptr;
//	}
//
//	bool operator<(const TSharedPtr* rhs) const
//	{
//		return Ptr < rhs->Ptr;
//	}
//
//	T* operator*() 
//	{
//		return Ptr;
//	}
//	const T* operator*() const
//	{
//		return Ptr;
//	}
//	operator T* () const
//	{
//		return Ptr;
//	}
//	T* operator->()
//	{
//		return Ptr;
//	}
//	const T* operator->() const
//	{
//		return Ptr;
//	}
//
//	bool IsNull()
//	{ 
//		return Ptr == nullptr;
//	}
//private:
//
//	inline void Set(T* ptr)
//	{
//		Ptr = ptr;
//		if (ptr)
//		{
//			ptr->AddRef();
//		}
//	}
//
//	inline void Release()
//	{
//		if (Ptr != nullptr)
//		{
//			Ptr->ReleaseRef();
//			Ptr = nullptr;
//		}
//	}
//
//private:
//
//	T* Ptr = nullptr;
//
//};