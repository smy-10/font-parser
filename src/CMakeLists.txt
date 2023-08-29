file(GLOB_RECURSE SRC . "*.cpp")

add_library(${LIB_NAME} SHARED ${SRC})

#link stage
target_link_libraries(${LIB_NAME} freetype icuuc)
include_directories(${FREETYPE2_INCLUDE_DIRS})

set_target_properties(${LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})