//
//  RhoMainView.h
//  rhorunner
//
//  Created by Dmitry Moskalchuk on 07.03.10.
//  Copyright 2010 Rhomobile Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol RhoMainView

- (UIView*)getView;

- (void)loadHTMLString:(NSString*)data;

- (void)back:(int)index;
- (void)forward:(int)index;
- (void)navigate:(NSString*)url tab:(int)index;
- (void)reload:(int)index;

- (void)executeJs:(NSString*)js tab:(int)index;

- (NSString*)currentLocation:(int)index;

- (void)switchTab:(int)index;
- (int)activeTab;

@end
