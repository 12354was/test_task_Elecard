#include "./image.h"

#include <algorithm>
#include <fstream>
#include <iostream>


//___________________BMP FUNCTIONS_______________________

enum {
        UV_MULTIPLIER = 2,
        PIXEL_DELTA = 2,
    };

BMP_Image::BMP_Image( const char* filename ) {
    this->read( filename );
};

BMP_Image::BMP_Image() {};

BMP_Image::~BMP_Image() {
    delete [] this->RGB_R;
    delete [] this->RGB_G;
    delete [] this->RGB_B;
    delete [] this->RGB_A;

    delete [] this->YUV_Y;
    delete [] this->YUV_U;
    delete [] this->YUV_V;
};

bool BMP_Image::read( const char* filename ) {
    std::ifstream file( filename, std::ios::in | std::ios::binary );

    if( !file.is_open() ) {
        std::cerr << "Error: Could not open the file...\n";
        return 1; 
    }

    //Read header and info about the bitmap of an image
    file.read( ( char* )( &this->file_header ), sizeof( BMPFileHeader ) );
    file.read( ( char* )( &this->info_header ), sizeof( BMPInfoHeader ) );

    std::cout << "Headers read.\n";

    uint32_t p_width = this->info_header.bitmap_width;
    uint32_t p_height = this->info_header.bitmap_height;

    //Bitmap rows are padded, so we need to calculate padding
    enum {
        ROW_OFFSET = 4,
        BITS_PER_BYTE = 8
    };

    uint32_t row_padding = ( ( ROW_OFFSET - ( p_width * this->info_header.bits_per_pixel / BITS_PER_BYTE ) ) % ROW_OFFSET) % ROW_OFFSET;

    //Allocate memory for eqch RGB comonent
    this->RGB_R = new uint8_t[ p_width * p_height ];
    this->RGB_G = new uint8_t[ p_width * p_height ];
    this->RGB_B = new uint8_t[ p_width * p_height ];
    this->RGB_A = new uint8_t[ p_width * p_height ];

    std::cout << "RGBA arrays memory allocated.\n";

    //Read the RGB components (stored as BGR) into the arrays
    for( uint16_t y = 0; y < p_height; ++y ) {
        for( uint16_t x = 0; x < p_width; ++x ) {
            file.read( ( char* )&this->RGB_B[ y * p_width + x ], sizeof( uint8_t ) );
            file.read( ( char* )&this->RGB_G[ y * p_width + x ], sizeof( uint8_t ) );
            file.read( ( char* )&this->RGB_R[ y * p_width + x ], sizeof( uint8_t ) );
            file.read( ( char* )&this->RGB_A[ y * p_width + x ], sizeof( uint8_t ) );
        }
        file.seekg( row_padding, std::ios_base::cur );
    }

    std::cout << "File read.\n";

    file.close();

    return 0;
};

bool BMP_Image::write( const char* filename ) {

    std::ofstream file_out( filename, std::ios::out | std::ios::binary );

    if( !file_out.is_open() ) {
        std::cerr << "Error: Could not create the file...\n";
        return 1; 
    }

    
    uint32_t p_width = this->info_header.bitmap_width;
    uint32_t p_height = this->info_header.bitmap_height;
    uint32_t b_size = p_width * p_height;

    //Apps may display image in an upside down way 

    //Write down Y commponent
    for ( int y = 0; y < b_size; ++y ) {
        file_out.write( ( char* ) &this->YUV_Y[ y ], sizeof( uint8_t ) );
    }
    //Write down V component
    for ( int v = 0; v < b_size / 4 ; ++v ) {
        file_out.write( ( char* ) &this->YUV_V[ v ], sizeof( uint8_t ) );
    }
    //Write down U component
    for ( int u = 0; u < b_size / 4 ; ++u ) {
        file_out.write( ( char* ) &this->YUV_U[ u ], sizeof( uint8_t ) );
    }
 
    file_out.close();

    return 0;
};

