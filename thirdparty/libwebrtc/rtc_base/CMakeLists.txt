aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 MathFunctions 链接库
add_library (WEBRTC_RTC_BASE ${DIR_LIB_SRCS})