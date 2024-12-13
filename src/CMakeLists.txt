aux_source_directory(. SRC_LIST)

add_executable(EvFTP ${SRC_LIST})

target_link_libraries(EvFTP event pthread)