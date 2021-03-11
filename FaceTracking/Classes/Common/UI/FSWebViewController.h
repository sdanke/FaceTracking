//
//  FSWebViewController.h
//  FaceTracking
//
//  Created by tangkunyin on 16/3/7.
//  Copyright © 2016年 www.shuoit.net. All rights reserved.
//

#import "FSBaseSubViewController.h"

@interface FSWebViewController : FSBaseSubViewController <WKUIDelegate, WKNavigationDelegate>

@property (nonatomic, strong) WKWebView *webView;

- (instancetype)initWithTitle:(NSString *)title webUrl:(NSString *)url;

@end
