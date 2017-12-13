/*
Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/




#ifndef HIPDNN_H
#define HIPDNN_H

#include <hip/hip_runtime_api.h>

#define HIPDNN_EXPORT

    
//uncomment this out to enable value substitution for
//incompatible types.
//HGSOS plan to support "best effort" at some point, as opposed to strict
//#define HIPDNN_ENABLE_SUBSTITUTION


typedef enum
{
    HIPDNN_STATUS_SUCCESS                      = 0,
    HIPDNN_STATUS_NOT_INITIALIZED              = 1,
    HIPDNN_STATUS_ALLOC_FAILED                 = 2,
    HIPDNN_STATUS_BAD_PARAM                    = 3,
    HIPDNN_STATUS_INTERNAL_ERROR               = 4,
    HIPDNN_STATUS_INVALID_VALUE                = 5,
    HIPDNN_STATUS_ARCH_MISMATCH                = 6,
    HIPDNN_STATUS_MAPPING_ERROR                = 7,
    HIPDNN_STATUS_EXECUTION_FAILED             = 8,
    HIPDNN_STATUS_NOT_SUPPORTED                = 9,
    HIPDNN_STATUS_LICENSE_ERROR                = 10,
    HIPDNN_STATUS_RUNTIME_PREREQUISITE_MISSING = 11,
} hipdnnStatus_t;


//=============================================================================

typedef enum
{
    HIPDNN_DATA_FLOAT  = 0,
    HIPDNN_DATA_DOUBLE = 1,
    HIPDNN_DATA_HALF   = 2,
    HIPDNN_DATA_INT8   = 3,
    HIPDNN_DATA_INT32  = 4,
    HIPDNN_DATA_INT8x4 = 5
} hipdnnDataType_t;


//=============================================================================

typedef enum
{
    HIPDNN_OP_TENSOR_ADD  = 0,
    HIPDNN_OP_TENSOR_MUL  = 1,
    HIPDNN_OP_TENSOR_MIN  = 2,
    HIPDNN_OP_TENSOR_MAX  = 3,
    HIPDNN_OP_TENSOR_SQRT = 4,
} hipdnnOpTensorOp_t;


//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION       = 0,
    HIPDNN_CROSS_CORRELATION = 1
} hipdnnConvolutionMode_t;

//=============================================================================

typedef enum
{
    HIPDNN_POOLING_MAX     = 0,
    HIPDNN_POOLING_AVERAGE_COUNT_INCLUDE_PADDING = 1, // count for average includes padded values
    HIPDNN_POOLING_AVERAGE_COUNT_EXCLUDE_PADDING = 2, // count for average does not include padded values
    HIPDNN_POOLING_MAX_DETERMINISTIC     = 3
} hipdnnPoolingMode_t;



//=============================================================================

typedef enum {
    HIPDNN_LRN_WITHIN_CHANNEL = 0,
    HIPDNN_LRN_CROSS_CHANNEL  = 1,
} hipdnnLRNMode_t;


//=============================================================================


typedef enum
{
    // bnScale, bnBias tensor dims are 1xCxHxWx.. (one value per CHW...-slice, normalized over N slice)
    HIPDNN_BATCHNORM_PER_ACTIVATION = 0,

    //bnScale, bnBias tensor dims are 1xCx1x1 (one value per C-dim normalized over Nx1xHxW subtensors)
    HIPDNN_BATCHNORM_SPATIAL        = 1,
} hipdnnBatchNormMode_t;


//=============================================================================

typedef enum
{
    HIPDNN_ACTIVATION_SIGMOID      = 0,
    HIPDNN_ACTIVATION_RELU,
    HIPDNN_ACTIVATION_TANH,
    HIPDNN_ACTIVATION_CLIPPED_RELU,
    HIPDNN_ACTIVATION_ELU,
    HIPDNN_ACTIVATION_PATHTRU,
    HIPDNN_ACTIVATION_SOFTRELU,
    HIPDNN_ACTIVATION_ABS,
    HIPDNN_ACTIVATION_POWER
} hipdnnActivationMode_t;


//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION_FWD_ALGO_IMPLICIT_GEMM         = 0,
    HIPDNN_CONVOLUTION_FWD_ALGO_IMPLICIT_PRECOMP_GEMM = 1,
    HIPDNN_CONVOLUTION_FWD_ALGO_GEMM                  = 2,
    HIPDNN_CONVOLUTION_FWD_ALGO_DIRECT                = 3,
    HIPDNN_CONVOLUTION_FWD_ALGO_FFT                   = 4,
    HIPDNN_CONVOLUTION_FWD_ALGO_FFT_TILING            = 5,
    HIPDNN_CONVOLUTION_FWD_ALGO_WINOGRAD              = 6,
    HIPDNN_CONVOLUTION_FWD_ALGO_WINOGRAD_NONFUSED     = 7,
    HIPDNN_CONVOLUTION_FWD_ALGO_COUNT                 = 8,
} hipdnnConvolutionFwdAlgo_t;


int ConvolutionFwdAlgoCount();

// call ConvolutionFwdAlgoCount first, caller's responsibility to
// make sure that i is not too large!
//
hipdnnConvolutionFwdAlgo_t  GetConvolutionFwdAlgo(int i);

//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_0                 = 0,  // non-deterministic
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_1                 = 1,
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_FFT               = 2,
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_3                 = 3,  // non-deterministic, algo0 with workspace
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_WINOGRAD          = 4,  // not implemented
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_WINOGRAD_NONFUSED = 5,
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_FFT_TILING        = 6,
    HIPDNN_CONVOLUTION_BWD_FILTER_ALGO_COUNT             = 7,   
} hipdnnConvolutionBwdFilterAlgo_t;


int ConvolutionBwdFilterAlgoCount();

// call ConvolutionBwdFilterAlgoCount first, caller's responsibility to
// make sure that i is not too large!
//
hipdnnConvolutionBwdFilterAlgo_t  GetConvolutionBwdFilterAlgo(int i);



//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_0                 = 0, // non-deterministic
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_1                 = 1,
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_FFT               = 2,
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_FFT_TILING        = 3,
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_WINOGRAD          = 4,
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_WINOGRAD_NONFUSED = 5,
    HIPDNN_CONVOLUTION_BWD_DATA_ALGO_COUNT             = 6,
} hipdnnConvolutionBwdDataAlgo_t;


int ConvolutionBwdDataAlgoCount();

// call ConvolutionBwdDataAlgoCount first, caller's responsibility to
// make sure that i is not too large!
//
hipdnnConvolutionBwdDataAlgo_t  GetConvolutionBwdDataAlgo(int i);


//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION_BWD_DATA_NO_WORKSPACE             = 0,
    HIPDNN_CONVOLUTION_BWD_DATA_PREFER_FASTEST           = 1,
    HIPDNN_CONVOLUTION_BWD_DATA_SPECIFY_WORKSPACE_LIMIT  = 2,
} hipdnnConvolutionBwdDataPreference_t;

//=============================================================================

typedef enum
{
    HIPDNN_SOFTMAX_FAST     = 0,         /* straightforward implementation */
    HIPDNN_SOFTMAX_ACCURATE = 1,         /* subtract max from every point to avoid overflow */
    HIPDNN_SOFTMAX_LOG      = 2
} hipdnnSoftmaxAlgorithm_t;

