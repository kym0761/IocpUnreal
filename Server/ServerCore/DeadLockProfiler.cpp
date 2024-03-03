#include "pch.h"
#include "DeadLockProfiler.h"

void FDeadLockProfiler::PushLock(const char* Name)
{
	LockGuard guard(Lock);

	//아이디를 찾거나, 발급한다.
	int32 lockId = 0;

	auto it = NameToId.find(Name);
	if (it == NameToId.end()) // Name에 해당되는 ID가 존재하지 않음.
	{
		lockId = static_cast<int32>(NameToId.size());
		NameToId[Name] = lockId;
		IdToName[lockId] = Name;
	}
	else
	{
		lockId = it->second;
	}


	//잡고있는 lock이 있었다면?
	if (LLockStack.empty() == false)
	{
		//기존에 발견되지 않은 케이스면, 데드락 여부를 다시 확인해야함.
		const int32 prevId = LLockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = LockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

void FDeadLockProfiler::PopLock(const char* Name)
{
	LockGuard guard(Lock);

	if (LLockStack.empty())
	{
		CRASH("MULTIPLE UNLOCK");
	}
	
	int32 lockId = NameToId[Name];
	if (LLockStack.top() != lockId)
	{
		CRASH("INVALID UNLOCK")
	}

	LLockStack.pop();

}

void FDeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(NameToId.size());
	DiscoveredOrder = vector<int32>(lockCount, -1);
	DiscoveredCount = 0;
	Finished = vector<bool>(lockCount, false);
	Parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		DFS(lockId);
	}

	DiscoveredOrder.clear();
	Finished.clear();
	Parent.clear();
}

void FDeadLockProfiler::DFS(int32 here)
{
	if (DiscoveredOrder[here] != -1)
	{
		return;
	}

	DiscoveredOrder[here] = DiscoveredCount++;


	auto it = LockHistory.find(here);
	if (it == LockHistory.end())
	{
		Finished[here] = true;
		return;
	}

	set<int32>& nextSet = it->second;
	for (int32 there : nextSet)
	{
		//처음 방문해봄
		if (DiscoveredOrder[there] == -1)
		{
			Parent[there] = here;
			DFS(there);
			continue;
		}

		//순서에 맞는 방향
		// here가 there보다 먼저 발견되었다면, there는 here의 후손이다.
		if (DiscoveredOrder[here] < DiscoveredOrder[there])
		{
			continue;
		}

		//순서에 맞지 않은 역방향
		//DFS가 완벽하게 종료되지 않았는데 접근을 하므로 데드락임.
		if (Finished[there] == false)
		{
			cout << IdToName[here] << " -> " << IdToName[there] << "\n";

			int32 now = here;
			while (true)
			{
				cout << IdToName[Parent[now]] << " -> " << IdToName[now] << "\n";
				now = Parent[now];
				
				if (now == there)
				{
					break;
				}
			}

			CRASH("DEADLOCK DETECTED");
		}



	}

	Finished[here] = true;

}
