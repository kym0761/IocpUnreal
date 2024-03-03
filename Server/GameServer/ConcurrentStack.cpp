#include "pch.h"
#include "ConcurrentStack.h"


//LockQueue<int32> qu;
//LockFreeStack<int32> st;
//
//void Push()
//{
//	while (true)
//	{
//		int32 v = rand() % 100;
//		st.Push(v);
//
//		this_thread::sleep_for(10ms);
//	}
//
//}
//
//void Pop()
//{
//	while (true)
//	{
//		int32 d = 0;
//		if (st.TryPop(OUT d))
//		{
//			cout << d << endl;
//		}
//	}
//}

//int main()
//{
//	//thread t1(Push);
//	//thread t2(Pop);
//	//thread t3(Pop);
//	//thread t4(Pop);
//
//
//	//t1.join();
//	//t2.join();
//	//t3.join();
//	//t4.join();
//
//
//	//CRASH("test");
//
//	int a = 3;
//	ASSERT_CRASH(a!=3);
//
//	//ASSERT_CRASH(a==3);
//
//	return 0;
//}
