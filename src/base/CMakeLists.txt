
set(SC_BASE_SOURCES
    sc_memmgr.cc
    sc_trace_fprintf.c
    sc_getopt.cc
    sc_benchmark.cc
)

if(MINGW OR MSVC OR BORLAND)
add_definitions( -DSC_BASE_DLL_EXPORTS )
endif()

if (${SC_MEMMGR_ENABLE_CHECKS})
	add_definitions( -DSC_MEMMGR_ENABLE_CHECKS )
endif()

SC_ADDLIB(base "${SC_BASE_SOURCES}" "")

if(MINGW OR MSVC OR BORLAND)
    target_link_libraries(base psapi.lib)
endif()

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/judy/src" )
    message( "Judy array source code not found. Downloading (don't sweat, it's public domain)..." )
    file( DOWNLOAD "http://github.com/mpictor/judy-template/archive/master.tar.gz"
          "${CMAKE_CURRENT_SOURCE_DIR}/judy.tar.gz" SHOW_PROGRESS )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/judy.tar.gz"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    )
    file( RENAME judy-template-master judy )
    file( REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/judy.tar.gz" )
    message( "Judy array source code extracted." )
endif(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/judy/src" )
