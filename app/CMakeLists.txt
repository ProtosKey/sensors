cmake_minimum_required(VERSION 3.16)
project(sensors-widget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 6.6: Shape.CurveRenderer, which the icons use
find_package(Qt6 6.6 REQUIRED COMPONENTS Core Gui Network Qml Quick)

add_executable(sensors-widget
    src/main.cpp
    src/SensorsClient.cpp
    src/SensorsClient.h
)

# The whole UI is QML. It is embedded into the binary, so the installed
# program does not depend on the source tree.
file(GLOB QML_FILES CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    components/*
    cards/*.qml
)
qt_add_resources(sensors-widget qml
    PREFIX "/"
    FILES main.qml ${QML_FILES}
)

target_link_libraries(sensors-widget PRIVATE
    Qt6::Core Qt6::Gui Qt6::Network Qt6::Qml Qt6::Quick
)
target_compile_options(sensors-widget PRIVATE -Wall -Wextra)

install(TARGETS sensors-widget RUNTIME DESTINATION bin)
