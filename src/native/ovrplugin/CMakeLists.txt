add_library(driver_remotexr SHARED main.cpp)

set_target_properties(driver_remotexr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/remotexr_driver/bin/win64/"
    PREFIX "../"
)
configure_file(driver.vrdrivermanifest "${CMAKE_BINARY_DIR}/remotexr_driver/driver.vrdrivermanifest" COPYONLY)