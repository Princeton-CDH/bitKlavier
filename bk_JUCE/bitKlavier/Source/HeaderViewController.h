/*
  ==============================================================================

    HeaderViewController.h
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef GALLERYVIEWCONTROLLER_H_INCLUDED
#define GALLERYVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKComponent.h"
#include "BKListener.h"

#include "BKConstructionSite.h"

class HeaderViewController : public BKComponent, public BKListener, public BKEditableComboBoxListener
{
public:
    HeaderViewController(BKAudioProcessor& p, BKConstructionSite* construction);
    ~HeaderViewController();
    
    void update(void);
    void fillGalleryCB(void);
    void fillPianoCB(void);
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void switchGallery(void);
    
    bool handleGalleryChange(void);
    
private:
    BKAudioProcessor& processor;
    
    int galleryIsDirtyAlertResult;

    Gallery::Ptr currentGallery;
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    void BKEditableComboBoxChanged(String text, BKEditableComboBox* cb) override;
    
    static void pianoMenuCallback(int result, HeaderViewController*);
    static void galleryMenuCallback(int result, HeaderViewController*);
    
    
    
    void addGalleriesFromFolder(File folder);
    
    PopupMenu getLoadMenu(void);
    PopupMenu getPianoMenu(void);
    PopupMenu getGalleryMenu(void);
    
    //OwnedArray<PopupMenu> submenus;
    //StringArray submenuNames;
    
    TextButton  editB;
    TextButton  pianoB;
    TextButton  galleryB;
    
    BKEditableComboBox galleryCB;
    BKEditableComboBox pianoCB;
    int lastGalleryCBId;
    bool galleryModalCallBackIsOpen;
    
    BKConstructionSite* construction;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    JUCE_LEAK_DETECTOR(HeaderViewController)
};

#endif  // GALLERYVIEWCONTROLLER_H_INCLUDED
