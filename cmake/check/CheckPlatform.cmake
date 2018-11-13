#
# 检测当前系统平台
#
IF(CMAKE_SIZEOF_VOID_P MATCHES 8)

	SET(PLATFORM 64)

	MESSAGE(STATUS "Detected ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR} 64-bit platform")

ELSE()

	SET(PLATFORM 32)

	MESSAGE(STATUS "Detected ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR} 32-bit platform")

ENDIF()


