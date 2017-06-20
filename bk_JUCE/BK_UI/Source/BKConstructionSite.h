/*
  ==============================================================================

    BKConstructionSite.h
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKCONSTRUCTIONSITE_H_INCLUDED
#define BKCONSTRUCTIONSITE_H_INCLUDED

#include "BKUtilities.h"

#include "BKListener.h"
#include "BKComponent.h"

#include "PluginProcessor.h"

#include "BKGraph.h"

class BKConstructionSite : public BKDraggableComponent, public KeyListener, public LassoSource<BKItem*>
{
public:
    BKConstructionSite(BKAudioProcessor& p, BKItemGraph* theGraph);
    
    ~BKConstructionSite(void);
    
    inline void redraw(void);

    void paint(Graphics& g) override;
    
    void itemIsBeingDragged(BKItem* thisItem, Point<int> startPosition);
    
    void pianoMapDidChange(BKItem* thisItem);
    
    void move(int which, bool fine);
    void remove(void);
    void align(int which);
    
    void addItem(BKPreparationType type, int which);
    
    inline void setCurrentItem(BKItem* item) { currentItem = item;}
    inline BKItem* getCurrentItem(void) { return currentItem;}

    void idDidChange(void);
    
private:
    
    int leftMost, rightMost, topMost, bottomMost;
    BKAudioProcessor& processor;
    
    BKItemGraph* graph;
    
    Point<int> lastPosition;

    bool connect; int lineOX, lineOY, lineEX, lineEY;
    bool multiple;
    
    BKItem* itemSource;
    BKItem* itemTarget;
    BKItem* itemToSelect;
    BKItem* lastItem;
    BKItem* currentItem;
    
    LassoComponent<BKItem*>* lasso;
    
    SelectedItemSet<BKItem*> selected;
    
    void findLassoItemsInArea (Array <BKItem*>& itemsFound,
                                                   const Rectangle<int>& area) override;
    
    SelectedItemSet<BKItem*>& getLassoSelection(void) override;
    
    
    
    void draw(void);
    
    void prepareItemDrag(BKItem* item, const MouseEvent& e, bool center);
    
    // Drag interface
    void itemWasDropped(BKPreparationType type, Array<int> data, int x, int y) override;

    
    void mouseDown (const MouseEvent& eo) override;
    
    void mouseUp (const MouseEvent& eo) override;
    
    void mouseDrag (const MouseEvent& e) override;
    
    void deleteItem (BKItem* item);
    
    bool keyPressed (const KeyPress& e, Component*) override;
    
    BKItem* getItemAtPoint(const int X, const int Y);

    int lastDownX, lastDownY;
    int lastAddedX, lastAddedY;
    
    
    
    JUCE_LEAK_DETECTOR(BKConstructionSite)
};



#endif  // BKCONSTRUCTIONSITE_H_INCLUDED
