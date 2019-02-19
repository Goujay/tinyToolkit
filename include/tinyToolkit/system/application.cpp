/**
 *
 *  作者: hm
 *
 *  说明: 程序信息
 *
 */


#include "os.h"
#include "application.h"

#include "../utilities/time.h"
#include "../utilities/file.h"
#include "../utilities/string.h"
#include "../utilities/filesystem.h"


namespace tinyToolkit
{
	/**
	 *
	 * 复制进程
	 *
	 * @param isCloseIO 是否关闭输出
	 *
	 * @return 复制结果
	 *
	 */
	bool Application::Fork(bool isCloseIO)
	{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

		return false;

#else

		/**
		 *
		 * fork函数返回两个值
		 *
		 * 对于子进程, 返回0
		 *
		 * 对于父进程, 返回子进程ID
		 *
		 */
		pid_t pid = ::fork();

		if (pid < 0)
		{
			return false;
		}

		if (pid != 0)  /// 父进程
		{
			exit(0);
		}

		/**
		 *
		 * 当进程是会话的领头进程时setsid()调用失败并返回(-1)
		 *
		 * setsid()调用成功后, 返回新的会话的ID, 调用setsid函数的进程成为新的会话的领头进程, 并与其父进程的会话组和进程组脱离
		 *
		 * 由于会话对控制终端的独占性, 进程同时与控制终端脱离
		 *
		 *
		 * 之前parent和child运行在同一个session里, parent是会话(session)的领头进程
		 *
		 * 所以作为session头的parent如果exit结束执行的话,那么会话 session组中的所有进程将都被杀死
		 *
		 * 执行setsid()之后, child将重新获得一个新的会话(session)id
		 *
		 * 这时parent退出之后, 将不会影响到child了
		 *
		 */
		pid = setsid();

		if (pid < -1)
		{
			return false;
		}

		if (isCloseIO)
		{
			int32_t fd = ::open("/dev/null", O_RDWR, 0);

			if (fd != -1)
			{
				::dup2(fd, STDIN_FILENO);   /// 关闭标准输入
				::dup2(fd, STDOUT_FILENO);  /// 关闭标准输出
				::dup2(fd, STDERR_FILENO);  /// 关闭标准错误输出

				::close(fd);
			}
		}

		return true;

	#endif
	}

	/**
	 *
	 * 进程是否存在
	 *
	 * @return 进程状态
	 *
	 */
	bool Application::Exist()
	{
		static std::pair<LockFile, bool> value({ }, true);

		if (value.second)
		{
		#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

			if (value.first.Open(String::Format(R"(C:\Users\Public\Documents\{}.pid)", Name()), true))
			{
				value.first << std::to_string(OS::ProcessID());
			}

		#else

			if (value.first.Open(String::Format("/var/run/{}.pid", Name()), true))
			{
				value.first << std::to_string(OS::ProcessID());
			}

		#endif

			value.second = false;
		}

		return !value.first.IsOpen();
	}

	/**
	 *
	 * 编译时间
	 *
	 * @return 编译时间
	 *
	 */
	std::time_t Application::CompileTime()
	{
		static std::pair<std::time_t, bool> value(0, true);

		if (value.second)
		{
			value.first = Time::FromTimeString(CompileTimeString().c_str());

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 程序完整路径
	 *
	 * @return 程序完整路径
	 *
	 */
	const std::string & Application::Path()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			char str[TINY_TOOLKIT_PATH_MAX + 1] = { 0 };

		#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

			GetModuleFileName(nullptr, str, TINY_TOOLKIT_PATH_MAX);

		#elif TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

			uint32_t size = TINY_TOOLKIT_PATH_MAX;

			_NSGetExecutablePath(str, &size);

			str[size] = '\0';

		#else

			readlink("/proc/self/exe", str, sizeof(str));

		#endif

			value.first = str;

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 程序名
	 *
	 * @return 程序名
	 *
	 */
	const std::string & Application::Name()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			value.first = Filesystem::Name(Path());

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 程序前缀
	 *
	 * @return 程序前缀
	 *
	 */
	const std::string & Application::Steam()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			value.first = Filesystem::Steam(Name());

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 程序扩展名
	 *
	 * @return 程序扩展名
	 *
	 */
	const std::string & Application::Extension()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			value.first = Filesystem::Extension(Name());

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 程序目录
	 *
	 * @return 程序目录
	 *
	 */
	const std::string & Application::Directory()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			value.first = Filesystem::ParentDirectory(Path());

			value.second = false;
		}

		return value.first;
	}

	/**
	 *
	 * 编译时间字符串
	 *
	 * @return 编译时间字符串
	 *
	 */
	const std::string & Application::CompileTimeString()
	{
		static std::pair<std::string, bool> value({}, true);

		if (value.second)
		{
			value.first = String::Format
			(
				"{:04d}-{:02d}-{:02d} {}",

				(((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 + (__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'),

				__DATE__[2] == 'n' ? 1 :
				__DATE__[2] == 'b' ? 2 :
				__DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) :
				__DATE__[2] == 'y' ? 5 :
				__DATE__[2] == 'n' ? 6 :
				__DATE__[2] == 'l' ? 7 :
				__DATE__[2] == 'g' ? 8 :
				__DATE__[2] == 'p' ? 9 :
				__DATE__[2] == 't' ? 10 :
				__DATE__[2] == 'v' ? 11 : 12,

				(__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 + (__DATE__[5] - '0'),

				__TIME__
			);

			value.second = false;
		}

		return value.first;
	}
}
