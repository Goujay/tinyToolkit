/**
 *
 *  作者: hm
 *
 *  说明: 通讯管理器
 *
 */


#include "manager.h"

#include "../system/os.h"
#include "../debug/trace.h"
#include "../utilities/net.h"


namespace tinyToolkit
{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

	/// todo

#else

	/**
	*
	* 获取本地名称
	*
	* @param socket 句柄
	* @param host 地址
	* @param port 端口
	*
	* @return 是否获取成功
	*
	*/
	static bool GetLocalName(int32_t socket, std::string & host, uint16_t & port)
	{
		struct sockaddr_in address { };

		std::size_t addressLen = sizeof(address);

		if (::getsockname(socket, (struct sockaddr *)&address, (socklen_t *)&addressLen) == 0)
		{
			host = inet_ntoa(address.sin_addr);

			port = ntohs(address.sin_port);

			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	*
	* 启用Nagle算法
	*
	* @param socket 句柄
	*
	* @return 是否设置成功
	*
	*/
	static bool EnableNoDelay(int32_t socket)
	{
		int32_t val = 1l;

		return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&val), sizeof(val)) == 0;
	}

	/**
	*
	* 启用非堵塞
	*
	* @param socket 句柄
	*
	* @return 是否设置成功
	*
	*/
	static bool EnableNonBlock(int32_t socket)
	{
		return fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) == 0;
	}

	/**
	*
	* 启用端口复用
	*
	* @param socket 句柄
	*
	* @return 是否设置成功
	*
	*/
	static bool EnableReusePort(int32_t socket)
	{
		int32_t val = 1l;

		return setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char *>(&val), sizeof(val)) == 0;
	}

	/**
	*
	* 启用地址复用
	*
	* @param socket 句柄
	*
	* @return 是否设置成功
	*
	*/
	static bool EnableReuseAddress(int32_t socket)
	{
		int32_t val = 1l;

		return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&val), sizeof(val)) == 0;
	}

#endif

	/**
	 *
	 * 获取单例
	 *
	 * @return 单例对象引用
	 *
	 */
	NetWorkManager & NetWorkManager::Instance()
	{
		return Singleton<NetWorkManager>::Instance();
	}

	/**
	 *
	 * 析构函数
	 *
	 */
	NetWorkManager::~NetWorkManager()
	{
		if (_status)
		{
			_status = false;

			if (_thread.joinable())
			{
				_thread.join();
			}

			if (_socket != -1)
			{
				::close(_socket);

				_socket = -1;
			}
		}
	}

	/**
	 *
	 * 启动udp客户端
	 *
	 * @param client 客户端
	 * @param host 主机地址
	 * @param port 主机端口
	 *
	 * @return 是否启动成功
	 *
	 */
	bool NetWorkManager::LaunchUDPClient(IUDPSession * client, const std::string & host, uint16_t port)
	{
		if (!Launch())
		{
			return false;
		}

#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

#else

		int32_t sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sock == -1)
		{
			TINY_TOOLKIT_ASSERT(false, "socket : {}", OS::LastErrorMessage())

			return false;
		}

		client->_remotePort = port;
		client->_remoteHost = Net::ParseHost(host.c_str());

		if (!EnableNonBlock(sock) || 
			!EnableReusePort(sock) ||
			!EnableReuseAddress(sock))
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}

		struct sockaddr_in address{ };

		address.sin_port = htons(client->_remotePort);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = Net::AsNetByte(client->_remoteHost.c_str());

		if (::connect(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1)
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}

		GetLocalName(sock, client->_localHost, client->_localPort);

		auto pipe = std::make_shared<UDPSessionPipe>(_socket, sock, client, NET_EVENT_TYPE::TRANSMIT);

#  if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

		struct kevent event[2]{ };

		EV_SET(&event[0], sock, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, (void *)&pipe->_netEvent);
		EV_SET(&event[1], sock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void *)&pipe->_netEvent);

		if (kevent(_socket, event, 2, nullptr, 0, nullptr) == -1)
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}
		else
		{
			pipe->_isConnect = true;

			client->_pipe = pipe;

			client->OnConnect();
		}

#  else

		struct epoll_event event{ };

		event.events = EPOLLIN;
		event.data.ptr = &pipe->_netEvent;

		if (epoll_ctl(_socket, EPOLL_CTL_ADD, sock, &event) == -1)
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}
		else
		{
			pipe->_isConnect = true;

			client->_pipe = pipe;

			client->OnConnect();
		}

