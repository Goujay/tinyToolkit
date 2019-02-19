/**
 *
 *  作者: hm
 *
 *  说明: 通讯服务器
 *
 */


#include "server.h"
#include "manager.h"


namespace tinyToolkit
{
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
	bool ITCPServer::Launch(const std::string & host, uint16_t port)
	{
		return NetWorkManager::Instance().LaunchTCPServer(this, host, port);
	}

	/**
	 *
	 * 关闭会话
	 *
	 */
	void ITCPServer::Close()
	{
		if (_pipe)
		{
			_pipe->Close();
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////


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
	bool IUDPServer::Launch(const std::string & host, uint16_t port)
	{
		return NetWorkManager::Instance().LaunchUDPServer(this, host, port);
	}

	/**
	 *
	 * 关闭会话
	 *
	 */
	void IUDPServer::Close()
	{
		if (_pipe)
		{
			_pipe->Close();
		}
	}
}
