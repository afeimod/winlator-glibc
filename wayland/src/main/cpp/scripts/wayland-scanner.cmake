# Use Wayland-Scanner to install protocol
if (WIN32)
    set(SCANNER_BIN ${SCRIPTS_BIN_DIR}/wayland-scanner)
elseif (UNIX AND NOT APPLE)
    set(SCANNER_BIN ${SCRIPTS_BIN_DIR}/wayland-scanner.exe)
elseif (APPLE)
    message(FATAL_ERROR "This project is not supported on MacOS.")
else ()
    message(FATAL_ERROR "This project is not supported on Unknown System.")
endif ()

function(wayland_scanner option type input output)
    if (NOT DEFINED SCANNER_BIN)
        message(FATAL_ERROR "SCANNER_BIN is not defined.")
    endif ()

    execute_process(
            COMMAND ${SCANNER_BIN} ${option} ${type} ${input} ${output}
            ERROR_VARIABLE error_out
            RESULT_VARIABLE ret
    )

    if (NOT ret EQUAL 0)
        message(FATAL_ERROR "Exec: [${SCANNER_BIN} ${option} ${type} ${input} ${output}] Error: ${error_out}")
    endif ()
endfunction()