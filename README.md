# Ray Tracing in Cornell box environment
My solution to Games101 exercise 7 

# My main tasks
1. Implement path tracing, a method in ray tracing
2. Build on top of the BVH mechanisms in the previous exercise.
3. Remove fireflies and shadow acnes

# Finding 
- For fireflies, the major cause are miscalculation. The clipping might factor in but if the situation is too serious, it's most certaintly miscalculation.
- For shadow artifacts, almost certaintly, it's either self-intersecting shadow acne or miscalculation.

# Result

<img width="450" height="436" alt="Screenshot from 2025-07-17 00-36-38" src="https://github.com/user-attachments/assets/ca720342-4f56-4811-bd14-ee8fcd8f3220" />

# light source and direct illumination only (similar )

<img width="400" height="417" alt="Screenshot from 2025-07-16 15-07-38" src="https://github.com/user-attachments/assets/ab50b192-22d5-4b3e-ac39-1038acb66933" />

# Prerequisites
You need OpenCV to build this project. 
If you're on Ubuntu, run 
1. sudo apt install libopencv-dev

# Build
then, go into ./build/ directory or create one if it's not there and run
1. cmake ..
2. make

# Run (How to use the program?)
1. ./Raytracing
2. You'll see an image "binary.ppm" to see the result of the ray tracing.

Codes for this HW are mostly provided by Games 101 teaching team. 
