
cmake_minimum_required(VERSION 3.14.2)

include_directories( ${EG_API_DIR} )
include_directories( ${EG_SRC_DIR} )

set( DATABASE_API ${EG_API_DIR}/database/database.hpp )

set( EG_SOURCES
    ${EG_SRC_DIR}/common/assert_verify.hpp
    ${EG_SRC_DIR}/common/backtrace.hpp
    ${EG_SRC_DIR}/common/backtrace.cpp
    ${EG_SRC_DIR}/common/escape.hpp
    ${EG_SRC_DIR}/common/escape.cpp
    ${EG_SRC_DIR}/common/file.hpp
    ${EG_SRC_DIR}/common/file.cpp
    ${EG_SRC_DIR}/common/require_semicolon.hpp
    
    ${EG_SRC_DIR}/io/archive.hpp
    ${EG_SRC_DIR}/io/indexed_object.hpp
    ${EG_SRC_DIR}/io/indexed_object.cpp
    ${EG_SRC_DIR}/io/loader.hpp
    ${EG_SRC_DIR}/io/loader.cpp
    ${EG_SRC_DIR}/io/storer.hpp
    ${EG_SRC_DIR}/io/storer.cpp
    
    ${EG_SRC_DIR}/database/eg.hpp
    ${EG_SRC_DIR}/database/objects.hpp
    ${EG_SRC_DIR}/database/objects.cpp
    ${EG_SRC_DIR}/database/input.hpp
    ${EG_SRC_DIR}/database/input.cpp
    ${EG_SRC_DIR}/database/session.hpp
    ${EG_SRC_DIR}/database/session.cpp
    ${EG_SRC_DIR}/database/parser.hpp
    ${EG_SRC_DIR}/database/parser.cpp
    ${EG_SRC_DIR}/database/abstract.hpp
    ${EG_SRC_DIR}/database/abstract.cpp
    
    ${EG_SRC_DIR}/database/database.cpp
    )
