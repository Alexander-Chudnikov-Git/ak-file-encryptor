set(AK_GRAPHICS_LIB  "lib-ak-gui")
set(AK_PROCESSOR_LIB "lib-ak-prc")

add_library(${AK_GRAPHICS_LIB} STATIC ${AK_GRAPHICS_SRC})

target_include_directories(${AK_GRAPHICS_LIB} PRIVATE ${AK_ENCRYPTOR_INCLUDE_DIRS}/)
target_link_directories(${AK_GRAPHICS_LIB} PRIVATE ${AK_ENCRYPTOR_INCLUDE_DIRS}/)

add_library(${AK_PROCESSOR_LIB} STATIC ${AK_PROCESSOR_SRC})

target_include_directories(${AK_PROCESSOR_LIB} PRIVATE ${AK_ENCRYPTOR_INCLUDE_DIRS}/)
target_link_directories(${AK_PROCESSOR_LIB} PRIVATE ${AK_ENCRYPTOR_INCLUDE_DIRS}/)

find_library(AKRYPT_LIBRARY NAMES akrypt)

set(AK_ENCRYPTOR_LIBS
    ${AK_GRAPHICS_LIB}
    ${AK_PROCESSOR_LIB}
)

set(SYSTEM_ENCRYPTOR_LIBS
    ncurses
    pthread
    libakrypt-base.so
    libakrypt.so
)
