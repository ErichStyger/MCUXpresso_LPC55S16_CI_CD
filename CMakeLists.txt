cmake_minimum_required(VERSION 3.1...3.27)

option(BUILD_SHARED_LIBS "Build using object libraries" 0)

# Required for McuLib, to know the target system
set(
  MCULIB_TARGET MCUXPRESSO CACHE STRING 
  "Select McuLib target: MCUXPRESSO, RP2040 or ESP32"
)

# turn variable on for verbose output, useful for build debugging. Or run 'ninja --verbose' in the build folder
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
# Path to toolchain file. This one has to be before 'project()' below
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

# Setup project name
project(LPC55S16_Blinky)

# set variables for directories
set(PROJECT_ROOT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")
set(MCULIB_DIR        "${PROJECT_ROOT_DIR}/McuLib")
set(EXECUTABLE         ${PROJECT_NAME}.elf)
set(SDK_DIR           "${PROJECT_ROOT_DIR}/sdk")
set(BUILD_DIR         "${PROJECT_ROOT_DIR}/build")
set(TEST_EXECUTABLE   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf")

set(LINKER_FILE "${SDK_DIR}/device/linker.ld")

enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build the executable based on the source files
add_executable(${EXECUTABLE} ${SRC_FILES})

# Check if ENABLE_UNIT_TESTING is set to ON in the CMake preset
if (ENABLE_UNIT_TESTING)
  message(STATUS "ENABLE_UNIT_TESTING is ON")
  add_compile_options(-DENABLE_UNIT_TESTS=1) # used to enable tests in code
else()
  message(STATUS "ENABLE_UNIT_TESTING is OFF")
  add_compile_options(-DENABLE_UNIT_TESTS=0) # used to disable tests in code
endif()

# Defines for the project
set(PROJECT_DEFINES
  -DCPU_LPC55S16JBD100
  -DCPU_LPC55S16JBD100_cm33
  -DSDK_OS_BAREMETAL
  -DSDK_DEBUGCONSOLE=1
  -DCR_INTEGER_PRINTF
  -DPRINTF_FLOAT_ENABLE=0
  -DSERIAL_PORT_TYPE_UART=1
  -D__MCUXPRESSO
  -D__USE_CMSIS
  -DDEBUG
  -D__NEWLIB__
)

# List of compiler defines, prefix with -D compiler option. Copy from the project settings
add_compile_options(
  ${PROJECT_DEFINES}
  -O0 
  -fno-common 
  -g3 
  -Wall 

  -ffunction-sections 
  -fdata-sections 

  -ffreestanding 
  -fno-builtin 
  -fmerge-constants 
  -mcpu=cortex-m4 
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard 
  -mthumb
)

target_compile_definitions(${EXECUTABLE} PRIVATE
  ${PROJECT_DEFINES}
)

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
  -L "${PROJECT_ROOT_DIR}/sdk/LPC55S16/mcuxpresso/" # needed to find libraries like libpower_hardabi.a
  -T "${LINKER_FILE}"
)

# McuLib to find our local header files
add_compile_options(-include "${PROJECT_ROOT_DIR}/src/IncludeMcuLibConfig.h")
add_compile_options(-I${PROJECT_ROOT_DIR}/src -I${PROJECT_ROOT_DIR}/sdk/drivers)
add_compile_options(-I${SDK_DIR}/device -I${SDK_DIR}/CMSIS -I${SDK_DIR}/board) 
if (ENABLE_UNIT_TESTING)
  add_compile_options(-I${MCULIB_DIR}/src)
endif()

add_subdirectory(./src                  src)
add_subdirectory(${SDK_DIR}             sdk)
add_subdirectory(${MCULIB_DIR}          McuLib)
add_subdirectory(${MCULIB_DIR}/rdimon   rdimon)

target_link_libraries(
  ${EXECUTABLE}
  PUBLIC srcLib
  PUBLIC sdkLib
  PUBLIC rdimonLib # file I/O with semihosting
)

# Optional: Print executable size as part of the post build process
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}
)

# Optional: Create hex, bin and S-Record files after the build
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.s19"
  COMMAND ${CMAKE_OBJCOPY} -O ihex "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex"
  COMMAND ${CMAKE_OBJCOPY} -O binary "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin" 
)

##################################################################################
# Unit Test support
if (ENABLE_UNIT_TESTING)
  # Adding some options to make CTest more verbose, helpful in case of failures.
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
  list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")

  # Include CTest support
  include(CTest) # note: this adds a BUILD_TESTING which defaults to ON, which can be used in other CMakeLists.txt

  # Just a check for the variable
  if (BUILD_TESTING)
    message(STATUS "BUILD_TESTING is ON")
  endif()

  message(STATUS "Adding unity")
  add_subdirectory(${MCULIB_DIR}/unity   unity)
endif()
