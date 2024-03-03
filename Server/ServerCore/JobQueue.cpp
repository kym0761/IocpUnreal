#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"


void FJobQueue::Push(JobRef job, bool bPushOnly)
{
	const int32 prevCount = JobCount.fetch_add(1);
	Jobs.Push(job); // WRITE_LOCK

	//JobQueue에 들어가 있는 Job 개수를 확인함.
	//첫번째 Job을 넣은 쓰레드가 실행까지 담당
	//다른 쓰레드가 Job을 넣고 
	//JobQueue 안의 개수가 1이상이면 이미 처리하고 있는 중이니 스킵함.
	if (prevCount == 0)
	{
		// 현재 쓰레드가 JobQueue를 사용중이 아니라면 Execute()gkdu Jobqueue 실행
		if (LCurrentJobQueue == nullptr && bPushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

//문제점?
// 1) 일감이 너무 몰리면 ?
// 2) DoAsync 타고 타고 가서 절대 끝나지 않는 상황이라면? (일감이 한 쓰레드한테 몰림)
void FJobQueue::Execute()
{
	LCurrentJobQueue = this; // 이 job큐를 실행하고 있다는 것을 알려줌.


	while (true)
	{
		vector<JobRef> jobs;
		Jobs.PopAll(OUT jobs); // WRITE_LOCK

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Execute();
		}
			
		// 남은 일감이 0개라면 종료
		//jobcount를 이미 측정한 상태에서, 값을 빼기 전에 jobCount와 같다면 결과는 0임
		//Jobcount가 0이면 처리할 일거리는 일단 없다는 의미니 종료함.
		//다음에 누군가가 이 일을 담당하게 될 것.
		if (JobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr; // 끝나게 된다면 nullptr로 만듬
			return;
		}

		//쓰레드의 제한시간이 지났다면, 빠져나옴.
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr; // 끝나게 된다면 nullptr로 만듬
			GGlobalQueue->Push(shared_from_this());	// 다른 쓰레드가 실행하도록 GlobalQueue에 넘김
			break;
		}

	}
}
