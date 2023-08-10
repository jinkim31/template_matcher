cmake_minimum_required(VERSION 3.12)
project(lightlink_view)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(lib/lightlink)
add_subdirectory(lib/event_thread)

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
        model/master.cpp
        model/model.cpp
        model/slave.cpp
        model/master_thread_worker.cpp
        view/device_navigator.cpp
        view/device_view.cpp
        util.cpp
        )

target_link_libraries(lightlink_view PRIVATE
        lightlink
        event_thread
        )
target_include_directories(lightlink_view PRIVATE
        lib/IconFontCppHeaders)