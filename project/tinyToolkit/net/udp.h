#ifndef __TINY_TOOLKIT__NET__UDP__H__
#define __TINY_TOOLKIT__NET__UDP__H__


/**
 *
 *  作者: hm
 *
 *  说明: udp服务
 *
 */


#include "event.h"
#include "server.h"
#include "buffer.h"
#include "handle.h"
#include "session.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API UDPSessionPipe : public IUDPPipe, public INetCompleter
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 * @param handle 管理句柄
		 * @param session 会话
		 * @param socket 会话句柄
		 * @param sSize 发送缓冲区大小
		 * @param rSize 接收缓冲区大小
		 *
		 */
		UDPSessionPipe(NetHandle & handle, IUDPSession * session, TINY_TOOLKIT_SOCKET_TYPE socket, std::size_t sSize, std::size_t rSize);

		/**
		 *
		 * 析构函数
		 *
		 */
		~UDPSessionPipe() override = default;

		/**
		 *
		 * 关闭会话
		 *
		 */
		void Close() override;

		/**
		 *
		 * 发送数据
		 *
		 * @param value 待发送数据
		 * @param size 待发送数据长度
		 * @param delay 延迟发送
		 *
		 */
		void Send(const void * value, std::size_t size, bool delay) override;

		/**
		 *
		 * 回调函数
		 *
		 * @param netEvent 网络事件
		 * @param sysEvent 系统事件
		 *
		 */
		void OnCallback(const NetEvent * netEvent, const void * sysEvent) override;

	protected:
		/**
		 *
		 * 连接处理
		 *
		 * @param netEvent 网络事件
		 * @param sysEvent 系统事件
		 *
		 */
		void DoConnect(const NetEvent * netEvent, const void * sysEvent);

		/**
		 *
		 * 交互处理
		 *
		 * @param netEvent 网络事件
		 * @param sysEvent 系统事件
		 *
		 */
		void DoTransmit(const NetEvent * netEvent, const void * sysEvent);

	public:
		bool _isSend{ false };
		bool _isReceive{ false };
		bool _isConnect{ false };

		NetEvent _netEvent{ };

	private:
		NetBuffer _sendBuffer;
		NetBuffer _receiveBuffer;

		NetHandle & _managerHandle;

		IUDPSession * _session{ nullptr };

		TINY_TOOLKIT_SOCKET_TYPE _socket{ TINY_TOOLKIT_SOCKET_INVALID };
	};

	class TINY_TOOLKIT_API UDPServerPipe : public IUDPPipe, public INetCompleter
	{
	public:
		/**
		 *
		 * 构造函数
		 *
		 * @param handle 管理句柄
		 * @param server 服务器
		 * @param socket 会话句柄
		 * @param sSize 发送缓冲区大小
		 * @param rSize 接收缓冲区大小
		 *
		 */
		UDPServerPipe(NetHandle & handle, IUDPServer * server, TINY_TOOLKIT_SOCKET_TYPE socket, std::size_t sSize, std::size_t rSize);

		/**
		 *
		 * 析构函数
		 *
		 */
		~UDPServerPipe() override = default;

		/**
		 *
		 * 关闭会话
		 *
		 */
		void Close() override;

		/**
		 *
		 * 发送数据
		 *
		 * @param value 待发送数据
		 * @param size 待发送数据长度
		 * @param delay 延迟发送
		 *
		 */
		void Send(const void * value, std::size_t size, bool delay) override;

		/**
		 *
		 * 回调函数
		 *
		 * @param netEvent 网络事件
		 * @param sysEvent 系统事件
		 *
		 */
		void OnCallback(const NetEvent * netEvent, const void * sysEvent) override;

	protected:
		/**
		 *
		 * 连接处理
		 *
		 * @param netEvent 网络事件
		 * @param sysEvent 系统事件
		 *
		 */
		void DoAccept(const NetEvent * netEvent, const void * sysEvent);

	public:
		NetEvent _netEvent{ };

	private:
		std::size_t _sSize{ 0 };
		std::size_t _rSize{ 0 };

		NetHandle & _managerHandle;

		IUDPServer * _server{ nullptr };

		TINY_TOOLKIT_SOCKET_TYPE _socket{ TINY_TOOLKIT_SOCKET_INVALID };
	};
}


#endif // __TINY_TOOLKIT__NET__UDP__H__
