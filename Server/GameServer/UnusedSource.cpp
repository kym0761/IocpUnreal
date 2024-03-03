
#include "pch.h"

#pragma region PreInfo

//template<typename T>
//class AutoLock
//{
//public:
//	AutoLock(T& l) : lock(l)
//	{
//		lock.lock();
//		cout << "lock ok" << endl;
//	}
//
//	virtual ~AutoLock()
//	{
//		lock.unlock();
//		cout << "unlock ok" << endl;
//	}
//
//private:
//
//	T& lock;
//
//};
//
//
//class SpinLock
//{
//private:
//
//	atomic<bool> locked = false;
//
//public:
//
//
//	void lock()
//	{
//		bool expected = false; // 예상되는 lock 값.
//		bool desired = true; //expected와 같을 때, 원하는 lock의 값
//
//		////CAS (compare and swap)
//		//if (locked == expected) //lock 값이 expected와 같으면 desired 값으로 변경시켜줌
//		//{
//		//	expected = locked;
//		//	locked = desired;
//		//	return true;
//		//}
//		//else 
//		//{
//		//	expected = locked; //이 부분 때문에 while 루프 안에서 expected 값을 다시 false시켜줘야함.
//		//	return false;
//		//}
//
//		//!! : expected parameter가 &(ref)이므로 파라미터를 false로 직접 설정이 안됨.
//		// _weak는 인터럽트에 의한 쓰레드 실패 등을 감안하고 만든 기능이며, strong은 실패 후에 알아서 자동으로 다시 시도하는 기능을 가짐.
//		while (locked.compare_exchange_strong(expected, desired) == false)
//		{
//			expected = false;
//		}
//
//		//이건 안됨. Lock 접근이 Atomic하지 않음.
//		//while (locked)
//		//{
//		//}
//		//locked = true;
//	}
//
//	void unlock()
//	{
//		//locked = false;
//		locked.store(false);
//	}
//
//};
//
//void ThreadTest()
//{
//	cout << "Thread OK" << "\n";
//
//}
//
//
//class SpinLock_Sleep
//{
//private:
//
//	atomic<bool> locked = false;
//
//public:
//
//
//	void lock()
//	{
//		bool expected = false; 
//		bool desired = true; 
//
//		while (locked.compare_exchange_strong(expected, desired) == false)
//		{
//			expected = false;
//
//			//this_thread::sleep_for(std::chrono::milliseconds(100)); 			//==this_thread::sleep_for(0ms);
//			this_thread::sleep_for(0ms); //==this_thread::yield();
//		}
//
//	}
//
//	void unlock()
//	{
//		locked.store(false);
//	}
//
//};
//
//
//mutex mm;
//queue<int32> q;
//HANDLE handle;
//
//void Producer()
//{
//	while (true)
//	{
//		{
//			unique_lock<mutex> lock(mm);
//			q.push(100);
//		}
//
//		SetEvent(handle); //handle을 signal 상태로
//		this_thread::sleep_for(1ms);
//	}
//}
//
//void Consumer()
//{
//	while (true)
//	{
//		//signal 상태가 될때까지 sleep 상태로 대기함.
//		WaitForSingleObject(handle, INFINITE);
//		//ResetEvent(handle);
//
//		unique_lock<mutex> lock(mm);
//		if (q.empty() == false)
//		{
//			int32 data = q.front();
//			q.pop();
//			cout << data << endl;
//		}
//	}
//}
//
//
//mutex mmm;
//queue<int32> qqq;
////cv는 유저 레벨 오브젝트다.(커널 오브젝트가 아님.)
//condition_variable cv;
//
//
//void Producer_conVal()
//{
//	while (true)
//	{
//		//1 lock
//		//2 공유변수 값을 수정
//		//3 unlock
//		//4 조건 변수로 다른 쓰레드에게 통지함.
//
//
//
//		{
//			unique_lock<mutex> lock(mm);
//			q.push(100);
//		}
//
//		cv.notify_one(); //wait 상태인 쓰레드 중에 단 1개만 깨운다.
//
//		this_thread::sleep_for(1ms);
//	}
//}
//
//void Consumer_conVal()
//{
//	while (true)
//	{
//		//1. lock
//		//2. 조건 변수 확인
//		//3. 만족 or 불만족
//		// 만족 -> 코드 실행
//		// 불만족 -> lock을 풀고 대시 상태 됨
//		
//		unique_lock<mutex> lock(mmm); //lock_guard 못씀 필요하다면 중간에 unlock해야하므로..
//		cv.wait(lock, []() {return qqq.empty() == false; });//cv가 불만족되면 lock이 풀려야하므로 lock을 parameter로 받는다.
//
//		//notify_one을 call했으면 조건이 만족할거라 생각할 수 있다.
//		//spurious wakeup(가짜기상)
//		//notify_one을 하면 lock을 잡고있을 상태가 아니기 때문에 cv.wait 안에 추가 조건을 걸어야한다.
//
//
//		if (q.empty() == false)
//		{
//			int32 data = q.front();
//			q.pop();
//			cout << data << endl;
//		}
//	}
//}
//
//int64 futureTest()
//{
//
//	return 10;
//}
//
//void promiseTest(std::promise<string>&& pr)
//{
//	pr.set_value("promise test ok");
//}
//
//void packageTaskTest(std::packaged_task<int64(void)>&& pt)
//{
//	pt();
//}
//
//int main()
//{
//	Test();
//
//	std::thread t(ThreadTest);
//	cout << "Main ok" << endl;
//	
//
//	int32 count = t.hardware_concurrency(); // cpu 코어 개수
//	//t.get_id(); // 쓰레드 id
//	//t.detach(); // t와 실제 쓰레드를 분리함. 보통은 사용 안함.
//	//t.joinable(); //실제로 구동 중인 쓰레드인지? join해도 되는지?
//	t.join();
//
//	/* //쓰레드 함수 사용 예시
//	vector<std::thread> threadv;
//
//	
//	for (int i = 0; i < 10; i++)
//	{
//		threadv.push_back(std::thread(ThreadTest));
//	}
//
//	for (int i = 0; i < 10; i++)
//	{
//		if (threadv[i].joinable())
//		{
//			threadv[i].join();
//		}
//	}
//	*/
//
//	/* //lock_guard 예시
//	mutex m;
//
//	m.lock();
//	//..
//
//	cout << "?" << endl;
//
//	m.unlock();
//
//	{
//		AutoLock<std::mutex> autoMutex(m);
//		//..
//		cout << "something" << endl;
//	}
//	
//	{
//	
//		std::lock_guard<mutex> autoMutex(m);
//
//		cout << "something 2" << endl;
//	}
//
//	{
//		std::unique_lock<mutex> uniqueMutex(m, std::defer_lock);
//
//		uniqueMutex.lock(); // lock을 명시적으로 하여 원하는 시점에 lock을 걸 수 있다.
//	}
//	*/
//
//	/*
//	//Event 예시
//
//	//이 함수는 커널 오브젝트를 return함. 사용자가 어떻게 할 수는 없고 단순히 1,2,3 등의 HANDLE 변수 이름의 int값만 나옴.
//	//usage count
//	//signal or non-signal <<bool
//	//auto / manual << bool
//	handle = CreateEvent(NULL, FALSE, FALSE, NULL); // 보안속성, bManualReset, bInitialState, ...
//	//만약 bManualReset이 TRUE라면 ResetEvent(handle)로 이벤트를 수동으로 리셋시켜주어야 했다.
//
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//
//	CloseHandle(handle);
//	*/
//
//	/*
//	//future
//	//mutex, condition_value를 사용하지 않고 단순히 처리할 수 있을 때, 한번에 발생하는 이벤트에 유용함.
//
//	//future 예시
//	//1 deferred - lazy evaluation
//	//2 async - 별도 쓰레드
//	//3 deferred | async - 알아서
//	std::future<int64> future01 = std::async(std::launch::async, futureTest);
//
//	future01.wait_for(10ms);
//	if (future01._Is_ready())
//	{
//		//
//	}
//	int64 result = future01.get();
//
//	std::promise<string> prom;
//	std::future<string> future02 = prom.get_future();
//
//	//thread tProm(promiseTest, std::move(prom));
//	thread tProm([](std::promise<string>&& p) {p.set_value("promise Mesg"); }, std::move(prom));
//
//	string s = future02.get();
//	cout << s << endl;
//	tProm.join();
//
//	std::packaged_task<int64(void)> task(futureTest);
//	std::future<int64> future03 = task.get_future();
//
//
//	std::thread tt([](std::packaged_task<int64(void)>&& pt) {pt(); }, std::move(task));
//	//std::thread tt(packageTaskTest, std::move(task));
//
//	int64 num = future03.get();
//	cout << num << endl;
//
//	tt.join();
//	*/
//
//
//	return 0;
//}

#pragma endregion