//=============================================================================

typedef enum
{
    HIPDNN_SOFTMAX_MODE_INSTANCE = 0,   /* compute the softmax over all C, H, W for each N */
    HIPDNN_SOFTMAX_MODE_CHANNEL = 1     /* compute the softmax over all C for each H, W, N */
} hipdnnSoftmaxMode_t;

//=============================================================================

typedef enum
{
    HIPDNN_TENSOR_NCHW = 0,          /* row major (wStride = 1, hStride = w) */
    HIPDNN_TENSOR_NHWC = 1,          /* feature maps interleaved ( cStride = 1 )*/
    HIPDNN_TENSOR_NCHW_VECT_C = 2    /* each image point is vector of element of C : the length of the vector is carried by the data type*/
} hipdnnTensorFormat_t;


//=============================================================================


/*
 * CUDNN propagate Nan
 */
typedef enum{
    HIPDNN_NOT_PROPAGATE_NAN  = 0,
    HIPDNN_PROPAGATE_NAN      = 1,
} hipdnnNanPropagation_t;


//=============================================================================


typedef enum
{
    HIPDNN_CONVOLUTION_FWD_NO_WORKSPACE            = 0,
    HIPDNN_CONVOLUTION_FWD_PREFER_FASTEST          = 1,
    HIPDNN_CONVOLUTION_FWD_SPECIFY_WORKSPACE_LIMIT = 2,
} hipdnnConvolutionFwdPreference_t;

//=============================================================================

typedef enum
{
    HIPDNN_CONVOLUTION_BWD_FILTER_NO_WORKSPACE            = 0,
    HIPDNN_CONVOLUTION_BWD_FILTER_PREFER_FASTEST          = 1,
    HIPDNN_CONVOLUTION_BWD_FILTER_SPECIFY_WORKSPACE_LIMIT = 2,
} hipdnnConvolutionBwdFilterPreference_t;

//=============================================================================


// platform specific typedefs

#if defined(__HIP_PLATFORM_HCC__) and not defined (__HIP_PLATFORM_NVCC__)
#include <hcc_detail/hipDNN_miopen.h>
#elif defined(__HIP_PLATFORM_NVCC__) and not defined (__HIP_PLATFORM_HCC__)
#include <nvcc_detail/hipDNN_cudnn.h>
#else 
#error("Must define exactly one of __HIP_PLATFORM_HCC__ or __HIP_PLATFORM_NVCC__");
#endif


