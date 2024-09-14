# Use prebuilt wayland-scanner to install protocol
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(SCANNER_BIN ${SCRIPTS_BIN_DIR}/wayland-scanner.exe)
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(SCANNER_BIN ${SCRIPTS_BIN_DIR}/wayland-scanner)
else ()
    message(FATAL_ERROR "Prebuilt wayland-scanner not supports your system.")
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