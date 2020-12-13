cmake_minimum_required(VERSION 3.19)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(GCC_PREFIX "" CACHE PATH "Path to the root of the cross compiler (gcc)")

if (GCC_PREFIX STREQUAL "")
	message(FATAL_ERROR "GCC_PREFIX not set - required")
else ()
	# set(CMAKE_C_COMPILER ${GCC_PREFIX}/bin/arm-none-eabi-gcc)
	# set(CMAKE_CXX_COMPILER ${GCC_PREFIX}/bin/arm-none-eabi-g++)
	set(CMAKE_SIZE ${GCC_PREFIX}/bin/arm-none-eabi-size)
	SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs --plugin=$$(${CMAKE_C_COMPILER} --print-file-name=liblto_plugin.so) <TARGET> <LINK_FLAGS> <OBJECTS>")
	SET(CMAKE_C_ARCHIVE_FINISH   true)
	SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs --plugin=$$(${CMAKE_CXX_COMPILER} --print-file-name=liblto_plugin.so) <TARGET> <LINK_FLAGS> <OBJECTS>")
	SET(CMAKE_CXX_ARCHIVE_FINISH   true)
endif ()

set(ASF_PREFIX "" CACHE PATH "Path to the root of the Atmel Software Framework (ASF)")

if (ASF_PREFIX STREQUAL "")
	message(FATAL_ERROR "ASF_PREFIX not set - required")
endif ()

add_compile_definitions("__SAMD21J18A__")
list(APPEND CUSTOM_COMMON_FLAGS
	"-ffreestanding"
	"-ffunction-sections"
	"-fdata-sections"
	"-flto"
	"--specs=nosys.specs"
)
list(APPEND CUSTOM_CXX_FLAGS
	"-fno-exceptions"
	"-fno-rtti"
)

list(APPEND CUSTOM_ARCH_FLAGS
	"-mthumb"
	"-march=armv6-m"
	"-mcpu=cortex-m0plus"
	"-mfloat-abi=soft"
)
list(APPEND CUSTOM_WARNING_FLAGS
	"-Wall"
	"-Wextra"
	"-Wno-unused-parameter"
)

list(APPEND CUSTOM_COMPILER_FLAGS ${CUSTOM_ARCH_FLAGS} ${CUSTOM_WARNING_FLAGS} ${CUSTOM_COMMON_FLAGS})
string(REPLACE ";" " " CMAKE_C_FLAGS "${CUSTOM_COMPILER_FLAGS}")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CUSTOM_COMPILER_FLAGS} ${CUSTOM_CXX_FLAGS}")

project(samd21playground)

set(TARGET samd21playground)

add_library(samd21core STATIC 
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/source/system_samd21.c
	${ASF_PREFIX}/sam0/utils/syscalls/gcc/syscalls.c
)
target_include_directories(samd21core PUBLIC 
	${ASF_PREFIX}/thirdparty/CMSIS/Include 
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/source
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/include
)

add_executable(${TARGET} 
	main.cpp 
)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${TARGET} PRIVATE cxx_std_11)
target_link_options(${TARGET} PUBLIC "LINKER:--static,--script,${ASF_PREFIX}/sam0/utils/linker_scripts/samd21/gcc/samd21j18a_flash.ld,-Map,${TARGET}.map,--cref,--gc-sections")

add_subdirectory(samd21driver)

target_link_libraries(samd21driver PRIVATE samd21core)
target_link_libraries(${TARGET} PRIVATE samd21driver samd21core)

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_NM} --demangle --format=sysv --print-size ${TARGET} > ${TARGET}.nm)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} ${TARGET} > ${TARGET}.size)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} --format=sysv --radix=16 ${TARGET} >> ${TARGET}.size)
