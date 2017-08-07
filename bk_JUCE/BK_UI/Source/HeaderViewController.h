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

class HeaderViewController : public BKComponent, public BKListener, public BKEditableComboBoxListener
{
public:
    HeaderViewController(BKAudioProcessor& p);
    ~HeaderViewController();
    
    void update(void);
    void fillGalleryCB(void);
    void fillPianoCB(void);
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    BKAudioProcessor& processor;
    
    void switchGallery(void);
    
    Gallery::Ptr currentGallery;
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    void BKEditableComboBoxChanged(String text, BKEditableComboBox* cb) override;
    
    static void loadMenuCallback(int result, HeaderViewController*);
    static void pianoMenuCallback(int result, HeaderViewController*);
    static void galleryMenuCallback(int result, HeaderViewController*);
    
    PopupMenu getLoadMenu(void);
    PopupMenu getPianoMenu(void);
    PopupMenu getGalleryMenu(void);
    //OwnedArray<PopupMenu> submenus;
    //StringArray submenuNames;
    
    TextButton  loadB;
    TextButton  pianoB;
    TextButton  galleryB;
    
    BKComboBox galleryCB;
    BKEditableComboBox pianoCB;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    JUCE_LEAK_DETECTOR(HeaderViewController)
};

#endif  // GALLERYVIEWCONTROLLER_H_INCLUDED
