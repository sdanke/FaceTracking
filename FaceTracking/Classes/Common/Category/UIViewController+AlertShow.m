//
//  UIViewController+AlertShow.m
//  FaceTracking
//
//  Created by sdanke on 2020/10/18.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import "UIViewController+AlertShow.h"
#import <SCLAlertView.h>

@implementation UIViewController (AlertShow)

- (void)showWarning:(NSString *)title message:(NSString *)message {
    SCLAlertView *alert = [[SCLAlertView alloc] init];
    [alert showWarning:self title:title subTitle:message closeButtonTitle:@"确认" duration:1.5f]; // Warning

}
@end
