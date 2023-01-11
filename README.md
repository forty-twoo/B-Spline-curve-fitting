# B-Spline-curve-fitting
It's a program that do B-spline curve fitting for discrete points in real time.

The fitting method include

- Global interpolation
  - Uniform 
  - Centripetal
  - Chorld
  - Universal
- Global Approximation
  - Uniform
  - Centripetal
  - Chorld
  - Universal

<img src="https://github.com/forty-twoo/B-Spline-curve-fitting/blob/master/images/show.gif" width="500px">

## Building 

**Prerequisite: CMake**

#### Linux

```git
git clone https://github.com/forty-twoo/B-Spline-curve-fitting.git
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
./B-Spline-curve-fitting
```

#### Windows

```
git clone https://github.com/forty-twoo/B-Spline-curve-fitting.git
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
# Find the .sln project then open it with IDE(eg.Visual Studio)
# Build solution then Run it
```



