![](http://i.imgur.com/B17UDhh.gif)


# umbra pixel
## live coding glsl environment

### built with
* SDL2
* OpenGL/GLEW

### purpose
this application generates a quad that covers the whole frustum and is intended to have you focus on the frag shader

because this is a pet project it will mostly be useful for me to practice raytracing, raymarching, and other 3d-related frag tricks

### usage
this is completely untested on basically everything, but should theoretically be cross-platform capable.
run the project and start editing `fragment_shader.frag`

### features
* resize your window to whatever you want on the fly
* the fragment shader is recompiled every time theh file is saved
* compile status outputs to `STDOUT`, check that if your shader doesn't update
