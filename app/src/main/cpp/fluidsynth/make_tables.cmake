# Use prebuilt make-tables
# set 'FLUIDSYNTH_BIN_DIR' before include this file
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(MAKE_TABLES_BIN ${FLUIDSYNTH_BIN_DIR}/make_tables.exe)
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(MAKE_TABLES_BIN ${FLUIDSYNTH_BIN_DIR}/make_tables)
    file(CHMOD 0755 MAKE_TABLES_BIN)
else ()
    message(FATAL_ERROR "Prebuilt make-tables not supports your system.")
endif ()

function(make_tables DEST_DIR)
    execute_process(
            COMMAND ${MAKE_TABLES_BIN} ${DEST_DIR}
            ERROR_VARIABLE error_out
            RESULT_VARIABLE ret
    )

    if (NOT ret EQUAL 0)
        message(FATAL_ERROR "Exec: [${MAKE_TABLES_BIN} ${DEST_DIR}] Error: ${error_out}")
    endif ()
endfunction()