cmake_minimum_required(VERSION 3.19)

set(ASF_PREFIX "" CACHE PATH "Path to the root of the Atmel Software Framework (ASF)")

if (ASF_PREFIX STREQUAL "")
	message(FATAL_ERROR "ASF_PREFIX not set - required")
endif ()

project(samd21-cmake-toolchain)

add_compile_definitions("__SAMD21J18A__")

include_directories(
	${ASF_PREFIX}/thirdparty/CMSIS/Include 
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/source
	${ASF_PREFIX}/sam0/utils/cmsis/samd21/include
)

set(TARGET samd21playground)

add_executable(${TARGET} 
main.cpp
${ASF_PREFIX}/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
${ASF_PREFIX}/sam0/utils/cmsis/samd21/source/system_samd21.c
${ASF_PREFIX}/sam0/utils/syscalls/gcc/syscalls.c
)

target_compile_features(${TARGET} PRIVATE cxx_std_11)

add_subdirectory(samd21driver)

# Without --gc-sections the linker complains about not finding __bss_start__
target_link_options(${TARGET} PUBLIC "LINKER:-static,--script,${ASF_PREFIX}/sam0/utils/linker_scripts/samd21/gcc/samd21j18a_flash.ld,-Map,${TARGET}.map,--cref,--gc-sections,--demangle")

target_link_libraries(${TARGET} PRIVATE samd21driver)

# Create Assembly file, print size and symbol table
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_NM} --demangle --format=sysv --print-size ${TARGET} > ${TARGET}.nm)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} ${TARGET} > ${TARGET}.size)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} --format=sysv --radix=16 ${TARGET} >> ${TARGET}.size)
add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_OBJDUMP} -h -S ${TARGET} >> ${TARGET}.S)