cmake_minimum_required(VERSION 3.10)
project(HangmanGame)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)

# Set the path to the file
file(GLOB TARGET_SRC "./src/*.cpp" )

# Add the executable target
add_executable(HangmanGame ${TARGET_SRC})

# Set the path to the data file in the source directory
set(DATA_FILE_PATH "${CMAKE_SOURCE_DIR}/src/data.csv")
# Copy the data file to the binary directory where the executable is created
configure_file("${DATA_FILE_PATH}" "${CMAKE_BINARY_DIR}/data.csv")
