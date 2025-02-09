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

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#define TOOLBAR_TYPE		0
#define TABBAR_TYPE			1
#define NOBAR_TYPE			2
#define VTABBAR_TYPE		3

#define NATIVE_BAR_PROPERTIES						@"properties"
#define NATIVE_BAR_ITEMS							@"items"

#define NATIVE_BAR_BACKGOUND_COLOR					@"backgroundColor"
#define NATIVE_BAR_ON_CHANGE_TAB_CALLBACK           @"on_change_tab_callback"
#define NATIVE_BAR_ICON_COLOR                       @"iOS7IconColor"
#define NATIVE_BAR_SELECTED_COLOR					@"iOS7SelectedColor"
#define NATIVE_BAR_DETAIL_COLOR                     @"detailColor"
#define NATIVE_BAR_MAX_WIDTH                        @"maxWidth"
#define NATIVE_BAR_FIXED_WIDTH                      @"fixedWidth"
#define NATIVE_BAR_TABBAR_INVISIBLE                 @"invisibleTabControl"

#define NATIVE_BAR_ITEM_LABEL						@"label"
#define NATIVE_BAR_ITEM_DETAIL_LABEL				@"detailLabel"
#define NATIVE_BAR_ITEM_ACTION						@"action"
#define NATIVE_BAR_ITEM_ICON						@"icon"
#define NATIVE_BAR_ITEM_RELOAD						@"reload"
#define NATIVE_BAR_ITEM_COLORED_ICON				@"coloredIcon"
#define NATIVE_BAR_ITEM_SELECTED_COLOR				@"selectedColor"
#define NATIVE_BAR_ITEM_DISABLED					@"disabled"
#define NATIVE_BAR_ITEM_WEB_BACKGROUND_COLOR		@"backgroundColor"
#define NATIVE_BAR_ITEM_USE_CURRENT_VIEW_FOR_TAB	@"useCurrentViewForTab"
#define NATIVE_BAR_ITEM_BACK_IMAGE                  @"backgroundImage"
#define NATIVE_BAR_ITEM_BACK_SEL_IMAGE              @"backgroundSelectedImage"
#define NATIVE_BAR_ITEM_GROUP_BOTTOM                @"groupBottom"
#define NATIVE_BAR_ITEM_INDENT_WIDTH                @"indentWidth"
#define NATIVE_BAR_ITEM_LABEL_SEL_COLOR             @"labelSelectedColor"
#define NATIVE_BAR_ITEM_DETAIL_SEL_COLOR            @"detailSelectedColor"

/*
 
struture of data with Toolbar/TabBar description
 
NSDictioanry
  "properties" - NSDictionary
                   "bkg_color" - NSString
 
  "items" - NSArray
			  NSDictionary
                "label" - NSString
                "action" - NSString
                "icon" - NSString
                "reload" - NSString
                "colored_icon" - NSString
                "selected_color" - NSString
                "disabled" - NSString
                "bkg_color" - NSString
 
*/
