//
//  FaceBankManager.h
//  FaceTracking
//
//  Created by sdanke on 2020/10/18.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//
#include "facebank.h"
#include <ncnn/ncnn/mat.h>
#import <Foundation/Foundation.h>


//NS_ASSUME_NONNULL_BEGIN

@interface FaceBankManager : NSObject
//SharedInstanceInterfaceBuilder(FaceBankManager)
+ (instancetype)sharedInstance;

- (void)syncFromDisk;
- (void)syncToDisk;
- (void)update:(ncnn::Mat &)data name:(NSString *)name;
- (NSString *)addNewFaceIfNeeded:(ncnn::Mat &)fea name:(NSString *)name;
- (std::string)findFace:(ncnn::Mat &)data;
- (std::string)findFaceFeature:(FEATURE &)data;
@end

//NS_ASSUME_NONNULL_END
