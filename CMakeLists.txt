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
    src/common/ScanPanel.cpp

    # ── Core (business logic / state) ────────────────────────────────────
    src/core/SessionState.cpp
    src/core/DataModels.cpp
    src/core/BluetoothManager.cpp

    # ── Training Data ─────────────────────────────────────────────────────
    src/training/data/ReviewDataTypes.cpp

    # ── UI — Shared Widgets ───────────────────────────────────────────────
    src/ui/widgets/ShotGridWidget.cpp        # shared: TrainingScreen + ShotCountTab

    # ── UI — Main Shell ───────────────────────────────────────────────────
    src/ui/MainWindow.cpp
    src/ui/widgets/ConsoleWidget.cpp
    src/ui/widgets/ReviewScreen.cpp
    src/ui/widgets/BluetoothPanel.cpp
    src/ui/widgets/QRPanel.cpp
    src/ui/widgets/StepFlow.cpp
    src/ui/widgets/ButtonGrid.cpp
    src/ui/widgets/SummaryBox.cpp

    # ── UI — Session Review Screen (4 tabs) ───────────────────────────────
    src/ui/review/ReviewPanel.cpp
    src/ui/review/ShotCountTab.cpp
    src/ui/review/SessionSummaryTab.cpp
    src/ui/review/SessionAlbumTab.cpp
    src/ui/review/ShoQReviewTab.cpp
    src/ui/review/SessionReviewScreen.cpp

    # ── Training UI ───────────────────────────────────────────────────────
    src/training/ui/TrainingPlaceholder.cpp
    src/training/ui/TrainingScreen.cpp
    src/training/ui/CameraPanel.cpp
    src/training/ui/TrainingWidget.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    Qt5::Bluetooth
    Qt5::DBus
)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB TRAINING_SOURCES "src/training/engine/*.cpp")
list(REMOVE_ITEM TRAINING_SOURCES "${CMAKE_SOURCE_DIR}/src/training/engine/main.cpp")

target_sources(${PROJECT_NAME} PRIVATE
    ${TRAINING_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Widgets Qt5::Bluetooth Qt5::DBus
    ${OpenCV_LIBS}
    Eigen3::Eigen
)