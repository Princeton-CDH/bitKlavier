/*
  ==============================================================================

    BKGraph.h
    Created: 6 Apr 2017 12:24:29pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKGRAPH_H_INCLUDED
#define BKGRAPH_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"
#include "BKListener.h"

#include "PluginProcessor.h"

#include "ItemMapper.h"

class BKItem : public ItemMapper, public BKDraggableComponent, public BKListener
{
public:
    typedef ReferenceCountedArray<BKItem>       RCArr;
    typedef ReferenceCountedObjectPtr<BKItem>   Ptr;
    typedef Array<Ptr>                          PtrArr;
    
    BKItem(BKPreparationType type, int Id, BKAudioProcessor& p);
    
    BKItem(ItemMapper::Ptr mapper, BKAudioProcessor& p);
    
    ~BKItem(void);
    
    void mouseDown(const MouseEvent& e) override;
    
    void paint(Graphics& g) override;
    
    void resized(void) override;
    
    void itemIsBeingDragged(const MouseEvent&, Point<int>) override;
    
    void keyPressedWhileSelected(const KeyPress&) override;

    void setItemType(BKPreparationType type, bool create);
    
    inline void setSelected(bool select) {isSelected = select; repaint();}
    inline bool getSelected(void) { return isSelected;}

    inline int getSelectedPianoId(void) const noexcept {return currentId;}
    inline void setSelectedPianoId(int Id)
    {
        menu.setSelectedItemIndex(Id, dontSendNotification);
    }
    
    inline Point<int> getItemPosition(void)
    {
        return getPosition();
    }
    
    inline void setItemBounds(int X, int Y, int width, int height)
    {
        DBG("SET X: " + String(X) + " Y: " + String(Y));
        
        setBounds(X,Y,width,height);
        
        saveBounds(getBounds());
    }
    
    void copy(BKItem::Ptr);
    // Public members
    Point<float> origin;
    
    Image image;
    RectanglePlacement placement;
    
    void bkTextFieldDidChange   (TextEditor&)           override {};
    void bkComboBoxDidChange    (ComboBox*)             override;
    void bkButtonClicked        (Button* b)             override {};
    void bkMessageReceived      (const String& message) override {};
    
    Point<int> lastClick;
    
    Point<int> position;
    
    void setImage(Image newImage);
    
private:
    BKAudioProcessor& processor;
    Label label;
    
    // Piano stuff
    BKComboBox menu; int currentId;
    
    BKItem::PtrArr connections;
    
    // UI stuff
    Component fullChild;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKItem)
};

class BKItemGraph
{
public:
    BKItemGraph(BKAudioProcessor& p):
    processor(p)
    {
        
    }
    
    ~BKItemGraph(void)
    {
        
    }

    BKItem::Ptr get(BKPreparationType type, int Id);
    
    void addAndRegisterItem(BKItem* item);
    void addItem(BKItem* item);
    void registerItem(BKItem* item);
    
    void removeItem(BKItem* thisItem);
    void unregisterItem(BKItem* thisItem);
    void removeAndUnregisterItem(BKItem* thisItem);
    
    bool contains(BKItem* thisItem);
    bool contains(BKPreparationType type, int Id);
    
    void removeUI(BKItem* itemToRemove);
    void removeKeymap(BKItem* itemToRemove);
    void clear(void);
    void connectUI(BKItem* item1, BKItem* item2);
    void disconnectUI(BKItem* item1, BKItem* item2);
    void connect(BKItem* item1, BKItem* item2);
    void connectWithoutCreatingNew(BKItem* item1, BKItem* item2);
    void disconnect(BKItem* item1, BKItem* item2);
    
    void reconnect(BKItem* item1, BKItem* item2);
    
    BKItem::Ptr getItem(ItemMapper::Ptr mapper);
    BKItem::Ptr createItem(ItemMapper::Ptr mapper);
    
    void update(BKPreparationType type, int which);
    void updateMod(BKPreparationType modType, int modId);
    
    void reconstruct(void);
    
    inline BKPreparationType getModType(BKPreparationType type)
    {
        return (BKPreparationType)(type+6);
    }
    
    inline bool isValidConnection(BKPreparationType type1, BKPreparationType type2)
    {
        if (type1 == PreparationTypeDirect)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeTuning ||
                type2 == PreparationTypeDirectMod ||
                type2 == PreparationTypeGenericMod ||
                type2 == PreparationTypeReset)
                return true;
        }
        else if (type1 == PreparationTypeNostalgic)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeTuning ||
                type2 == PreparationTypeNostalgicMod ||
                type2 == PreparationTypeGenericMod ||
                type2 == PreparationTypeSynchronic ||
                type2 == PreparationTypeReset)
                return true;
        }
        else if (type1 == PreparationTypeSynchronic)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeTuning ||
                type2 == PreparationTypeSynchronicMod ||
                type2 == PreparationTypeGenericMod ||
                type2 == PreparationTypeNostalgic ||
                type2 == PreparationTypeTempo ||
                type2 == PreparationTypeReset)
                return true;
        }
        else if (type1 == PreparationTypeTuning)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeSynchronic ||
                type2 == PreparationTypeDirect ||
                type2 == PreparationTypeNostalgic ||
                type2 == PreparationTypeTuningMod ||
                type2 == PreparationTypeGenericMod ||
                type2 == PreparationTypeReset)
                return true;
        }
        else if (type1 == PreparationTypeTempo)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeSynchronic ||
                type2 == PreparationTypeTempoMod ||
                type2 == PreparationTypeGenericMod ||
                type2 == PreparationTypeReset)
                return true;
        }
        else if (type1 == PreparationTypeKeymap)
        {
            if (type2 != PreparationTypeKeymap)
                return true;
        }
        else if (type1 == PreparationTypeDirectMod)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeDirect)
                return true;
        }
        else if (type1 == PreparationTypeNostalgicMod)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeNostalgic)
                return true;
        }
        else if (type1 == PreparationTypeSynchronicMod)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeSynchronic)
                return true;
        }
        else if (type1 == PreparationTypeTuningMod)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeTuning)
                return true;
        }
        else if (type1 == PreparationTypeTempoMod)
        {
            if (type2 == PreparationTypeKeymap ||
                type2 == PreparationTypeTempo)
                return true;
        }
        else if (type1 == PreparationTypeReset)
        {
            if (type2 == PreparationTypeKeymap ||
                (type2 >= PreparationTypeDirect && type2 <= PreparationTypeTempo))
                return true;
        }
        else if (type1 == PreparationTypePianoMap)
        {
            if (type2 == PreparationTypeKeymap)
                return true;
        }
        else if (type1 == PreparationTypeGenericMod)
        {
            if (type2 == PreparationTypeKeymap ||
                (type2 >= PreparationTypeDirect && type2 <= PreparationTypeTempo))
                return true;
        }
        
        
        return false;
    }
    
    inline void select(BKItem* item)
    {
        item->setSelected(true);
    }
    
    inline void deselect(BKItem* item)
    {
        item->setSelected(false);
    }
    
    inline void deselectAll(void)
    {
        for (auto item : items) item->setSelected(false);
    }
    
    inline void selectAll(void)
    {
        for (auto item : items) item->setSelected(true);
    }
    
    inline BKItem::PtrArr getSelectedItems(void) const noexcept
    {
        BKItem::PtrArr selectedItems;
        
        for (auto item : items)
        {
            if (item->getSelected()) selectedItems.add(item);
        }
        
        return selectedItems;
    }
    
    inline BKItem::RCArr getItems(void)
    {
        return items;
    }
    
    
    inline Array<int> getPreparationIds(BKItem::PtrArr theseItems)
    {
        Array<int> whichPreps;
        for (auto item : theseItems) whichPreps.add(item->getId());
        
        return whichPreps;
    }
    
    
    Array<Line<int>> getLines(void);
    
    inline void print(void)
    {
        DBG("\n~ ~ ~ ~ ~ ~ ~ GRAPH ~ ~ ~ ~ ~ ~ ~:\n");
        for (auto item : items)
        {
            item->print();
        }
        DBG("\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    }
    
    void updateClipboard(void);

    BKItem::RCArr clipboard;
    
    
    int itemIdCount;
    
private:
    BKAudioProcessor& processor;
    
    BKItem::RCArr items;

    void addPreparationToKeymap(BKPreparationType thisType, int thisId, int keymapId);
    void removePreparationFromKeymap(BKPreparationType thisType, int thisId, int keymapId);
    void linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning);
    void linkSynchronicWithTempo(Synchronic::Ptr synchronic, Tempo::Ptr thisTempo);
    void linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic);
    
    void route(bool connect, bool reconfigure, BKItem* item1, BKItem* item2);
    
    void disconnectTuningFromSynchronic(BKItem* item);
    void disconnectTuningFromNostalgic(BKItem* item);
    void disconnectTuningFromDirect(BKItem* item);
    void disconnectTempoFromSynchronic(BKItem* synchronicItem);
    void disconnectSynchronicFromNostalgic(BKItem* thisItem);
    
    JUCE_LEAK_DETECTOR(BKItemGraph)
};



#endif  // BKGRAPH_H_INCLUDED
