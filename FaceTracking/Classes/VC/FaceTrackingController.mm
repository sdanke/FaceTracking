//
//  NewFaceController.m
//  FaceTracking
//
//  Created by sdanke on 2020/10/13.
//  Copyright © 2020 www.shuoit.net. All rights reserved.
//

#include "face_reco_manager.h"
#include "facebank.h"
#include "Utils.hpp"

#import <opencv2/imgcodecs/ios.h>
#import "FaceTrackingController.h"
#import "ELCameraControlCapture.h"
#import <SCLAlertView.h>
#import "FaceBankManager.h"
#import "AddFaceController.h"

using namespace facereco;
@interface FaceTrackingController () {
    FaceRecoManager *_frmanager;
    std::vector<TrackResult> _curDets;
    
}
@property (weak, nonatomic) IBOutlet UIImageView *cameraView;
@property (weak, nonatomic) IBOutlet UIView *widgeView;

@property (strong, nonatomic) UIImage *currentFrame;
@property (strong, nonatomic) FaceBankManager *faceBankManager;
@property (strong, nonatomic) NSMutableDictionary *trackNameMap;
@end

@implementation FaceTrackingController

- (void)viewDidLoad {
    [super viewDidLoad];
//    [self.view setNeedsLayout];
//    self->_curDets = new std::vector<TrackResult>;
    self.cameraView.x = 0;
    self.cameraView.y = 0;
    self.cameraView.width = self.view.width;
    self.cameraView.height = self.view.height;
    [self disablePreview];
    [self.view bringSubviewToFront:self.widgeView];
    
    [self setupManager];
//    self.originCanvas = [self.canvasView.image copy];
    // Do any additional setup after loading the view.
}

- (void)dealloc {
    if (self->_frmanager != nullptr) {
        delete self->_frmanager;
        self->_frmanager = nullptr;
    }
//    if (self->_curDets != nullptr) {
//        delete self->_curDets;
//        self->_curDets = nullptr;
//    }
    
}

# pragma Face Tacking Manager
- (void)setupManager {
    int det_insize = 320;
    int emb_insize = 112;
    float max_cosine_distance = 0.2;
    int nn_budget = 64;
    
    NSString *detParam = [[NSBundle mainBundle] pathForResource:@"face" ofType:@"param"];
    NSString *detBin = [[NSBundle mainBundle] pathForResource:@"face" ofType:@"bin"];
    
    NSString *embParam = [[NSBundle mainBundle] pathForResource:@"mfn_sim" ofType:@"param"];
    NSString *embBin = [[NSBundle mainBundle] pathForResource:@"mfn_sim" ofType:@"bin"];
    
    self->_frmanager = new FaceRecoManager(det_insize, false, emb_insize, max_cosine_distance, nn_budget);
    self->_frmanager->load_model([detParam UTF8String], [detBin UTF8String], [embParam UTF8String], [embBin UTF8String]);
    
}
- (IBAction)didClickClose:(UIButton *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)didClickChange:(UIButton *)sender {
    [self.cameraCapture switchCamera];
}


- (IBAction)didClickAppend:(UIButton *)sender {
    cv::Mat input;
    UIImageToMat(self.currentFrame, input);
    cv::cvtColor(input, input, cv::COLOR_RGBA2BGR);
    std::vector<DetResult> boxes;
    std::vector<cv::Mat> faceImages;
    std::vector<ncnn::Mat> features;
    int retval = self->_frmanager->detect_embedding(input, boxes, faceImages, features);
    if (retval != 1) {
        [self showWarning:@"警告" message:@"未识别到人脸"];
        return;
    }
    cv::Mat faceMat;
    cv::cvtColor(faceImages[0], faceMat, cv::COLOR_BGR2RGB);
    UIImage *faceImage = MatToUIImage(faceMat);
    [self showSubmitWithFaceImage:faceImage feature:features[0]];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    //get touch position relative to main view
    CGPoint point = [[touches anyObject] locationInView:self.view];
    //convert point to the white layer's coordinates
    point = [self.cameraView.layer convertPoint:point fromLayer:self.cameraView.layer];
    CGFloat x_scale = self.cameraView.image.size.width / self.cameraView.width;
    CGFloat y_scale = self.cameraView.image.size.height / self.cameraView.height;
    point.x *= x_scale;
    point.y *= y_scale;
    for (TrackResult &det : _curDets) {
        if (point.x > det.x and point.x < det.x + det.w and point.y > det.y and point.y < det.y + det.h) {
            UIImage *faceImg = [self.cameraView.image getSubImage:CGRectMake(det.x, det.y, det.w, det.h)];
            ncnn::Mat feature(det.feature.rows(), det.feature.cols(), 1, (uchar*)det.feature.data());
//            eigen2ncnn(det.feature, feature);
            [self showAddFaceVCWithImage:faceImg feature:feature];
            break;
        }
    }
}

