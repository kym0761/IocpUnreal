#pragma once

class FGlobalQueue
{
public:
	FGlobalQueue();
	~FGlobalQueue();

	void					Push(JobQueueRef jobQueue);
	JobQueueRef				Pop();

private:
	FLockQueue<JobQueueRef> JobQueues;
};
