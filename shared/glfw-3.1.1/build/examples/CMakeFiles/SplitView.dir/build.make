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
include examples/CMakeFiles/SplitView.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/SplitView.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/SplitView.dir/flags.make

examples/CMakeFiles/SplitView.dir/splitview.c.o: examples/CMakeFiles/SplitView.dir/flags.make
examples/CMakeFiles/SplitView.dir/splitview.c.o: ../examples/splitview.c
	$(CMAKE_COMMAND) -E cmake_progress_report "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/CMakeFiles" $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object examples/CMakeFiles/SplitView.dir/splitview.c.o"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/SplitView.dir/splitview.c.o   -c "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/examples/splitview.c"

examples/CMakeFiles/SplitView.dir/splitview.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SplitView.dir/splitview.c.i"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/examples/splitview.c" > CMakeFiles/SplitView.dir/splitview.c.i

examples/CMakeFiles/SplitView.dir/splitview.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SplitView.dir/splitview.c.s"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/examples/splitview.c" -o CMakeFiles/SplitView.dir/splitview.c.s

examples/CMakeFiles/SplitView.dir/splitview.c.o.requires:
.PHONY : examples/CMakeFiles/SplitView.dir/splitview.c.o.requires

examples/CMakeFiles/SplitView.dir/splitview.c.o.provides: examples/CMakeFiles/SplitView.dir/splitview.c.o.requires
	$(MAKE) -f examples/CMakeFiles/SplitView.dir/build.make examples/CMakeFiles/SplitView.dir/splitview.c.o.provides.build
.PHONY : examples/CMakeFiles/SplitView.dir/splitview.c.o.provides

examples/CMakeFiles/SplitView.dir/splitview.c.o.provides.build: examples/CMakeFiles/SplitView.dir/splitview.c.o

# Object files for target SplitView
SplitView_OBJECTS = \
"CMakeFiles/SplitView.dir/splitview.c.o"

# External object files for target SplitView
SplitView_EXTERNAL_OBJECTS =

examples/SplitView.app/Contents/MacOS/SplitView: examples/CMakeFiles/SplitView.dir/splitview.c.o
examples/SplitView.app/Contents/MacOS/SplitView: examples/CMakeFiles/SplitView.dir/build.make
examples/SplitView.app/Contents/MacOS/SplitView: src/libglfw3.a
examples/SplitView.app/Contents/MacOS/SplitView: examples/CMakeFiles/SplitView.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable SplitView.app/Contents/MacOS/SplitView"
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SplitView.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/SplitView.dir/build: examples/SplitView.app/Contents/MacOS/SplitView
.PHONY : examples/CMakeFiles/SplitView.dir/build

examples/CMakeFiles/SplitView.dir/requires: examples/CMakeFiles/SplitView.dir/splitview.c.o.requires
.PHONY : examples/CMakeFiles/SplitView.dir/requires

examples/CMakeFiles/SplitView.dir/clean:
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" && $(CMAKE_COMMAND) -P CMakeFiles/SplitView.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/SplitView.dir/clean

examples/CMakeFiles/SplitView.dir/depend:
	cd "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/examples" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples" "/Users/szhao/Documents/school & self_study/cs488/spring2016/cs488/shared/glfw-3.1.1/build/examples/CMakeFiles/SplitView.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : examples/CMakeFiles/SplitView.dir/depend