bool BMP_Image::convert_to_YUV420p() {
    uint32_t p_width = this->info_header.bitmap_width;
    uint32_t p_height = this->info_header.bitmap_height;

    this->YUV_Y = new uint8_t[ p_width * p_height ];
    this->YUV_U = new uint8_t[ p_width * p_height / UV_MULTIPLIER / UV_MULTIPLIER ];
    this->YUV_V = new uint8_t[ p_width * p_height / UV_MULTIPLIER / UV_MULTIPLIER ];

    for( uint16_t y = 0; y < p_height; ++y ) {
        for( uint16_t x = 0; x < p_width; ++x ) {
            uint32_t linear_index = y * p_width + x;            
            this->YUV_Y[ linear_index ] = ( ( this->RGB_A[ linear_index ] * 66  + this->RGB_G[ linear_index ] * 129 + this->RGB_B[ linear_index ] * 25 + 128 ) >> 8 ) + 16;
        }
    }

    for ( uint16_t y = 0; y < p_height / UV_MULTIPLIER; ++y ) {
        for ( uint16_t x = 0; x < p_width / UV_MULTIPLIER; ++x ) {
            uint16_t R_sum = 0, G_sum = 0, B_sum = 0, avg_num = 4;

            for ( uint16_t idy = 0; idy < PIXEL_DELTA; ++idy ) {
                for ( uint16_t jdx = 0; jdx < PIXEL_DELTA; ++jdx ) {
                    uint32_t current_x = x * 2 + jdx;
                    uint32_t current_y = y * 2 + idy;
                    //Calculating average for neightboring pixels
                    if ( current_x < p_width && current_y < p_height ) {
                        uint32_t current_pixel = current_y * p_width + current_x; 
                        R_sum += this->RGB_A[ ( current_pixel ) ];
                        G_sum += this->RGB_G[ ( current_pixel ) ];
                        B_sum += this->RGB_B[ ( current_pixel ) ];
                    }
                    else {
                        --avg_num;
                    }
                }
            }
            uint8_t avg_R = R_sum / avg_num;
            uint8_t avg_G = G_sum / avg_num;
            uint8_t avg_B = B_sum / avg_num;

            uint8_t U = ( ( -38 * avg_R  - 74 * avg_G + 112 * avg_B + 128 ) >> 8 ) + 128;
            uint8_t V = ( ( 112 * avg_R - 94 * avg_G - 18 * avg_B + 128 ) >> 8 ) + 128;

            uint16_t UV_data_offset = y * ( p_width / UV_MULTIPLIER ) + x;

            this->YUV_U[ UV_data_offset ] = U;
            this->YUV_V[ UV_data_offset ] = V;

        }
    }

    return 0;
}

bool BMP_Image::apply_watermark( const char* filename, uint16_t frame_height, uint16_t frame_width, uint32_t file_size ) {
    uint16_t frame_count = file_size / ( ( frame_height * frame_width * 3 ) / 2 );
    uint32_t frame_size = frame_height * frame_width;
    uint32_t true_frame_size = frame_height * frame_width * 3 / 2;

    uint32_t y_size = frame_height * frame_width;
    uint32_t uv_size = y_size / UV_MULTIPLIER / UV_MULTIPLIER;

    uint32_t watermark_width = this->info_header.bitmap_width;
    uint32_t watermark_height = this->info_header.bitmap_height;
    uint32_t watermark_y_size = watermark_width * watermark_height;
    uint32_t watermark_uv_size = ( watermark_width / UV_MULTIPLIER ) * ( watermark_height / UV_MULTIPLIER );

    std::fstream outfile( filename, std::ios::in | std::ios::out | std::ios::binary );


    if( !outfile.is_open() ) {
        std::cout << "Couldn't open the file for writing watermark...\n";
        return 1;
    }

    for ( uint16_t f = 0; f < frame_count; ++f ) {
        outfile.clear();
        outfile.seekg( f * true_frame_size, outfile.beg );

        for ( uint16_t h = 0; h < watermark_height; ++h ) {
            outfile.write( ( char* )( this->YUV_Y + h * watermark_width ), watermark_width );
            outfile.seekg( frame_width - watermark_width, outfile.cur );
        }

        //Move to U component
        outfile.seekg( f * true_frame_size + y_size, outfile.beg );
        for ( uint16_t h = 0; h < watermark_height / UV_MULTIPLIER; ++h ) {
            outfile.write( ( char* )( this->YUV_U + h * watermark_width / UV_MULTIPLIER ), watermark_width / UV_MULTIPLIER );
            outfile.seekg( frame_width / UV_MULTIPLIER - watermark_width / UV_MULTIPLIER, outfile.cur );
        }

        // //Move to V component
        outfile.seekg( f * true_frame_size + y_size + uv_size, outfile.beg );
        for ( uint16_t h = 0; h < watermark_height / UV_MULTIPLIER; ++h ) {
            outfile.write( ( char* )( this->YUV_V + h * watermark_width / UV_MULTIPLIER ), watermark_width / UV_MULTIPLIER );
            outfile.seekg( frame_width / UV_MULTIPLIER - watermark_width / UV_MULTIPLIER, outfile.cur );
        } 
    }

    outfile.close();

    return 0;
}
