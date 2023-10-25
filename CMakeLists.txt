cmake_minimum_required(VERSION 3.14)

project(GiSTProject LANGUAGES CXX)

#===========================================|Attention|===========================================|
#Modify this variable to set your Qt path! Do not change it directly in CMAKE_PREFIX_PATH
set(QT_SEARCH_PATH "/home/shahabseddigh/Qt/6.5.1/gcc_64/lib/cmake/Qt6")
#set(QT_SEARCH_PATH "/opt/Qt5.14.1/5.14.1/gcc_64/lib/cmake/Qt5")
#set(QT_SEARCH_PATH "/opt/Qt/6.4.0/gcc_64/lib/cmake/Qt6/")
#===========================================|Attention|===========================================|

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH ${QT_SEARCH_PATH})

#======================================|Check if required libraries exist|======================================|
if( NOT (EXISTS "${PROJECT_SOURCE_DIR}/thirdParty/absl/" AND EXISTS "${PROJECT_SOURCE_DIR}/thirdParty/re2/") )
	message(STATUS "thirdParty libraries not found inside ${PROJECT_SOURCE_DIR}/thirdParty/! Generating...")
	
	execute_process(COMMAND "chmod" "+x" "dependency-builder.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/thirdParty/"
			COMMAND_ECHO STDOUT)
			
	execute_process(COMMAND "${PROJECT_SOURCE_DIR}/thirdParty/dependency-builder.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/thirdParty/"
			COMMAND_ECHO STDOUT)
else()
	message(STATUS "thirdParty libraries found inside ${PROJECT_SOURCE_DIR}! proceeding...")
endif()
#======================================|Check if required libraries exist|======================================|

#For re2 (Relative path! Do not modify!!)
list(APPEND CMAKE_PREFIX_PATH "./thirdParty/absl" "./thirdParty/re2")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
#For re2
find_package(absl REQUIRED)
find_package(re2 REQUIRED)

add_executable(GiSTProject
  main.cpp
  functions/DiskIO.cpp
  functions/DB_CRUD/create.cpp
  functions/DB_CRUD/read.cpp
  functions/DBfiles_utils.cpp
  functions/general_utils.cpp
  tests/insertTest.cpp
  extension_implementation/core_BTree/gist_btree.cpp extension_implementation/core_BTree/gist_btree.h extension_implementation/btree_ext.cpp
  headers/macros.hpp
  headers/configs.hpp
  headers/includes.hpp
  core_GiST/gist.cpp core_GiST/gist.h core_GiST/gist_compat.h core_GiST/gist_cursor.cpp core_GiST/gist_cursor.h core_GiST/gist_cursorext.cpp core_GiST/gist_cursorext.h core_GiST/gist_defs.h core_GiST/gist_ext.h core_GiST/gist_file.cpp core_GiST/gist_file.h core_GiST/gist_htab.cpp core_GiST/gist_htab.h core_GiST/gist_p.cpp core_GiST/gist_p.h core_GiST/gist_query.cpp core_GiST/gist_query.h core_GiST/gist_ustk.cpp core_GiST/gist_ustk.h core_GiST/vec_t.cpp core_GiST/vec_t.h
  headers/types.hpp
  headers/part_class.hpp
  headers/include_handler.hpp
  functions/part_class.cpp
		functions/commMan.cpp
		headers/commMan.hpp
		headers/nfaQueryParser.hpp
		functions/nfaQueryParser.cpp
		headers/tcpInterfaceQT.hpp
		functions/tcpInterfaceQT.cpp
		#core_GiST/gist_support.cpp core_GiST/gist_support.h

)
target_link_libraries(GiSTProject Qt${QT_VERSION_MAJOR}::Core re2::re2 Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS GiSTProject
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
