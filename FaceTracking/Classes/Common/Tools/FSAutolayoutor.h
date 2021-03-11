//
//  FSAutolayoutor.h
//  FaceTracking
//
//  Created by tangkunyin on 16/3/7.
//  Copyright © 2016年 www.shuoit.net. All rights reserved.
//

#import "Masonry.h"

//等边距、全屏宽等上下、全屏高等左右
#define FS_EqualMargins(margin) UIEdgeInsetsMake(margin, margin, margin, margin)
#define FS_WidthEqualMargins(margin) UIEdgeInsetsMake(margin, 0, margin, 0)
#define FS_HeightEqualMargins(margin) UIEdgeInsetsMake(0, margin, 0, margin)


/**
 *  使用Masonry库封装的、介于自动布局和手动布局间的轻量级局相关工具。
 *  - 注意事项：
 *   1、部分方法需要设置子控件大小(CGSize)值，否则布局系统则会抛出异常；
 *   2、该工具请结合"FSUICreator"使用，具体用法参考DEMO；
 *   3、本类的方法仅适合【两个元素间】的相对定位，如果多余两个参考元素，请使用原生Masonry实现
 */
@interface FSAutolayoutor : NSObject


//中心定位或局部满屏定位
+ (void)layView:(UIView *)view fullOfTheView:(UIView *)superView;
+ (void)layView:(UIView *)view atCenterOfTheView:(UIView *)superView;
+ (void)layView:(UIView *)view atTheView:(UIView *)superView margins:(UIEdgeInsets)margins;


//以下三个方法均为左定位（左上，左中，左下）
+ (void)layView:(UIView *)subview atTheLeftTopOfTheView:(UIView *)container offset:(CGSize)offset;
+ (void)layView:(UIView *)subview atTheLeftMiddleOfTheView:(UIView *)container offset:(float)offset;
+ (void)layView:(UIView *)subview atTheLeftBottomOfTheView:(UIView *)container offset:(CGSize)offset;

//以下三个方法均为右定位（右上，右中，右下）
+ (void)layView:(UIView *)subview atTheRightTopOfTheView:(UIView *)container offset:(CGSize)offset;
+ (void)layView:(UIView *)subview atTheRightMiddleOfTheView:(UIView *)container offset:(float)offset;
+ (void)layView:(UIView *)subview atTheRightBottomOfTheView:(UIView *)container offset:(CGSize)offset;

//以下二个方法为上下定位（中上，中下）
+ (void)layView:(UIView *)subview atTheTopMiddleOfTheView:(UIView *)container offset:(float)offset;
+ (void)layView:(UIView *)subview atTheBottomMiddleOfTheView:(UIView *)container offset:(float)offset;


//以下方法均为外部定位，如果不指定type值，则默认在目标控件中间位置
+ (void)layView:(UIView *)sourceView toTheLeftOfTheView:(UIView *)targetView span:(float)span;
+ (void)layView:(UIView *)sourceView toTheLeftOfTheView:(UIView *)targetView span:(float)span alignmentType:(FSLayoutAlignmentType)alignmentType;


+ (void)layView:(UIView *)sourceView toTheRightOfTheView:(UIView *)targetView span:(float)span;
+ (void)layView:(UIView *)sourceView toTheRightOfTheView:(UIView *)targetView span:(float)span alignmentType:(FSLayoutAlignmentType)alignmentType;


+ (void)layView:(UIView *)sourceView aboveTheView:(UIView *)targetView span:(float)span;
+ (void)layView:(UIView *)sourceView aboveTheView:(UIView *)targetView span:(float)span alignmentType:(FSLayoutAlignmentType)alignmentType;

+ (void)layView:(UIView *)sourceView belowTheView:(UIView *)targetView span:(float)span;
+ (void)layView:(UIView *)sourceView belowTheView:(UIView *)targetView span:(float)span alignmentType:(FSLayoutAlignmentType)alignmentType;

@end
