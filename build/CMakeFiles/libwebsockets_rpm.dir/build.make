# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/botho/git/libwebsockets

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/botho/git/libwebsockets/build

# Utility rule file for libwebsockets_rpm.

# Include the progress variables for this target.
include CMakeFiles/libwebsockets_rpm.dir/progress.make

CMakeFiles/libwebsockets_rpm:
	cpack -G TGZ --config CPackSourceConfig.cmake
	/usr/bin/cmake -E copy libwebsockets-1.4.tar.gz /home/botho/git/libwebsockets/build/RPM/SOURCES
	/usr/bin/rpmbuild -bb --define="_topdir /home/botho/git/libwebsockets/build/RPM" --buildroot=/home/botho/git/libwebsockets/build/RPM/tmp /home/botho/git/libwebsockets/build/RPM/SPECS/libwebsockets.spec

libwebsockets_rpm: CMakeFiles/libwebsockets_rpm
libwebsockets_rpm: CMakeFiles/libwebsockets_rpm.dir/build.make
.PHONY : libwebsockets_rpm

# Rule to build all files generated by this target.
CMakeFiles/libwebsockets_rpm.dir/build: libwebsockets_rpm
.PHONY : CMakeFiles/libwebsockets_rpm.dir/build

CMakeFiles/libwebsockets_rpm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/libwebsockets_rpm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/libwebsockets_rpm.dir/clean

CMakeFiles/libwebsockets_rpm.dir/depend:
	cd /home/botho/git/libwebsockets/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/botho/git/libwebsockets /home/botho/git/libwebsockets /home/botho/git/libwebsockets/build /home/botho/git/libwebsockets/build /home/botho/git/libwebsockets/build/CMakeFiles/libwebsockets_rpm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/libwebsockets_rpm.dir/depend
