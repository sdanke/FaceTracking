//
//  UIImage+FSUIImage.h
//  FaceTracking
//
//  Created by tangkunyin on 16/3/10.
//  Copyright © 2016年 www.shuoit.net. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (FSUIImage)

+ (UIImage *)imageWithNamed:(NSString *)name;

-(UIImage*)getSubImage:(CGRect)rect;
@end
