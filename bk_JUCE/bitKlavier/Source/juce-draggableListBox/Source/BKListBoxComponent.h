#pragma once
#include "JuceHeader.h"
#include "MyListComponent.h"
#include "../../PluginProcessor.h"
#include "../../BKLookAndFeel.h"
#include "../../BKKeyboard.h"
#include "../../BKKeyboardState.h"

class BKListBoxComponent   : public BKComponent,
public BKKeymapKeyboardStateListener, public BKListener, public ChangeListener
{
public:
    BKListBoxComponent(BKAudioProcessor &p);
    virtual ~BKListBoxComponent()
    {
        //used to communicate with iterator from from mainview
        p.updateState->iteratorViewActive = false;
        
    }
    
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    void paint (Graphics&) override;
    void resized() override;
    bool keyPressed(const KeyPress&) override;
    virtual void bkButtonClicked (Button* b) override;
    PopupMenu getPianoOptionMenu();
    PopupMenu getMidiInputSelectMenu()
    {
        BKPopupMenu menu;
        
        Keymap::Ptr keymap = p.gallery->getIteratorUpKeymap();
        
        int id = 1;
        if (keymap->isDefaultSelected())
        {
            menu.addItem(PopupMenu::Item(cMidiInputDefaultDisplay).setID(id).setTicked(true));
        }
        else menu.addItem(PopupMenu::Item(cMidiInputDefaultDisplay).setID(id));
        
        id++;
        if (keymap->isOnscreenSelected())
        {
            menu.addItem(PopupMenu::Item("Onscreen Keyboard").setID(id).setTicked(true));
        }
        else menu.addItem(PopupMenu::Item("Onscreen Keyboard").setID(id));
        
        for (auto device : p.getMidiInputDevices())
        {
            if (keymap->getMidiInputIdentifiers().contains(device.identifier))
                menu.addItem(PopupMenu::Item(device.name).setID(++id).setTicked(true));
            else menu.addItem(PopupMenu::Item(device.name).setID(++id));
        }
        return std::move(menu);
    }
    static void pianoOptionMenuCallback(int res, BKListBoxComponent*);
    static void midiInputSelectCallback(int result, BKListBoxComponent* vc)
    {
        if (result <= 0) return;
        
        BKAudioProcessor& processor = vc->p;
        
        Keymap::Ptr keymap = processor.gallery->getIteratorUpKeymap();
        Keymap::Ptr _keymap = processor.gallery->getIteratorDownKeymap();
        if (result == 1)
        {
            keymap->setDefaultSelected(!keymap->isDefaultSelected());
            _keymap->setDefaultSelected(!_keymap->isDefaultSelected());
        }
        else if (result == 2)
        {
            keymap->setOnscreenSelected(!keymap->isOnscreenSelected());
            _keymap->setOnscreenSelected(!_keymap->isOnscreenSelected());
        }
        else
        {
            MidiDeviceInfo device = processor.getMidiInputDevices()[result-3];
            if (keymap->getMidiInputIdentifiers().contains(device.identifier))
            {
                keymap->removeMidiInputSource(device);
                _keymap->removeMidiInputSource(device);
            }
            else
            {
                keymap->addMidiInputSource(device);
                _keymap->addMidiInputSource(device);
            }
        }

//        vc->getMidiInputSelectMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(vc->midiInputSelectButton), ModalCallbackFunction::forComponent(midiInputSelectCallback, vc));
//        
        processor.updateState->editsMade = true;
    }
    
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    void setListBoxRow(int index)
    {
        listBox.selectRow(index);
    }
private:
    BKButtonAndMenuLAF laf;
    TextButton addBtn;
    
    ToggleButton iteratorOnOff;
    //Combobox pianoMenu;
    Label upKeyboardLabel;
    BKKeymapKeyboardState upKeyboardState;
    std::unique_ptr<BKKeymapKeyboardComponent> upKeyboard;
    BKTextButton midiInputSelectButton;
    Label downKeyboardLabel;
    BKKeymapKeyboardState downKeyboardState;
    std::unique_ptr<BKKeymapKeyboardComponent> downKeyboard;
    juce::AudioPluginInstance::WrapperType wrapperType;

    MyListBoxItemData& itemData;
    MyListBoxModel listBoxModel;
    DraggableListBox listBox;
    BKAudioProcessor &p;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKListBoxComponent)
};

//==============================================================================
/**
 An effect filter that simple makes image monochrome.
 (This will only work on images/components that aren't opaque, of course).
 @see Component::setComponentEffect
 */
class MonochromeEffect  : public ImageEffectFilter
{
public:
    //==============================================================================
    /** Creates a default drop-shadow effect.
     To customise the shadow's appearance, use the setShadowProperties() method.
     @param backgroundColour (optional) - for some alpha values you might prefer a background (such as black).
     */
    MonochromeEffect(juce::Colour backgroundColour = juce::Colours::transparentBlack) { bgColour_ = backgroundColour; }

    /** Destructor. */
    ~MonochromeEffect() {}

    //==============================================================================
    /** @internal */
    void applyEffect (juce::Image& image, juce::Graphics& g, float scaleFactor, float alpha)
    {
        g.fillAll(bgColour_);
        image.desaturate();
        g.setOpacity (alpha);
        g.drawImageAt (image, 0, 0);
    }
private:
    //==============================================================================
    juce::Colour bgColour_;
    JUCE_LEAK_DETECTOR (MonochromeEffect)
};
