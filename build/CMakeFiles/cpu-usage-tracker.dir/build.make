# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/piotr/cpu-usage-tracker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/piotr/cpu-usage-tracker/build

# Include any dependencies generated for this target.
include CMakeFiles/cpu-usage-tracker.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cpu-usage-tracker.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cpu-usage-tracker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cpu-usage-tracker.dir/flags.make

CMakeFiles/cpu-usage-tracker.dir/main.c.o: CMakeFiles/cpu-usage-tracker.dir/flags.make
CMakeFiles/cpu-usage-tracker.dir/main.c.o: ../main.c
CMakeFiles/cpu-usage-tracker.dir/main.c.o: CMakeFiles/cpu-usage-tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/piotr/cpu-usage-tracker/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cpu-usage-tracker.dir/main.c.o"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/cpu-usage-tracker.dir/main.c.o -MF CMakeFiles/cpu-usage-tracker.dir/main.c.o.d -o CMakeFiles/cpu-usage-tracker.dir/main.c.o -c /home/piotr/cpu-usage-tracker/main.c

CMakeFiles/cpu-usage-tracker.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cpu-usage-tracker.dir/main.c.i"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/piotr/cpu-usage-tracker/main.c > CMakeFiles/cpu-usage-tracker.dir/main.c.i

CMakeFiles/cpu-usage-tracker.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cpu-usage-tracker.dir/main.c.s"
	/usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/piotr/cpu-usage-tracker/main.c -o CMakeFiles/cpu-usage-tracker.dir/main.c.s

# Object files for target cpu-usage-tracker
cpu__usage__tracker_OBJECTS = \
"CMakeFiles/cpu-usage-tracker.dir/main.c.o"

# External object files for target cpu-usage-tracker
cpu__usage__tracker_EXTERNAL_OBJECTS =

cpu-usage-tracker: CMakeFiles/cpu-usage-tracker.dir/main.c.o
cpu-usage-tracker: CMakeFiles/cpu-usage-tracker.dir/build.make
cpu-usage-tracker: CMakeFiles/cpu-usage-tracker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/piotr/cpu-usage-tracker/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable cpu-usage-tracker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cpu-usage-tracker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cpu-usage-tracker.dir/build: cpu-usage-tracker
.PHONY : CMakeFiles/cpu-usage-tracker.dir/build

CMakeFiles/cpu-usage-tracker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cpu-usage-tracker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cpu-usage-tracker.dir/clean

CMakeFiles/cpu-usage-tracker.dir/depend:
	cd /home/piotr/cpu-usage-tracker/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/piotr/cpu-usage-tracker /home/piotr/cpu-usage-tracker /home/piotr/cpu-usage-tracker/build /home/piotr/cpu-usage-tracker/build /home/piotr/cpu-usage-tracker/build/CMakeFiles/cpu-usage-tracker.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cpu-usage-tracker.dir/depend

