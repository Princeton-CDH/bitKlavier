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

#include "BKViewController.h"

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
    
    struct TouchEvent
    {
        TouchEvent (const MouseInputSource& ms)
        : source (ms)
        {}
        
        void pushPoint (juce::Point<float> newPoint, ModifierKeys newMods)
        {
            currentPosition = newPoint;
            modifierKeys = newMods;
            
            if (lastPoint.getDistanceFrom (newPoint) > 5.0f)
            {
                if (lastPoint != juce::Point<float>())
                {
                    Path newSegment;
                    newSegment.startNewSubPath (lastPoint);
                    newSegment.lineTo (newPoint);
                    
                    PathStrokeType (1.0f, PathStrokeType::curved, PathStrokeType::rounded).createStrokedPath (newSegment, newSegment);
                    path.addPath (newSegment);
                    
                }
                
                lastPoint = newPoint;
            }
        }
        
        MouseInputSource source;
        Path path;
        juce::Point<float> lastPoint, currentPosition;
        ModifierKeys modifierKeys;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TouchEvent)
    };
    
    void drawTouch (TouchEvent& touch, Graphics& g)
    {
        g.setColour (Colours::antiquewhite);
        g.fillPath (touch.path);
        
        const float radius = 10.0f;
        
        g.drawEllipse (touch.currentPosition.x - radius,
                       touch.currentPosition.y - radius,
                       radius * 2.0f, radius * 2.0f, 2.0f);
        
        g.setFont (14.0f);
        
        String desc ("Mouse #");
        desc << touch.source.getIndex();
        
        float pressure = touch.source.getCurrentPressure();
        
        if (pressure > 0.0f && pressure < 1.0f)
            desc << "  (pressure: " << (int) (pressure * 100.0f) << "%)";
        
        if (touch.modifierKeys.isCommandDown()) desc << " (CMD)";
        if (touch.modifierKeys.isShiftDown())   desc << " (SHIFT)";
        if (touch.modifierKeys.isCtrlDown())    desc << " (CTRL)";
        if (touch.modifierKeys.isAltDown())     desc << " (ALT)";
        
        g.drawText (desc,
                    Rectangle<int> ((int) touch.currentPosition.x - 200,
                                    (int) touch.currentPosition.y - 60,
                                    400, 20),
                    Justification::centredTop, false);
    }
    
    inline int getNumberOfTouches(void)
    {
        return touches.size();
    }
    
    bool edittingComment;
    
    OwnedArray<HashMap<int,int>> pastemap;
private:
    
    OwnedArray<TouchEvent> touches;
    
    TouchEvent* getTouchEvent (const MouseInputSource& source)
    {
        for (int i = 0; i < touches.size(); ++i)
        {
            TouchEvent* t = touches.getUnchecked(i);
            
            if (t->source == source)
                return t;
        }
        
        return nullptr;
    }
    
    void mouseHold(Component* frame, bool onItem) override;
    
    int leftMost, rightMost, topMost, bottomMost;
    BKAudioProcessor& processor;
    
    BKItemGraph* graph;
    
    juce::Point<int> lastPosition;

    bool connect; int lineOX, lineOY, lineEX, lineEY;
    bool multiple;
    bool held;

    
    
    BKItem* itemSource;
    BKItem* itemTarget;
    BKItem* itemToSelect;
    BKItem* lastItem;
    BKItem* currentItem;
    BKItem* upperLeftest;
    
    std::unique_ptr<LassoComponent<BKItem*>> lasso;
    
    SelectedItemSet<BKItem*> selected;
    
    void findLassoItemsInArea (Array <BKItem*>& itemsFound,
                                                   const Rectangle<int>& area) override;
    
    SelectedItemSet<BKItem*>& getLassoSelection(void) override;

    void draw(void);
    
    void prepareItemDrag(BKItem* item, const MouseEvent& e, bool center);
    
    void resized() override;
    
    void mouseDown (const MouseEvent& eo) override;
    
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
