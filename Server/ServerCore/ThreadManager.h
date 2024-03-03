#pragma once


#include <thread>
#include <functional>

class FThreadManager
{
public:

	FThreadManager();
	~FThreadManager();


	void Launch(function<void(void)> Callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	//GlobalQueue 실행
	static void DoGlobalQueueWork();
	//예약된 Job을 분배
	static void DistributeReservedJobs();
private:

	mutex Lock;
	vector<thread> Threads;

};

