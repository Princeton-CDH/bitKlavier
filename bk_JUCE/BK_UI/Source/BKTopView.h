/*
  ==============================================================================

    BKTopView.h
    Created: 27 Mar 2017 12:24:08pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKTOPVIEWCONTROLLER_H_INCLUDED
#define BKTOPVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "SynchronicViewController.h"
#include "NostalgicViewController.h"
#include "DirectViewController.h"
#include "TuningViewController.h"
#include "TempoViewController.h"

#include "GeneralViewController.h"

#include "PreparationMapViewController.h"
#include "KeymapViewController.h"

#include "BKComponent.h"
#include "BKListener.h"

class BKTopViewController : public BKComponent, public BKListener
{
public:
    BKTopViewController(BKAudioProcessor& p);
    ~BKTopViewController();
    
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
    KeymapViewController                  kvc;
    GeneralViewController                 gvc;
    SynchronicViewController              svc;
    NostalgicViewController               nvc;
    DirectViewController                  dvc;
    TuningViewController                  tvc;
    TempoViewController                   ovc;
    
    void drawNewPreparationMap(int Id);
    void removeLastPreparationMap(int Id);
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override{};
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    
    
    String                              processPianoMapString(const String& message);
    String                              processModMapString(const String& message);
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
    
    BKLabel                             modMapL;
    BKTextField                         modMapTF;
    
    BKLabel                             resetMapL;
    BKTextField                         resetMapTF;
    JUCE_LEAK_DETECTOR(BKTopViewController)
};


#endif  // BKTOPVIEWCONTROLLER_H_INCLUDED
