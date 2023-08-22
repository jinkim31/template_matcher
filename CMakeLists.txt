cmake_minimum_required(VERSION 3.12)
project(lightlink_view)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(lib/lightlink)
add_subdirectory(lib/event_thread)
add_subdirectory(lib/sync)
include_directories(lib/iconFontCppHeaders)
find_library(libnfd REQUIRED NAMES libnfd.a HINTS lib/nativefiledialog)
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        imgui_bundle
        GIT_REPOSITORY https://github.com/pthom/imgui_bundle.git
        GIT_PROGRESS TRUE
        GIT_TAG main
)
FetchContent_MakeAvailable(imgui_bundle)
list(APPEND CMAKE_MODULE_PATH ${IMGUIBUNDLE_CMAKE_PATH})
include(imgui_bundle_add_app)
set(IMGUI_BUNDLE_FETCH_OPENCV ON)
set(HELLOIMGUI_WIN32_NO_CONSOLE OFF)

imgui_bundle_add_app(lightlink_view
        main.cpp
        application.cpp
        model/model.cpp
        model/vision_worker.cpp
        view/view.cpp
        )

target_link_libraries(lightlink_view PRIVATE
        lightlink
        event_thread
        sync
        ${libnfd}
        )
target_include_directories(lightlink_view PRIVATE
        lib/IconFontCppHeaders
        lib/nativefiledialog)