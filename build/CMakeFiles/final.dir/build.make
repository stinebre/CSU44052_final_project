# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.30.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.30.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/estinebruner/projects/computer_graphics/final

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/estinebruner/projects/computer_graphics/final/build

# Include any dependencies generated for this target.
include CMakeFiles/final.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/final.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/final.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/final.dir/flags.make

CMakeFiles/final.dir/final/final.cpp.o: CMakeFiles/final.dir/flags.make
CMakeFiles/final.dir/final/final.cpp.o: /Users/estinebruner/projects/computer_graphics/final/final/final.cpp
CMakeFiles/final.dir/final/final.cpp.o: CMakeFiles/final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/estinebruner/projects/computer_graphics/final/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/final.dir/final/final.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/final.dir/final/final.cpp.o -MF CMakeFiles/final.dir/final/final.cpp.o.d -o CMakeFiles/final.dir/final/final.cpp.o -c /Users/estinebruner/projects/computer_graphics/final/final/final.cpp

CMakeFiles/final.dir/final/final.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/final.dir/final/final.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/estinebruner/projects/computer_graphics/final/final/final.cpp > CMakeFiles/final.dir/final/final.cpp.i

CMakeFiles/final.dir/final/final.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/final.dir/final/final.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/estinebruner/projects/computer_graphics/final/final/final.cpp -o CMakeFiles/final.dir/final/final.cpp.s

CMakeFiles/final.dir/final/render/shader.cpp.o: CMakeFiles/final.dir/flags.make
CMakeFiles/final.dir/final/render/shader.cpp.o: /Users/estinebruner/projects/computer_graphics/final/final/render/shader.cpp
CMakeFiles/final.dir/final/render/shader.cpp.o: CMakeFiles/final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/estinebruner/projects/computer_graphics/final/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/final.dir/final/render/shader.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/final.dir/final/render/shader.cpp.o -MF CMakeFiles/final.dir/final/render/shader.cpp.o.d -o CMakeFiles/final.dir/final/render/shader.cpp.o -c /Users/estinebruner/projects/computer_graphics/final/final/render/shader.cpp

CMakeFiles/final.dir/final/render/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/final.dir/final/render/shader.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/estinebruner/projects/computer_graphics/final/final/render/shader.cpp > CMakeFiles/final.dir/final/render/shader.cpp.i

CMakeFiles/final.dir/final/render/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/final.dir/final/render/shader.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/estinebruner/projects/computer_graphics/final/final/render/shader.cpp -o CMakeFiles/final.dir/final/render/shader.cpp.s

# Object files for target final
final_OBJECTS = \
"CMakeFiles/final.dir/final/final.cpp.o" \
"CMakeFiles/final.dir/final/render/shader.cpp.o"

# External object files for target final
final_EXTERNAL_OBJECTS =

final: CMakeFiles/final.dir/final/final.cpp.o
final: CMakeFiles/final.dir/final/render/shader.cpp.o
final: CMakeFiles/final.dir/build.make
final: libglfw.3.1.dylib
final: libglad.a
final: CMakeFiles/final.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/estinebruner/projects/computer_graphics/final/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable final"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/final.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/final.dir/build: final
.PHONY : CMakeFiles/final.dir/build

CMakeFiles/final.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/final.dir/cmake_clean.cmake
.PHONY : CMakeFiles/final.dir/clean

CMakeFiles/final.dir/depend:
	cd /Users/estinebruner/projects/computer_graphics/final/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/estinebruner/projects/computer_graphics/final /Users/estinebruner/projects/computer_graphics/final /Users/estinebruner/projects/computer_graphics/final/build /Users/estinebruner/projects/computer_graphics/final/build /Users/estinebruner/projects/computer_graphics/final/build/CMakeFiles/final.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/final.dir/depend

