
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

set( EG_LIBRARY
	${EG_API_DIR}/eg/allocator.hpp
	${EG_API_DIR}/eg/backtrace.hpp
	${EG_API_DIR}/eg/clock.cpp
	${EG_API_DIR}/eg/clock.hpp
	${EG_API_DIR}/eg/common.hpp
	${EG_API_DIR}/eg/coroutine.hpp
	${EG_API_DIR}/eg/default_traits.hpp
	${EG_API_DIR}/eg/eg_common_strings.hpp
	${EG_API_DIR}/eg/event.hpp
	${EG_API_DIR}/eg/event_log.cpp
	${EG_API_DIR}/eg/frame.hpp
	${EG_API_DIR}/eg/frame.cpp
	${EG_API_DIR}/eg/include.hpp
	${EG_API_DIR}/eg/iterators.hpp
	${EG_API_DIR}/eg/macros.hpp
	${EG_API_DIR}/eg/operations.hpp
	${EG_API_DIR}/eg/resource.cpp
	${EG_API_DIR}/eg/resource.hpp
	${EG_API_DIR}/eg/result_type.hpp
	${EG_API_DIR}/eg/scheduler.hpp
	${EG_API_DIR}/eg/basic_scheduler.cpp
	${EG_API_DIR}/eg/type_path.hpp
	${EG_API_DIR}/eg/variant.hpp
	)
	
set( EG_API
    ${EG_API_DIR}/eg_compiler/eg.hpp
    ${EG_API_DIR}/eg_compiler/objects.hpp
    ${EG_API_DIR}/eg_compiler/input.hpp
    ${EG_API_DIR}/eg_compiler/interface.hpp
    ${EG_API_DIR}/eg_compiler/allocator.hpp
    ${EG_API_DIR}/eg_compiler/concrete.hpp
    ${EG_API_DIR}/eg_compiler/derivation.hpp
    ${EG_API_DIR}/eg_compiler/link.hpp
    ${EG_API_DIR}/eg_compiler/translation_unit.hpp
    ${EG_API_DIR}/eg_compiler/layout.hpp
    ${EG_API_DIR}/eg_compiler/identifiers.hpp
    ${EG_API_DIR}/eg_compiler/name_resolution.hpp
    ${EG_API_DIR}/eg_compiler/invocation.hpp
    ${EG_API_DIR}/eg_compiler/instruction.hpp
	)
	
set( EG_API_SESSIONS
    ${EG_API_DIR}/eg_compiler/sessions/session.hpp
    ${EG_API_DIR}/eg_compiler/sessions/parser_session.hpp
    ${EG_API_DIR}/eg_compiler/sessions/interface_session.hpp
    ${EG_API_DIR}/eg_compiler/sessions/implementation_session.hpp
    ${EG_API_DIR}/eg_compiler/sessions/operations_session.hpp
	)
	
set( EG_API_CODEGEN
    ${EG_API_DIR}/eg_compiler/codegen/codegen.hpp
	)
	
set( EG_API_IO
    ${EG_API_DIR}/eg_compiler/io/archive.hpp
    ${EG_API_DIR}/eg_compiler/io/indexed_object.hpp
    ${EG_API_DIR}/eg_compiler/io/loader.hpp
    ${EG_API_DIR}/eg_compiler/io/storer.hpp
	)
	
set( EG_SOURCES
    ${EG_SRC_DIR}/io/indexed_object.cpp
    ${EG_SRC_DIR}/io/loader.cpp
    ${EG_SRC_DIR}/io/storer.cpp
    
    ${EG_SRC_DIR}/eg/eg.cpp
    ${EG_SRC_DIR}/eg/objects.cpp
    ${EG_SRC_DIR}/eg/input.cpp
    ${EG_SRC_DIR}/eg/interface.cpp
    ${EG_SRC_DIR}/eg/allocator.cpp
    ${EG_SRC_DIR}/eg/concrete.cpp
    ${EG_SRC_DIR}/eg/derivation.cpp
    ${EG_SRC_DIR}/eg/link.cpp
    ${EG_SRC_DIR}/eg/translation_unit.cpp
    ${EG_SRC_DIR}/eg/layout.cpp
    ${EG_SRC_DIR}/eg/identifiers.cpp
    ${EG_SRC_DIR}/eg/name_resolution.cpp
    ${EG_SRC_DIR}/eg/invocation.cpp
    ${EG_SRC_DIR}/eg/instruction.cpp
    
    ${EG_SRC_DIR}/eg/sessions/session.cpp
    ${EG_SRC_DIR}/eg/sessions/parser_session.cpp
    ${EG_SRC_DIR}/eg/sessions/interface_session.cpp
    ${EG_SRC_DIR}/eg/sessions/implementation_session.cpp
    ${EG_SRC_DIR}/eg/sessions/operations_session.cpp
    
    ${EG_SRC_DIR}/eg/codegen/codegen.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_includes.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_interface.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_operations.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_runtime.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_generics.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_structures.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_implementation.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_instructions.cpp
    ${EG_SRC_DIR}/eg/codegen/codegen_concrete.cpp
    )
    
add_library( eglib 
                    ${DATABASE_API}
					${EG_API}
					${EG_API_IO}
					${EG_API_SESSIONS}
					${EG_API_CODEGEN}
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
install( FILES ${EG_API} DESTINATION include/eg_compiler )
install( FILES ${EG_API_IO} DESTINATION include/eg_compiler/io )
install( FILES ${EG_API_SESSIONS} DESTINATION include/eg_compiler/sessions )
install( FILES ${EG_API_CODEGEN} DESTINATION include/eg_compiler/codegen )
install( TARGETS eglib EXPORT eg DESTINATION lib)
install( EXPORT eg NAMESPACE EG:: FILE eg-config.cmake DESTINATION share )