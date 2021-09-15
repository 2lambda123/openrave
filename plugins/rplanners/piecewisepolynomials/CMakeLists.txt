find_package(Eigen3 REQUIRED)
message(STATUS "Eigen3 version: ${Eigen3_VERSION}")
include_directories(${EIGEN3_INCLUDE_DIRS})

add_library(piecewisepolynomials STATIC polynomialcommon.h polynomialtrajectory.h polynomialtrajectory.cpp polynomialchecker.h polynomialchecker.cpp quinticinterpolator.h quinticinterpolator.cpp feasibilitychecker.h)
set_target_properties(piecewisepolynomials PROPERTIES COMPILE_FLAGS "${PLUGIN_COMPILE_FLAGS}" LINK_FLAGS "${PLUGIN_LINK_FLAGS}")		
add_dependencies(piecewisepolynomials interfacehashes_target)		