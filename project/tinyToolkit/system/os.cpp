/**
 *
 *  作者: hm
 *
 *  说明: 进程信息
 *
 */


#include "os.h"


namespace tinyToolkit
{
	/**
	 *
	 * 获取线程id
	 *
	 * @return 线程id
	 *
	 */
	uint64_t OS::ThreadID()
	{
		uint64_t tid = 0;

	#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		tid = ::GetCurrentThreadId();

	#elif TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_LINUX

		tid = static_cast<uint64_t>(::syscall(SYS_gettid));

	#elif TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

		pthread_threadid_np(nullptr, &tid);

	#else

		tid = static_cast<uint64_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));

	#endif

		return tid;
	}

	/**
	 *
	 * 获取当前进程id
	 *
	 * @return pid
	 *
	 */
	uint64_t OS::ProcessID()
	{
	#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		return ::GetCurrentProcessId();

	#else

		return static_cast<uint64_t>(::getpid());

	#endif
	}

	/**
		 *
		 * 最后一个错误代码
		 *
		 * @return 错误代码
		 *
		 */
	int32_t OS::LastErrorCode()
	{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		return ::GetLastError();

#else

		return errno;

#endif
	}

	/**
	 *
	 * 最后一个错误信息
	 *
	 * @return 错误信息
	 *
	 */
	const char * OS::LastErrorMessage()
	{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		return strerror(LastErrorCode());

#else

		return strerror(LastErrorCode());

#endif
	}
}
