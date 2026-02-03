cmake_minimum_required(VERSION 3.16)
project(EngineeringSupporterTools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Quick Qml Sql Network REQUIRED)

set(TARGET_NAME EngineeringSupporter)
set(PROJECT_VERSION "1.0.0")

# バージョンをC++から参照できるように定義
add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")

# インクルードディレクトリの設定
include_directories(include data network)

qt_add_executable(${TARGET_NAME}
    MACOS_BUNDLE
    src/main.cpp
)

qt_add_qml_module(${TARGET_NAME}
    URI EngineeringSupporterTools
    VERSION 1.0
    QML_FILES ui/main.qml
    SOURCES 
        src/Backend.cpp include/Backend.h
        data/SqlDatabase.cpp data/SqlDatabase.h
        data/KeyValueStore.cpp data/KeyValueStore.h
        network/GitHubUpdater.cpp network/GitHubUpdater.h
)

target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Quick Qt6::Qml Qt6::Sql Qt6::Network)