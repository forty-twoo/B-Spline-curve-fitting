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

<img src="https://github.com/forty-twoo/B-Spline-curve-fitting/blob/master/images/show.gif" width="800px">

<img src="https://github.com/forty-twoo/B-Spline-curve-fitting/blob/master/images/pic.png" width="800px">

## Built With

- [tinynurbs](https://github.com/pradeep-pyro/tinynurbs)
- [imgui](https://github.com/ocornut/imgui)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
- OpenGL

The core fitting code are wrote by myself and the primary Reference is [https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/](https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/) 



### Usage

Right click your mouse to add points, drag your left mouse to move point. You could click the check box of the left panel based on your need.

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



## License
Distributed under the MIT License. See `LICENSE` for more information.