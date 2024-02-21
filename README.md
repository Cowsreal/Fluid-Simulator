# SPH Fluid Simulator
 
## About this Program

This is a SPH(Smoothed Particle Hydrodynamics) based fluid simulation where the position of each particle is updated on a frame by frame basis according to the Navier-Stokes equation of motion for fluids. The physics computations are done single threaded on the CPU and visualized via OpenGL.

[Video Demo (Youtube)](https://youtu.be/CzYODLT_00g)

## Usage

1. Clone this repository

2. From the root of the repository, 
    ```
    xcopy res build\Release\res /E /I
    mkdir build
    cd build
    cmake -G "Visual Studio 17 2022" -A Win32 ..
    ```

3. Navigate to /build and build the project using release build either in Visual Studio or in terminal via MSBuild.

4. Run build/release/main.exe

## Future Work

* Lots of numerically instable regimes in parameter space.
* Figure out GPU parallelization methods in order to improve framerate
* Background density parameter is incredibly buggy; particles experience clumping when background density is non zero.
* Clean up code

## Acknowledgements

This project uses OpenGL abstraction classes from "The Cherno"'s [tutorial series](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2). Also, this [paper](https://matthias-research.github.io/pages/publications/sca03.pdf) by Müller et al. on the subject was a very helpful resource as well as this [video](https://www.youtube.com/watch?v=rSKMYc1CQHE) from the Coding Adventure series by Sebastian Lague.