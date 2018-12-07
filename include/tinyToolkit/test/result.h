#ifndef __TINY_TOOLKIT__TEST__RESULT__H__
#define __TINY_TOOLKIT__TEST__RESULT__H__


/**
 *
 *  作者: hm
 *
 *  说明: 解析结果
 *
 */


#include "../container/message.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API TestResult
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 * @param result 结果
		 *
		 */
		explicit TestResult(bool result) : _result(result)
		{

		}

		/**
		 *
		 * 构造函数
		 *
		 * @param rhs 实例化对象
		 *
		 */
		TestResult(TestResult && rhs) noexcept : _result(rhs._result),
												 _message(std::move(rhs._message))
		{

		}

		/**
		 *
		 * 构造函数
		 *
		 * @param lhs 实例化对象
		 *
		 */
		TestResult(const TestResult & lhs) = default;

		/**
		 *
		 * 析构函数
		 *
		 */
		virtual ~TestResult() = default;

		/**
		 *
		 * 重载()操作
		 *
		 * @return 结果
		 *
		 */
		explicit operator bool() const
		{
			return _result;
		}

		/**
		 *
		 * 信息
		 *
		 * @return 信息
		 *
		 */
		std::string String() const
		{
			return _message.String();
		}

		/**
		 *
		 * 重载<<操作
		 *
		 * @tparam TypeT [all built-in types]
		 *
		 * @param value 数据
		 *
		 * @return 实例化对象
		 *
		 */
		template <typename TypeT>
		TestResult & operator<<(TypeT && value)
		{
			_message << std::forward<TypeT>(value);

			return *this;
		}

		/**
		 *
		 * 重载=操作
		 *
		 * @param rhs 实例化对象
		 *
		 */
		TestResult & operator=(TestResult && rhs) noexcept
		{
			_result = rhs._result;
			_message = std::move(rhs._message);

			return *this;
		}

		/**
		 *
		 * 重载=操作
		 *
		 * @param lhs 实例化对象
		 *
		 */
		TestResult & operator=(const TestResult & lhs)
		{
			if (this != &lhs)
			{
				_result = lhs._result;
				_message = lhs._message;
			}

			return *this;
		}

	protected:
		bool _result{ true };

		Message _message{};
	};

	class TINY_TOOLKIT_API TestSuccessResult : public tinyToolkit::TestResult
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 */
		TestSuccessResult() : TestResult(true)
		{

		}

		/**
		 *
		 * 析构函数
		 *
		 */
		~TestSuccessResult() override = default;
	};

	class TINY_TOOLKIT_API TestFailureResult : public tinyToolkit::TestResult
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 */
		TestFailureResult() : TestResult(false)
		{

		}

		/**
		 *
		 * 析构函数
		 *
		 */
		~TestFailureResult() override = default;
	};

	/**
	 *
	 * 重载<<操作
	 *
	 * @param os 输出流
	 * @param result 测试结果
	 *
	 * @return 输出流
	 *
	 */
	std::ostream & operator<<(std::ostream & os, const TestResult & result)
	{
		return os << result.String();
	}
}


#endif // __TINY_TOOLKIT__TEST__RESULT__H__