#  endif
#
#endif

		return true;
	}

	/**
	 *
	 * 启动udp服务器
	 *
	 * @param server 服务器
	 * @param host 主机地址
	 * @param port 主机端口
	 *
	 * @return 是否启动成功
	 *
	 */
	bool NetWorkManager::LaunchUDPServer(IUDPServer * server, const std::string & host, uint16_t port)
	{
		if (!Launch())
		{
			return false;
		}

#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

#else

		int32_t sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sock == -1)
		{
			TINY_TOOLKIT_ASSERT(false, "socket : {}", OS::LastErrorMessage())

			return false;
		}

		server->_port = port;
		server->_host = Net::ParseHost(host.c_str());

		if (!EnableNonBlock(sock) || 
			!EnableReusePort(sock) ||
			!EnableReuseAddress(sock))
		{
			::close(sock);

			server->OnError();

			return false;
		}

		struct sockaddr_in address{ };

		address.sin_port = htons(server->_port);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = Net::AsNetByte(server->_host.c_str());

		if (::bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr)) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}

		auto pipe = std::make_shared<UDPServerPipe>(_socket, sock, server, NET_EVENT_TYPE::ACCEPT);

#  if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

		struct kevent event[2]{ };

		EV_SET(&event[0], sock, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, (void *)&pipe->_netEvent);
		EV_SET(&event[1], sock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void *)&pipe->_netEvent);

		if (kevent(_socket, event, 2, nullptr, 0, nullptr) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}
		else
		{
			server->_pipe = pipe;
		}

#  else

		struct epoll_event event{ };

		event.events = EPOLLIN;
		event.data.ptr = &pipe->_netEvent;

		if (epoll_ctl(_socket, EPOLL_CTL_ADD, sock, &event) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}
		else
		{
			server->_pipe = pipe;
		}

#  endif
#
#endif

		return true;
	}

	/**
	 *
	 * 启动tcp客户端
	 *
	 * @param client 客户端
	 * @param host 主机地址
	 * @param port 主机端口
	 *
	 * @return 是否启动成功
	 *
	 */
	bool NetWorkManager::LaunchTCPClient(ITCPSession * client, const std::string & host, uint16_t port)
	{
		if (!Launch())
		{
			return false;
		}

#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

#else

		int32_t sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == -1)
		{
			TINY_TOOLKIT_ASSERT(false, "socket : {}", OS::LastErrorMessage())

			return false;
		}

		client->_remotePort = port;
		client->_remoteHost = Net::ParseHost(host.c_str());

		if (!EnableNoDelay(sock) || 
			!EnableNonBlock(sock) ||
			!EnableReusePort(sock) ||
			!EnableReuseAddress(sock))
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}

		struct sockaddr_in address{ };

		address.sin_port = htons(client->_remotePort);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = Net::AsNetByte(client->_remoteHost.c_str());

		int32_t ret = ::connect(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in));

		if (ret == 0)
		{
			GetLocalName(sock, client->_localHost, client->_localPort);

			auto pipe = std::make_shared<TCPSessionPipe>(_socket, sock, client, NET_EVENT_TYPE::TRANSMIT);

#  if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

			struct kevent event[2]{ };

			EV_SET(&event[0], sock, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, (void *)&pipe->_netEvent);
			EV_SET(&event[1], sock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void *)&pipe->_netEvent);

			if (kevent(_socket, event, 2, nullptr, 0, nullptr) == -1)
			{
				::close(sock);

				client->OnConnectFailed();

				return false;
			}
			else
			{
				pipe->_isConnect = true;

				client->_pipe = pipe;

				client->OnConnect();
			}

#  else

			struct epoll_event event{ };

			event.events = EPOLLIN;
			event.data.ptr = &pipe->_netEvent;

			if (epoll_ctl(_socket, EPOLL_CTL_ADD, sock, &event) == -1)
			{
				::close(sock);

				client->OnConnectFailed();

				return false;
			}
			else
			{
				pipe->_isConnect = true;

				client->_pipe = pipe;

				client->OnConnect();
			}

#  endif
		}
		else if (ret < 0 && errno != EINPROGRESS)
		{
			::close(sock);

			client->OnConnectFailed();

			return false;
		}
		else
		{
			GetLocalName(sock, client->_localHost, client->_localPort);

			auto pipe = std::make_shared<TCPSessionPipe>(_socket, sock, client, NET_EVENT_TYPE::CONNECT);

#  if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

			struct kevent event[2]{ };

			EV_SET(&event[0], sock, EVFILT_READ,  EV_ADD | EV_DISABLE, 0, 0, (void *)&pipe->_netEvent);
			EV_SET(&event[1], sock, EVFILT_WRITE, EV_ADD | EV_ENABLE,  0, 0, (void *)&pipe->_netEvent);

			if (kevent(_socket, event, 2, nullptr, 0, nullptr) == -1)
			{
				::close(sock);

				client->OnConnectFailed();

				return false;
			}
			else
			{
				client->_pipe = pipe;
			}

#  else

			struct epoll_event event{ };

			event.events = EPOLLOUT;
			event.data.ptr = &pipe->_netEvent;

			if (epoll_ctl(_socket, EPOLL_CTL_ADD, sock, &event) == -1)
			{
				::close(sock);

				client->OnConnectFailed();

				return false;
			}
			else
			{
				client->_pipe = pipe;
			}

#  endif
		}

