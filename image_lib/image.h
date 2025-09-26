#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <vector>

struct BMPFileHeader {
    uint16_t file_type;    // BM 
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset_data;   //Starting address, of the byte where the bitmap image data (pixel array) can be found.
} __attribute__( ( packed ) );

struct BMPInfoHeader {
    uint32_t size;                         // Size of this header
    int32_t bitmap_width;                  // Width of bitmap in pixels
    int32_t bitmap_height;                 // Height of bitmap in pixels
    uint16_t number_of_color_planes;       // Number of color planes 
    uint16_t bits_per_pixel;               // Bits per pixel
    uint32_t compression;
    uint32_t image_bitmap_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} __attribute__( ( packed ) );


struct BMP_Image {

    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    uint8_t* RGB_R = nullptr;
    uint8_t* RGB_G = nullptr;
    uint8_t* RGB_B = nullptr;
    uint8_t* RGB_A = nullptr;

    uint8_t* YUV_Y = nullptr;
    uint8_t* YUV_U = nullptr;
    uint8_t* YUV_V = nullptr;
    
    BMP_Image( const char* filename );
    BMP_Image();
    ~BMP_Image();

    bool read( const char* filename );
    bool write( const char* filename );
    bool convert_to_YUV420p( );
    bool apply_watermark ( const char* filename, uint16_t frame_height, uint16_t frame_width, uint32_t file_size );
};

#endif
