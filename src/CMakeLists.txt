configure_file (
    "${PROJECT_SOURCE_DIR}/src/version.h.in"
    "${PROJECT_SOURCE_DIR}/src/version.h"
)

add_executable(ral 
    "ral.cpp" "core.cpp" "env.cpp" "printer.cpp" 
    "reader.cpp" "types.cpp"
    "easylogging++.cpp")
