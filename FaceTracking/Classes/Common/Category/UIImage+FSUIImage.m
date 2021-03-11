//
//  UIImage+FSUIImage.m
//  FaceTracking
//
//  Created by tangkunyin on 16/3/10.
//  Copyright © 2016年 www.shuoit.net. All rights reserved.
//

#import "UIImage+FSUIImage.h"

@implementation UIImage (FSUIImage)

+ (UIImage *)imageWithNamed:(NSString *)name;
{
    UIImage *image = [[self imageNamed:name] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
    return image;
}

-(UIImage*)getSubImage:(CGRect)rect
{
    CGImageRef subImageRef = CGImageCreateWithImageInRect(self.CGImage, rect);
    CGRect smallBounds = CGRectMake(0, 0, CGImageGetWidth(subImageRef), CGImageGetHeight(subImageRef));
    
    UIGraphicsBeginImageContext(smallBounds.size);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextDrawImage(context, smallBounds, subImageRef);
    UIImage* smallImage = [UIImage imageWithCGImage:subImageRef];
    UIGraphicsEndImageContext();
    
    return smallImage;
}

@end
