file(GLOB_RECURSE SRC_FILES "*.h" "*.c")
add_library(NESulator MODULE ${SRC_FILES})
