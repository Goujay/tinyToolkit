#ifndef __TINY_TOOLKIT__NET__SESSION__H__
#define __TINY_TOOLKIT__NET__SESSION__H__


/**
 *
 *  作者: hm
 *
 *  说明: 通讯会话
 *
 */


#include "pipe.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API ITCPSession
	{
	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		virtual ~ITCPSession() = default;

		/**
		 *
		 * 接收数据触发回调函数
		 *
		 * @param data 接收的数据缓冲区
		 * @param size 接收的数据缓冲区长度
		 *
		 */
		virtual void OnReceive(const char * data, std::size_t size) = 0;

		/**
		 *
		 * 连接成功触发回调函数
		 *
		 */
		virtual void OnConnect() = 0;

		/**
		 *
		 * 断开连接触发回调函数
		 *
		 */
		virtual void OnDisconnect() = 0;

		/**
		 *
		 * 连接失败触发回调函数
		 *
		 */
		virtual void OnConnectFailed() = 0;

		/**
		 *
		 * 启动
		 *
		 * @param host 主机地址
		 * @param port 主机端口
		 *
		 * @return 是否启动成功
		 *
		 */
		bool Launch(const std::string & host, uint16_t port);

		/**
		 *
		 * 关闭会话
		 *
		 */
		void Close();

		/**
		 *
		 * 发送数据
		 *
		 * @param value 待发送数据
		 * @param size 待发送数据长度
		 *
		 */
		void Send(const void * value, std::size_t size);

	public:
		uint16_t _localPort{ 0 };
		uint16_t _remotePort{ 0 };

		std::string _localHost{ };
		std::string _remoteHost{ };

		std::shared_ptr<ITCPPipe> _pipe{ };
	};

	class TINY_TOOLKIT_API IUDPSession
	{
	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		virtual ~IUDPSession() = default;

		/**
		 *
		 * 接收数据触发回调函数
		 *
		 * @param host 接收的主机地址
		 * @param port 接收的主机端口
		 * @param data 接收的数据缓冲区
		 * @param size 接收的数据缓冲区长度
		 *
		 */
		virtual void OnReceive(const char * host, uint16_t port, const char * data, std::size_t size) = 0;

		/**
		 *
		 * 连接成功触发回调函数
		 *
		 */
		virtual void OnConnect() = 0;

		/**
		 *
		 * 断开连接触发回调函数
		 *
		 */
		virtual void OnDisconnect() = 0;

		/**
		 *
		 * 连接失败触发回调函数
		 *
		 */
		virtual void OnConnectFailed() = 0;

		/**
		 *
		 * 启动
		 *
		 * @param host 主机地址
		 * @param port 主机端口
		 *
		 * @return 是否启动成功
		 *
		 */
		bool Launch(const std::string & host, uint16_t port);

		/**
		 *
		 * 关闭会话
		 *
		 */
		void Close();

		/**
		 *
		 * 发送数据
		 *
		 * @param host 待发送主机地址
		 * @param port 待发送主机端口
		 * @param value 待发送数据
		 * @param size 待发送数据长度
		 *
		 */
		void Send(const char * host, uint16_t port, const void * value, std::size_t size);

	public:
		uint16_t _localPort{ 0 };
		uint16_t _remotePort{ 0 };

		std::string _localHost{ };
		std::string _remoteHost{ };

		std::shared_ptr<IUDPPipe> _pipe{ };
	};
}


#endif // __TINY_TOOLKIT__NET__SESSION__H__
