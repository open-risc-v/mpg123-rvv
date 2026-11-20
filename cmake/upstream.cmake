# 上游接入：mpg123（autotools）。两侧各从自己的 SRC_DIR 编一份 prefix。
if(NOT DEFINED PKG_PREFIX)
  set(PKG_PREFIX "${CMAKE_BINARY_DIR}/pkg/prefix")
  add_custom_target(pkg ALL
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-build.sh" "${SRC_DIR}" "${CMAKE_BINARY_DIR}" "${RVV_MARCH}" "$<IF:$<BOOL:${RVV_KERNELS}>,1,0>"
    COMMENT "构建上游 mpg123（${SRC_DIR}）"
    VERBATIM)
  set(HAVE_PKG_TARGET TRUE)
endif()

add_executable(mpg123_harness test/pkg_mpg123.c)
if(HAVE_PKG_TARGET)
  add_dependencies(mpg123_harness pkg)
endif()
target_include_directories(mpg123_harness PRIVATE "${PKG_PREFIX}/include")
target_link_directories(mpg123_harness PRIVATE "${PKG_PREFIX}/lib")
target_link_libraries(mpg123_harness PRIVATE mpg123 m)
target_compile_options(mpg123_harness PRIVATE ${RVV_FLAGS})

add_test(NAME mpg123_harness COMMAND mpg123_harness)
