
#import "ICamera.h"
#import "CameraSingletonBase.h"

@interface CameraSingleton : CameraSingletonBase<ICameraSingleton> {
}


-(NSString*)getInitialDefaultID;


-(void) enumerate:(id<IMethodResult>)methodResult;
-(void) getCameraByType:(NSString*)cameraType methodResult:(id<IMethodResult>)methodResult;

-(void) choosePicture:(id<IMethodResult>)methodResult;
-(void) saveImageToDeviceGallery:(NSString*)pathToImage;


@end