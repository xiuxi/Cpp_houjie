# CMakeList.txt: MySTL 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("MySTL")

# 将源代码添加到此项目的可执行文件。
add_executable (MySTL "MySTL.cpp" "out/build/allocator.h" "out/build/construct.h" "源1.cpp" "out/build/alloc.h" "out/build/uninitialized.h" "out/build/type_traits.h" "out/build/iterator.h" "out/build/vector.h" "out/build/list.h" "out/build/deque.h" "out/build/stack.h" "out/build/queue.h" "out/build/heap_algo.h" "out/build/rb_tree.h" "out/build/set.h" "out/build/map.h" "out/build/hashtable.h" "out/build/unordered_map.h" "out/build/unordered_set.h" "out/build/numeric.h" "out/build/algobase.h" "out/build/set_algo.h" "out/build/algo.h" "out/build/functional.h" "out/build/astring.h" "out/build/basic_string.h" "out/build/exceptdef.h" "out/build/memory.h" "out/build/util.h")

# TODO: 如有需要，请添加测试并安装目标。
