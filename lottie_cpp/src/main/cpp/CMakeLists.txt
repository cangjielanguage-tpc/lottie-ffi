# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(lottie4cj)

# the minimum version of CMake.
#cmake_minimum_required(VERSION 3.5.0)
#project(lottie)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_SKIP_RPATH TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
                    ${CMAKE_CURRENT_SOURCE_DIR}/render/drawing
                    )

add_subdirectory(thirdparty)

add_library(lottie_cpp SHARED
                           cangjie/cj_unique_id.cpp
                           cangjie/animator.cpp
                           cangjie/cj_animator.cpp
                           cangjie/cj_lottie_controller.cpp
                           cangjie/init.cpp
                           cangjie/lottie_config_data_bridge.cpp
                           cangjie/lottie_controller_bridge.cpp
                           cangjie/lottie_data_config.cpp
                           cangjie/lottie_listener.cpp
                           cangjie/lottie_type.cpp
                           cangjie/lottie_utils.cpp
                           cangjie/lottie_xcomponent_node.cpp
                           task_pool/task_pool.cpp
                           datamanager/lottie_config_data.cpp
                           datamanager/data_parse.cpp
                           cache/file_cache.cpp
                           cache/generator_key.cpp
                           cache/memory_cache.cpp
                           controller/lottie_handler.cpp
                           datamanager/data_manager.cpp
                           render/lottie_render.cpp
                           render/lottie_render_layer.cpp
                           render/lottie_render_node.cpp
                           node/lottie_manager_map.cpp
                           render/drawing/brush.cpp
                           render/drawing/gly_path_bag.cpp
                           render/drawing/matrix.cpp
                           render/drawing/path.cpp
                           render/drawing/pen.cpp
                           render/drawing/rect.cpp
                           render/drawing/round_rect.cpp
                           render/drawing/shader_effect.cpp
                           render/drawing/color.cpp
                           render/drawing/canvas.cpp
                           render/drawing/bitmap.cpp
                           render/drawing/point.cpp
                           render/drawing/font_mgr.cpp
                           render/drawing/font.cpp
                           render/drawing/text_blob.cpp
                           render/drawing/draw_filter.cpp
                           render/drawing/text_typography.cpp
                           render/drawing/font_collection.cpp
                           )

target_link_libraries(lottie_cpp PUBLIC libace_napi.z.so
                                     libace_ndk.z.so
                                     rlottie::rlottie
                                     libhilog_ndk.z.so
                                     libnative_drawing.so
                                     libohresmgr.so
                                     librawfile.z.so
                                     librcp_c.so
                                     libohcrypto.so
                                     libnative_window.so
                                     libffrt.z.so
                                     boundscheck
                                     )

add_compile_definitions(LOTTIE_LOGGING_SUPPORT)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(lottie_cpp PUBLIC RELEASE_MODE=1)
    target_compile_options(lottie_cpp PUBLIC -fvisibility=hidden)
    target_link_options(lottie_cpp PUBLIC -s)
else()
    target_compile_definitions(lottie_cpp PUBLIC RELEASE_MODE=0)
    target_link_libraries(lottie_cpp PUBLIC libhitrace_ndk.z.so)
endif()
if("${OHOS_ARCH}" STREQUAL "arm64-v8a")
    target_link_directories(lottie_cpp PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/lib/aarch64-linux-ohos)
elseif("${OHOS_ARCH}" STREQUAL "armeabi-v7a")
    target_link_directories(lottie_cpp PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/lib/arm-linux-ohos)
elseif("${OHOS_ARCH}" STREQUAL "x86_64")
    target_link_directories(lottie_cpp PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/lib/x86_64-linux-ohos)
endif()

