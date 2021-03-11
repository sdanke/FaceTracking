
//
//  AppMacros.h
//  FaceTracking
//
//  Created by tangkunyin on 16/3/7.
//  Copyright © 2016年 www.shuoit.net. All rights reserved.
//


// 开发模式选项，可自定义功能。上线时一定要关掉
#define isDevelopModel 1

// -------------------------------------------------    尺寸定义开始  -------------------------------//
//屏幕宽高定义
#define kVCWIDTH self.view.frame.size.width
#define kVCHEIGHT self.view.frame.size.height
#define KDeviceWidth [UIScreen mainScreen].bounds.size.width
#define KDeviceHeight [UIScreen mainScreen].bounds.size.height
#define FullViewWidth self.frame.size.width
#define FullViewHeight self.frame.size.height

#define SINGLE_LINE_ADJUST_OFFSET   ((1 / [UIScreen mainScreen].scale) / 2)

//满屏CGRect
#define FullScreenRect [UIScreen mainScreen].bounds
#define FullVCRect CGRectMake(0,0, KDeviceWidth, KDeviceHeight-64)
#define FullTabbarVCRect CGRectMake(0,0, KDeviceWidth, KDeviceHeight-64-49)

// -------------------------------------------------    颜色定义开始  -------------------------------//
#define COLOR(r,g,b) [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1]
#define GRAYCOLOR(c) COLOR(c,c,c)
#define ColorWithAlpha(r,g,b,a) [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:a]
#define UIColorFromRGBValue(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]
#define RandomColorWithAlpha(a) [UIColor colorWithRed:arc4random_uniform(255)/255.0 green:arc4random_uniform(255)/255.0 blue:arc4random_uniform(255)/255.0 alpha:a]

//通用视觉规范
#define FSWhiteColor COLOR(255, 255, 255)          //纯白
#define FSBlueColor COLOR(0, 153, 255)             //蓝色
#define FSOrangeColor COLOR(253, 106, 60)          //橙色
#define FSYellowColor COLOR(239, 166, 68)          //黄色
#define FSBlackColor COLOR(68, 68, 68)             //浅黑色
#define FSGrayColor COLOR(102, 102, 102)           //灰色
#define FSlightGrayColor COLOR(153, 153, 153)      //浅灰色
#define FSCoffeeColor COLOR(185,152,99)            //咖啡色
#define FSCommonBgColor  COLOR(248, 248, 248)      //灰白色
#define FSSeparatorLineColor COLOR(232, 232, 232)  //分割线颜色
#define FSTranslucentColor ColorWithAlpha(0,0,0,0.55)  //半透明颜色

// -------------------------------------------------    字体定义开始  -------------------------------//
#define SysFontWithSize(s) [UIFont systemFontOfSize:s]
#define PFNFontWithSize(s) [UIFont fontWithName:@"PingFangSC-Regular" size:s]
#define PFBFontWithSize(s) [UIFont fontWithName:@"PingFangSC-Thin" size:s]
#define HTFontWithSize(s) [UIFont fontWithName:@"Heiti SC" size:s]

// -------------------------------------------------    工具定义开始  -------------------------------//
//定义构造单例的宏
#define SharedInstanceInterfaceBuilder(ClassName) \
+ (instancetype)sharedInstance;

#define SharedInstanceBuilder(ClassName) \
+ (instancetype)sharedInstance\
{\
    static dispatch_once_t onceToken;\
    static ClassName* instance;\
    dispatch_once(&onceToken, ^{\
    instance = [[ClassName alloc] init];\
    });\
    return instance;\
}

//"performSelector may cause a leak because its selector is unknown"警告原因及其解决办法
#define SuppressPerformSelectorLeakWarning(Stuff) \
do { \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Warc-performSelector-leaks\"") \
Stuff; \
_Pragma("clang diagnostic pop") \
} while (0)

//"MethodDeprecatedSelectorWarning"方法过时解决办法(严重不推荐使用，除非做了严格判断知道会出问题时用来消除警告)
#define MethodDeprecatedSelectorWarning(Stuff) \
do { \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"") \
Stuff; \
_Pragma("clang diagnostic pop") \
} while (0)

//app版本信息(升级版本号和编译版本号)
#define AppVersionNumber [[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleShortVersionString"]
#define AppBuildVersionNumber [[[NSBundle mainBundle] infoDictionary] valueForKey:(NSString *)kCFBundleVersionKey]

//日志打印
#if isDevelopModel
#define FSLog(...) NSLog(__VA_ARGS__);NSLog(@"Function is: %s line at: %d" ,__PRETTY_FUNCTION__, __LINE__)
#else
#define FSLog(...) ((void)0)
#endif

//拨打电话
#define makeCall(phoneNumber) [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"tel://%@",phoneNumber]]];


// -------------------------------------------------    UIKit自定义定义开始  -------------------------------//

//tableView 分割线左边距归零
#define clearTableViewSeparator \
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath\
{\
    if ([cell respondsToSelector:@selector(setSeparatorInset:)]){\
        [cell setSeparatorInset:UIEdgeInsetsZero];\
    }\
    if ([cell respondsToSelector:@selector(setLayoutMargins:)]){\
        [cell setLayoutMargins:UIEdgeInsetsZero];\
    }\
}

#define setTableViewSeparatorZeroMargin(_tableView) \
if ([_tableView respondsToSelector:@selector(setSeparatorInset:)]){\
    [_tableView setSeparatorInset:UIEdgeInsetsZero];\
}\
if ([_tableView respondsToSelector:@selector(setLayoutMargins:)]){\
    [_tableView setLayoutMargins:UIEdgeInsetsZero];\
}
