##############################------FEAShelper------###############################
set(TARGET_FEASHelper FEASHelper)
file(GLOB FEASHelper_srcs "${SRC_DIR}/*.cpp")
add_executable(${TARGET_FEASHelper} ${FEASHelper_srcs})
target_include_directories(${TARGET_FEASHelper} PRIVATE
    ${common_headers}
    ${libbase_headers}
    ${liblog_headers}
    ${libcutis_headers}
)
target_link_libraries(${TARGET_FEASHelper} PRIVATE
    base
    log
)
