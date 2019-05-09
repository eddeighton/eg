
cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

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
    
    ${EG_SRC_DIR}/eg/eg.hpp
    ${EG_SRC_DIR}/eg/eg.cpp
    ${EG_SRC_DIR}/eg/objects.hpp
    ${EG_SRC_DIR}/eg/objects.cpp
    ${EG_SRC_DIR}/eg/input.hpp
    ${EG_SRC_DIR}/eg/input.cpp
    ${EG_SRC_DIR}/eg/session.hpp
    ${EG_SRC_DIR}/eg/session.cpp
    ${EG_SRC_DIR}/eg/parser_session.hpp
    ${EG_SRC_DIR}/eg/parser_session.cpp
    ${EG_SRC_DIR}/eg/abstract.hpp
    ${EG_SRC_DIR}/eg/abstract.cpp
    ${EG_SRC_DIR}/eg/concrete.hpp
    ${EG_SRC_DIR}/eg/concrete.cpp
    ${EG_SRC_DIR}/eg/codegen.hpp
    ${EG_SRC_DIR}/eg/codegen.cpp
    ${EG_SRC_DIR}/eg/derivation.hpp
    ${EG_SRC_DIR}/eg/derivation.cpp
    ${EG_SRC_DIR}/eg/interface_session.hpp
    ${EG_SRC_DIR}/eg/interface_session.cpp
    ${EG_SRC_DIR}/eg/layout.hpp
    ${EG_SRC_DIR}/eg/layout.cpp
    ${EG_SRC_DIR}/eg/implementation_session.hpp
    ${EG_SRC_DIR}/eg/implementation_session.cpp
    ${EG_SRC_DIR}/eg/identifiers.hpp
    ${EG_SRC_DIR}/eg/identifiers.cpp
    ${EG_SRC_DIR}/eg/operations_session.hpp
    ${EG_SRC_DIR}/eg/operations_session.cpp
    ${EG_SRC_DIR}/eg/name_resolution.hpp
    ${EG_SRC_DIR}/eg/name_resolution.cpp
    ${EG_SRC_DIR}/eg/invocation.hpp
    ${EG_SRC_DIR}/eg/invocation.cpp
    ${EG_SRC_DIR}/eg/instruction.hpp
    ${EG_SRC_DIR}/eg/instruction.cpp
    )
    
add_library( eglib 
                    ${DATABASE_API}
                    ${EG_SOURCES}
				)

set_target_properties( eglib PROPERTIES DEBUG_POSTFIX d )

link_boost( eglib filesystem )
link_boost( eglib iostreams )
link_boost( eglib serialization )
link_clang( eglib )

install( TARGETS eglib DESTINATION bin)