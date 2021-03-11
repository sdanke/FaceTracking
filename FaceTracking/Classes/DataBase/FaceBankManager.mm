//
//  FaceBankManager.m
//  FaceTracking
//
//  Created by sdanke on 2020/10/18.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#import "FaceBankManager.h"

using namespace facereco;
@interface FaceBankManager () {
    FaceBank *_facebank;
}
@property (nonatomic, strong) NSMutableArray    *faceBankData;
@end

@implementation FaceBankManager
//SharedInstanceBuilder(FaceBankManager)
+ (instancetype)sharedInstance {
    static dispatch_once_t onceToken;
    static FaceBankManager* instance;
    dispatch_once(&onceToken, ^{
        instance = [[FaceBankManager alloc] init];
        [instance initFaceBank];
        [instance syncFromDisk];
    });
    return instance;
}

- (void)initFaceBank {
    _facebank = new FaceBank(0.4, 1.6);
    
}

- (void)releaseFaceBank {
    if (self->_facebank != nullptr) {
        delete self->_facebank;
        self->_facebank = nullptr;
    }
}

-(void)dealloc {
    [self releaseFaceBank];
}

- (void)syncFromDisk {
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"facebank" ofType:@"plist"];
    NSArray *diskData = [NSArray arrayWithContentsOfFile:filePath];
    [self.faceBankData removeAllObjects];
    [self.faceBankData addObjectsFromArray:diskData];
    _facebank->reset();
    for (NSDictionary *dict in self.faceBankData) {
        NSData *fea = dict[@"feature"];
        std::string label = [dict[@"name"] UTF8String];
        _facebank->append_feature((float *)fea.bytes, label);
    }
}

- (void)syncToDisk {
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"facebank" ofType:@"plist"];
    [self.faceBankData writeToFile:filePath atomically:YES];
}

- (void)update:(ncnn::Mat &)data name:(NSString *)name {
    std::string label = [name UTF8String];
    self->_facebank->update_feature(data, label);
    [self.faceBankData addObject:@{
        @"name": name,
        @"feature": [NSData dataWithBytes:data length:kFaceFeatureDims]
    }];
    [self syncToDisk];
}

- (NSString *)addNewFaceIfNeeded:(ncnn::Mat &)fea name:(NSString *)name {
    std::string retval = [self findFace:fea];
    [self update:fea name:name];
    if (retval == kFaceUnknownID) {
        
        return @"添加成功!";
    }
    return @"更新成功";
}

//- (NSString *)addNewFaceFeaIfNeeded:(FEATURE &)fea name:(NSString *)name {
//    std::string retval = [self findFaceFeature:fea];
//    [self update:fea name:name];
//    if (retval == kFaceUnknownID) {
//
//        return @"添加成功!";
//    }
//    return @"更新成功";
//}

- (std::string)findFace:(ncnn::Mat &)data {
    return _facebank->find_face(data);
}

- (std::string)findFaceFeature:(FEATURE &)data {
    return _facebank->find_face(data);
}

#pragma Lazy
- (NSMutableArray *)faceBankData {
    if (!_faceBankData) {
        _faceBankData = [NSMutableArray array];
    }
    return _faceBankData;
}
@end
