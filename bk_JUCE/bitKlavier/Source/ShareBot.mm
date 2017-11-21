/*
  ==============================================================================

    ShareBot.mm
    Created: 15 Nov 2017 2:47:41pm
    Author:  airship

  ==============================================================================
*/

#import <Foundation/Foundation.h>

#include "ShareBot.h"

#if JUCE_MAC
#import <AppKit/AppKit.h>

//==============================================================================
@interface CocoaShareBot : NSObject<NSSharingServiceDelegate>
{
@private
    ShareBot* owner;
}
@end

@implementation CocoaShareBot

- (id) initWithOwner: (ShareBot*) owner_
{
    if ((self = [super init]) != nil)
    {
        owner = owner_;
    }
    
    return self;
}

- (void) dealloc
{
    [super dealloc];
}

- (void) share:(NSURL*)gallery to:(NSSharingServiceName)serviceName
{
    // Items to share
    NSString* tag = @"Created with bitKlavier (c).";
    NSAttributedString *text = [[NSAttributedString alloc] initWithString:tag];

    NSArray * shareItems = [NSArray arrayWithObjects:text, gallery, nil];
    
    NSSharingService *service = [NSSharingService sharingServiceNamed:serviceName];
    service.delegate = self;
    [service performWithItems:shareItems];
}

@end

#elif JUCE_IOS
#import <UIKit/UIKit.h>

//==============================================================================
@interface CocoaShareBot : UIViewController
{
@private
    ShareBot* owner;
    
    
}
@end

@implementation CocoaShareBot

- (id) initWithOwner: (ShareBot*) owner_
{
    if ((self = [super init]) != nil)
    {
        owner = owner_;
    }
    
    return self;
}

- (void) dealloc
{
    [super dealloc];
}

- (void) share:(NSURL*)gallery
{
    // Items to share
    NSString* tag = @"Created with bitKlavier (c).";
    NSAttributedString *text = [[NSAttributedString alloc] initWithString:tag];
    
    NSArray * shareItems = [NSArray arrayWithObjects:text, gallery, nil];
    
    UIActivityViewController *activityVC = [[UIActivityViewController alloc] initWithActivityItems:shareItems applicationActivities:nil];
    
    NSArray *excludeActivities = @[UIActivityTypeAirDrop,
                                   UIActivityTypePrint,
                                   UIActivityTypeAssignToContact,
                                   UIActivityTypeSaveToCameraRoll,
                                   UIActivityTypeAddToReadingList,
                                   UIActivityTypePostToFlickr,
                                   UIActivityTypePostToVimeo];
    
    activityVC.excludedActivityTypes = excludeActivities;
    
    [self presentViewController:activityVC animated:YES completion:nil];
}

@end



#endif

ShareBot::ShareBot(void)
{

}

ShareBot::~ShareBot(void)
{
    
}

void ShareBot::share(String galleryPath, int where)
{
    CocoaShareBot* bot = [[CocoaShareBot alloc] initWithOwner: this];

    [[UIApplication sharedApplication].keyWindow.rootViewController addChildViewController:bot];
    
    NSString* path = [[NSString stringWithCString:galleryPath.toRawUTF8() encoding:[NSString defaultCStringEncoding]] stringByRemovingPercentEncoding];
    
    NSURL* url = [NSURL fileURLWithPath:path];
    
#if JUCE_MAC
    NSSharingServiceName service;
    
    if (where == 0) service = NSSharingServiceNameComposeEmail;
    else if (where == 1) service = NSSharingServiceNameComposeMessage;
    else if (where == 2) service = NSSharingServiceNamePostOnFacebook;
    
    if (bot != nil) {
        [bot share:url to:service];
    }
#elif JUCE_IOS
    if (bot != nil)
    {
        [bot share:url];
    }
#endif
}



