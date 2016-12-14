find_package(Eigen3 REQUIRED)
find_package(GSL REQUIRED)
#find_package(Boost REQUIRED)

set(external_includes ${GSL_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

set(external_libs "-lpthread" ${GSL_LIBRARIES})




