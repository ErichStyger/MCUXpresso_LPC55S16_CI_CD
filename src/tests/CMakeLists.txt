# file: Collect all files that need to be compiled. 
# You can use a GLOB function as shown here, or explicitly mention the specific files
#file(GLOB FILES *.c *.h)

# Need to include CTest on every CMakeLists.txt which is going to use tests
include(CTest)

set(THIS_LIBRARY_NAME srcTestsLib)

file(GLOB FILES
  *.c
)

# add_library: With this declaration, you express the intent to build a library. 
# The first argument is the name of the library, 
# the second argument are the files that will be compiled to create your library.
add_library(${THIS_LIBRARY_NAME} ${FILES})

# target_link_libraries: If you link with other libraries, list them here
target_link_libraries(
  ${THIS_LIBRARY_NAME}
  PRIVATE McuLib
  PRIVATE srcLib
  PRIVATE unityLib
)

# target_include_directories: Libraries need to publish their header files 
# so that you can import them in source code. This statement expresses where to find the files 
# - typically in an include directory of your projects.
target_include_directories(
  ${THIS_LIBRARY_NAME}
  PUBLIC
  .
)
#################################################################################
# using SEGGER J-Run
#################################################################################
set (RUNNER_CTEST_COMMAND "$ENV{SEGGER_PATH}/JRun" --verbose --device LPC55S16 --silent --rtt -if SWD)

set (passRegex "\\*\\* PASSED \\*\\*\\*")
set (failRegex "\\*\\*\\* FAILED \\*\\*\\*")
set_property(TEST PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")
set_property(TEST PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")

add_test(
  NAME Led_1
  COMMAND ${RUNNER_CTEST_COMMAND} --args "1" ${TEST_EXECUTABLE}
)

add_test(
  NAME Led_2
  COMMAND ${RUNNER_CTEST_COMMAND} --args "1" ${TEST_EXECUTABLE}
)

set_tests_properties(
  Led_1 Led_2
  PROPERTIES
  PASS_REGULAR_EXPRESSION "${passRegex}"
  FAIL_REGULAR_EXPRESSION "${failRegex}"
  TIMEOUT 15
)

# #################################################################################
# # using NXP LinkServer Runner
# #################################################################################
# #set (RUNNER_CTEST_MODE  --mode semihost)
# set (RUNNER_CTEST_MODE  --mode serial:COM70:115200)
# set (RUNNER_CTEST_CONTROL  --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***")
# set (RUNNER_CTEST_COMMAND $ENV{LINKSERVER_PATH}/LinkServer --log-level 1 run  ${RUNNER_CTEST_MODE} ${RUNNER_CTEST_CONTROL})
# set (RUNNER_CTEST_EXECUTABLE lpc55s16 ${TEST_EXECUTABLE})

# add_test(
#   NAME Led_1
#   COMMAND ${RUNNER_CTEST_COMMAND} --send "1" ${RUNNER_CTEST_EXECUTABLE}
# )

# add_test(
#   NAME Led_2
#   COMMAND ${RUNNER_CTEST_COMMAND} --send "2" ${RUNNER_CTEST_EXECUTABLE}
# )

# set_tests_properties(
#   Led_1 Led_2 
#   PROPERTIES TIMEOUT 15
# )
