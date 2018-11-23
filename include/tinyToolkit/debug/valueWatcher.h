#ifndef __TINY_TOOLKIT__DEBUG__VALUE_WATCHER__H__
#define __TINY_TOOLKIT__DEBUG__VALUE_WATCHER__H__


/**
 *
 *  作者: hm
 *
 *  说明: 数值观察
 *
 */


#include "../pool/callback.h"
#include "../utilities/string.h"


namespace tinyToolkit
{
	template <typename ValueTypeT = int64_t>
	class TINY_TOOLKIT_API ValueWatcher
	{
		typedef std::function<void(ValueTypeT oldValue, ValueTypeT newValue)> FunctionType;

	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		~ValueWatcher()
		{
			for (auto &iter : _callbackPool)
			{
				TINY_TOOLKIT_DELETE_PTR(iter.second)
			}
		}

		void Set(const char * key, ValueTypeT value)
		{
			std::lock_guard<std::mutex> lock(_lock);

			ValueTypeT oldValue = { };

			{
				auto iter = _valuePool.find(key);

				if (iter == _valuePool.end())
				{
					_valuePool.emplace(key, value);
				}
				else
				{
					oldValue = iter->second;

					iter->second = value;
				}
			}

			{
				auto iter = _callbackPool.find(key);

				if (iter != _callbackPool.end())
				{
					iter->second->Call(std::forward<ValueTypeT>(oldValue), std::forward<ValueTypeT>(value));
				}
			}
		}

		void AddCallBack(const std::string & key, FunctionType function)
		{
			std::lock_guard<std::mutex> lock(_lock);

			auto iter = _callbackPool.find(key);

			if (iter == _callbackPool.end())
			{
				_callbackPool.insert(std::make_pair(key, new CallBackPool<void, ValueTypeT, ValueTypeT>));

				iter = _callbackPool.find(key);
			}

			iter->second->Register(std::move(function));
		}

		void Print()
		{
			String::Print("+--------------------------------------------------|--------------------------------------------------+\n");
			String::Print("|{:^50}|{:^50}|\n", "Key", "Value");

			for (auto &iter : _valuePool)
			{
				String::Print("+--------------------------------------------------|--------------------------------------------------+\n");
				String::Print("|{:^50}|{:^50}|\n", iter.first, iter.second);
			}

			String::Print("+--------------------------------------------------|--------------------------------------------------+\n");
		}

	protected:
		std::mutex _lock;

		std::unordered_map<std::string, ValueTypeT> _valuePool;
		std::unordered_map<std::string, CallBackPool<void, ValueTypeT, ValueTypeT> *> _callbackPool;
	};
}


#endif // __TINY_TOOLKIT__DEBUG__VALUE_WATCHER__H__
