
function(create_cxx_library libname libtype std)
    set(cxx_std_ "cxx_std_${std}")
    add_library(${libname} ${libtype})

    if(libtype STREQUAL "INTERFACE")
        target_compile_features(${libname} INTERFACE ${cxx_std_})
    else()
        target_compile_features(${libname} PUBLIC ${cxx_std_})
        set_target_properties(${libname}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        )
    endif()
endfunction()


function(create_cxx_executable exename libs)
    add_executable(${exename})

    foreach (lib ${libs})
        target_link_libraries(${exename} PRIVATE ${lib})
    endforeach()
endfunction()
