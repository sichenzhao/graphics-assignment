# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.2.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.2.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build"

# Include any dependencies generated for this target.
include tests/CMakeFiles/glfwinfo.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/glfwinfo.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/glfwinfo.dir/flags.make

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o: tests/CMakeFiles/glfwinfo.dir/flags.make
tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o: ../tests/glfwinfo.c
	$(CMAKE_COMMAND) -E cmake_progress_report "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/CMakeFiles" $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/glfwinfo.dir/glfwinfo.c.o   -c "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/tests/glfwinfo.c"

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/glfwinfo.dir/glfwinfo.c.i"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/tests/glfwinfo.c" > CMakeFiles/glfwinfo.dir/glfwinfo.c.i

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/glfwinfo.dir/glfwinfo.c.s"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/tests/glfwinfo.c" -o CMakeFiles/glfwinfo.dir/glfwinfo.c.s

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.requires:
.PHONY : tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.requires

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.provides: tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.requires
	$(MAKE) -f tests/CMakeFiles/glfwinfo.dir/build.make tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.provides.build
.PHONY : tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.provides

tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.provides.build: tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o: tests/CMakeFiles/glfwinfo.dir/flags.make
tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o: ../deps/getopt.c
	$(CMAKE_COMMAND) -E cmake_progress_report "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/CMakeFiles" $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o   -c "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/deps/getopt.c"

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/glfwinfo.dir/__/deps/getopt.c.i"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/deps/getopt.c" > CMakeFiles/glfwinfo.dir/__/deps/getopt.c.i

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/glfwinfo.dir/__/deps/getopt.c.s"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/deps/getopt.c" -o CMakeFiles/glfwinfo.dir/__/deps/getopt.c.s

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.requires:
.PHONY : tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.requires

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.provides: tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.requires
	$(MAKE) -f tests/CMakeFiles/glfwinfo.dir/build.make tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.provides.build
.PHONY : tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.provides

tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.provides.build: tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o

# Object files for target glfwinfo
glfwinfo_OBJECTS = \
"CMakeFiles/glfwinfo.dir/glfwinfo.c.o" \
"CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o"

# External object files for target glfwinfo
glfwinfo_EXTERNAL_OBJECTS =

tests/glfwinfo: tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o
tests/glfwinfo: tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o
tests/glfwinfo: tests/CMakeFiles/glfwinfo.dir/build.make
tests/glfwinfo: src/libglfw3.a
tests/glfwinfo: tests/CMakeFiles/glfwinfo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable glfwinfo"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glfwinfo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/glfwinfo.dir/build: tests/glfwinfo
.PHONY : tests/CMakeFiles/glfwinfo.dir/build

tests/CMakeFiles/glfwinfo.dir/requires: tests/CMakeFiles/glfwinfo.dir/glfwinfo.c.o.requires
tests/CMakeFiles/glfwinfo.dir/requires: tests/CMakeFiles/glfwinfo.dir/__/deps/getopt.c.o.requires
.PHONY : tests/CMakeFiles/glfwinfo.dir/requires

tests/CMakeFiles/glfwinfo.dir/clean:
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" && $(CMAKE_COMMAND) -P CMakeFiles/glfwinfo.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/glfwinfo.dir/clean

tests/CMakeFiles/glfwinfo.dir/depend:
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/tests" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/tests/CMakeFiles/glfwinfo.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : tests/CMakeFiles/glfwinfo.dir/depend

