#ifndef __TINY_TOOLKIT__UTILITIES__NET__H__
#define __TINY_TOOLKIT__UTILITIES__NET__H__


/**
 *
 *  作者: hm
 *
 *  说明: 网络处理
 *
 */


#include "../common/common.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API Net
	{
	public:
		/**
		 *
		 * 转换网络字节序
		 *
		 * @param value 待转换字符串
		 *
		 * @return 网络字节序
		 *
		 */
		static uint32_t AsNetByte(const char * value);

		/**
		 *
		 * 转换主机字节序
		 *
		 * @param value 待转换字符串
		 *
		 * @return 主机字节序
		 *
		 */
		static uint32_t AsHostByte(const char * value);

		/**
		 *
		 * 转换网络字节序范围
		 *
		 * @param value 待转换字符串
		 * @param head 转换后字节序范围首部
		 * @param tail 转换后字节序范围尾部
		 *
		 * @return 是否转换成功
		 *
		 */
		static bool AsNetByte(const std::string & value, uint32_t & head, uint32_t & tail);

		/**
		 *
		 * 转换主机字节序范围
		 *
		 * @param value 待转换字符串
		 * @param head 转换后字节序范围首部
		 * @param tail 转换后字节序范围尾部
		 *
		 * @return 是否转换成功
		 *
		 */
		static bool AsHostByte(const std::string & value, uint32_t & head, uint32_t & tail);

		/**
		 *
		 * 转换为字符串
		 *
		 * @param value 待转换主机字节序
		 *
		 * @return 字符串
		 *
		 */
		static std::string AsString(uint32_t value);

		/**
		 *
		 * 解析域名
		 *
		 * @param host 待解析域名
		 *
		 * @return 解析后域名
		 *
		 */
		static std::string ParseHost(const char * host);
	};
}


#endif // __TINY_TOOLKIT__UTILITIES__NET__H__
