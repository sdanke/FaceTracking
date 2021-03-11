//
//  FSMarkDownView.h
//  FaceTracking
//
//  Created by tangkunyin on 2018/9/8.
//  Copyright © 2018年 www.shuoit.net. All rights reserved.
//

#import <UIKit/UIKit.h>

// 支持MarkDown预览的View
@interface FSMarkDownView : WKWebView

/**
 * standard markDown text content
 * @return content
 */
@property (nonatomic, copy) NSString *content;


/**
 * standard markDown text content url
 * @return content
 */
@property (nonatomic, copy) NSString *contentUrl;



@property (nonatomic, nonnull, copy, readonly) NSString *htmlString;


- (instancetype)initWithFrame:(CGRect)frame;

@end
