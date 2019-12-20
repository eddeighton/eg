
cmake_minimum_required(VERSION 3.14.2)

######################################
#Sort out the boost dependencies
find_path( INDICATORS_INCLUDEDIR NAMES indicators PATHS ${EG_THIRD_PARTY_DIR}/install/indicators/include/ )

INCLUDE_DIRECTORIES( ${INDICATORS_INCLUDEDIR} )
