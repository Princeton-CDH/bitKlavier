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

#include "PluginProcessor.h"

#include "Preparation.h"

class BKItem : public BKDraggableComponent, public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<BKItem>   Ptr;
    typedef Array<BKItem::Ptr>                  PtrArr;
    typedef ReferenceCountedArray<BKItem>       RCArr;
    
    BKItem(BKPreparationType type, int Id, BKAudioProcessor& p);
    ~BKItem(void);
    
    
    void mouseUp(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    
    void paint(Graphics& g) override;
    
    void resized(void) override;

    void connectWith(BKItem* item);
    
    bool isConnectedWith(BKItem* item);
    
    void disconnectFrom(BKItem* toDisconnect);
    
    void itemIsBeingDragged(const MouseEvent&) override;
    void keyPressedWhileSelected(const KeyPress&) override;
    
    inline BKPreparationType getType() const noexcept { return type; }
    inline int getId() const noexcept { return Id; }
    
    inline void setSelected(bool select) {isSelected = select; repaint();}
    inline bool getSelected(void) { return isSelected;}
    
    inline BKItem::PtrArr getConnections(void) const noexcept
    {
        return connections;
    }
    
    inline void print(void)
    {
        for (auto item : connections)
        {
            DBG(cPreparationTypes[type]+String(Id)+" ==> " +cPreparationTypes[item->getType()]+String(item->getId()));
        }
    }
    
    // Public members
    Point<float> origin;
    
private:
    BKAudioProcessor& processor;
    Label label;
    Component fullChild;
    
    
    BKItem::PtrArr connections;
    
    BKPreparationType type;
    int Id;
    String name;
    
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
    
    void add(BKItem* itemToAdd);
    bool contains(BKItem* thisItem);
    void remove(BKItem* itemToRemove);
    void connect(BKItem* item1, BKItem* item2);
    void disconnect(BKItem* item1, BKItem* item2);
    
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
    
    inline BKItem::RCArr getAllItems(void) const noexcept
    {
        return items;
    }
    
    inline BKItem* getItemAtXY(int x, int y) const noexcept
    {
        
    }
    
    Array<Line<float>> getLines(void);
    
    inline void drawLine(float ox, float oy, float ex, float ey)
    {
        //lines.add(Line<float>(ox,oy,ex,ey));
    }
    
    inline void print(void)
    {
        DBG("\n~ ~ ~ ~ ~ ~ ~ GRAPH ~ ~ ~ ~ ~ ~ ~:\n");
        for (auto item : items)
        {
            DBG(cPreparationTypes[item->getType()]+String(item->getId()));
            
            for (auto conn : item->getConnections())
                DBG("                   ==> " +cPreparationTypes[item->getType()]+String(item->getId()));
        }
        DBG("\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    }
    
    
    
private:
    BKAudioProcessor& processor;
    BKItem::RCArr items;
    
    void linkPreparationWithKeymap(bool link, BKPreparationType thisType, int thisId, Keymap::Ptr thisKeymap);
    void linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning);
    void linkSynchronicWithTempo(Synchronic::Ptr synchronic, Tempo::Ptr thisTempo);
    void linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic);
    
    void route(bool connect, BKItem* item1, BKItem* item2);
    
    void disconnectTuningFromSynchronic(BKItem* item);
    void disconnectTuningFromNostalgic(BKItem* item);
    void disconnectTuningFromDirect(BKItem* item);
    void disconnectTempoFromSynchronic(BKItem* synchronicItem);
    void disconnectSynchronicFromNostalgic(BKItem* thisItem);
    
    JUCE_LEAK_DETECTOR(BKItemGraph)
};



#endif  // BKGRAPH_H_INCLUDED
