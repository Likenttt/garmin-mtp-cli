cmake_minimum_required(VERSION 3.22)

project(garmin_mtp_cli C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(COMMON_SOURCES
    src/cli.c
    src/file_type.c
    src/manual.c
    src/memory.c
    src/mtp_tree.c
    src/path.c
    src/process_conflict.c)

set(APP_SOURCES
    src/commands.c
    src/main.c
    src/mtp_device.c
    src/progress.c)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(LIBMTP QUIET IMPORTED_TARGET libmtp)
endif()

if(TARGET PkgConfig::LIBMTP)
  set(LIBMTP_TARGET PkgConfig::LIBMTP)
else()
  find_path(LIBMTP_INCLUDE_DIR NAMES libmtp.h)
  find_library(LIBMTP_LIBRARY NAMES mtp libmtp)

  if(NOT LIBMTP_INCLUDE_DIR OR NOT LIBMTP_LIBRARY)
    message(FATAL_ERROR "libmtp was not found. Install libmtp, or configure with a vcpkg toolchain file.")
  endif()

  add_library(libmtp_imported UNKNOWN IMPORTED)
  set_target_properties(libmtp_imported PROPERTIES
      IMPORTED_LOCATION "${LIBMTP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBMTP_INCLUDE_DIR}")

  find_library(LIBUSB_LIBRARY NAMES usb-1.0 libusb-1.0)
  if(LIBUSB_LIBRARY)
    set_property(TARGET libmtp_imported APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${LIBUSB_LIBRARY}")
  endif()

  if(APPLE)
    find_library(IOKIT_FRAMEWORK IOKit)
    if(IOKIT_FRAMEWORK)
      set_property(TARGET libmtp_imported APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${IOKIT_FRAMEWORK}")
    endif()
  endif()

  set(LIBMTP_TARGET libmtp_imported)
endif()

add_executable(garmin-mtp ${COMMON_SOURCES} ${APP_SOURCES})
target_include_directories(garmin-mtp PRIVATE src)
target_link_libraries(garmin-mtp PRIVATE ${LIBMTP_TARGET})

add_custom_command(TARGET garmin-mtp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:garmin-mtp>
            $<TARGET_FILE_DIR:garmin-mtp>/garminmtp${CMAKE_EXECUTABLE_SUFFIX})

add_executable(test-garmin-mtp
    tests/cli_test.c
    tests/file_type_test.c
    tests/manual_test.c
    tests/mtp_tree_test.c
    tests/path_test.c
    tests/process_conflict_test.c
    tests/test_main.c
    ${COMMON_SOURCES})
target_include_directories(test-garmin-mtp PRIVATE src)
target_link_libraries(test-garmin-mtp PRIVATE ${LIBMTP_TARGET})

if(MSVC)
  target_compile_options(garmin-mtp PRIVATE /W4)
  target_compile_options(test-garmin-mtp PRIVATE /W4)
else()
  target_compile_options(garmin-mtp PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(test-garmin-mtp PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME unit COMMAND test-garmin-mtp)

install(TARGETS garmin-mtp RUNTIME DESTINATION bin)
install(PROGRAMS $<TARGET_FILE_DIR:garmin-mtp>/garminmtp${CMAKE_EXECUTABLE_SUFFIX}
        DESTINATION bin)