#ifdef __cplusplus
extern "C" {
#endif


HIPDNN_EXPORT hipdnnStatus_t hipdnnCreate(hipdnnHandle_t *handle);

HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroy(hipdnnHandle_t handle);

HIPDNN_EXPORT hipdnnStatus_t hipdnnSetStream(hipdnnHandle_t handle, hipdnnStream_t streamId);

HIPDNN_EXPORT hipdnnStatus_t hipdnnGetStream(hipdnnHandle_t handle, hipdnnStream_t *streamId);

HIPDNN_EXPORT size_t hipdnnGetVersion(void);

HIPDNN_EXPORT hipdnnStatus_t hipdnnCreateTensorDescriptor(hipdnnTensorDescriptor_t *tensorDesc);


HIPDNN_EXPORT hipdnnStatus_t  hipdnnSetTensor4dDescriptor(hipdnnTensorDescriptor_t tensorDesc,
                                            hipdnnTensorFormat_t format, 
                                            hipdnnDataType_t dataType,
                                            int n, int c, int h, int w);        

        
HIPDNN_EXPORT hipdnnStatus_t hipdnnGetTensor4dDescriptor(    hipdnnTensorDescriptor_t tensorDesc,
                                            hipdnnDataType_t *dataType,
                                            int *n, int *c, int *h, int *w,
                                            int *nStride, int *cStride,
                                            int *hStride, int *wStride);

HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroyTensorDescriptor(hipdnnTensorDescriptor_t tensorDesc);


HIPDNN_EXPORT hipdnnStatus_t hipdnnAddTensor(    hipdnnHandle_t handle,
                                                const void *alpha,
                                                const hipdnnTensorDescriptor_t     aDesc, 
                                                const void *A,
                                                const void *beta,
                                                const hipdnnTensorDescriptor_t    cDesc, void *C);

#ifdef HGSOS       

//cudnn has more rich notion of TensorOp.  Need to implement following that at some point                                      
HIPDNN_EXPORT hipdnnStatus_t  hipdnnCreateOpTensorDescriptor(
                                hipdnnOpTensorDescriptor_t          *opTensorDesc );

HIPDNN_EXPORT hipdnnStatus_t  hipdnnSetOpTensorDescriptor(
                                hipdnnOpTensorDescriptor_t           opTensorDesc,
                                hipdnnOpTensorOp_t                   opTensorOp,
                                hipdnnDataType_t                     opTensorCompType,
                                hipdnnNanPropagation_t               opTensorNanOpt );

HIPDNN_EXPORT hipdnnStatus_t  hipdnnGetOpTensorDescriptor(
                                const hipdnnOpTensorDescriptor_t     opTensorDesc,
                                hipdnnOpTensorOp_t                  *opTensorOp,
                                hipdnnDataType_t                    *opTensorCompType,
                                hipdnnNanPropagation_t              *opTensorNanOpt );

HIPDNN_EXPORT hipdnnStatus_t  hipdnnDestroyOpTensorDescriptor(
                                        hipdnnOpTensorDescriptor_t  opTensorDesc );
#endif                                
                                              
HIPDNN_EXPORT hipdnnStatus_t hipdnnOpTensor(    hipdnnHandle_t handle,
                                const hipdnnOpTensorDescriptor_t opTensorDesc,
                                const void *alpha1,
                                const hipdnnTensorDescriptor_t aDesc, 
                                const void *A,
                                const void *alpha2,
                                const hipdnnTensorDescriptor_t bDesc, 
                                const void *B,
                                const void *beta,
                                const hipdnnTensorDescriptor_t cDesc, 
                                void *C);
                
HIPDNN_EXPORT hipdnnStatus_t hipdnnSetTensor(    hipdnnHandle_t handle,
                                const hipdnnTensorDescriptor_t yDesc, 
                                void *y,
                                const void *valuePtr);


HIPDNN_EXPORT hipdnnStatus_t hipdnnScaleTensor(    hipdnnHandle_t handle,
                                                const hipdnnTensorDescriptor_t yDesc, 
                                                void *y,
                                                const void *alpha);
                                    

HIPDNN_EXPORT hipdnnStatus_t hipdnnCreateFilterDescriptor(hipdnnFilterDescriptor_t *filterDesc);


HIPDNN_EXPORT hipdnnStatus_t hipdnnCreateConvolutionDescriptor(
                                hipdnnConvolutionDescriptor_t *convDesc);


HIPDNN_EXPORT hipdnnStatus_t hipdnnSetConvolution2dDescriptor(    
                                    hipdnnConvolutionDescriptor_t convDesc,
                                    int pad_h, int pad_w,
                                    int u, int v,
                                    int upscalex, int upscaley,
                                    hipdnnConvolutionMode_t mode,
                                    hipdnnDataType_t computeType)  ;

HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolution2dDescriptor(
                                const hipdnnConvolutionDescriptor_t convDesc,
                                int *pad_h, int *pad_y,
                                int *u, int *v,
                                int *upscalex, int *upscaley,
                                hipdnnConvolutionMode_t *mode,
                                hipdnnDataType_t* computeType);
                                

HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolution2dForwardOutputDim(
                                const hipdnnConvolutionDescriptor_t convDesc, //HGSOSOS const
                                const hipdnnTensorDescriptor_t inputTensorDesc,
                                const hipdnnFilterDescriptor_t filterDesc,
                                int *n, int *c, int *h, int *w);


HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroyConvolutionDescriptor(hipdnnConvolutionDescriptor_t convDesc);

HIPDNN_EXPORT hipdnnStatus_t
hipdnnFindConvolutionForwardAlgorithm(    hipdnnHandle_t handle,
                                        const hipdnnTensorDescriptor_t xDesc,
                                        const hipdnnFilterDescriptor_t wDesc,
                                        const hipdnnConvolutionDescriptor_t convDesc,
                                        const hipdnnTensorDescriptor_t yDesc,
                                        const int requestedAlgoCount,
                                        int *returnedAlgoCount,
                                        hipdnnConvolutionFwdAlgoPerf_t *perfResults);
                                        
HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionForwardAlgorithm(    
                                        hipdnnHandle_t handle,
                                        const hipdnnTensorDescriptor_t xDesc,
                                        const hipdnnFilterDescriptor_t wDesc,
                                        const hipdnnConvolutionDescriptor_t convDesc,
                                        const hipdnnTensorDescriptor_t yDesc,
                                        hipdnnConvolutionFwdPreference_t preference,
                                        size_t memoryLimitInBytes,
                                        hipdnnConvolutionFwdAlgo_t *algo);


HIPDNN_EXPORT hipdnnStatus_t hipdnnFindConvolutionForwardAlgorithmEx(
                                        hipdnnHandle_t handle,
                                        const hipdnnTensorDescriptor_t xDesc,
                                        const void *x,
                                        const hipdnnFilterDescriptor_t wDesc,
                                        const void *w,
                                        const hipdnnConvolutionDescriptor_t convDesc,
                                        const hipdnnTensorDescriptor_t yDesc,
                                        void *y,
                                        const int requestedAlgoCount,
                                        int *returnedAlgoCount,
                                        hipdnnConvolutionFwdAlgoPerf_t *perfResults,
                                        void *workSpace,
                                        size_t workSpaceSizeInBytes);



HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionForwardWorkspaceSize(    
                                            hipdnnHandle_t handle,
                                            const hipdnnTensorDescriptor_t xDesc,  
                                            const hipdnnFilterDescriptor_t wDesc,  
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            const hipdnnTensorDescriptor_t yDesc,
                                            hipdnnConvolutionFwdAlgo_t algo,  
                                            size_t *sizeInBytes);


HIPDNN_EXPORT hipdnnStatus_t hipdnnConvolutionForward(    
                                            hipdnnHandle_t handle,
                                            const void *alpha,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const void *x,
                                            const hipdnnFilterDescriptor_t wDesc,
                                            const void *w,
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            hipdnnConvolutionFwdAlgo_t algo,
                                            void *workSpace,
                                            size_t workSpaceSizeInBytes,  
                                            const void *beta,
                                            const hipdnnTensorDescriptor_t yDesc,
                                            void *y);
                                            

HIPDNN_EXPORT hipdnnStatus_t hipdnnConvolutionBackwardBias(    
                                            hipdnnHandle_t handle,
                                            const void *alpha,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const void *dy,
                                            const void *beta,
                                            const hipdnnTensorDescriptor_t dbDesc,
                                            void *db);


HIPDNN_EXPORT hipdnnStatus_t hipdnnFindConvolutionBackwardFilterAlgorithm(    
                                            hipdnnHandle_t handle,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            const hipdnnFilterDescriptor_t dwDesc,
                                            const int requestedAlgoCount,
                                            int *returnedAlgoCount,
                                            hipdnnConvolutionBwdFilterAlgoPerf_t *perfResults);
                                                
HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionBackwardFilterAlgorithm(
                                            hipdnnHandle_t handle,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            const hipdnnFilterDescriptor_t dwDesc,
                                            hipdnnConvolutionBwdFilterPreference_t preference,
                                            size_t memoryLimitInBytes,
                                            hipdnnConvolutionBwdFilterAlgo_t *algo);

HIPDNN_EXPORT hipdnnStatus_t hipdnnFindConvolutionBackwardFilterAlgorithmEx(    
                                            hipdnnHandle_t handle,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const void *x,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const void *dy,
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            const hipdnnFilterDescriptor_t dwDesc,
                                            void *dw,
                                            const int requestedAlgoCount,
                                            int *returnedAlgoCount,
                                            hipdnnConvolutionBwdFilterAlgoPerf_t *perfResults,
                                            void *workSpace,
                                            size_t workSpaceSizeInBytes);



HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionBackwardFilterWorkspaceSize(
                                            hipdnnHandle_t handle,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const hipdnnConvolutionDescriptor_t convDesc,
                                            const hipdnnFilterDescriptor_t dwDesc,  
                                            hipdnnConvolutionBwdFilterAlgo_t algo,
                                            size_t *sizeInBytes);


HIPDNN_EXPORT hipdnnStatus_t hipdnnConvolutionBackwardFilter(    
                                    hipdnnHandle_t handle,
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    hipdnnConvolutionBwdFilterAlgo_t algo,
                                    void *workSpace,
                                    size_t workSpaceSizeInBytes,
                                    const void *beta,
                                    const hipdnnFilterDescriptor_t dwDesc,
                                    void *dw);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionBackwardDataWorkspaceSize(    
                                    hipdnnHandle_t handle,
                                    const hipdnnFilterDescriptor_t wDesc,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    hipdnnConvolutionBwdDataAlgo_t algo,  
                                    size_t *sizeInBytes);


HIPDNN_EXPORT hipdnnStatus_t hipdnnFindConvolutionBackwardDataAlgorithm(    
                                    hipdnnHandle_t handle,
                                    const hipdnnFilterDescriptor_t wDesc,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    const int requestedAlgoCount,
                                    int *returnedAlgoCount,
                                    hipdnnConvolutionBwdDataAlgoPerf_t *perfResults);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetConvolutionBackwardDataAlgorithm(    
                                    hipdnnHandle_t handle,
                                    const hipdnnFilterDescriptor_t wDesc,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    hipdnnConvolutionBwdDataPreference_t preference,
                                    size_t memoryLimitInBytes,
                                    hipdnnConvolutionBwdDataAlgo_t *algo);

HIPDNN_EXPORT hipdnnStatus_t hipdnnFindConvolutionBackwardDataAlgorithmEx(
                                    hipdnnHandle_t handle,
                                    const hipdnnFilterDescriptor_t wDesc,
                                    const void *w,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx,
                                    const int requestedAlgoCount,
                                    int *returnedAlgoCount,
                                    hipdnnConvolutionBwdDataAlgoPerf_t *perfResults,
                                    void *workSpace,
                                    size_t workSpaceSizeInBytes);


HIPDNN_EXPORT hipdnnStatus_t hipdnnConvolutionBackwardData(     
                                    hipdnnHandle_t handle,
                                    const void *alpha,
                                    const hipdnnFilterDescriptor_t wDesc,
                                    const void *w,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnConvolutionDescriptor_t convDesc,
                                    hipdnnConvolutionBwdDataAlgo_t algo,
                                    void *workSpace,
                                    size_t workSpaceSizeInBytes,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx);
                                


HIPDNN_EXPORT hipdnnStatus_t hipdnnSoftmaxForward(
                                    hipdnnHandle_t handle,
                                    hipdnnSoftmaxAlgorithm_t algo,  
                                    hipdnnSoftmaxMode_t mode,   
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x, 
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    void *y);


HIPDNN_EXPORT  hipdnnStatus_t hipdnnSoftmaxBackward(    
                                    hipdnnHandle_t handle,
                                    hipdnnSoftmaxAlgorithm_t algo,  
                                    hipdnnSoftmaxMode_t mode,
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    const void *y,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy, const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx);


HIPDNN_EXPORT hipdnnStatus_t hipdnnCreatePoolingDescriptor(
                                    hipdnnPoolingDescriptor_t *poolingDesc);


HIPDNN_EXPORT hipdnnStatus_t hipdnnSetPooling2dDescriptor(    
                                    hipdnnPoolingDescriptor_t poolingDesc,
                                    hipdnnPoolingMode_t mode,
                                    hipdnnNanPropagation_t maxpoolingNanOpt, 
                                    int windowHeight,
                                    int windowWidth,
                                    int verticalPadding,
                                    int horizontalPadding,
                                    int verticalStride,
                                    int horizontalStride);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetPooling2dDescriptor(    
                                    const hipdnnPoolingDescriptor_t poolingDesc,
                                    hipdnnPoolingMode_t *mode,
                                    hipdnnNanPropagation_t *maxpoolingNanOpt, 
                                    int *windowHeight,
                                    int *windowWidth,
                                    int *verticalPadding,
                                    int *horizontalPadding,
                                    int *verticalStride,
                                    int *horizontalStride);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetPooling2dForwardOutputDim(    
                                    const hipdnnPoolingDescriptor_t poolingDesc,
                                    const hipdnnTensorDescriptor_t inputTensorDesc,
                                    int *n, int *c, int *h, int *w);

HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroyPoolingDescriptor(
                                    hipdnnPoolingDescriptor_t poolingDesc);

HIPDNN_EXPORT  hipdnnStatus_t hipdnnPoolingForward(
                                    hipdnnHandle_t handle,
                                    const hipdnnPoolingDescriptor_t poolingDesc,
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    void *y);
                                    
HIPDNN_EXPORT hipdnnStatus_t hipdnnPoolingBackward(    
                                    hipdnnHandle_t handle,
                                    const hipdnnPoolingDescriptor_t poolingDesc,
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    const void *y,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x, const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx);

HIPDNN_EXPORT  hipdnnStatus_t hipdnnCreateActivationDescriptor(
                                    hipdnnActivationDescriptor_t *activationDesc);

HIPDNN_EXPORT hipdnnStatus_t hipdnnSetActivationDescriptor(    
                                    hipdnnActivationDescriptor_t activationDesc, //HGSOS const
                                    hipdnnActivationMode_t mode,
                                    hipdnnNanPropagation_t reluNanOpt, 
                                    double reluCeiling);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetActivationDescriptor(    
                                    const hipdnnActivationDescriptor_t activationDesc,
                                    hipdnnActivationMode_t *mode,
                                    hipdnnNanPropagation_t *reluNanOpt,  
                                    double* reluCeiling);

HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroyActivationDescriptor(
                                    hipdnnActivationDescriptor_t activationDesc);

HIPDNN_EXPORT hipdnnStatus_t hipdnnActivationForward(
                                    hipdnnHandle_t handle,
                                    hipdnnActivationDescriptor_t activationDesc, //HGSOS not const in cudnn.
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    void *y);

HIPDNN_EXPORT hipdnnStatus_t hipdnnActivationBackward(    
                                    hipdnnHandle_t handle,
                                    hipdnnActivationDescriptor_t activationDesc,  //HGSOS not const in cuda
                                    const void *alpha,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    const void *y,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx);


HIPDNN_EXPORT hipdnnStatus_t hipdnnCreateLRNDescriptor(hipdnnLRNDescriptor_t *normDesc);

HIPDNN_EXPORT hipdnnStatus_t hipdnnSetLRNDescriptor(    
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t mode,  
                                    unsigned lrnN, double lrnAlpha,
                                    double lrnBeta, double lrnK);


HIPDNN_EXPORT hipdnnStatus_t hipdnnGetLRNDescriptor(    
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t *mode, 
                                    unsigned* lrnN, 
                                    double* lrnAlpha,
                                    double* lrnBeta, 
                                    double* lrnK);


HIPDNN_EXPORT hipdnnStatus_t hipdnnDestroyLRNDescriptor(hipdnnLRNDescriptor_t lrnDesc);

HIPDNN_EXPORT hipdnnStatus_t hipdnnLRNCrossChannelForward(    
                                    hipdnnHandle_t handle,
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t lrnMode,  
                                    const void* alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    void *y);

HIPDNN_EXPORT hipdnnStatus_t hipdnnLRNCrossChannelForwardEx(    
                                    hipdnnHandle_t handle,
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t lrnMode,  
                                    const void* alpha,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x,
                                    const void *beta,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    void *y,
                                    size_t workspacesize,
                                    void *workspace);


HIPDNN_EXPORT hipdnnStatus_t hipdnnLRNCrossChannelBackward(    
                                    hipdnnHandle_t handle,
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t lrnMode,  
                                    const void* alpha,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    const void *y,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x, const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx);
                                
HIPDNN_EXPORT hipdnnStatus_t hipdnnLRNCrossChannelBackwardEx(
                                    hipdnnHandle_t handle,
                                    hipdnnLRNDescriptor_t normDesc,
                                    hipdnnLRNMode_t lrnMode,  
                                    const void* alpha,
                                    const hipdnnTensorDescriptor_t yDesc,
                                    const void *y,
                                    const hipdnnTensorDescriptor_t dyDesc,
                                    const void *dy,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    const void *x, const void *beta,
                                    const hipdnnTensorDescriptor_t dxDesc,
                                    void *dx,
                                    size_t workspacesize,
                                    void* workspace);


HIPDNN_EXPORT hipdnnStatus_t hipdnnDeriveBNTensorDescriptor(    
                                    hipdnnTensorDescriptor_t derivedBnDesc,
                                    const hipdnnTensorDescriptor_t xDesc,
                                    hipdnnBatchNormMode_t mode);


HIPDNN_EXPORT hipdnnStatus_t hipdnnBatchNormalizationForwardTraining(    hipdnnHandle_t handle,
                                            hipdnnBatchNormMode_t mode,
                                            void *alpha, 
                                            void *beta,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const void *x,
                                            const hipdnnTensorDescriptor_t yDesc,
                                            void *y,
                                            const hipdnnTensorDescriptor_t bnScaleBiasMeanVarDesc,
                                            void *bnScale, void *bnBias,
                                            double exponentialAverageFactor,
                                            void *resultRunningMean,
                                            void *resultRunningVariance,
                                            double epsilon,
                                            void *resultSaveMean,
                                            void *resultSaveInvVariance);

HIPDNN_EXPORT hipdnnStatus_t hipdnnnBatchNormalizationForwardInference(    
                                            hipdnnHandle_t handle,
                                            hipdnnBatchNormMode_t mode,
                                            void *alpha, 
                                            void *beta,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const void *x,
                                            const hipdnnTensorDescriptor_t yDesc,
                                            void *y,
                                            const hipdnnTensorDescriptor_t bnScaleBiasMeanVarDesc,
                                            const void *bnScale, 
                                            const void *bnBias,
                                            const void *estimatedMean,
                                            const void *estimatedVariance,
                                            double epsilon);
                                            
HIPDNN_EXPORT hipdnnStatus_t hipdnnBatchNormalizationBackward(
                                            hipdnnHandle_t handle,
                                            hipdnnBatchNormMode_t mode,
                                            const void *alphaDataDiff,
                                            const void *betaDataDiff,
                                            const void *alphaParamDiff,
                                            const void *betaParamDiff,
                                            const hipdnnTensorDescriptor_t xDesc,
                                            const void *x,
                                            const hipdnnTensorDescriptor_t dyDesc,
                                            const void *dy,
                                            const hipdnnTensorDescriptor_t dxDesc,
                                            void *dx,
                                            const hipdnnTensorDescriptor_t bnScaleBiasDiffDesc,
                                            const void *bnScale,
                                            void *resultBnScaleDiff,
                                            void *resultBnBiasDiff,
                                            double epsilon,
                                            const void *savedMean,
                                            const void *savedInvVariance);

HIPDNN_EXPORT hipdnnStatus_t hipdnnSetTensorNdDescriptor(
                                hipdnnTensorDescriptor_t             tensorDesc,
                                hipdnnDataType_t                     dataType,
                                int                                 nbDims,
                                const int                           dimA[],
                                const int                           strideA[] );

HIPDNN_EXPORT hipdnnStatus_t hipdnnGetTensorNdDescriptor(
                                const hipdnnTensorDescriptor_t       tensorDesc,
                                int                                 nbDimsRequested,
                                hipdnnDataType_t                    *dataType,
                                int                                *nbDims,
                                int                                 dimA[],
                                int                                 strideA[] );

hipdnnStatus_t hipdnnCreateDropoutDescriptor(hipdnnDropoutDescriptor_t * dropoutDesc);

hipdnnStatus_t hipdnnDropoutGetStatesSize(hipdnnHandle_t handle, size_t * sizeInBytes);

hipdnnStatus_t hipdnnSetDropoutDescriptor(hipdnnDropoutDescriptor_t dropoutDesc,
                                                    hipdnnHandle_t handle,
                                                    float dropout,
                                                    void * states,
                                                    size_t stateSizeInBytes,
                                                    unsigned long long seed);

hipdnnStatus_t hipdnnDestroyDropoutDescriptor(hipdnnDropoutDescriptor_t dropoutDesc);

hipdnnStatus_t hipdnnSetFilterNdDescriptor(
                                hipdnnFilterDescriptor_t             filterDesc,
                                hipdnnDataType_t                     dataType, // image data type
                                hipdnnTensorFormat_t                 format,
                                int                                 nbDims,
                                const int                           filterDimA[] );

hipdnnStatus_t hipdnnGetFilterNdDescriptor(
                                const hipdnnFilterDescriptor_t       filterDesc,
                                int                                 nbDimsRequested,
                                hipdnnDataType_t                    *dataType, // image data type
                                hipdnnTensorFormat_t                *format,
                                int                                *nbDims,
                                int                                 filterDimA[] );


hipdnnStatus_t hipdnnDestroyFilterDescriptor(
                                hipdnnFilterDescriptor_t             filterDesc );

hipdnnStatus_t hipdnnSetConvolutionNdDescriptor(
hipdnnConvolutionDescriptor_t convDesc,
int arrayLength, /* nbDims-2 size */
const int padA[],
const int filterStrideA[],
const int dilationA[],
hipdnnConvolutionMode_t mode,
hipdnnDataType_t computeType ); // convolution data type

hipdnnStatus_t hipdnnSetPoolingNdDescriptor(
hipdnnPoolingDescriptor_t poolingDesc,
const hipdnnPoolingMode_t mode,
const hipdnnNanPropagation_t maxpoolingNanOpt,
int nbDims,
const int windowDimA[],
const int paddingA[],
const int strideA[] );


// human-readable error messages
const char * hipdnnGetErrorString(hipdnnStatus_t status);

/* RNN API */
typedef enum 
  {
    HIPDNN_RNN_RELU = 0, // Stock RNN with ReLu activation
    HIPDNN_RNN_TANH = 1, // Stock RNN with tanh activation
    HIPDNN_LSTM = 2,     // LSTM with no peephole connections
    HIPDNN_GRU = 3       // Using h' = tanh(r * Uh(t-1) + Wx) and h = (1 - z) * h' + z * h(t-1);
  } hipdnnRNNMode_t;

typedef enum
  {
   HIPDNN_UNIDIRECTIONAL = 0,
   HIPDNN_BIDIRECTIONAL = 1      // Using output concatination at each step. Do we also want to support output sum?
  } hipdnnDirectionMode_t;

typedef enum
  {
   HIPDNN_LINEAR_INPUT = 0,
   HIPDNN_SKIP_INPUT = 1    
  } hipdnnRNNInputMode_t;  
    
  
typedef enum 
  {
    HIPDNN_RNN_ALGO_STANDARD = 0, 
    HIPDNN_RNN_ALGO_PERSIST_STATIC = 1,
    HIPDNN_RNN_ALGO_PERSIST_DYNAMIC = 2
  } hipdnnRNNAlgo_t;  
  

hipdnnStatus_t hipdnnCreateRNNDescriptor(hipdnnRNNDescriptor_t * rnnDesc);
hipdnnStatus_t hipdnnDestroyRNNDescriptor(hipdnnRNNDescriptor_t rnnDesc);

                   
// Expensive. Creates the plan for the specific settings.
hipdnnStatus_t  hipdnnCreatePersistentRNNPlan(hipdnnRNNDescriptor_t rnnDesc,
                                             const int minibatch,
                                             const hipdnnDataType_t dataType,
                                             hipdnnPersistentRNNPlan_t * plan);
                                             
// Attaches the plan to the descriptor. 
hipdnnStatus_t  hipdnnSetPersistentRNNPlan(hipdnnRNNDescriptor_t rnnDesc,
                                          hipdnnPersistentRNNPlan_t plan);
                                          
hipdnnStatus_t  hipdnnDestroyPersistentRNNPlan(hipdnnPersistentRNNPlan_t plan);
                                          


hipdnnStatus_t  hipdnnSetRNNDescriptor_v6(hipdnnHandle_t handle, 
                                                hipdnnRNNDescriptor_t rnnDesc,
                                                const int hiddenSize, 
                                                const int numLayers, 
                                                hipdnnDropoutDescriptor_t dropoutDesc, // Between layers, not between recurrent steps.
                                                hipdnnRNNInputMode_t inputMode,                                                 
                                                hipdnnDirectionMode_t direction, 
                                                hipdnnRNNMode_t mode, 
                                                hipdnnRNNAlgo_t algo, 
                                                hipdnnDataType_t dataType);


hipdnnStatus_t  hipdnnSetRNNDescriptor(hipdnnRNNDescriptor_t rnnDesc,
                                                int hiddenSize, 
                                                int numLayers, 
                                                hipdnnDropoutDescriptor_t dropoutDesc, // Between layers, not between recurrent steps.
                                                hipdnnRNNInputMode_t inputMode,                                                 
                                                hipdnnDirectionMode_t direction, 
                                                hipdnnRNNMode_t mode, 
                                                hipdnnDataType_t dataType);



// dataType in the RNN descriptor is used to determine math precision
// dataType in weight descriptors and input descriptors is used to describe storage

hipdnnStatus_t  hipdnnGetRNNWorkspaceSize( hipdnnHandle_t              handle,
                                                    const hipdnnRNNDescriptor_t rnnDesc,  
                                                    const int seqLength, 
                                                    const hipdnnTensorDescriptor_t    *xDesc,
                                                    size_t                     *sizeInBytes
                                                    );
                                                      
hipdnnStatus_t  hipdnnGetRNNTrainingReserveSize( hipdnnHandle_t              handle,
                                                          const hipdnnRNNDescriptor_t rnnDesc,  
                                                          const int seqLength, 
                                                          const hipdnnTensorDescriptor_t    *xDesc,
                                                          size_t                     *sizeInBytes
                                                    );

                                                    
hipdnnStatus_t  hipdnnGetRNNParamsSize( hipdnnHandle_t              handle,
                                                 const hipdnnRNNDescriptor_t rnnDesc,  
                                                 const hipdnnTensorDescriptor_t    xDesc,                                                    
                                                 size_t                     *sizeInBytes,
                                                 hipdnnDataType_t dataType
                                                    );

hipdnnStatus_t  hipdnnGetRNNLinLayerMatrixParams( hipdnnHandle_t              handle,
                             const hipdnnRNNDescriptor_t rnnDesc,  
                             const int layer,
                             const hipdnnTensorDescriptor_t xDesc, 
                             const hipdnnFilterDescriptor_t wDesc, 
                             const void * w, 
                             const int linLayerID,  
                             hipdnnFilterDescriptor_t linLayerMatDesc, 
                             void ** linLayerMat
                             );

hipdnnStatus_t  hipdnnGetRNNLinLayerBiasParams( hipdnnHandle_t              handle,
                             const hipdnnRNNDescriptor_t rnnDesc,  
                             const int layer,
                             const hipdnnTensorDescriptor_t xDesc, 
                             const hipdnnFilterDescriptor_t wDesc, 
                             const void * w, 
                             const int linLayerID, 
                             hipdnnFilterDescriptor_t linLayerBiasDesc, 
                             void ** linLayerBias                       
                             );


hipdnnStatus_t  hipdnnRNNForwardInference( hipdnnHandle_t handle, 
                                                    const hipdnnRNNDescriptor_t rnnDesc, 
                                                    const int seqLength, 
                                                    const hipdnnTensorDescriptor_t * xDesc, 
                                                    const void * x, 
                                                    const hipdnnTensorDescriptor_t hxDesc, 
                                                    const void * hx, 
                                                    const hipdnnTensorDescriptor_t cxDesc, 
                                                    const void * cx, 
                                                    const hipdnnFilterDescriptor_t wDesc, 
                                                    const void * w, 
                                                    const hipdnnTensorDescriptor_t *yDesc,  
                                                    void * y, 
                                                    const hipdnnTensorDescriptor_t hyDesc, 
                                                    void * hy, 
                                                    const hipdnnTensorDescriptor_t cyDesc, 
                                                    void * cy, 
                                                    void * workspace, 
                                                    size_t workSpaceSizeInBytes);             

                                           

hipdnnStatus_t  hipdnnRNNForwardTraining( hipdnnHandle_t handle, 
                                                   const hipdnnRNNDescriptor_t rnnDesc, 
                                                   const int seqLength, 
                                                   const hipdnnTensorDescriptor_t *xDesc, 
                                                   const void * x, 
                                                   const hipdnnTensorDescriptor_t hxDesc, 
                                                   const void * hx, 
                                                   const hipdnnTensorDescriptor_t cxDesc, 
                                                   const void * cx, 
                                                   const hipdnnFilterDescriptor_t wDesc, 
                                                   const void * w, 
                                                   const hipdnnTensorDescriptor_t *yDesc,  
                                                   void * y, 
                                                   const hipdnnTensorDescriptor_t hyDesc, 
                                                   void * hy, 
                                                   const hipdnnTensorDescriptor_t cyDesc, 
                                                   void * cy, 
                                                   void * workspace, 
                                                   size_t workSpaceSizeInBytes,
                                                   void * reserveSpace, 
                                                   size_t reserveSpaceSizeInBytes);

hipdnnStatus_t  hipdnnRNNBackwardData( hipdnnHandle_t handle, 
                                                const hipdnnRNNDescriptor_t rnnDesc, 
                                                const int seqLength, 
                                                const hipdnnTensorDescriptor_t * yDesc, 
                                                const void * y,                                                
                                                const hipdnnTensorDescriptor_t * dyDesc, 
                                                const void * dy, 
                                                const hipdnnTensorDescriptor_t dhyDesc, 
                                                const void * dhy, 
                                                const hipdnnTensorDescriptor_t dcyDesc, 
                                                const void * dcy, 
                                                const hipdnnFilterDescriptor_t wDesc, 
                                                const void * w, 
                                                const hipdnnTensorDescriptor_t hxDesc, 
                                                const void * hx,                                                                  
                                                const hipdnnTensorDescriptor_t cxDesc, 
                                                const void * cx,                                                 
                                                const hipdnnTensorDescriptor_t * dxDesc, 
                                                void * dx, 
                                                const hipdnnTensorDescriptor_t dhxDesc,
                                                void * dhx,
                                                const hipdnnTensorDescriptor_t dcxDesc,
                                                void * dcx,
                                                void * workspace,
                                                size_t workSpaceSizeInBytes,
                                                void * reserveSpace, 
                                                size_t reserveSpaceSizeInBytes );


hipdnnStatus_t  hipdnnRNNBackwardWeights( hipdnnHandle_t handle, 
                                                   const hipdnnRNNDescriptor_t rnnDesc, 
                                                   const int seqLength, 
                                                   const hipdnnTensorDescriptor_t * xDesc, 
                                                   const void * x, 
                                                   const hipdnnTensorDescriptor_t hxDesc, 
                                                   const void * hx,                                                   
                                                   const hipdnnTensorDescriptor_t * yDesc, 
                                                   const void * y,
                                                   const void * workspace, 
                                                   size_t workSpaceSizeInBytes, 
                                                   const hipdnnFilterDescriptor_t dwDesc, 
                                                   void * dw,
                                                   const void * reserveSpace, 
                                                   size_t reserveSpaceSizeInBytes );

hipdnnStatus_t hipdnnBatchNormalizationForwardInference(
                                hipdnnHandle_t                       handle,
                                hipdnnBatchNormMode_t                mode,
                                const void                         *alpha, // alpha[0] = result blend factor
                                const void                         *beta,  // beta[0] = dest layer blend factor
                                const hipdnnTensorDescriptor_t       xDesc,
                                const void                         *x,     // NxCxHxW
                                const hipdnnTensorDescriptor_t       yDesc,
                                void                               *y,     // NxCxHxW
                                const hipdnnTensorDescriptor_t       bnScaleBiasMeanVarDesc,
                                const void                         *bnScale,
                                const void                         *bnBias,
                                const void                         *estimatedMean,
                                const void                         *estimatedVariance,
                                double                              epsilon );
 
#ifdef __cplusplus
}
#endif




#endif // HIPDNN_H
