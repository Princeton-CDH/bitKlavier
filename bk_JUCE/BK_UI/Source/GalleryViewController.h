/*
  ==============================================================================

    GalleryViewController.h
    Created: 27 Mar 2017 12:47:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef GALLERYVIEWCONTROLLER_H_INCLUDED
#define GALLERYVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "PreparationMapViewController.h"

#include "BKComponent.h"
#include "BKListener.h"

class GalleryViewController : public BKComponent, public BKListener
{
public:
    GalleryViewController(BKAudioProcessor& p);
    ~GalleryViewController();
    
    void update(void);
    void fillGalleryCB(void);
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    BKAudioProcessor& processor;
    
    float pmapH;
    Rectangle<int> upperLeft;
    void switchPianos(void);
    void switchGallery(void);
    
    Gallery::Ptr currentGallery;
    BKComponent*                          loadvc;
    BKComponent*                          pvc;
    PreparationMapViewController::PtrArr  pmvc;
    
    void drawNewPreparationMap(int Id);
    void removeLastPreparationMap(int Id);
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    
    
    String                              processPianoMapString(const String& message);
    String                              processmodificationMapString(const String& message);
    String                              processResetMapString(const String& message);
    
    TextButton                          addPMapButton;
    TextButton                          removePMapButton;
    TextButton                          saveButton, loadButton, loadJsonButton;
    TextButton                          removePianoButton;
    
    OwnedArray<TextButton>              loadButtons;
    
    BKLabel                             galleryL;
    BKComboBox                          galleryCB;
    
    BKLabel                             pianoL;
    BKComboBox                          pianoCB;
    
    BKLabel                             pianoMapL;
    BKTextField                         pianoMapTF;
    
    // Dynamify piano stuff/
   
    BKLabel                             pianoNameL;
    BKTextField                         pianoNameTF;
    
    
    BKLabel                             modificationMapL;
    BKTextField                         modificationMapTF;
    
    BKLabel                             resetMapL;
    BKTextField                         resetMapTF;
    JUCE_LEAK_DETECTOR(GalleryViewController)
};

#endif  // GALLERYVIEWCONTROLLER_H_INCLUDED
