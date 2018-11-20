#ifndef __TINY_TOOLKIT__LOG__O_STREAM_SINK__H__
#define __TINY_TOOLKIT__LOG__O_STREAM_SINK__H__


/**
 *
 *  作者: hm
 *
 *  说明: 日志输出流节点
 *
 */


#include "sink.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API OStreamLogSink : public ILogSink
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 * @param name 节点名称
		 * @param stream 输出流
		 *
		 */
		explicit OStreamLogSink(std::string name, std::shared_ptr<std::ostream> stream) : ILogSink(std::move(name)),
																						  _stream(std::move(stream))
		{

		};

		/**
		 *
		 * 析构函数
		 *
		 */
		~OStreamLogSink() override
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
			Flush();
		}

		/**
		 *
		 * 刷新日志
		 *
		 */
		void Flush() override
		{
			if (_stream)
			{
				_stream->flush();
			}
		}

		/**
		 *
		 * 重新打开日志
		 *
		 */
		void Reopen() override
		{
			if (_stream)
			{
				_stream->clear();  /// 清除错误标志
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

			if (_stream)
			{
				*_stream << (Layout() ? Layout()->Format(event) : event.message);
			}

			if (_autoFlush)
			{
				Flush();
			}
		}

		/**
		 *
		 * 输出流
		 *
		 * @return 输出流
		 *
		 */
		const std::shared_ptr<std::ostream> & Stream() const
		{
			return _stream;
		}

	protected:
		std::shared_ptr<std::ostream> _stream{ };
	};
}


#endif // __TINY_TOOLKIT__LOG__O_STREAM_SINK__H__
