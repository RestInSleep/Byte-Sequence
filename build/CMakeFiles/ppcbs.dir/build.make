# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jan/katalog/SIK/projekt1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jan/katalog/SIK/projekt1/build

# Include any dependencies generated for this target.
include CMakeFiles/ppcbs.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ppcbs.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ppcbs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ppcbs.dir/flags.make

CMakeFiles/ppcbs.dir/ppcbs.c.o: CMakeFiles/ppcbs.dir/flags.make
CMakeFiles/ppcbs.dir/ppcbs.c.o: /home/jan/katalog/SIK/projekt1/ppcbs.c
CMakeFiles/ppcbs.dir/ppcbs.c.o: CMakeFiles/ppcbs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jan/katalog/SIK/projekt1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ppcbs.dir/ppcbs.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbs.dir/ppcbs.c.o -MF CMakeFiles/ppcbs.dir/ppcbs.c.o.d -o CMakeFiles/ppcbs.dir/ppcbs.c.o -c /home/jan/katalog/SIK/projekt1/ppcbs.c

CMakeFiles/ppcbs.dir/ppcbs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ppcbs.dir/ppcbs.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/ppcbs.c > CMakeFiles/ppcbs.dir/ppcbs.c.i

CMakeFiles/ppcbs.dir/ppcbs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ppcbs.dir/ppcbs.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/ppcbs.c -o CMakeFiles/ppcbs.dir/ppcbs.c.s

CMakeFiles/ppcbs.dir/err.c.o: CMakeFiles/ppcbs.dir/flags.make
CMakeFiles/ppcbs.dir/err.c.o: /home/jan/katalog/SIK/projekt1/err.c
CMakeFiles/ppcbs.dir/err.c.o: CMakeFiles/ppcbs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jan/katalog/SIK/projekt1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ppcbs.dir/err.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbs.dir/err.c.o -MF CMakeFiles/ppcbs.dir/err.c.o.d -o CMakeFiles/ppcbs.dir/err.c.o -c /home/jan/katalog/SIK/projekt1/err.c

CMakeFiles/ppcbs.dir/err.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ppcbs.dir/err.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/err.c > CMakeFiles/ppcbs.dir/err.c.i

CMakeFiles/ppcbs.dir/err.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ppcbs.dir/err.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/err.c -o CMakeFiles/ppcbs.dir/err.c.s

CMakeFiles/ppcbs.dir/common.c.o: CMakeFiles/ppcbs.dir/flags.make
CMakeFiles/ppcbs.dir/common.c.o: /home/jan/katalog/SIK/projekt1/common.c
CMakeFiles/ppcbs.dir/common.c.o: CMakeFiles/ppcbs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jan/katalog/SIK/projekt1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/ppcbs.dir/common.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ppcbs.dir/common.c.o -MF CMakeFiles/ppcbs.dir/common.c.o.d -o CMakeFiles/ppcbs.dir/common.c.o -c /home/jan/katalog/SIK/projekt1/common.c

CMakeFiles/ppcbs.dir/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ppcbs.dir/common.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jan/katalog/SIK/projekt1/common.c > CMakeFiles/ppcbs.dir/common.c.i

CMakeFiles/ppcbs.dir/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ppcbs.dir/common.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jan/katalog/SIK/projekt1/common.c -o CMakeFiles/ppcbs.dir/common.c.s

# Object files for target ppcbs
ppcbs_OBJECTS = \
"CMakeFiles/ppcbs.dir/ppcbs.c.o" \
"CMakeFiles/ppcbs.dir/err.c.o" \
"CMakeFiles/ppcbs.dir/common.c.o"

# External object files for target ppcbs
ppcbs_EXTERNAL_OBJECTS =

ppcbs: CMakeFiles/ppcbs.dir/ppcbs.c.o
ppcbs: CMakeFiles/ppcbs.dir/err.c.o
ppcbs: CMakeFiles/ppcbs.dir/common.c.o
ppcbs: CMakeFiles/ppcbs.dir/build.make
ppcbs: CMakeFiles/ppcbs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jan/katalog/SIK/projekt1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable ppcbs"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ppcbs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ppcbs.dir/build: ppcbs
.PHONY : CMakeFiles/ppcbs.dir/build

CMakeFiles/ppcbs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ppcbs.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ppcbs.dir/clean

CMakeFiles/ppcbs.dir/depend:
	cd /home/jan/katalog/SIK/projekt1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jan/katalog/SIK/projekt1 /home/jan/katalog/SIK/projekt1 /home/jan/katalog/SIK/projekt1/build /home/jan/katalog/SIK/projekt1/build /home/jan/katalog/SIK/projekt1/build/CMakeFiles/ppcbs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ppcbs.dir/depend

