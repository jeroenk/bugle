#ifndef CUDA_H
#define CUDA_H

#define __CUDA__

#ifdef __OPENCL__
#error Cannot include both opencl.h and cuda.h
#endif

/* Vectors */
#include "cuda_vectors.h"

#define __constant__ __attribute__((constant))
#define __device__ __attribute__((device))
#define __global__ __attribute__((global))
#define __host__ __attribute__((host))
#define __shared__ __attribute__((shared))

#include "annotations.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cuda_math_functions.h"

struct _3DimensionalVector {
  unsigned x, y, z;
} threadIdx, blockIdx, blockDim, gridDim;

__device__ void __syncthreads();


__device__ float abs (float x);

#ifdef __cplusplus
}
#endif

/* Thread block dimensions */

// Must define a dimension

#ifndef __1D_THREAD_BLOCK
#ifndef __2D_THREAD_BLOCK
#ifndef __3D_THREAD_BLOCK

#error You must specify the dimension of a work group by defining one of __1D_THREAD_BLOCK, __2D_THREAD_BLOCK or __3D_THREAD_BLOCK

#endif
#endif
#endif

// Must define only one dimension

#ifdef __1D_THREAD_BLOCK
#ifdef __2D_THREAD_BLOCK
#error Cannot define __1D_THREAD_BLOCK and __2D_THREAD_BLOCK
#endif
#ifdef __3D_THREAD_BLOCK
#error Cannot define __1D_THREAD_BLOCK and __3D_THREAD_BLOCK
#endif
#endif

#ifdef __2D_THREAD_BLOCK
#ifdef __1D_THREAD_BLOCK
#error Cannot define __2D_THREAD_BLOCK and __1D_THREAD_BLOCK
#endif
#ifdef __3D_THREAD_BLOCK
#error Cannot define __2D_THREAD_BLOCK and __3D_THREAD_BLOCK
#endif
#endif

#ifdef __3D_THREAD_BLOCK
#ifdef __1D_THREAD_BLOCK
#error Cannot define __3D_THREAD_BLOCK and __1D_THREAD_BLOCK
#endif
#ifdef __2D_THREAD_BLOCK
#error Cannot define __3D_THREAD_BLOCK and __2D_THREAD_BLOCK
#endif
#endif

// Generate axioms for different work group sizes

#ifdef __1D_THREAD_BLOCK
__axiom(blockDim.y == 1);
__axiom(blockDim.z == 1);
#endif

#ifdef __2D_THREAD_BLOCK
__axiom(blockDim.z == 1);
#endif


/* Thread block grid dimensions */

// Must define a dimension

#ifndef __1D_GRID
#ifndef __2D_GRID
#ifndef __3D_GRID

#error You must specify the dimension of the grid of thread blocks by defining one of __1D_GRID, __2D_GRID or __3D_GRID

#endif
#endif
#endif

// Must define only one dimension

#ifdef __1D_GRID
#ifdef __2D_GRID
#error Cannot define __1D_GRID and __2D_GRID
#endif
#ifdef __3D_GRID
#error Cannot define __1D_GRID and __3D_GRID
#endif
#endif

#ifdef __2D_GRID
#ifdef __1D_GRID
#error Cannot define __2D_GRID and __1D_GRID
#endif
#ifdef __3D_GRID
#error Cannot define __2D_GRID and __3D_GRID
#endif
#endif

#ifdef __3D_GRID
#ifdef __1D_GRID
#error Cannot define __3D_GRID and __1D_GRID
#endif
#ifdef __2D_GRID
#error Cannot define __3D_GRID and __2D_GRID
#endif
#endif

// Generate axioms for different grid sizes

#ifdef __1D_GRID
__axiom(gridDim.y == 1);
__axiom(gridDim.z == 1);
#endif

#ifdef __2D_GRID
__axiom(gridDim.z == 1);
#endif



#endif
