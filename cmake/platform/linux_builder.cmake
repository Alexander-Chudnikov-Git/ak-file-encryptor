include(cmake/utils/get_linux_kernel.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APPLICATION_NAME "ak-file-encryptor.desktop")
set(APPLICATION_PATH ${PROJECT_SOURCE_DIR}/resources/${APPLICATION_NAME})

set(ICON_FOLDER ${PROJECT_SOURCE_DIR}/resources/icons/linux/)

add_compile_options(
    -fvisibility=hidden
    -pedantic
    -Wall
    -Wextra
    -Wcast-align
    -Wcast-qual
    -Wctor-dtor-privacy
    -Wdisabled-optimization
    -Wformat=2
    -Winit-self
    -Wlogical-op
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wnoexcept
    -Woverloaded-virtual
    -Wredundant-decls
    -Wshadow
    -Wsign-promo
    -Wstrict-null-sentinel
    -Wstrict-overflow=5
    -Wswitch-default
    -Wundef
    -Wno-unused-variable
    -Wno-error=redundant-decls
    -Ofast
)

include(cmake/platform/library_build.cmake)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${AK_ENCRYPTOR_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${AK_ENCRYPTOR_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${AK_ENCRYPTOR_LIBS} ${SYSTEM_ENCRYPTOR_LIBS} )

# Install desktop file and icons
install(FILES ${APPLICATION_PATH} DESTINATION /usr/share/applications)
install(DIRECTORY ${ICON_FOLDER} DESTINATION /usr/share/icons/hicolor)

# Install the executable
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
