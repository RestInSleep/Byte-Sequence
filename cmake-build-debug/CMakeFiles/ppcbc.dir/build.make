# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /home/jan/Downloads/clion-2023.3/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /home/jan/Downloads/clion-2023.3/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jan/katalog/SIK/projekt1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jan/katalog/SIK/projekt1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ppcbc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ppcbc.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ppcbc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ppcbc.dir/flags.make

CMakeFiles/ppcbc.dir/ppcbc.c.o: CMakeFiles/ppcbc.dir/flags.make
CMakeFiles/ppcbc.dir/ppcbc.c.o: /home/jan/katalog/SIK/projekt1/ppcbc.c
CMakeFiles/ppcbc.dir/ppcbc.c.o: CMakeFiles/ppcbc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jan/katalog/SIK/projekt1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ppcbc.dir/ppcbc.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbc.dir/ppcbc.c.o -MF CMakeFiles/ppcbc.dir/ppcbc.c.o.d -o CMakeFiles/ppcbc.dir/ppcbc.c.o -c /home/jan/katalog/SIK/projekt1/ppcbc.c

CMakeFiles/ppcbc.dir/ppcbc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ppcbc.dir/ppcbc.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/ppcbc.c > CMakeFiles/ppcbc.dir/ppcbc.c.i

CMakeFiles/ppcbc.dir/ppcbc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ppcbc.dir/ppcbc.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/ppcbc.c -o CMakeFiles/ppcbc.dir/ppcbc.c.s

CMakeFiles/ppcbc.dir/err.c.o: CMakeFiles/ppcbc.dir/flags.make
CMakeFiles/ppcbc.dir/err.c.o: /home/jan/katalog/SIK/projekt1/err.c
CMakeFiles/ppcbc.dir/err.c.o: CMakeFiles/ppcbc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jan/katalog/SIK/projekt1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ppcbc.dir/err.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbc.dir/err.c.o -MF CMakeFiles/ppcbc.dir/err.c.o.d -o CMakeFiles/ppcbc.dir/err.c.o -c /home/jan/katalog/SIK/projekt1/err.c

CMakeFiles/ppcbc.dir/err.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ppcbc.dir/err.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/err.c > CMakeFiles/ppcbc.dir/err.c.i

CMakeFiles/ppcbc.dir/err.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ppcbc.dir/err.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/err.c -o CMakeFiles/ppcbc.dir/err.c.s

CMakeFiles/ppcbc.dir/common.c.o: CMakeFiles/ppcbc.dir/flags.make
CMakeFiles/ppcbc.dir/common.c.o: /home/jan/katalog/SIK/projekt1/common.c
CMakeFiles/ppcbc.dir/common.c.o: CMakeFiles/ppcbc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jan/katalog/SIK/projekt1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/ppcbc.dir/common.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbc.dir/common.c.o -MF CMakeFiles/ppcbc.dir/common.c.o.d -o CMakeFiles/ppcbc.dir/common.c.o -c /home/jan/katalog/SIK/projekt1/common.c

CMakeFiles/ppcbc.dir/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ppcbc.dir/common.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/common.c > CMakeFiles/ppcbc.dir/common.c.i

CMakeFiles/ppcbc.dir/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ppcbc.dir/common.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/common.c -o CMakeFiles/ppcbc.dir/common.c.s

# Object files for target ppcbc
ppcbc_OBJECTS = \
"CMakeFiles/ppcbc.dir/ppcbc.c.o" \
"CMakeFiles/ppcbc.dir/err.c.o" \
"CMakeFiles/ppcbc.dir/common.c.o"

# External object files for target ppcbc
ppcbc_EXTERNAL_OBJECTS =

ppcbc: CMakeFiles/ppcbc.dir/ppcbc.c.o
ppcbc: CMakeFiles/ppcbc.dir/err.c.o
ppcbc: CMakeFiles/ppcbc.dir/common.c.o
ppcbc: CMakeFiles/ppcbc.dir/build.make
ppcbc: CMakeFiles/ppcbc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/jan/katalog/SIK/projekt1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable ppcbc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ppcbc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ppcbc.dir/build: ppcbc
.PHONY : CMakeFiles/ppcbc.dir/build

CMakeFiles/ppcbc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ppcbc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ppcbc.dir/clean

CMakeFiles/ppcbc.dir/depend:
	cd /home/jan/katalog/SIK/projekt1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jan/katalog/SIK/projekt1 /home/jan/katalog/SIK/projekt1 /home/jan/katalog/SIK/projekt1/cmake-build-debug /home/jan/katalog/SIK/projekt1/cmake-build-debug /home/jan/katalog/SIK/projekt1/cmake-build-debug/CMakeFiles/ppcbc.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ppcbc.dir/depend

