#pragma once
#include <mutex>

template<typename T>
class LockQueue
{
public:

	LockQueue()
	{
	}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator= (const LockQueue&) = delete;

	void Push(T val)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(val));
		_condVar.notify_one();
	}

	bool TryPop(T& val)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty()) // LockStack에서 empty를 구현하는 거는 전혀 의미가 없음.
		{
			return false;
		}


		val = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& val)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this]() {return _queue.empty() == false; });

		val = std::move(_queue.top());
		_queue.pop();

	}


private:


	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};