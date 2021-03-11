//
//  NewFaceController.m
//  FaceTracking
//
//  Created by sdanke on 2020/10/12.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import "BaseCameraController.h"
#import "ELCameraControlCapture.h"

@interface BaseCameraController ()
// 相机部分
@property (nonatomic, strong, readwrite) ELCameraControlCapture *cameraCapture;
@property (nonatomic, strong) AVCaptureVideoPreviewLayer *preLayer;

//@property (assign, atomic) BOOL isDetecting;
@end

@implementation BaseCameraController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (authStatus == AVAuthorizationStatusRestricted || authStatus == AVAuthorizationStatusDenied) {
        // 没有权限
        NSLog(@"没有权限");
    } else {
        // 有权限
    }
    [self setCameraUI];
}

- (void)dealloc {
    
}

#pragma Public

- (void)didReceiveImage:(UIImage *)image {
    
}

- (void)didReceiveCVMat:(cv::Mat &)image {
    
}
# pragma Camera Setting


- (CGFloat)degreesToRadians:(CGFloat)degrees {
    return M_PI * (degrees / 180.0);
}

- (UIImage *)rotatedByDegrees:(CGFloat)degrees image:(UIImage *)image {
    // calculate the size of the rotated view's containing box for our drawing space
    UIView *rotatedViewBox = [[UIView alloc] initWithFrame:CGRectMake(0, 0, image.size.width, image.size.height)];
    CGAffineTransform t = CGAffineTransformMakeRotation([self degreesToRadians:degrees]);
    rotatedViewBox.transform = t;
    CGSize rotatedSize = rotatedViewBox.frame.size;
    
    // Create the bitmap context
    UIGraphicsBeginImageContext(rotatedSize);
    CGContextRef bitmap = UIGraphicsGetCurrentContext();
    
    // Move the origin to the middle of the image so we will rotate and scale around the center.
    CGContextTranslateCTM(bitmap, rotatedSize.width/2, rotatedSize.height/2);
    
    //   // Rotate the image context
    CGContextRotateCTM(bitmap, [self degreesToRadians:degrees]);
    
    // Now, draw the rotated/scaled image into the context
    CGContextScaleCTM(bitmap, 1.0, -1.0);
    CGContextDrawImage(bitmap, CGRectMake(-image.size.width / 2, -image.size.height / 2, image.size.width, image.size.height), [image CGImage]);
    
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}

- (void)setCameraUI {
    [self setVideoPreview];
    weakify(self);
    self.cameraCapture.imageBlock = ^(UIImage *image) {
        strongify(self);
        // 根据方向旋转图片
        __block float degrees = 0.0f;
        __block UIImage *temp = nil;
        weakify(self);
        dispatch_sync(dispatch_get_main_queue(), ^{
            strongify(self);
            if ([[UIApplication sharedApplication] statusBarOrientation] == UIInterfaceOrientationPortrait) {
                degrees = 90.0f;
            } else {
                degrees = -90.0f;
            }
            temp = [self rotatedByDegrees:degrees image:image];
            [self didReceiveImage:temp];
//            [self didReceiveImage:image];
        });

    };
//    strongify(self);
//    weakify(self);
    self.cameraCapture.cvMatBlock = ^(cv::Mat &image) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            strongify(self);
            cv::transpose(image, image);
            cv::flip(image, image, 1);
            [self didReceiveCVMat:image];
        });
    };
    [self.cameraCapture startSession];
}

- (void)setVideoPreview {
    self.preLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.cameraCapture.captureSession];
    self.preLayer.backgroundColor = [[UIColor redColor] CGColor];
//    UIEdgeInsets insets = self.view.safeAreaInsets;
//    CGRect screen = [[UIScreen mainScreen] bounds];
    self.preLayer.frame = self.view.frame;
    self.preLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self.view.layer addSublayer:self.preLayer];
}

- (void)disablePreview {
    [self.preLayer removeFromSuperlayer];
    self.preLayer = nil;
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/


#pragma mark - Lazy

- (ELCameraControlCapture *)cameraCapture {
    if (!_cameraCapture) {
        _cameraCapture = [[ELCameraControlCapture alloc] init];
    }
    return _cameraCapture;
}

@end
