//
//  UIViewController+AlertShow.h
//  FaceTracking
//
//  Created by sdanke on 2020/10/18.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (AlertShow)
- (void)showWarning:(NSString *)title message:(NSString *)message;
@end

NS_ASSUME_NONNULL_END
