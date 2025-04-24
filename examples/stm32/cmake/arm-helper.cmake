function(arm_post_build TARGET)
        set(TARGET_FILE ${TARGET}${CMAKE_EXECUTABLE_SUFFIX_C})

        # Print executable size
        # add_custom_command(TARGET ${TARGET}
        # POST_BUILD
        # COMMAND arm-none-eabi-size ${CMAKE_BINARY_DIR}/${TARGET_FILE})
        set(BINARY_PATH ${CMAKE_BINARY_DIR}/examples/stm32)
        # Create hex file
        add_custom_command(TARGET ${TARGET}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${BINARY_PATH}/${TARGET_FILE} ${BINARY_PATH}/${TARGET}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${BINARY_PATH}/${TARGET_FILE} ${BINARY_PATH}/${TARGET}.bin)

        set_property(
                TARGET ${TARGET}
                APPEND
                PROPERTY ADDITIONAL_CLEAN_FILES 
                ${BINARY_PATH}/${TARGET}.bin
                ${BINARY_PATH}/${TARGET}.hex
                ${BINARY_PATH}/${TARGET}.map
        )
endfunction()

