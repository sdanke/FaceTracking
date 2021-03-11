//
//  NewFaceController.h
//  FaceTracking
//
//  Created by sdanke on 2020/10/12.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ELCameraControlCapture;
NS_ASSUME_NONNULL_BEGIN

@interface BaseCameraController : UIViewController
@property (nonatomic, strong, readonly) ELCameraControlCapture *cameraCapture;

- (void)setVideoPreview;
- (void)disablePreview;
- (void)didReceiveImage:(UIImage *)image;
- (void)didReceiveCVMat:(cv::Mat &)image;
@end

NS_ASSUME_NONNULL_END
