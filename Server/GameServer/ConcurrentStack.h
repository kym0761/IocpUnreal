#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:

	LockStack()
	{
	}

	LockStack(const LockStack&) = delete;
	LockStack& operator= (const LockStack&) = delete;

	void Push(T val)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(val));
		_condVar.notify_one();
	}

	bool TryPop(T& val)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) // LockStack에서 empty를 구현하는 거는 전혀 의미가 없음
		{
			return false;
		}


		val = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& val)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this]() {return _stack.empty() == false; });

		val = std::move(_stack.top());
		_stack.pop();

	}


private:


	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template <typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& val) : data(val), next(nullptr)
		{
		}
		
		T data;
		Node* next;
	};

public:

	void Push(const T& val)
	{
	
		Node* node = new Node(val);
		node->next = _head;

		/*
		if (_head == node->next)
		{
			_head = node;
			return true;
		}
		else
		{
			node->next = _head;
			return false;
		}
		*/

		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//node->next = _head;
		}

	}

	bool TryPop(T& outVal)
	{
		Node* oldHead = _head;


		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr)
		{
			return false;
		}


		outVal = oldHead->data;


		//삭제 보류, ref count를 이용한 메모리 할당 해제 적용 필요..
		//delete oldHead;


		return true;
	}

private:

	atomic<Node*> _head;
};