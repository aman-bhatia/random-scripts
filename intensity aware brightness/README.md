# Intensity Aware Brightness

Generally, if we increase the brightness of a image, it is increased linearly i.e. each pixels intensity is multiplied with a constant factor. Here I have tried to change brightness on the basis of the intensity value of the pixel. Hence, if the intensity of the pixel is high, it will be multiplied with a small factor, and if it the intensity of pixel is low, it will be multiplied with a large factor. The factor is decided by a gaussian function over the intensity. The sigma of the gaussian function can be twaeked for better results.

# Result

| Original               | Linear Brightness Result    | Intensity Aware Brightness Result   |
| ---------------------- |:---------------------------:| -----------------------------------:|
| ![alt text][original]  | ![alt text][result_default] | ![alt text][result_intensity-aware] |

[original]: ./test.jpg "Original"
[result_default]: ./result_default.jpg "Default Result"
[result_intensity-aware]: ./result_intensity-aware.jpg "Intensity Aware Result"
