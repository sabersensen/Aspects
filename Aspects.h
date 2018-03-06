//
//  Aspects.h
//  Aspects - A delightful, simple library for aspect oriented programming.
//
//  Copyright (c) 2014 Peter Steinberger. Licensed under the MIT license.
//  Aspects 主要是给view controller 使用 而不是用于高频率hook 比如每秒1000次

#import <Foundation/Foundation.h>


/**
 hook执行时期

 - AspectPositionAfter: 原方法执行完之后调用 (default)
 - AspectPositionInstead: 替换原方法
 - AspectPositionBefore: 原方法执行完之前调用
 - AspectOptionAutomaticRemoval: 在hook执行完自动移除
 */
typedef NS_OPTIONS(NSUInteger, AspectOptions) {
    AspectPositionAfter   = 0,            
    AspectPositionInstead = 1,
    AspectPositionBefore  = 2,
    
    AspectOptionAutomaticRemoval = 1 << 3
};

/**
 隐式协议：允许我们调用remove去撤销一个hook
 */
@protocol AspectToken <NSObject>

/**
 撤销一个aspect

 @return 撤销成功返回YES,否则返回NO
 */
- (BOOL)remove;

@end

/**
  AspectInfo protocol是我们block语法里面的第一个参数。
 */
@protocol AspectInfo <NSObject>

/**
 当前被hooked的实例

 @return 实例对象
 */
- (id)instance;

/**
 返回被hooked方法的原始的invocation

 @return invocation
 */
- (NSInvocation *)originalInvocation;

/**
 懒加载

 @return 返回所有参数
 */
- (NSArray *)arguments;

@end

/**
 Aspects利用的OC的消息转发机制来进行hook。这样会有一些性能开销。不要把Aspects加到经常被使用的方法里面。Aspects是用来设计给view/controller 代码使用的，而不是用来hook每秒调用1000次的方法的。
 
 添加Aspects之后，会返回一个隐式的token，这个token可以用来注销hook方法的。所有的调用都是线程安全的。
 */
@interface NSObject (Aspects)

/**
 这些参数都会被填充到匹配的block的签名里
 你也可以使用一个空block，或者一个简单的id<AspectInfo>
 不支持hook静态static方法的

 @param selector 是要给它增加切面的原方法 返回一个遵循<AspectInfo>的id对象，这个对象继承了方法的所有参数，
 @param options 是代表这个切片增加在原方法的before / instead / after
 @param block 这个block复制了正在被hook的方法的签名signature类型
 @param error 错误信息
 @return 返回一个可以用来撤销aspect的token
 */
+ (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;

/**
 hook实例方法 为一个具体实例的seletor的执行 之前/或者被替换/之后 添加一个block代码
 */
- (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;

@end


/**
 错误码的类型

 - AspectErrorSelectorBlacklisted: 这些选择器如release，retain，autorelease被加入黑名单
 - AspectErrorDoesNotRespondToSelector: 选择器未找到
 - AspectErrorSelectorDeallocPosition: 当正在执行的hook被释放时，只有原方法执行之前被允许
 - AspectErrorSelectorAlreadyHookedInClassHierarchy: 静态hook子类相同的方法是不被允许的
 - AspectErrorFailedToAllocateClassPair: runtime创建一个类失败
 - AspectErrorMissingBlockSignature: block没有被签名编译不能被唤起
 - AspectErrorIncompatibleBlockSignature: 签名block没有匹配到方法或者太大
 - AspectErrorRemoveObjectAlreadyDeallocated: 已经准备释放(for removing)
 */
typedef NS_ENUM(NSUInteger, AspectErrorCode) {
    AspectErrorSelectorBlacklisted = 1,
    AspectErrorDoesNotRespondToSelector = 2,
    AspectErrorSelectorDeallocPosition = 3,
    AspectErrorSelectorAlreadyHookedInClassHierarchy = 4,
    AspectErrorFailedToAllocateClassPair = 5,
    AspectErrorMissingBlockSignature = 6,
    AspectErrorIncompatibleBlockSignature = 7,

    AspectErrorRemoveObjectAlreadyDeallocated = 100
};

/// 声明Apsect的ErrorDomain
extern NSString *const AspectErrorDomain;
