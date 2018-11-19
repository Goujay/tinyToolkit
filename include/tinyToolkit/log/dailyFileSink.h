#ifndef __TINY_TOOLKIT__LOG__DAILY_FILE_SINK__H__
#define __TINY_TOOLKIT__LOG__DAILY_FILE_SINK__H__


/**
 *
 *  作者: hm
 *
 *  说明: 时间文件日志节点
 *
 */


#include "sink.h"
#include "file.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API DailyFileSink : public ILogSink
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 * @param path 日志路径
		 * @param hour 时
		 * @param minutes 分
		 * @param seconds 秒
		 *
		 */
		explicit DailyFileSink(const char * path, int32_t hour = 0, int32_t minutes = 0, int32_t seconds = 0) : _hour(hour),
																												_minutes(minutes),
																												_seconds(seconds),
																												_path(path)

		{
			if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
			{
				throw std::logic_error("Invalid Time");
			}

			if (!_file.Open(CalculatePath(_path)))
			{
				throw std::logic_error("Open daily log file failed : " + _file.Path());
			}

			RotatingTime();
		}

		/**
		 *
		 * 构造函数
		 *
		 * @param path 日志路径
		 * @param hour 时
		 * @param minutes 分
		 * @param seconds 秒
		 *
		 */
		explicit DailyFileSink(std::string path, int32_t hour, int32_t minutes, int32_t seconds) : _hour(hour),
																								   _minutes(minutes),
																								   _seconds(seconds),
																								   _path(std::move(path))

		{
			if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
			{
				throw std::logic_error("Invalid Time");
			}

			if (!_file.Open(CalculatePath(_path)))
			{
				throw std::logic_error("Open daily log file failed : " + _file.Path());
			}

			RotatingTime();
		}

		/**
		 *
		 * 析构函数
		 *
		 */
		~DailyFileSink() override
		{
			Close();
		}

		/**
		 *
		 * 关闭日志
		 *
		 */
		void Close() override
		{
			_file.Close();
		}

		/**
		 *
		 * 刷新日志
		 *
		 */
		void Flush() override
		{
			_file.Flush();
		}

		/**
		 *
		 * 重新打开日志
		 *
		 */
		void Reopen() override
		{
			if (!_file.Reopen())
			{
				throw std::logic_error("ReOpen daily log file failed : " + _file.Path());
			}
		}

		/**
		 *
		 * 写入日志
		 *
		 * @param event 日志事件
		 *
		 */
		void Write(const LogEvent & event) override
		{
			if (Filter() && Filter()->Decide(event))
			{
				return;
			}

			if (tinyToolkit::Time::Seconds() >= _time)
			{
				_file.Close();

				if (!_file.Open(CalculatePath(_path)))
				{
					throw std::logic_error("Open daily log file failed : " + _file.Path());
				}

				RotatingTime();
			}

			_file.Write(Layout() ? Layout()->Format(event) : event.message);

			if (_autoFlush)
			{
				Flush();
			}
		}

		/**
		 *
		 * 是否已经打开
		 *
		 * @return 是否已经打开
		 *
		 */
		bool IsOpen()
		{
			return _file.IsOpen();
		}

		/**
		 *
		 * 日志大小
		 *
		 * @return 日志大小
		 *
		 */
		std::size_t Size() const
		{
			return _file.Size();
		}

		/**
		 *
		 * 下次生成日志时间
		 *
		 * @return 下次生成日志时间
		 *
		 */
		std::time_t NextTime() const
		{
			return _time;
		}

		/**
		 *
		 * 日志路径
		 *
		 * @return 日志路径
		 *
		 */
		const std::string & Path() const
		{
			return _file.Path();
		}

	protected:
		/**
		 *
		 * 计算下一次生成日志的时间
		 *
		 */
		void RotatingTime()
		{
			std::tm date = tinyToolkit::Time::LocalTm();

			date.tm_hour = _hour;
			date.tm_min  = _minutes;
			date.tm_sec  = _seconds;

			_time = tinyToolkit::Time::FromTm(date);

			if (_time < tinyToolkit::Time::Seconds())
			{
				_time += TINY_TOOLKIT_DAY;
			}
		}

		/**
		 *
		 * 推导日志路径
		 *
		 * @param path 日志路径
		 *
		 * @return 日志路径
		 *
		 */
		std::string CalculatePath(const std::string & path)
		{
			std::size_t pos = path.rfind('.');

			if (pos == std::string::npos)  /// name_2018_01_01_00_00_00
			{
				return tinyToolkit::String::Format
				(
					"{}_{}",
					path,
					tinyToolkit::Time::CurrentLocalTimeString("%4d_%02d_%02d_%02d_%02d_%02d")
				);
			}
			else if (pos == 0)  /// 2018_01_01_00_00_00.log
			{
				return tinyToolkit::String::Format
				(
					"{}{}",
					tinyToolkit::Time::CurrentLocalTimeString("%4d_%02d_%02d_%02d_%02d_%02d"),
					path
				);
			}
			else   /// name_2018_01_01_00_00_00.log
			{
				return tinyToolkit::String::Format
				(
					"{}_{}{}",
					path.substr(0, pos),
					tinyToolkit::Time::CurrentLocalTimeString("%4d_%02d_%02d_%02d_%02d_%02d"),
					path.substr(pos)
				);
			}
		}

	protected:
		LogFile _file;

		int32_t _hour{ 0 };
		int32_t _minutes{ 0 };
		int32_t _seconds{ 0 };

		std::time_t _time{ 0 };

		std::string _path{ };
	};
}


#endif // __TINY_TOOLKIT__LOG__DAILY_FILE_SINK__H__
