# file: Collect all files that need to be compiled. 
# You can use a GLOB function as shown here, or explicitly mention the specific files
#file(GLOB FILES *.c *.h)

# Need to include CTest on every CMakeLists.txt which is going to use tests
include(CTest)

set(THIS_LIBRARY_NAME srcLib)

file(GLOB FILES
  *.c
)

if (ENABLE_UNIT_TESTING)
  message(STATUS "Adding src tests")
  add_subdirectory(./tests                  tests)
endif()

if(ENABLE_COVERAGE)
  # Set coverage option only for some files, otherwise use add_compile_options(--coverage)
  #add_compile_options(--coverage) # all files
  set_source_files_properties(
    leds.c
    shell.c
    main.c
    PROPERTIES COMPILE_FLAGS --coverage
  )
endif()

# add_library: With this declaration, you express the intent to build a library. 
# The first argument is the name of the library, 
# the second argument are the files that will be compiled to create your library.
add_library(${THIS_LIBRARY_NAME} STATIC ${FILES})

# target_link_libraries: If you link with other libraries, list them here
target_link_libraries(
  ${THIS_LIBRARY_NAME}
  PRIVATE McuLib
  PRIVATE sdkLib
  #PRIVATE gcovLib   # own gcov wrapper library
)

if (ENABLE_UNIT_TESTING)
  message(STATUS "Adding src tests library")
  target_link_libraries(
    ${THIS_LIBRARY_NAME}
    PUBLIC srcTestsLib
  )
endif()

# target_include_directories: Libraries need to publish their header files 
# so that you can import them in source code. This statement expresses where to find the files 
# - typically in an include directory of your projects.
target_include_directories(
  ${THIS_LIBRARY_NAME}
  PRIVATE 
  .
)
