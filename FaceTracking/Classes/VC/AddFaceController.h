//
//  AddFaceController.h
//  FaceTracking
//
//  Created by sdanke on 2021/3/7.
//  Copyright © 2021 www.shuoit.net. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AddFaceController : UIViewController
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (strong, nonatomic) UIImage            *faceImage;

@property (nonatomic, copy) void (^callback)(NSString *name);

@end

NS_ASSUME_NONNULL_END
