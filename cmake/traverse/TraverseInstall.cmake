#
# 安装目录头文件, TRAVERSE_INSTALL(source_dir, install_dir)
#
MACRO (TRAVERSE_INSTALL_INCLUDE)

	IF (NOT ${ARGC} EQUAL 2)

		MESSAGE(FATAL_ERROR "There is two and only two parameter")

	ENDIF ()

	FILE(GLOB_RECURSE install_list	"${ARGV0}/*.h"
									"${ARGV0}/*.hh"
									"${ARGV0}/*.hpp")

	INSTALL(PROGRAMS ${install_list} DESTINATION ${ARGV1})

ENDMACRO (TRAVERSE_INSTALL_INCLUDE)