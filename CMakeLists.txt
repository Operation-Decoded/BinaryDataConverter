# --- Project Setup ---
cmake_minimum_required (VERSION 3.12)

project ("BinaryDataConverter")

SET(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

# --- Dependencies ---
CPMAddPackage(
  NAME Boost
  VERSION 1.79.0
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG "boost-1.79.0"
)

CPMAddPackage(
  NAME AriaCsvParser
  VERSION 0.0.1
  GITHUB_REPOSITORY "AriaFallah/csv-parser"
  GIT_TAG "c432072c208303e04a9d6b43ecd83d7d568d2981"
)

# --- Building ---
add_executable (BinaryDataConverter "src/BinaryDataConverter.cpp" "src/CSVChannel.cpp" "src/BinaryChannel.cpp" "src/SurviveChannel.cpp")

target_link_libraries(BinaryDataConverter PRIVATE Boost::json Boost::algorithm Boost::program_options AriaCsvParser)

# --- Install ---
install(TARGETS BinaryDataConverter DESTINATION BinaryDataConverter)
install(FILES LICENSE THIRD-PARTY-NOTICE DESTINATION BinaryDataConverter/license)
install(FILES README.md DESTINATION BinaryDataConverter)
install(DIRECTORY files/ DESTINATION BinaryDataConverter)