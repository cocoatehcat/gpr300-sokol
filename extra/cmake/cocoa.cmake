set(BUILD_WITH_COCOA_DEFAULT TRUE)

option(BUILD_WITH_COCOA "cocoa Enabled" ${BUILD_WITH_COCOA_DEFAULT})
message("BUILD_WITH_COCOA: ${BUILD_WITH_COCOA}")

if(BUILD_WITH_COCOA)
  set(COCOA_DIR ${THIRDPARTY_DIR}/cocoa)
  set(COCOA_FILES
    ${COCOA_DIR}/materialsReader.cpp
    ${COCOA_DIR}/materialsReader.h)

  add_library(cocoa STATIC ${COCOA_FILES})

  target_link_libraries(cocoa PRIVATE batteries)
  target_link_libraries(cocoa PRIVATE glm)

endif()