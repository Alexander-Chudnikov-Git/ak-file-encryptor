message(STATUS "CXX compiler:      ${CMAKE_CXX_COMPILER_ID}")

if(NOT CMAKE_RELEASE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# [SOURCE DIRECTORIES]
set(AK_ENCRYPTOR_SRC_DIR       "${PROJECT_SOURCE_DIR}/src")
set(AK_GRAPHICS_SRC_DIR        "${AK_ENCRYPTOR_SRC_DIR}/gui")
set(AK_PROCESSOR_SRC_DIR       "${AK_ENCRYPTOR_SRC_DIR}/processor")
set(AK_LOGGER_SRC_DIR          "${AK_ENCRYPTOR_SRC_DIR}/log")

# [INCLUDE DIRECTORIES]
set(AK_ENCRYPTOR_INCLUDE_DIRS
    ${AK_ENCRYPTOR_SRC_DIR}
    ${AK_GRAPHICS_SRC_DIR}
    ${AK_PROCESSOR_SRC_DIR}
)

# [SOURCE FILES]
file(GLOB AK_ENCRYPTOR_SRC CONFIGURE_DEPENDS
    "${AK_ENCRYPTOR_SRC_DIR}/*.hpp"
    "${AK_ENCRYPTOR_SRC_DIR}/*.cpp"
)

file(GLOB AK_GRAPHICS_SRC CONFIGURE_DEPENDS
    "${AK_GRAPHICS_SRC_DIR}/*.hpp"
    "${AK_GRAPHICS_SRC_DIR}/*.cpp"
)

file(GLOB AK_PROCESSOR_SRC CONFIGURE_DEPENDS
    "${AK_PROCESSOR_SRC_DIR}/*.hpp"
    "${AK_PROCESSOR_SRC_DIR}/*.cpp"
)

file(GLOB AK_LOGGER_SRC CONFIGURE_DEPENDS
    "${AK_LOGGER_SRC_DIR}/*.hpp"
    "${AK_LOGGER_SRC_DIR}/*.cpp"
)

if(MACOS)
    #include(cmake/platform/macos_builder.cmake)
elseif(LINUX)
    include(cmake/platform/linux_builder.cmake)
elseif(WIN32)
    #include(cmake/platform/windows_builder.cmake)
else()
    message(WARNING "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
    include(cmake/platform/linux_builder.cmake)
endif()

message(${AK_GRAPHICS_SRC})

# [SOURCE GROUPS]
source_group("Encryptor Base"  FILES ${AK_ENCRYPTOR_SRC})
source_group("User Interface"  FILES ${AK_GRAPHICS_SRC})
source_group("Processing"      FILES ${AK_PROCESSOR_SRC})
source_group("Log Handler"     FILES ${AK_LOGGER_SRC})

if(CMAKE_RELEASE AND CMAKE_UPX_COMPRESS)
	message(STATUS "Stripping with ${CMAKE_STRIP}")
	message(STATUS "Applying UPX compression")

	if(APPLE)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}> && upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
	else()
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${PROJECT_NAME}> && upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
	endif()

elseif(CMAKE_RELEASE)
	message(STATUS "Stripping with ${CMAKE_STRIP}")

	if(APPLE)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
	else()
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${PROJECT_NAME}>)
	endif()

elseif(CMAKE_UPX_COMPRESS)
	message(STATUS "Applying UPX compression")

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
endif()
