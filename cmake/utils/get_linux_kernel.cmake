execute_process(
    COMMAND cat /etc/os-release
    COMMAND grep -oP "^ID=\\K[a-zA-Z0-9]*"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE OS_RELEASE_RESULT
    OUTPUT_VARIABLE CMAKE_SYSTEM_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(OS_RELEASE_RESULT)
    message(WARNING "Unknown kernel ID, /etc/os-release not found.")
    set(CMAKE_SYSTEM_ID "unknown")
endif()

message(STATUS "Kernel Name:       ${CMAKE_SYSTEM_NAME}")
message(STATUS "Kernel Version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Kernel ID:         ${CMAKE_SYSTEM_ID}")
message(STATUS "Processor:         ${CMAKE_SYSTEM_PROCESSOR}")

if("${CMAKE_SYSTEM_ID}" STREQUAL "ubuntu")
    message(FATAL_ERROR "Don't you fucking dare to run my code on this garbage.")
endif()
