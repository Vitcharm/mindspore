include(CMakePackageConfigHelpers)

set(LIB_DIR ${MAIN_DIR}-${COMPONENT_NAME}/lib)
set(INC_DIR ${MAIN_DIR}-${COMPONENT_NAME}/include)
set(TURBO_DIR ${MAIN_DIR}-${COMPONENT_NAME}/third_party/libjpeg-turbo)
set(OPENCV_DIR ${MAIN_DIR}-${COMPONENT_NAME}/third_party/opencv)
set(PROTOBF_DIR ${MAIN_DIR}-${COMPONENT_NAME}/third_party/protobuf)
set(FLATBF_DIR ${MAIN_DIR}-${COMPONENT_NAME}/third_party/flatbuffers)

set(LIB_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/lib)
set(INC_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/include)
set(TURBO_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/third_party/libjpeg-turbo)
set(OPENCV_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/third_party/opencv)
set(PROTOBF_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/third_party/protobuf)
set(FLATBF_DIR_RUN_X86 ${MAIN_DIR}-${RUN_X86_COMPONENT_NAME}/third_party/flatbuffers)

if (BUILD_MINDDATA STREQUAL "lite")
    install(DIRECTORY ${TOP_DIR}/mindspore/ccsrc/minddata/dataset/include/ DESTINATION ${INC_DIR} COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    if (PLATFORM_ARM64)
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libjpeg.so DESTINATION ${TURBO_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libturbojpeg.so DESTINATION ${TURBO_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/arm64-v8a/libopencv_core.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/arm64-v8a/libopencv_imgcodecs.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/arm64-v8a/libopencv_imgproc.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
    elseif (PLATFORM_ARM32)
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libjpeg.so DESTINATION ${TURBO_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libturbojpeg.so DESTINATION ${TURBO_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/armeabi-v7a/libopencv_core.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/armeabi-v7a/libopencv_imgcodecs.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/armeabi-v7a/libopencv_imgproc.so DESTINATION ${OPENCV_DIR}/lib COMPONENT ${COMPONENT_NAME})
    else ()
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR_RUN_X86} COMPONENT ${RUN_X86_COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libjpeg.so.62.3.0 DESTINATION ${TURBO_DIR_RUN_X86}/lib RENAME libjpeg.so.62 COMPONENT ${RUN_X86_COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/libjpeg-turbo/lib/libturbojpeg.so.0.2.0 DESTINATION ${TURBO_DIR_RUN_X86}/lib RENAME libturbojpeg.so.0 COMPONENT ${RUN_X86_COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/libopencv_core.so.4.2.0 DESTINATION ${OPENCV_DIR_RUN_X86}/lib RENAME libopencv_core.so.4.2 COMPONENT ${RUN_X86_COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/libopencv_imgcodecs.so.4.2.0 DESTINATION ${OPENCV_DIR_RUN_X86}/lib RENAME libopencv_imgcodecs.so.4.2 COMPONENT ${RUN_X86_COMPONENT_NAME})
        install(FILES ${TOP_DIR}/third_party/opencv/build/lib/libopencv_imgproc.so.4.2.0 DESTINATION ${OPENCV_DIR_RUN_X86}/lib RENAME libopencv_imgproc.so.4.2 COMPONENT ${RUN_X86_COMPONENT_NAME})
    endif ()
endif ()


if (BUILD_MINDDATA STREQUAL "lite_cv")
    install(DIRECTORY ${TOP_DIR}/mindspore/ccsrc/minddata/dataset/kernels/image/lite_cv DESTINATION ${INC_DIR} COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    if (PLATFORM_ARM64)
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
    elseif (PLATFORM_ARM32)
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
    else ()
        install(FILES ${TOP_DIR}/mindspore/lite/build/minddata/libminddata-lite.so DESTINATION ${LIB_DIR_RUN_X86} COMPONENT ${RUN_X86_COMPONENT_NAME})
    endif ()
endif ()

if (PLATFORM_ARM64)
    install(FILES ${TOP_DIR}/mindspore/lite/build/src/libmindspore-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
    install(FILES ${TOP_DIR}/mindspore/core/ir/dtype/type_id.h DESTINATION ${INC_DIR}/ir/dtype COMPONENT ${COMPONENT_NAME})
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/include/ DESTINATION ${INC_DIR} COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/schema/ DESTINATION ${INC_DIR}/schema COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN "inner" EXCLUDE)
    install(FILES ${TOP_DIR}/mindspore/lite/build/nnacl/liboptimize.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
    install(DIRECTORY ${TOP_DIR}/third_party/flatbuffers/include DESTINATION ${FLATBF_DIR} COMPONENT ${COMPONENT_NAME})
elseif (PLATFORM_ARM32)
    install(FILES ${TOP_DIR}/mindspore/lite/build/src/libmindspore-lite.so DESTINATION ${LIB_DIR} COMPONENT ${COMPONENT_NAME})
    install(FILES ${TOP_DIR}/mindspore/core/ir/dtype/type_id.h DESTINATION ${INC_DIR}/ir/dtype COMPONENT ${COMPONENT_NAME})
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/include/ DESTINATION ${INC_DIR} COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/schema/ DESTINATION ${INC_DIR}/schema COMPONENT ${COMPONENT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN "inner" EXCLUDE)
    install(DIRECTORY ${TOP_DIR}/third_party/flatbuffers/include DESTINATION ${FLATBF_DIR} COMPONENT ${COMPONENT_NAME})
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    get_filename_component(CXX_DIR ${CMAKE_CXX_COMPILER} PATH)
    file(GLOB LIB_LIST ${CXX_DIR}/libstdc++-6.dll ${CXX_DIR}/libwinpthread-1.dll ${CXX_DIR}/libssp-0.dll ${CXX_DIR}/libgcc_s_seh-1.dll)
    install(FILES ${TOP_DIR}/build/mindspore/tools/converter/converter_lite.exe DESTINATION ${TOP_DIR}/build/mindspore/package COMPONENT ${COMPONENT_NAME})
    install(FILES ${LIB_LIST} DESTINATION ${TOP_DIR}/build/mindspore/package COMPONENT ${COMPONENT_NAME})
    install(FILES ${TOP_DIR}/build/mindspore/tools/converter/libconverter_parser.a DESTINATION ${TOP_DIR}/build/mindspore/package COMPONENT ${PARSER_NAME})
else ()
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/include/ DESTINATION ${INC_DIR_RUN_X86} COMPONENT ${RUN_X86_COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    install(DIRECTORY ${TOP_DIR}/mindspore/lite/schema/ DESTINATION ${INC_DIR_RUN_X86}/schema COMPONENT ${RUN_X86_COMPONENT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN "inner" EXCLUDE)
    install(FILES ${TOP_DIR}/mindspore/core/ir/dtype/type_id.h DESTINATION ${INC_DIR_RUN_X86}/ir/dtype COMPONENT ${RUN_X86_COMPONENT_NAME})
    install(DIRECTORY ${TOP_DIR}/third_party/flatbuffers/include DESTINATION ${FLATBF_DIR_RUN_X86} COMPONENT ${RUN_X86_COMPONENT_NAME})
    install(FILES ${TOP_DIR}/mindspore/lite/build/src/libmindspore-lite.so DESTINATION ${LIB_DIR_RUN_X86} COMPONENT ${RUN_X86_COMPONENT_NAME})

    install(FILES ${TOP_DIR}/third_party/protobuf/build/lib/libprotobuf.so.19.0.0 DESTINATION ${PROTOBF_DIR}/lib RENAME libprotobuf.so.19 COMPONENT ${COMPONENT_NAME})
    install(FILES ${TOP_DIR}/third_party/flatbuffers/build/libflatbuffers.so.1.11.0 DESTINATION ${FLATBF_DIR}/lib RENAME libflatbuffers.so.1 COMPONENT ${COMPONENT_NAME})
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR ZIP)
else ()
    set(CPACK_GENERATOR TGZ)
endif ()
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
if (PLATFORM_ARM64 OR PLATFORM_ARM32)
    set(CPACK_COMPONENTS_ALL ${COMPONENT_NAME})
elseif (WIN32)
    set(CPACK_COMPONENTS_ALL ${COMPONENT_NAME})
else ()
    set(CPACK_COMPONENTS_ALL ${COMPONENT_NAME} ${RUN_X86_COMPONENT_NAME})
endif ()
set(CPACK_PACKAGE_FILE_NAME ${MAIN_DIR})
if (WIN32)
    set(CPACK_PACKAGE_DIRECTORY ${TOP_DIR}/output)
else ()
    set(CPACK_PACKAGE_DIRECTORY ${TOP_DIR}/output/tmp)
endif()
set(CPACK_PACKAGE_CHECKSUM SHA256)
include(CPack)
