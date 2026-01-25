# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

# Define executable.
add_library(${SourceLib} STATIC)

add_executable(${SourceExecutable}
    main.cpp
    ${EXTRA_SOURCES}
)

# Add definitions/includes if cross-compiling (toolchain sets them)
if(DEFINED EXTRA_DEFS)
    target_compile_definitions(${SourceExecutable} PRIVATE ${EXTRA_DEFS})
    target_compile_definitions(${SourceLib} PRIVATE ${EXTRA_DEFS})
endif()
target_compile_definitions(${SourceExecutable} PRIVATE LOGGING_ENABLED=$<BOOL:${LOGGING_ENABLED}>)
target_compile_definitions(${SourceLib} PRIVATE LOGGING_ENABLED=$<BOOL:${LOGGING_ENABLED}>)

# Attach include dirs to the target.
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

target_include_directories(${SourceExecutable} PUBLIC ${INCLUDE_DIRS} )
target_include_directories(${SourceLib} PUBLIC ${INCLUDE_DIRS} )

if(DEFINED EXTRA_INCLUDES)
    target_include_directories(${SourceExecutable} PRIVATE ${EXTRA_INCLUDES})
    target_include_directories(${SourceLib} PRIVATE ${EXTRA_INCLUDES})
endif()

# Add subdirectories (each adds sources/includes).
add_subdirectory(features)
add_subdirectory(hardware_interface)
add_subdirectory(helper)
add_subdirectory(lib)
add_subdirectory(runtimes)

target_link_libraries(${SourceExecutable} PUBLIC ${SourceLib})

if(ARM_BUILD)
    if(NOT BUILD_GLASSES_HOST)
        target_sources(${SourceExecutable} PRIVATE
            hardware_interface/usb/USB_DEVICE/App/usb_device.c
            hardware_interface/usb/USB_DEVICE/App/usbd_cdc_if.c
            hardware_interface/usb/USB_DEVICE/App/usbd_desc.c
            hardware_interface/usb/USB_DEVICE/Target/usbd_conf.c

            hardware_interface/usb/STM32_USB_Device_Library/Core/Src/usbd_core.c
            hardware_interface/usb/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
            hardware_interface/usb/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
            hardware_interface/usb/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
        )

        set(USB_INCLUDES
            hardware_interface/usb/STM32_USB_Device_Library/Core/Inc
            hardware_interface/usb/STM32_USB_Device_Library/Class/CDC/Inc
            hardware_interface/usb/USB_DEVICE/App/
            hardware_interface/usb/USB_DEVICE/Target
        )
    else()
        target_compile_definitions(${SourceExecutable} PRIVATE BUILD_GLASSES_HOST HAL_HCD_MODULE_ENABLED)
        target_compile_definitions(${SourceLib} PRIVATE BUILD_GLASSES_HOST HAL_HCD_MODULE_ENABLED)

        target_sources(${SourceExecutable} PRIVATE
            hardware_interface/usb/USB_HOST/App/usb_host.c
            hardware_interface/usb/USB_HOST/Target/usbh_conf.c

            hardware_interface/usb/STM32_USB_Host_Library/Core/Src/usbh_core.c
            hardware_interface/usb/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
            hardware_interface/usb/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
            hardware_interface/usb/STM32_USB_Host_Library/Core/Src/usbh_pipes.c

            lib/STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_hcd.c
            hardware_interface/usb/STM32_USB_Host_Library/Class/AOA/Src/usbh_aoa.c
        )

        set(USB_INCLUDES
            hardware_interface/usb/USB_HOST/App/
            hardware_interface/usb/USB_HOST/Target/
            hardware_interface/usb/STM32_USB_Host_Library/Core/Inc
            hardware_interface/usb/STM32_USB_Host_Library/Class/AOA/Inc
        )
    endif()

    target_include_directories(${SourceExecutable} PRIVATE ${USB_INCLUDES})
    target_include_directories(${SourceLib} PRIVATE ${USB_INCLUDES})
endif()
