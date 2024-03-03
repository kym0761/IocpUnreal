#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"



class FJobQueue : public enable_shared_from_this<FJobQueue>
{
public:


	/*JobQueue 처리*/
	void DoAsync(CallbackType&& callback)
	{
		//Jobqueue에 일감을 넣은 뒤, 일처리를 할지 말지 결정함.
		Push(make_shared<FJob>(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		//Jobqueue에 일감을 넣은 뒤, 일처리를 할지 말지 결정함.
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(make_shared<FJob>(owner, memFunc, std::forward<Args>(args)...));
	}
	//~~~


	/*JobTimer 세팅*/
	void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{
		//tickAfter 시간이 됐을 때 동작하도록 예약
		JobRef job = make_shared<FJob>(std::move(callback));
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		//tickAfter 시간이 됐을 때 동작하도록 예약
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<FJob>(owner, memFunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	void ClearJobs() { Jobs.Clear(); }


public:

	void Push(JobRef job, bool bPushOnly = false);
	void Execute();

protected:
	FLockQueue<JobRef> Jobs;
	Atomic<int32> JobCount = 0;
};

