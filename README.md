# Image Viewer Plugin ![Build Status](https://github.com/ManiVaultStudio/ImageViewerPlugin/actions/workflows/build.yml/badge.svg?branch=master)

Viewer plugin for high dimensional images in the [ManiVault](https://github.com/ManiVaultStudio/core) visual analytics framework.

```bash
git clone git@github.com:ManiVaultStudio/ImageViewerPlugin.git
```

Drag & drop an image data set from the data hierarchy into an over viewer or right-click a the data hierarchy item and select `View -> Image Viewer`. 
You can add multiple images to an image viewer instance and change their visibility and layering order.

<p align="middle">
  <img src="https://github.com/ManiVaultStudio/ImageViewerPlugin/assets/58806453/1b790f05-64cb-47d4-bd6a-913e64d25356" align="middle"  width="80%" /> </br>
  Image viewers with open setting panel and single-channel colormapped images (left), a false color RGB image (right top) and a 2D colormapped image (right bottom). Each viewer shows a different high-dimensional image with several dozen data channels.
</p>

## Features
- Colormapping by dimension/channel:
  - `Mono`: Using a one-dimensional colormap
  - `Duo`: Using a two-dimensional colormap
  - `RGB`/`HSL`/`LAB`: map any data channel to any of the respective color space dimensions
  - Adjust brightness and contrast of each mapped dimensions 
- Zoom and pan navigation
- Value interpolation: `Nearest Neighbor` and `Bilinear`
- Selections:
  - Automatically synchronize selections (default) or delay global notification until manual selection is finished
  - Several selection types: `Rectangle` (default), `Lasso`, `Brush` and `Polygon`
- Creating subsets: Create subset from selection
- Multiple layers:
  - View multiple image data sets
  - Adjust transparency individually
- Inspect pixel values of all currently rendered channels

### Keybindings

**Selection**
- `R` Rectangle (default)
- `L` Lasso
- `B` Brush, circular: Mouse wheel adjusts the radius
- `P` Polygon: left-click to add a vertex, right-click to finish
- `S` Sample: single pixel, opens pixel value inspection overlay
- `Shift` Add to selection (while pressed)
- `Control` Remove from selection (while pressed)

**Navigation**
- `Spacebar` Switch to navigation when in selection mode (while pressed)
- `Mouse grab` Pan
- `Mouse wheel` Zoom
- `D` Zoom around selection
- `Z` Reset zoom to overview

## Gallery
Have a look at the [wiki](https://github.com/ManiVaultStudio/ImageViewerPlugin/wiki/Image-viewer-plugin-for-ManiVault) for more showcases of the image viewer's feature set.

<p align="middle">
  <img src="https://github.com/ManiVaultStudio/ImageViewerPlugin/assets/58806453/454d202d-c6fe-4fc1-aaee-7166112a1ea9" align="middle"  width="50%" /> </br>
  Sample the pixel values
</p>
