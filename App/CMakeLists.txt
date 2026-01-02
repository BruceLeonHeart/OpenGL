# 将源代码添加到此项目的可执行文件。


include_directories(${GLEW_INCLUDE_DIRS})
add_executable (App "src/App.cpp" "src/App.h")

# find_package(glfw3 3.4) - 这里的 glfw3 是CMake查找的包名（package name）取决于cmake配置文件
# target_link_libraries(... glfw) - 这里的 glfw 是库的目标名（target name） 取决于源码cmake中target Name
target_link_directories(App PRIVATE ${GLEW_LIBS})
target_link_libraries(App PRIVATE OpenGL::GL glfw glew32s)

# 设置资源文件路径
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET App PROPERTY CXX_STANDARD 20)
endif()