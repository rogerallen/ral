configure_file (
    "${PROJECT_SOURCE_DIR}/src/version.h.in"
    "${PROJECT_SOURCE_DIR}/src/version.h"
)

add_executable(ral 
    "ral.cpp"
    "malCore.cpp" "malEnv.cpp" "malTypes.cpp"
    "printer.cpp" "reader.cpp")
