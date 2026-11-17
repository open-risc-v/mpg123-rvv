# 上游接入：mpg123（autotools，发行包直链）。交叉时上游已由 scripts/local-cross.sh
# 编到 PKG_PREFIX，探针只链接静态库 libmpg123.a。

add_executable(mpg123_harness test/pkg_mpg123.c)
target_include_directories(mpg123_harness PRIVATE "${PKG_PREFIX}/include")
target_link_directories(mpg123_harness PRIVATE "${PKG_PREFIX}/lib")
target_link_libraries(mpg123_harness PRIVATE mpg123 m)
target_compile_options(mpg123_harness PRIVATE ${RVV_FLAGS})

add_test(NAME mpg123_harness COMMAND mpg123_harness)
