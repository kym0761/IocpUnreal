#pragma once

class FGlobalQueue
{
public:
	FGlobalQueue();
	~FGlobalQueue();

	//Push하면, ThreadManager에서 JobQueue를 다른 여유 있는 쓰레드에게 JobQueue를 넘겨
	//Job 처리를 할 수 있도록 하게 만듬.
	void					Push(JobQueueRef jobQueue);

	//ThreadManager의 DoGlobalQueueWork() 참고
	JobQueueRef				Pop();

private:
	FLockQueue<JobQueueRef> JobQueues;
};
