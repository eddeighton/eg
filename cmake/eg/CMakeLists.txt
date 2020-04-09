
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

set( EG_LIBRARY
	${EG_API_DIR}/eg/backtrace.hpp
	${EG_API_DIR}/eg/clock.cpp
	${EG_API_DIR}/eg/clock.hpp
	${EG_API_DIR}/eg/common.hpp
	${EG_API_DIR}/eg/coroutine.hpp
	${EG_API_DIR}/eg/default_traits.hpp
	${EG_API_DIR}/eg/eg_common_strings.hpp
	${EG_API_DIR}/eg/event.hpp
	${EG_API_DIR}/eg/event_log.cpp
	${EG_API_DIR}/eg/include.hpp
	${EG_API_DIR}/eg/iterators.hpp
	${EG_API_DIR}/eg/macros.hpp
	${EG_API_DIR}/eg/operations.hpp
	${EG_API_DIR}/eg/resource.cpp
	${EG_API_DIR}/eg/resource.hpp
	${EG_API_DIR}/eg/result_type.hpp
	${EG_API_DIR}/eg/scheduler.hpp
	${EG_API_DIR}/eg/type_path.hpp
	${EG_API_DIR}/eg/variant.hpp
	)

set( EG_SOURCES
    
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
    ${EG_SRC_DIR}/eg/interface.hpp
    ${EG_SRC_DIR}/eg/interface.cpp
    ${EG_SRC_DIR}/eg/concrete.hpp
    ${EG_SRC_DIR}/eg/concrete.cpp
    ${EG_SRC_DIR}/eg/derivation.hpp
    ${EG_SRC_DIR}/eg/derivation.cpp
    ${EG_SRC_DIR}/eg/layout.hpp
    ${EG_SRC_DIR}/eg/layout.cpp
    ${EG_SRC_DIR}/eg/identifiers.hpp
    ${EG_SRC_DIR}/eg/identifiers.cpp
    ${EG_SRC_DIR}/eg/name_resolution.hpp
    ${EG_SRC_DIR}/eg/name_resolution.cpp
    ${EG_SRC_DIR}/eg/invocation.hpp
    ${EG_SRC_DIR}/eg/invocation.cpp
    ${EG_SRC_DIR}/eg/instruction.hpp
    ${EG_SRC_DIR}/eg/instruction.cpp
    
    ${EG_SRC_DIR}/eg/sessions/session.hpp
    ${EG_SRC_DIR}/eg/sessions/session.cpp
    ${EG_SRC_DIR}/eg/sessions/parser_session.hpp
    ${EG_SRC_DIR}/eg/sessions/parser_session.cpp
    ${EG_SRC_DIR}/eg/sessions/interface_session.hpp
    ${EG_SRC_DIR}/eg/sessions/interface_session.cpp
    ${EG_SRC_DIR}/eg/sessions/implementation_session.hpp
    ${EG_SRC_DIR}/eg/sessions/implementation_session.cpp
    ${EG_SRC_DIR}/eg/sessions/operations_session.hpp
    ${EG_SRC_DIR}/eg/sessions/operations_session.cpp
    
    ${EG_SRC_DIR}/eg/codegen/codegen.hpp
    ${EG_SRC_DIR}/eg/codegen/codegen.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_includes.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_interface.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_operations.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_runtime.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_generics.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_structures.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_implementation.cpp
    )
    
add_library( eglib 
                    ${DATABASE_API}
                    ${EG_SOURCES}
				)

set_target_properties( eglib PROPERTIES FOLDER eg )

set_target_properties( eglib PROPERTIES DEBUG_POSTFIX d )

link_boost( eglib filesystem )
link_boost( eglib iostreams )
link_boost( eglib serialization )
link_clang( eglib )
link_common( eglib )

install( FILES ${EG_LIBRARY} DESTINATION include/eg )
install( TARGETS eglib DESTINATION lib)