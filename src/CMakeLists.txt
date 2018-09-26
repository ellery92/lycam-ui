# dir: src
file(GLOB lycam-ui-SRCS
  "*.cc"
  "lycam.qrc")

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Instruct CMake to run rcc automatically when needed
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5UiTools CONFIG REQUIRED)

include_directories(Qt5Widgets_INCLUDE_DIRS)
include_directories(Qt5Uitools_INCLUDE_DIRS)

add_executable(lycam-ui ${lycam-ui-SRCS})

if(WIN32)
  set(LYCAM_INCLUDE_DIRS "d:/linux-root/lib/lycam-c/include"
    CACHE PATH "path to lycam-c include directory")
  set(LYCAM_LIB_DIRS "d:/linux-root/lib/lycam-c/lib/"
    CACHE PATH "path to lycam-c lib path")
  target_include_directories(lycam-ui PRIVATE ${LYCAM_INCLUDE_DIRS})
  find_library(lycam-c lycam-c ${LYCAM_LIB_DIRS})
else()
  find_library(lycam-c
    NAMES lycam-c
    PATH "/usr/local/lib")
endif()

target_link_libraries(lycam-ui Qt5::Widgets Qt5::UiTools)
target_link_libraries(lycam-ui ${lycam-c})
