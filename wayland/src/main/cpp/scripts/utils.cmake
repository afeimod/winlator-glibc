function(check_repo path)
    file(GLOB FILES_IN_DIRECTORY RELATIVE ${path} *)
    if(FILES_IN_DIRECTORY STREQUAL "")
        message(FATAL_ERROR "Repository is empty, please execute 'git submodule update' before building.")
    endif ()
endfunction()

function(aarch64_only)
    check_c_source_compiles("
    #ifndef __aarch64__
        #error Support aarch64 only.
    #endif
" IS_AARCH64)
    if (NOT DEFINED IS_AARCH64)
        message(FATAL_ERROR "Support aarch64 only.")
    endif ()
endfunction()