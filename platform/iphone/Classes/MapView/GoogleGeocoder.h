/*------------------------------------------------------------------------
* (The MIT License)
* 
* Copyright (c) 2008-2011 Rhomobile, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* http://rhomobile.com
*------------------------------------------------------------------------*/

#ifdef __IPHONE_3_0

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import "MapAnnotation.h"

//#define kGeoCodeURL @"https://maps.google.com/maps/geo?q="
#define kGeoCodeURL @"https://maps.googleapis.com/maps/api/geocode/json?address="

@interface GoogleGeocoder : NSObject {
    NSMutableArray *annotations;
    MapAnnotation* currentAnnotation;
    NSString *theElement;
    NSString *gapikey;
@public
    id  actionTarget;
    SEL onDidFindAddress;
}

// 
@property (assign) id  actionTarget;
@property (assign) SEL onDidFindAddress;
@property (nonatomic,retain) NSString *theElement;
@property (nonatomic,retain) NSMutableArray *annotations;
@property (copy) NSString* gapikey;

//
-(void)start;
-(id)initWithAnnotations:(NSMutableArray*)annotations apikey:(NSString*)key;

-(BOOL)geocode:(MapAnnotation *)annotation;
-(CLLocation*)stringCooridinatesToCLLocation;
@end


#endif
