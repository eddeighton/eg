
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
    ${EG_SRC_DIR}/database/eg.cpp
    ${EG_SRC_DIR}/database/objects.hpp
    ${EG_SRC_DIR}/database/objects.cpp
    ${EG_SRC_DIR}/database/input.hpp
    ${EG_SRC_DIR}/database/input.cpp
    ${EG_SRC_DIR}/database/session.hpp
    ${EG_SRC_DIR}/database/session.cpp
    ${EG_SRC_DIR}/database/parser_session.hpp
    ${EG_SRC_DIR}/database/parser_session.cpp
    ${EG_SRC_DIR}/database/abstract.hpp
    ${EG_SRC_DIR}/database/abstract.cpp
    ${EG_SRC_DIR}/database/concrete.hpp
    ${EG_SRC_DIR}/database/concrete.cpp
    ${EG_SRC_DIR}/database/codegen.hpp
    ${EG_SRC_DIR}/database/codegen.cpp
    ${EG_SRC_DIR}/database/derivation.hpp
    ${EG_SRC_DIR}/database/derivation.cpp
    ${EG_SRC_DIR}/database/interface_session.hpp
    ${EG_SRC_DIR}/database/interface_session.cpp
    ${EG_SRC_DIR}/database/layout.hpp
    ${EG_SRC_DIR}/database/layout.cpp
    ${EG_SRC_DIR}/database/database.cpp
    ${EG_SRC_DIR}/database/implementation_session.hpp
    ${EG_SRC_DIR}/database/implementation_session.cpp
    ${EG_SRC_DIR}/database/clang_utils.hpp
    ${EG_SRC_DIR}/database/clang_utils.cpp
    ${EG_SRC_DIR}/database/identifiers.hpp
    ${EG_SRC_DIR}/database/identifiers.cpp
    ${EG_SRC_DIR}/database/operations_session.hpp
    ${EG_SRC_DIR}/database/operations_session.cpp
    )
