cmake_minimum_required(VERSION 3.16)
project(PulseStation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Bluetooth DBus)

add_executable(${PROJECT_NAME}
    main.cpp

    # ── Common (shared utilities / theme) ─────────────────────────────────
    src/common/CustomButton.cpp
    src/common/PillWidget.cpp
    src/common/SnackBar.cpp

    # ── Core (business logic / state) ────────────────────────────────────
    src/core/SessionState.cpp
    src/core/DataModels.cpp
    src/core/BluetoothManager.cpp

    # ── UI ────────────────────────────────────────────────────────────────
    src/ui/MainWindow.cpp
    src/ui/widgets/ConsoleWidget.cpp
    src/ui/widgets/ReviewScreen.cpp
    src/ui/widgets/TrainingPlaceholder.cpp
    src/ui/widgets/BluetoothPanel.cpp
    src/ui/widgets/QRPanel.cpp
    src/ui/widgets/StepFlow.cpp
    src/ui/widgets/ButtonGrid.cpp
    src/ui/widgets/SummaryBox.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    Qt5::Bluetooth
    Qt5::DBus
)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
