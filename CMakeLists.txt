cmake_minimum_required(VERSION 3.16)
project(EngineeringSupporterTools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Quick Qml Sql Network REQUIRED)

# Keep legacy QML resource prefix to avoid changing qrc paths (suppresses QTP0001 warning)
qt_policy(SET QTP0001 OLD)

set(TARGET_NAME EngineeringSupporter)
set(PROJECT_VERSION "1.0.0")

# バージョンをC++から参照できるように定義
add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")

# インクルードディレクトリの設定
include_directories(include data network)

qt_add_executable(${TARGET_NAME}
    src/main.cpp
)

# macOSで.appバンドルとしてビルドすることを明示
set_target_properties(${TARGET_NAME} PROPERTIES 
    MACOS_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ターゲット定義の完了（これがないと一部の環境で.app構成がスキップされる）
qt_finalize_executable(${TARGET_NAME})

# QMLモジュールの追加
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

# アップデーターツールの追加
add_executable(Updater src/updater/main.cpp)
set_target_properties(Updater PROPERTIES OUTPUT_NAME "Updater")

target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Quick Qt6::Qml Qt6::Sql Qt6::Network)