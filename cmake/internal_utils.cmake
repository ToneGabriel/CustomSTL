
function(_set_default_output_directories target)
    set_target_properties(${target}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endfunction()


# create_cxx_library(libname, libtype, std, inc, src...)
function(create_cxx_library libname libtype std inc)
    set(cxx_std_ "cxx_std_${std}")
    add_library(${libname} ${libtype})

    if(libtype STREQUAL "INTERFACE")
        target_compile_features(${libname} INTERFACE ${cxx_std_})
        target_include_directories(${libname} INTERFACE ${inc})
    else()
        target_compile_features(${libname} PUBLIC ${cxx_std_})
        target_include_directories(${libname} PUBLIC ${inc})
        target_sources(${libname} PRIVATE ${ARGN})
        _set_default_output_directories(${libname})
    endif()
endfunction()


# create_cxx_executable(exename, libs, src...)
# libs - "lib1;lib2;..."
function(create_cxx_executable exename libs)
    add_executable(${exename} ${ARGN})
    _set_default_output_directories(${exename})

    foreach (lib ${libs})
        target_link_libraries(${exename} PRIVATE ${lib})
    endforeach()
endfunction()
