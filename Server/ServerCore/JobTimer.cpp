#include "pch.h"
#include "JobTimer.h"

void FJobTimer::Reserve(uint64 tickAfter, weak_ptr<FJobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = new JobData(owner, job);

	WRITE_LOCK;

	Items.push(TimerItem{ executeTick, jobData });
}

void FJobTimer::Distribute(uint64 now)
{
	// 한 번에 1 쓰레드만 통과
	if (bDistributing.exchange(true) == true)
		return;

	vector<TimerItem> timerItems;

	{
		WRITE_LOCK;

		while (Items.empty() == false)
		{
			const TimerItem& timerItem = Items.top(); //예약 시간이 가장 먼저인 것을 얻음
			if (now < timerItem.executeTick) // 예약시간이 아직 되지 않았다면 이 루프를 벗어남
				break;

			timerItems.push_back(timerItem);
			Items.pop();
		}
	}

	//처리해야할 일감을 JobQueue에 넣음
	for (TimerItem& item : timerItems)
	{
		if (JobQueueRef owner = item.jobData->Owner.lock())
		{
			owner->Push(item.jobData->Job);
		}
		
		delete item.jobData;
	}

	// 끝났으면 풀어준다
	bDistributing.store(false);
}

void FJobTimer::Clear()
{
	WRITE_LOCK;

	while (Items.empty() == false)
	{
		const TimerItem& timerItem = Items.top();
		delete timerItem.jobData;
		Items.pop();
	}
}
