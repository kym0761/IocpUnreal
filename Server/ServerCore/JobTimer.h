#pragma once


struct JobData //FJobTimer Reserve()에서 만들어짐
{
	JobData(weak_ptr<FJobQueue> owner, JobRef job) : Owner(owner), Job(job)
	{

	}

	weak_ptr<FJobQueue>	Owner;
	JobRef Job;
};

struct TimerItem // FJobTimer의 Reserve에서 Items.push()할 때 이 구조체 데이터를 통해 Job을 넣음.
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick; //priority queue가 less<>로 되어있음.
	}

	uint64 executeTick = 0;
	JobData* jobData = nullptr;
};


class FJobTimer
{
public:
	void Reserve(uint64 tickAfter, weak_ptr<FJobQueue> owner, JobRef job);
	void Distribute(uint64 now);
	void Clear();

private:
	USE_LOCK;
	priority_queue<TimerItem> Items;
	atomic<bool> bDistributing = false;
};
