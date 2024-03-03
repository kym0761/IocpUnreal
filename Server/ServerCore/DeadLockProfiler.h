#pragma once
class FDeadLockProfiler
{
public:

	void PushLock(const char* Name);
	void PopLock(const char* Name);
	void CheckCycle();

private:

	void DFS(int32 here);


private:

	unordered_map<const char*, int32> NameToId;
	unordered_map<int32, const char*> IdToName;

	//쓰레드마다 독립적인 lock스택을 사용하고 있음. CoreTLS.h 참고
	//stack<int32> LLockStack;
	
	map<int32, set<int32>> LockHistory;

	Mutex Lock;

private:
	vector<int32> DiscoveredOrder; //노드가 발견된 순서 기록
	int32 DiscoveredCount = 0; //노드가 발견된 순서
	vector<bool> Finished; //DFS[i]가 종료되었는지에 대한 여부
	vector<int32> Parent;

};

