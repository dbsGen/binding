//
//  MyObject.m
//  Test2
//
//  Created by Gen on 15-1-21.
//  Copyright (c) 2015年 Gen. All rights reserved.
//

#import "MyObject.h"
#import <Foundation/Foundation.h>

@implementation MyObject


- (id)init {
    self = [super init];
    if (self) {
        _a = 12;
        self.b = 123;
    }
    return self;
}

+ (NSString*)getOutput:(NSString*)input
{
    return input;
}

@end
