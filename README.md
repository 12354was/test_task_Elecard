## Build
```bash
mkdir build
cmake -B .\build\ -G "MinGW Makefiles" .   
cd ./build 
cmake --build .
```

## Usage 
```bash
./bmp_to_yuv420_watermark.exe <BMP_path> <YUV_path> <YUV_width> <YUV_height> <YUV_file_size>
```
