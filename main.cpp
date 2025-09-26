#include "image.h"

#include <string>
#include <iostream>

enum {
    ARG_BMP_FILE_PATH   = 1,
    ARG_YUV_FILE_PATH   = 2,
    ARG_YUV_FILE_WIDTH  = 3,
    ARG_YUV_FILE_HEIGHT = 4,
    ARG_YUV_FILE_SIZE   = 5
};

int main( int argc, char* argv[] ) {
    if( argc <= 1 ) {
        std::cout << "Error, not enough arguments provided.\n";
        std::cout << "Please provide arguments in tihs order: <Path to BMP file> <Path to YUV video file>" 
                    "<YUV file width> <YUV file height> <YUV file size>\n";
        return 1;
    }

    const char* BMP_file_path = argv[ ARG_BMP_FILE_PATH ];
    const char* YUV_file_path = argv[ ARG_YUV_FILE_PATH ];
    const uint16_t YUV_width = ( uint16_t )std::stoi( argv[ ARG_YUV_FILE_WIDTH ] ); 
    const uint16_t YUV_height = ( uint16_t )std::stoi( argv[ ARG_YUV_FILE_HEIGHT ] );
    const uint32_t YUV_size = ( uint32_t )std::stoul( argv [ ARG_YUV_FILE_SIZE ] );

    std::cout << YUV_width << " " << YUV_height << " " << YUV_size << "\n";

    BMP_Image img;
    if ( !img.read( BMP_file_path ) ) {
        img.convert_to_YUV420p( );
        img.apply_watermark( YUV_file_path, YUV_height, YUV_width, YUV_size );
    }
    else {
        std::cout << "Incorrect file to path...\n";
    }

    return 0;
}
