cmake_minimum_required( VERSION 3.14.0 )
project( bmp_to_yuv420_watermark )

add_executable( ${PROJECT_NAME} 
    main.cpp 
    ./image_lib/image.cpp
)

target_include_directories( bmp_to_yuv420_watermark PRIVATE
    ${CMAKE_SOURCE_DIR}/image_lib
)
