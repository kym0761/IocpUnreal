#include "pch.h"
#include "GlobalQueue.h"

FGlobalQueue::FGlobalQueue()
{
}

FGlobalQueue::~FGlobalQueue()
{
}

void FGlobalQueue::Push(JobQueueRef jobQueue)
{
	JobQueues.Push(jobQueue);
}

JobQueueRef FGlobalQueue::Pop()
{
	return JobQueues.Pop();
}
