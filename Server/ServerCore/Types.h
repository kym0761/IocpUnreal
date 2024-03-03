#pragma once

#include <mutex>
#include <atomic>

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

template<typename T>
using Atomic = std::atomic<T>;

using Mutex = std::mutex;
using CondVar = std::condition_variable;
using UniqueLock = std::unique_lock<std::mutex>;
using LockGuard = std::lock_guard<std::mutex>;

#define USING_SHARED_PTR(name)	using name##Ref = std::shared_ptr<class name>;
//ex : USING_SHARED_PTR(FPlayer)

// shared_ptr
using IocpCoreRef = std::shared_ptr<class FIocpCore>;
using IocpObjectRef = std::shared_ptr<class FIocpObject>;
using SessionRef = std::shared_ptr<class FSession>;
using ListenerRef = std::shared_ptr<class FListener>;
using ServerServiceRef = std::shared_ptr<class FServerService>;
using ClientServiceRef = std::shared_ptr<class FClientService>;
using SendBufferRef = std::shared_ptr<class FSendBuffer>;
//using SendBufferChunkRef = std::shared_ptr<class FSendBufferChunk>;
using PacketSessionRef = std::shared_ptr<class FPacketSession>;
using JobRef = std::shared_ptr<class FJob>;
using JobQueueRef = std::shared_ptr<class FJobQueue>;

#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))
#define len16(arr)		static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)		static_cast<int32>(sizeof(arr)/sizeof(arr[0]))


//stompallocator 활성화
//allocator.h에서 사용함
//#define _STOMP