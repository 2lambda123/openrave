file(GLOB ann_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_definitions(-DANN_STATIC_LIBRARY) # for windows compilation, just exports the symbols (even though library is being built statically)
add_library(ANN STATIC ${ann_files} include/ANN/ANN.h include/ANN/ANNx.h)
