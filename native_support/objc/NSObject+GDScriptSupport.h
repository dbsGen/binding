

@interface NSObject (GDScriptSupport)

- (void)addUserSignal:(NSString*)signal withParams:(NSArray*)params;
- (void)emitSignal:(NSString*)signal withParams:(NSArray*)params;

@end