//
//  MyObject.h
//  Test2
//
//  Created by Gen on 15-1-21.
//  Copyright (c) 2015年 Gen. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef struct _Struct {
    int a,b,c;
} Struce;

@interface MyObject : NSObject {
    int _a;
}

@property (nonatomic, assign) float b;

+ (NSString*)getOutput:(NSString*)input;

@end
