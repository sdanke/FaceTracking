//
//  AddFaceController.m
//  FaceTracking
//
//  Created by sdanke on 2021/3/7.
//  Copyright © 2021 www.shuoit.net. All rights reserved.
//

#import "AddFaceController.h"

@interface AddFaceController ()
@property (weak, nonatomic) IBOutlet UITextField *textField;
@end

@implementation AddFaceController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.imageView.image = self.faceImage;
    // Do any additional setup after loading the view.
}
- (IBAction)addFace:(UIButton *)sender {
    if (self.textField.text.length == 0) {
        [self showToastTitle:@"提示" message:@"名字不能为空!"];
        return;
    }
    if (self.callback) {
        self.callback(self.textField.text);
    }
    
}
- (IBAction)cancel:(UIButton *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
