# Add a Symbian project for building in release mode
function(add_symbian_project FOLDER)
    add_custom_target(${FOLDER})
    add_custom_command(
        TARGET ${FOLDER}
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/build.py --config urel  --result ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER}/group/ ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER}/group)
endfunction()

function(add_symbian_patch NAME)
    add_symbian_project(${NAME})

    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${NAME}
	    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/patch"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/patch")
    
    add_custom_command(
	    TARGET ${NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/patch/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/group/${NAME}/GCCE/urel/${NAME}.dll" "${CMAKE_BINARY_DIR}/bin/patch/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/group/${NAME}.map" "${CMAKE_BINARY_DIR}/bin/patch/")
endfunction()