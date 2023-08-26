cmake_minimum_required(VERSION 3.14)

project(cleanUpTry LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(cleanUpTry
  main.cpp
  functions/DiskIO.cpp
  functions/DB_CRUD/create.cpp
  functions/DB_CRUD/read.cpp
  functions/DBfiles_utils.cpp
  functions/general_utils.cpp
  tests/insertTest.cpp
  extension_implementation/core_BTree/gist_btree.cpp extension_implementation/core_BTree/gist_btree.h
  extension_implementation/btree_ext.cpp
  headers/macros.hpp
  headers/configs.hpp
  headers/includes.hpp
  core_GiST/gist.cpp core_GiST/gist.h core_GiST/gist_compat.h core_GiST/gist_cursor.cpp core_GiST/gist_cursor.h core_GiST/gist_cursorext.cpp core_GiST/gist_cursorext.h core_GiST/gist_defs.h core_GiST/gist_ext.h core_GiST/gist_file.cpp core_GiST/gist_file.h core_GiST/gist_htab.cpp core_GiST/gist_htab.h core_GiST/gist_p.cpp core_GiST/gist_p.h core_GiST/gist_query.cpp core_GiST/gist_query.h core_GiST/gist_ustk.cpp core_GiST/gist_ustk.h core_GiST/vec_t.cpp core_GiST/vec_t.h
  headers/types.hpp
  headers/part_class.hpp
  headers/include_handler.hpp
  functions/part_class.cpp
  #core_GiST/gist_support.cpp core_GiST/gist_support.h

)
target_link_libraries(cleanUpTry Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS cleanUpTry
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)