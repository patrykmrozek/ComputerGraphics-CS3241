cmake_minimum_required(VERSION 3.16)

project(tut1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)


add_executable(tut1 main.cpp)

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)


if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        "-framework OpenGL"
        "-framework GLUT"
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GL_SILENCE_DEPRECATION
    )
endif()


include(GNUInstallDirs)
install(TARGETS tut1
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
