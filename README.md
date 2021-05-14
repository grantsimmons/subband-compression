# subband-compression
This project is my term project for Stevens' CPE-462 "Image Processing and Coding" course.

The goal of this project is to implement efficient image compression and reconstruction by utilizing Subband (Wavelet) decompression, scalar quatization, and entropy encoding.

This tool only uses C++ Standard libraries, so in most cases compilation is straight forward. Ensure that you have Git, g++ compiler, and Make installed on a UNIX-like operating system or environment (like WSL or MinGW).

```
sudo apt-get install g++ git make
```

## Compilation and Usage

1.	Clone the Git repo
	
```
git clone https://github.com/grantsimmons/subband-compression.git
```

2.	Configure the compression parameters. As of now, this must be done by directly modifying the source code. Modify the following parameters in dsp_test/haar_test.cpp:

```cpp
const bool quantization_invert_bin_step = false;
const double quantization_bin_step = 4;
const int wavelet_block_size = 8;
const double quantization_threshold = 0.8;
```

Detailed descriptions of these parameters can be found in the commented source code.

3.	Build the project with Make

```
cd subband-compression && make
```

4.	Run the executable to write the file

```
./haar_test.out -w image_examples/lenna.512 > image_results.txt
```

5.	Examine the output
This process will generate a file called “out.wlt” with the extension representing that the file contains an encoded wavelet transform. This is the encoded image file. Debug information can be found in “image_results.txt.” The new, compressed file size can be seen with the following command:

```
du -sh out.wlt
> 128K    out.wlt
```

Compare this with the original image to verify that the new file is smaller:
  
```
du -sh image_examples/lenna.512
> 256K    image_examples/lenna.512
```

6.	Convert the compressed image to a viewable bitmap file

```
./haar_test.out -r out.wlt > image_results.txt
```

7.	View the file
For convenience, this project provides tools to convert between PNG files to raw grayscale values to view them. Convert the raw grayscale output to a viewable PNG file with the following command:

```
make img IMG_X=512 IMG_Y=512 IMG=recovered
```

Open the file in your favorite image viewer/editor
  
```
gimp recovered.png
```