- (void)showAddFaceVCWithImage:(UIImage *)image feature:(ncnn::Mat &)feature {
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"main" bundle:nil];
    AddFaceController * vc = (AddFaceController *)[storyboard instantiateViewControllerWithIdentifier:@"AddFaceController"];
    vc.faceImage = image;
    __block ncnn::Mat fea = feature;
    weakify(self);
    vc.callback = ^(NSString * _Nonnull name) {
        strongify(self);
        NSString *message = [self.faceBankManager addNewFaceIfNeeded:fea name:name];
        [self showToastTitle:@"提醒" message:message];
    };
    [self showDetailViewController:vc sender:nil];

}
- (void)showSubmitWithFaceImage:(UIImage *)image feature:(ncnn::Mat &)feature {
    NSString *title = @"添加新成员";
    NSString *message = @"请输入新成员姓名";
    NSString *cancel = @"取消";
    NSString *done = @"确认";
    __block ncnn::Mat fea = feature;
    SCLALertViewTextFieldBuilder *textField = [SCLALertViewTextFieldBuilder new].title(@"姓名");
    
    weakify(self);
    SCLALertViewButtonBuilder *doneButton = [SCLALertViewButtonBuilder new].title(done)
    .validationBlock(^BOOL{
        NSString *code = [textField.textField.text copy];
        return [code length] > 0;
    })
    .actionBlock(^{
        strongify(self);
        NSString *text = [textField.textField.text copy];
        NSString *message = [self.faceBankManager addNewFaceIfNeeded:fea name:text];
        [self showToastTitle:@"提醒" message:message];
    });
    
    SCLAlertViewBuilder *builder = [SCLAlertViewBuilder new]
    .showAnimationType(SCLAlertViewShowAnimationFadeIn)
    .hideAnimationType(SCLAlertViewHideAnimationFadeOut)
    .shouldDismissOnTapOutside(NO)
    .addTextFieldWithBuilder(textField)
    .addButtonWithBuilder(doneButton);
    
    SCLAlertViewShowBuilder *showBuilder = [SCLAlertViewShowBuilder new]
    .style(SCLAlertViewStyleCustom)
    .image(image)
    .color([UIColor blueColor])
    .title(title)
    .subTitle(message)
    .closeButtonTitle(cancel)
    .duration(0.0f);

    [showBuilder showAlertView:builder.alertView onViewController:self];
}

- (void)didReceiveImage:(UIImage *)image {
    self.currentFrame = [image copy];
//    std::vector<TrackResult> results;
    _curDets.clear();
    cv::Mat frame;
    UIImageToMat(image, frame, true);
    cv::cvtColor(frame, frame, cv::COLOR_RGBA2BGR);
    self->_frmanager->tracking_cv(frame, _curDets);
    [self drawRectsOn:image rectsInfo:_curDets];
}

//- (void)didReceiveCVMat:(cv::Mat &)image {
//    std::vector<TrackResult> results;
//    cv::cvtColor(image, image, cv::COLOR_BGRA2BGR);
//    self->_frmanager->tracking_cv(image, results);
//
//    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
//    self.currentFrame = MatToUIImage(image);
//
//    [self drawRectsOn:[self.currentFrame copy] rectsInfo:results];
//}

- (NSString *)getTrackLabel:(int)trackID feature:(FEATURE &)feature {
    NSString *key = [NSString stringWithFormat:@"%d", trackID];
    if ([self.trackNameMap objectForKey:key]) {
        return self.trackNameMap[key];
    }
    std::string name = [self.faceBankManager findFaceFeature:feature];
    
    if (name != kFaceUnknownID) {
        self.trackNameMap[key] = [NSString stringWithUTF8String:name.c_str()];
        return self.trackNameMap[key];
    }
    return @"Person";
}

- (void)drawRectsOn:(UIImage *)image rectsInfo:(std::vector<TrackResult> &)rectsInfo {
    if (rectsInfo.size() == 0) {
        self.cameraView.image = image;
        return;
    }
    UIGraphicsBeginImageContext(image.size);
    [image drawAtPoint:CGPointMake(0, 0)];
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, fmax(image.size.width/200, 1));
    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];

    
//    for(TrackResult &box : rectsInfo) {
    for (int i = 0; i < rectsInfo.size(); ++i) {
            
        TrackResult box = rectsInfo[i];
        UIColor *color = [UIColor greenColor];
        CGContextAddRect(context, CGRectMake(box.x, box.y, box.w, box.h));
//        std::string name = [self.faceBankManager findFaceFeature:box.feature];
        NSString *label = [self getTrackLabel:box.track_id feature:box.feature];
//        NSString *label = @"Person";
        [label drawAtPoint:CGPointMake(box.x, box.y) withAttributes:@{NSFontAttributeName:[UIFont systemFontOfSize:35], NSParagraphStyleAttributeName:style, NSForegroundColorAttributeName:color}];
        
        CGContextSetStrokeColorWithColor(context, [color CGColor]);
        CGContextStrokePath(context);
    }
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    self.cameraView.image = newImage;
}

- (FaceBankManager *)faceBankManager {
    return  [FaceBankManager sharedInstance];
}

- (NSMutableDictionary *)trackNameMap {
    if (!_trackNameMap) {
        _trackNameMap = [NSMutableDictionary dictionary];
    }
    return _trackNameMap;
}
@end
