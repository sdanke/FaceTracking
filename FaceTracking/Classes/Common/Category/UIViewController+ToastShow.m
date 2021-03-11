//
//  UIViewController+ToastShow.m
//  FaceTracking
//
//  Created by sdanke on 2020/10/20.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import "UIViewController+ToastShow.h"
#import <RKDropdownAlert.h>
@implementation UIViewController (ToastShow)

- (void)showToastTitle:(NSString *)title message:(NSString *)message {
    [RKDropdownAlert title:title message:message];
}
@end
