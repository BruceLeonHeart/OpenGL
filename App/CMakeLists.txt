# 将源代码添加到此项目的可执行文件。
add_executable (App "App.cpp" "App.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET App PROPERTY CXX_STANDARD 20)
endif()