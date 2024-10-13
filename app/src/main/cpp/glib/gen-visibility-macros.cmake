find_package(Python3)
if (NOT Python3_FOUND)
    message(FATAL_ERROR "Python3 not found, please install it to continue.")
endif ()

function(gen_visibility_macros py_path v1 v2 v3 v4)
    execute_process(
            COMMAND ${Python3_EXECUTABLE} ${py_path} ${v1} ${v2} ${v3} ${v4}
            ERROR_VARIABLE error_out
            RESULT_VARIABLE ret
    )
    if (NOT ret EQUAL 0)
        message(FATAL_ERROR "Exec: [${Python3_EXECUTABLE} ${py_path} ${v1} ${v2} ${v3} ${v4}] Error: ${error_out}")
    endif ()
endfunction()