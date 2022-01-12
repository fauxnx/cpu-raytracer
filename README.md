# cpu-raytracer

A basic cpu raytracer (technically a path tracer) I made as a personal project during my studies. It's entirely based on Peter Shirley's amazing [Ray Tracing in One Weekend](https://raytracing.github.io/) series, with some optimizations of my own. Notably, I upgraded the vector class to use intrinsics, which made renders roughly three times faster, and I implemented an extremely basic thread pool to speed things up even more. The codebase is a bit messy, since this is rather old code, though I'll hopefully get around to tidying it up one of these days...