#endif

		return true;
	}

	/**
	 *
	 * 启动tcp服务器
	 *
	 * @param server 服务器
	 * @param host 主机地址
	 * @param port 主机端口
	 *
	 * @return 是否启动成功
	 *
	 */
	bool NetWorkManager::LaunchTCPServer(ITCPServer * server, const std::string & host, uint16_t port)
	{
		if (!Launch())
		{
			return false;
		}

#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

#else

		int32_t sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == -1)
		{
			TINY_TOOLKIT_ASSERT(false, "socket : {}", OS::LastErrorMessage())

			return false;
		}

		server->_port = port;
		server->_host = Net::ParseHost(host.c_str());

		if (!EnableNoDelay(sock) || 
			!EnableNonBlock(sock) ||
			!EnableReusePort(sock) ||
			!EnableReuseAddress(sock))
		{
			::close(sock);

			server->OnError();

			return false;
		}

		struct sockaddr_in address{ };

		address.sin_port = htons(server->_port);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = Net::AsNetByte(server->_host.c_str());

		if (::bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}

		if (::listen(sock, 20) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}

		auto pipe = std::make_shared<TCPServerPipe>(_socket, sock, server, NET_EVENT_TYPE::ACCEPT);

#  if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

		struct kevent event[2]{ };

		EV_SET(&event[0], sock, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, (void *)&pipe->_netEvent);
		EV_SET(&event[1], sock, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, (void *)&pipe->_netEvent);

		if (kevent(_socket, event, 2, nullptr, 0, nullptr) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}
		else
		{
			server->_pipe = pipe;
		}

#  else

		struct epoll_event event{ };

		event.events = EPOLLIN;
		event.data.ptr = &pipe->_netEvent;

		if (epoll_ctl(_socket, EPOLL_CTL_ADD, sock, &event) == -1)
		{
			::close(sock);

			server->OnError();

			return false;
		}
		else
		{
			server->_pipe = pipe;
		}

#  endif
#
#endif

		return true;
	}

	/**
	 *
	 * 启动
	 *
	 * @return 是否启动成功
	 *
	 */
	bool NetWorkManager::Launch()
	{
		if (_socket == -1)
		{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

			/// todo

#elif TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

			_socket = kqueue();

#else

			_socket = epoll_create(TINY_TOOLKIT_NET_COUNT);

#endif

			if (_socket == -1)
			{
				TINY_TOOLKIT_ASSERT(false, "create socket error : {}", OS::LastErrorMessage())

				return false;
			}

			_status = true;
			_thread = std::thread(&NetWorkManager::ThreadProcess, this);

			return true;
		}

		return _socket != -1;
	}

	/**
	 *
	 * app线程逻辑函数
	 *
	 */
	void NetWorkManager::ThreadProcess()
	{
#if TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_WINDOWS

		/// todo

#elif TINY_TOOLKIT_PLATFORM == TINY_TOOLKIT_PLATFORM_APPLE

		static struct kevent events[TINY_TOOLKIT_NET_COUNT]{ };

		while (_status)
		{
			struct timespec timeout{ };

			timeout.tv_sec = 1;
			timeout.tv_nsec = 0;

			int32_t count = kevent(_socket, nullptr, 0, events, TINY_TOOLKIT_NET_COUNT, &timeout);

			if (count == -1)
			{
				if (errno != EINTR)
				{
					TINY_TOOLKIT_ASSERT(false, OS::LastErrorMessage())

					return;
				}

				continue;
			}

			for (int32_t i = 0; i < count; ++i)
			{
				auto * eventValue = (NetEvent *)events[i].udata;

				if (eventValue && eventValue->_callback)
				{
					eventValue->_callback(eventValue, &events[i]);
				}
			}
		}

#else

		static struct epoll_event events[TINY_TOOLKIT_NET_COUNT]{ };

		while (_status)
		{
			int32_t count = epoll_wait(_socket, events, TINY_TOOLKIT_NET_COUNT, 10);

			if (count == -1)
			{
				if (errno != EINTR)
				{
					TINY_TOOLKIT_ASSERT(false, OS::LastErrorMessage())

					return;
				}

				continue;
			}

			for (int32_t i = 0; i < count; ++i)
			{
				auto * eventValue = (NetEvent *)events[i].data.ptr;

				if (eventValue && eventValue->_callback)
				{
					eventValue->_callback(eventValue, &events[i]);
				}
			}
		}

#endif
	}
}
