# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\JetBrains\CLion 2019.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\CLion 2019.3\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\PackFileCpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\PackFileCpp\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PackFileCpp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PackFileCpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PackFileCpp.dir/flags.make

CMakeFiles/PackFileCpp.dir/main.cpp.obj: CMakeFiles/PackFileCpp.dir/flags.make
CMakeFiles/PackFileCpp.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\PackFileCpp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/PackFileCpp.dir/main.cpp.obj"
	D:\PROGRA~1\Dev-Cpp\MinGW64\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\PackFileCpp.dir\main.cpp.obj -c D:\PackFileCpp\main.cpp

CMakeFiles/PackFileCpp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PackFileCpp.dir/main.cpp.i"
	D:\PROGRA~1\Dev-Cpp\MinGW64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\PackFileCpp\main.cpp > CMakeFiles\PackFileCpp.dir\main.cpp.i

CMakeFiles/PackFileCpp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PackFileCpp.dir/main.cpp.s"
	D:\PROGRA~1\Dev-Cpp\MinGW64\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\PackFileCpp\main.cpp -o CMakeFiles\PackFileCpp.dir\main.cpp.s

# Object files for target PackFileCpp
PackFileCpp_OBJECTS = \
"CMakeFiles/PackFileCpp.dir/main.cpp.obj"

# External object files for target PackFileCpp
PackFileCpp_EXTERNAL_OBJECTS =

PackFileCpp.exe: CMakeFiles/PackFileCpp.dir/main.cpp.obj
PackFileCpp.exe: CMakeFiles/PackFileCpp.dir/build.make
PackFileCpp.exe: CMakeFiles/PackFileCpp.dir/linklibs.rsp
PackFileCpp.exe: CMakeFiles/PackFileCpp.dir/objects1.rsp
PackFileCpp.exe: CMakeFiles/PackFileCpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\PackFileCpp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable PackFileCpp.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\PackFileCpp.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PackFileCpp.dir/build: PackFileCpp.exe

.PHONY : CMakeFiles/PackFileCpp.dir/build

CMakeFiles/PackFileCpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\PackFileCpp.dir\cmake_clean.cmake
.PHONY : CMakeFiles/PackFileCpp.dir/clean

CMakeFiles/PackFileCpp.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\PackFileCpp D:\PackFileCpp D:\PackFileCpp\cmake-build-debug D:\PackFileCpp\cmake-build-debug D:\PackFileCpp\cmake-build-debug\CMakeFiles\PackFileCpp.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PackFileCpp.dir/depend

