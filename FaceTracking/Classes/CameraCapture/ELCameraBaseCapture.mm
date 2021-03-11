//
//  ELCameraBaseCapture.m
//  AVCaptureDemo
//
//  Created by yin linlin on 2018/5/17.
//  Copyright © 2018年 yin linlin. All rights reserved.
//

#import "ELCameraBaseCapture.h"

@interface ELCameraBaseCapture()<AVCaptureVideoDataOutputSampleBufferDelegate>

@end

@implementation ELCameraBaseCapture

- (instancetype)init {
    if (self = [super init]) {
        [self sessionConfig];
    }
    return self;
}

- (void)sessionConfig {
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddInput:self.captureInput]) {
        [self.captureSession addInput:self.captureInput];
    }
    if ([self.captureSession canAddOutput:self.imageOutput]) {
        [self.captureSession addOutput:self.imageOutput];
    }
    dispatch_queue_t queue = dispatch_queue_create("cameraQueue", NULL);
    [self.videoOutput setSampleBufferDelegate:self queue:queue];
    [self.captureSession commitConfiguration];
}


- (void)startSession {
    if (![self.captureSession isRunning]) {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [self.captureSession startRunning];
        });
    }
}


- (void)stopSession {
    if ([self.captureSession isRunning]) {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [self.captureSession stopRunning];
        });
    }
}

- (void)takePhoto:(void(^)(UIImage *image, NSError *error))complete {
    AVCaptureConnection *conntion = [self.imageOutput connectionWithMediaType:AVMediaTypeVideo];
    if (!conntion) {
        NSLog(@"拍照失败!");
        if (complete) {
            complete(nil,nil);
        }
        return;
    }
    [self.imageOutput captureStillImageAsynchronouslyFromConnection:conntion completionHandler:^(CMSampleBufferRef imageDataSampleBuffer, NSError *error) {
        if (error || imageDataSampleBuffer == nil) {
            if (complete)
                complete(nil,error);
            return;
        }
        NSData *imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:imageDataSampleBuffer];
        UIImage *image = [UIImage imageWithData:imageData];
        if (complete)
            complete(image,nil);
    }];
}

#pragma mark - lazy load
- (AVCaptureSession *)captureSession {
    if (!_captureSession) {
        _captureSession = [[AVCaptureSession alloc] init];
        //设置session采集质量 
//        _captureSession.sessionPreset = AVCaptureSessionPresetPhoto;
//        _captureSession.sessionPreset = AVCaptureSessionPreset640x480;
        _captureSession.sessionPreset = AVCaptureSessionPreset1920x1080;
    }
    return _captureSession;
}

- (AVCaptureDevice *)captureDevice {
    if (!_captureDevice) {
        _captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    return _captureDevice;
}

- (AVCaptureDeviceInput *)captureInput {
    if (!_captureInput) {
        NSError *error;
        _captureInput = [AVCaptureDeviceInput deviceInputWithDevice:self.captureDevice error:&error];
    }
    return _captureInput;
}

- (AVCaptureStillImageOutput *)imageOutput {
    if (!_imageOutput) {
        _imageOutput = [[AVCaptureStillImageOutput alloc] init];
        // 这是输出流的设置参数AVVideoCodecJPEG参数表示以JPEG的图片格式输出图片
        NSDictionary *outputSettings = [[NSDictionary alloc] initWithObjectsAndKeys:AVVideoCodecJPEG, AVVideoCodecKey, nil];
        [_imageOutput setOutputSettings:outputSettings];
    }
    return _imageOutput;
}

- (AVCaptureVideoDataOutput *)videoOutput {
    if (!_videoOutput) {
        _videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        _videoOutput.alwaysDiscardsLateVideoFrames = YES;
        _videoOutput.videoSettings = [NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey];
        AVCaptureConnection *videoCon = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];

        // 原来的刷脸没有这句话.因此录制出来的视频是有90度转角的, 这是默认情况
        if ([videoCon isVideoOrientationSupported]) {
            videoCon.videoOrientation = AVCaptureVideoOrientationPortrait;
            // 下面这句是默认系统video orientation情况!!!!,如果要outputsample图片输出的方向是正的那么需要将这里设置称为portrait
            //videoCon.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
        }
        if ([_captureSession canAddOutput:_videoOutput]) {
            [_captureSession addOutput:_videoOutput];
        }
    }
    return _videoOutput;
}

- (UIImage *)imageFromSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    // 如果不加这个后面 context 可能会创建失败
    if (width <= 0 || height <= 0) {
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return nil;
    }
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef newContext = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, colorSpace, kCGImageByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    if (newContext) {
        CGImageRef newImage = CGBitmapContextCreateImage(newContext);
        CGContextRelease(newContext);
        CGColorSpaceRelease(colorSpace);
        // 需要的图片帧数据
        UIImage *image = [UIImage imageWithCGImage:newImage scale:1 orientation:UIImageOrientationUp];
        CGImageRelease(newImage);
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return image;
    } else {
        CGColorSpaceRelease(colorSpace);
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return nil;
    }

    return nil;
}

- (cv::Mat)cvMatFromSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    //Processing here
    int bufferWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    int bufferHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
    unsigned char *pixel = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);

    //put buffer in open cv, no memory copied
    cv::Mat mat = cv::Mat(bufferHeight,bufferWidth,CV_8UC4,pixel,CVPixelBufferGetBytesPerRow(pixelBuffer));

    //End processing
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    return mat;
}

- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    //通过抽样缓存数据创建一个UIImage对象
    
    if (self.imageBlock) {
        UIImage* image = [self imageFromSampleBuffer:sampleBuffer];
        self.imageBlock(image);
    }
    if (self.cvMatBlock) {
        cv::Mat mat = [self cvMatFromSampleBuffer:sampleBuffer];
        self.cvMatBlock(mat);
    }
}
                       
@end
