cmake_minimum_required(VERSION 3.16)
project(AteneaUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

qt_add_executable(Atenea
    ${SOURCES}
    resources/resources.qrc 
)

# Agrega recursos QML (.qrc)
qt6_add_resources(Atenea qml_resources
    PREFIX "/qml"
    FILES
        qml/Main.qml
        qml/ChartView.qml
        qml/MapView.qml
)

# Linkea con bibliotecas Qt necesarias
target_link_libraries(Atenea PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)