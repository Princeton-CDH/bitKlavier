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

class BKConstructionSite : public LassoSource<BKItem*>, public MouseHoldListener
{
public:
    BKConstructionSite(BKAudioProcessor& p, /*Viewport* viewPort, */ BKItemGraph* theGraph);
    
    ~BKConstructionSite(void);
    
    void redraw(void);

    void paint(Graphics& g) override;
    
    void itemIsBeingDragged(BKItem* thisItem, const MouseEvent& e);
    
    void pianoMapDidChange(BKItem* thisItem);
    
    void move(int which, bool fine);
    void deleteSelected(void);
    void align(int which);
    void copy(void);
    BKItem::PtrArr duplicate(BKItem::PtrArr these);
    bool inPaste, inCopyDrag, inCut;
    void paste(void);
    void cut(void);
    void selectAll(void);
    
    inline int getNumSelected(void) { return graph->getSelectedItems().size(); }
    
    void addItemsFromClipboard(void);
    
    void addItem(BKPreparationType type, bool center = false);
    
    inline void setCurrentItem(BKItem* item) { currentItem = item;}
    inline BKItem* getCurrentItem(void) { return currentItem;}

    void idDidChange(void);
    
    bool altDown;
    
    inline void clear(void)
    {
        graph->clear();
    }
    
    bool itemOutsideBounds(Rectangle<int>);
    
    static void editMenuCallback(int result, BKConstructionSite*);
    
private:
    
    void mouseHold(Component* frame, bool onItem) override;
    
    int leftMost, rightMost, topMost, bottomMost;
    BKAudioProcessor& processor;
    
    BKItemGraph* graph;
    
    Point<int> lastPosition;

    bool connect; int lineOX, lineOY, lineEX, lineEY;
    bool multiple;
    bool held;

    
    
    BKItem* itemSource;
    BKItem* itemTarget;
    BKItem* itemToSelect;
    BKItem* lastItem;
    BKItem* currentItem;
    BKItem* upperLeftest;
    
    ScopedPointer<LassoComponent<BKItem*>> lasso;
    
    SelectedItemSet<BKItem*> selected;
    
    void findLassoItemsInArea (Array <BKItem*>& itemsFound,
                                                   const Rectangle<int>& area) override;
    
    SelectedItemSet<BKItem*>& getLassoSelection(void) override;

    void draw(void);
    
    void prepareItemDrag(BKItem* item, const MouseEvent& e, bool center);
    
    void resized() override;
    
    void mouseDown (const MouseEvent& eo) override;
    
    void mouseDoubleClick (const MouseEvent& eo) override;
    
    void mouseUp (const MouseEvent& eo) override;
    
    void mouseDrag (const MouseEvent& e) override;
    
    void mouseMove (const MouseEvent& e) override;
    
    void deleteItem (BKItem* item);
    
    BKItem* getItemAtPoint(const int X, const int Y);

    int lastX, lastY;
    int lastEX,lastEY;
    
    Component clickFrame;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    JUCE_LEAK_DETECTOR(BKConstructionSite)
};



#endif  // BKCONSTRUCTIONSITE_H_INCLUDED
