#import <Foundation/Foundation.h>
@class UIView;

@interface TriangleRenderer : NSObject
- (instancetype)initWithView:(UIView*)view;
- (void)draw;
@end

