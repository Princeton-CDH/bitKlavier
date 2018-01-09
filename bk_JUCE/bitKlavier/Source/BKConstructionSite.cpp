/*
  ==============================================================================

    BKConstructionSite.cpp
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKConstructionSite.h"

#include "MainViewController.h"

#define NUM_COL 6

#define DRAW_MULTITOUCH 0

BKConstructionSite::BKConstructionSite(BKAudioProcessor& p, /*Viewport* vp,*/ BKItemGraph* theGraph):
altDown(false),
processor(p),
graph(theGraph),
connect(false),
lastX(10),
lastY(10),
held(false),
edittingComment(false)
{
    addAndMakeVisible(clickFrame);
    clickFrame.setSize(5,5);
    
    setWantsKeyboardFocus(false);
    graph->deselectAll();
    
    addMouseListener(this, true);
}

BKConstructionSite::~BKConstructionSite(void)
{
}

void BKConstructionSite::resized()
{
    repaint();
}

void BKConstructionSite::paint(Graphics& g)
{
    g.setColour(Colours::burlywood.withMultipliedBrightness(0.4));
    g.fillAll();
    
    if (connect)
    {
        g.setColour(Colours::lightgrey);
        
        
        g.drawLine(lineOX, lineOY, lineEX, lineEY, (processor.platform == BKIOS) ? 2 : 3);
        
    }
    
    for (auto line : graph->getLines())
    {
        g.setColour(Colours::goldenrod);

        g.drawLine(line.getStartX(), line.getStartY(), line.getEndX(), line.getEndY(), (processor.platform == BKIOS) ? 1 : 2);
    }
    
#if DRAW_MULTITOUCH
    for (int i = 0; i < touches.size(); ++i)
        drawTouch (*touches.getUnchecked(i), g);
#endif
}

bool BKConstructionSite::itemOutsideBounds(Rectangle<int> bounds)
{
    for (auto item : graph->getItems())
    {
        juce::Point<int> xy = item->getPosition();
        DBG("XY TEST: " + String(xy.x) + " " + String(xy.y));
        
        DBG("bounds: " + String(bounds.getX()) + " " + String(bounds.getHeight()));
        if ((xy.x > bounds.getWidth()) || (xy.y > bounds.getHeight())) return true;
    }
    return false;
}

void BKConstructionSite::redraw(void)
{
    removeAllChildren();
    
    graph->reconstruct();
    
    draw();
}

void BKConstructionSite::move(int which, bool fine)
{
    if (processor.updateState->currentDisplay != DisplayNil) return;
    
    float changeX = 0;
    float changeY = 0;
    
    if (which == 0) // Up
        changeY = fine ? -2 : -10;
    else if (which == 1) // Right
        changeX = fine ? 2 : 10;
    else if (which == 2) // Down
        changeY = fine ? 2 : 10;
    else if (which == 3) // Left
        changeX = fine ? -2 : -10;
    
    for (auto item : graph->getSelectedItems())
        item->setTopLeftPosition(item->getX() + changeX, item->getY() + changeY);
    
    repaint();
}

void BKConstructionSite::selectAll(void)
{
    graph->selectAll();
    repaint();
}

void BKConstructionSite::deleteSelected(void)
{
    BKItem::PtrArr selectedItems = graph->getSelectedItems();
    
    for (int i = selectedItems.size(); --i >= 0;)
    {
        selectedItems[i]->print();
        deleteItem(selectedItems[i]);
    }
    
    selected.deselectAll();
    
    selectedItems.clear();
}

void BKConstructionSite::align(int which)
{
    if (processor.updateState->currentDisplay != DisplayNil) return;
    
    if (graph->getSelectedItems().size() <= 1) return;
    
    bool top = false, bottom = false, left = false, right = false;
    
    float mostTop = getBottom(), mostBottom = 0.0, mostLeft = getRight(), mostRight = 0.0;
    
    for (auto item : graph->getSelectedItems())
    {
        float X = item->getX() + item->getWidth() / 2;
        float Y = item->getY() + item->getHeight() / 2;
        
        if (X < mostLeft)   mostLeft = X;
        
        if (X > mostRight)  mostRight = X;
        
        if (Y < mostTop)    mostTop = Y;
        
        if (Y > mostBottom) mostBottom = Y;
    }
    
    if (which == 0)         top = true;
    else if (which == 1)    right = true;
    else if (which == 2)    bottom = true;
    else if (which == 3)    left = true;
    
    for (auto item : graph->getSelectedItems())
    {
        float X = (left ? mostLeft : (right ? mostRight : item->getX() + item->getWidth() / 2));
        float Y = (top ? mostTop : (bottom ? mostBottom : item->getY() + item->getHeight() / 2));
        
        item->setCentrePosition(X, Y);
    }
    
    repaint();
}

void BKConstructionSite::itemIsBeingDragged(BKItem* thisItem, const MouseEvent& e)
{
    repaint();
}

void BKConstructionSite::pianoMapDidChange(BKItem* thisItem)
{
    processor.currentPiano->configure();
}

void BKConstructionSite::draw(void)
{
    for (auto item : processor.currentPiano->getItems()) addAndMakeVisible(item);
        
    if (processor.updateState->loadedJson)
    {
        int keymapCount = 0, prepCount = 0, otherCount = 0, modCount = 0, ttCount = 0;
        
        for (auto item : graph->getItems())
        {
            BKPreparationType type = item->getType();
            
            if (type == PreparationTypeKeymap)
            {

                int col = (int)(keymapCount / NUM_COL);
                int row = keymapCount % NUM_COL;
                
                int X = 10 + (row * 155);
                int Y = 50 + (col * 25);
                
                item->setTopLeftPosition(X, Y);
                
                keymapCount++;
            }
            else if (type <= PreparationTypeNostalgic)
            {

                int col = (int)(prepCount / NUM_COL);
                int row = prepCount % NUM_COL;
                
                int X = 10 + (row * 155);
                int Y = 350 + (col * 25);
                
                item->setTopLeftPosition(X, Y);
                
                prepCount++;
                
            }
            else if (type == PreparationTypeTuning || type == PreparationTypeTempo)
            {

                int col = (int)(ttCount / NUM_COL);
                int row = ttCount % NUM_COL;
                
                int X = 10 + (row * 155);
                int Y = 500 + (col * 25);
                
                
                item->setTopLeftPosition(X, Y);
                
                ttCount++;

            }
            else if (type > PreparationTypeKeymap)
            {

                int col = (int)(modCount / NUM_COL);
                int row = modCount % NUM_COL;
                
                int X = 10 + (row * 155);
                int Y = 200 + (col * 25);
                
                
                item->setTopLeftPosition(X, Y);
                
                modCount++;

            }
            else
            {

                int col = (int)(otherCount / NUM_COL);
                int row = otherCount % NUM_COL;
                
                int X = 10 + (row * 155);
                int Y = 350 + (col * 25);
                
                item->setTopLeftPosition(X, Y);
                
                otherCount++;
            }
            DBG("itemxy: " + String(item->getX()) + " " + String(item->getY()));
        }
    }
    
    repaint();
}


void BKConstructionSite::prepareItemDrag(BKItem* item, const MouseEvent& e, bool center)
{
    if (center)
    {
        float X = item->getPosition().getX() + item->getWidth() / 2.0f;
        float Y = item->getPosition().getY() + item->getHeight() / 2.0f;
        juce::Point<float>pos(X,Y);
        MouseEvent newEvent = e.withNewPosition(pos);
        
        item->prepareDrag(newEvent);
    }
    else
    {
        item->prepareDrag(e);
    }
}
void BKConstructionSite::deleteItem (BKItem* item)
{
    graph->removeItem(item);
    removeChildComponent(item);
}

void BKConstructionSite::addItem(BKPreparationType type, bool center)
{
    int thisId = -1;
    
    if (type != PreparationTypeGenericMod && type != PreparationTypeComment)
    {
        thisId = processor.gallery->getNewId(type);
        
        processor.gallery->addTypeWithId(type, thisId);
    }
    
    BKItem* toAdd = new BKItem(type, thisId, processor);
    
    if (type == PreparationTypePianoMap)
    {
        toAdd->setPianoTarget(processor.currentPiano->getId());
        toAdd->configurePianoCB();
    }
    else if (type == PreparationTypeComment)
    {
        toAdd->configureComment();
    }
    
#if JUCE_IOS
    toAdd->setTopLeftPosition(lastX, lastY);
#endif
    
#if JUCE_MAC || JUCE_WINDOWS
    if (center)
    {
        toAdd->setTopLeftPosition(300, 250);
    }
    else
    {
        toAdd->setTopLeftPosition(lastX, lastY);
    }
#endif
    
    lastX += 10; lastY += 10;
    
    graph->addItem(toAdd);
    
    addAndMakeVisible(toAdd);
}

BKItem::PtrArr BKConstructionSite::duplicate(BKItem::PtrArr these)
{
    int distFromOrigin = 20000;
    
    BKItem::PtrArr clipboard;
    
    for (auto item : these)
    {
        BKItem* newItem = new BKItem(item->getType(), item->getId(), processor);
        
        // relying on mouse anyway
        newItem->setBounds(item->getX(),
                           item->getY(),
                           item->getWidth(),
                           item->getHeight());
        
        float dist = newItem->getPosition().getDistanceFromOrigin();
        
        if (dist < distFromOrigin)
        {
            distFromOrigin = dist;
            upperLeftest = newItem;
        }
        
        clipboard.add(newItem);
    }
    
    for (int i = 0; i < these.size(); i++)
    {
        BKItem* thisItem = these[i];
        BKItem* clipboardItem = clipboard[i];
        
        for (auto connection : thisItem->getConnections())
        {
            BKPreparationType connectionType = connection->getType();
            int connectionId = connection->getId();
            
            BKItem* connectionItem = nullptr;
            
            for (auto item : clipboard)
            {
                if (item->getType() == connectionType && item->getId() == connectionId) connectionItem = item;
            }
            
            if (connectionItem != nullptr)
            {
                clipboardItem->addConnection(connectionItem);
                connectionItem->addConnection(clipboardItem);
            }
        }
    }
    
    return clipboard;
}

void BKConstructionSite::copy(void)
{
    
    BKItem::PtrArr selected = graph->getSelectedItems();

    processor.setClipboard(duplicate(selected));
    
    getParentComponent()->grabKeyboardFocus();
}

void BKConstructionSite::paste(void)
{
    BKItem::PtrArr clipboard = processor.getClipboard();
    
    if (!clipboard.size() || (upperLeftest == nullptr)) return;
    
    BKItem::PtrArr newItems = duplicate(clipboard);
    
    graph->deselectAll();
    
    int offsetX = upperLeftest->getX(); int offsetY = upperLeftest->getY();
    
    for (auto item : newItems)
    {
        BKPreparationType type = item->getType(); int Id = item->getId();

        if (processor.currentPiano->contains(type, Id))
        {
            // duplicate (all we need to do is set to new Id and add to gallery)
            int newId = processor.gallery->duplicate(type, Id);
            
            item->setId(newId);
            
        }
        
        item->setSelected(true);
        
        item->setTopLeftPosition((item->getX()-offsetX) + lastEX, (item->getY()-offsetY) + lastEY);
        
        processor.currentPiano->add(item);
        
        newItems.add(item);
    }
    
    processor.currentPiano->configure();
    
    redraw();
    
    getParentComponent()->grabKeyboardFocus();
}

void BKConstructionSite::cut(void)
{
    copy();
    
    for (auto item : graph->getSelectedItems()) deleteItem(item);
    
    graph->deselectAll();
    
    getParentComponent()->grabKeyboardFocus();
}

#if TRY_UNDO
void BKConstructionSite::undo(void)
{
    processor.timeTravel(false);
}

void BKConstructionSite::redo(void)
{
    processor.timeTravel(true);
}
#endif

void BKConstructionSite::mouseMove (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    if (e.x != lastEX) lastX = e.x;
    
    if (e.y != lastEY) lastY = e.y;
    
    lastEX = e.x;
    lastEY = e.y;
}

void BKConstructionSite::editMenuCallback(int result, BKConstructionSite* vc)
{
    BKAudioProcessor& processor = vc->processor;
    
#if TRY_UNDO
    if (result == CUT_ID ||
        result == PASTE_ID ||
        result == ALIGN_VERTICAL ||
        result == ALIGN_HORIZONTAL ||
        result == DELETE_ID ||
        result == KEYMAP_ID ||
        result == DIRECT_ID ||
        result == NOSTALGIC_ID ||
        result == SYNCHRONIC_ID ||
        result == TEMPO_ID ||
        result == TUNING_ID||
        result == MODIFICATION_ID||
        result == PIANOMAP_ID||
        result == RESET_ID)
    {
        processor.updateHistory();
    }
    
#endif
    
    
    if (result == OFF_ID)
    {
        // OFF
        processor.clearBitKlavier();
    }
    else if (result == KEYBOARD_ID)
    {
        ((MainViewController*)vc->getParentComponent())->toggleDisplay();
    }
    else if (result == EDIT_ID)
    {
        vc->processor.updateState->setCurrentDisplay(vc->currentItem->getType(), vc->currentItem->getId());
    }
    else if (result == COPY_ID)
    {
        vc->copy();
    }
    else if (result == CUT_ID)
    {
        vc->cut();
    }
    else if (result == PASTE_ID)
    {
        vc->paste();
    }
#if TRY_UNDO
    else if (result == UNDO_ID)
    {
        vc->undo();
    }
    else if (result == REDO_ID)
    {
        vc->redo();
    }
#endif
    else if (result == ALIGN_VERTICAL)
    {
        vc->align(0);
    }
    else if (result == ALIGN_HORIZONTAL)
    {
        vc->align(3);
    }
    else if (result == DELETE_ID)
    {
        vc->deleteSelected();
    }
    else if (result == COMMENT_ID)
    {
        vc->addItem(PreparationTypeComment, true);
    }
    else if (result == KEYMAP_ID)
    {
        vc->addItem(PreparationTypeKeymap, true);
    }
    else if (result == DIRECT_ID)
    {
        vc->addItem(PreparationTypeDirect, true);
    }
    else if (result == NOSTALGIC_ID)
    {
        vc->addItem(PreparationTypeNostalgic, true);
    }
    else if (result == SYNCHRONIC_ID)
    {
        vc->addItem(PreparationTypeSynchronic, true);
    }
    else if (result == TUNING_ID)
    {
        vc->addItem(PreparationTypeTuning, true);
    }
    else if (result == TEMPO_ID)
    {
        vc->addItem(PreparationTypeTempo, true);
    }
    else if (result == MODIFICATION_ID)
    {
        vc->addItem(PreparationTypeGenericMod, true);
    }
    else if (result == PIANOMAP_ID)
    {
        vc->addItem(PreparationTypePianoMap, true);
    }
    else if (result == RESET_ID)
    {
        vc->addItem(PreparationTypeReset, true);
        
    }
    
    // EDIT
    else if (result == KEYMAP_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeKeymap);
    }
    else if (result == DIRECT_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeDirect);
    }
    else if (result == NOSTALGIC_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeNostalgic);
    }
    else if (result == SYNCHRONIC_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeSynchronic);
    }
    else if (result == TUNING_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTuning);
    }
    else if (result == TEMPO_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTempo);
    }
    else if (result == DIRECTMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeDirectMod);
    }
    else if (result == NOSTALGICMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeNostalgicMod);
    }
    else if (result == SYNCHRONICMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeSynchronicMod);
    }
    else if (result == TUNINGMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTuningMod);
    }
    else if (result == TEMPOMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeTempoMod);
    }
}

void BKConstructionSite::mouseDoubleClick(const MouseEvent& eo)
{
    /*
    BKItem* item = dynamic_cast<BKItem*> (eo.originalComponent->getParentComponent());

    if (item != nullptr)
    {
        if (item->getType() == PreparationTypeComment)
        {
            //item->
        }
    }
     */
}

void BKConstructionSite::mouseHold(Component* frame, bool onItem)
{
#if JUCE_IOS
    frame->setTopLeftPosition(frame->getX()+20, frame->getY());
    
    if (onItem)
    {
        //getItemOptionMenu().showMenuAsync(PopupMenu::Options().withTargetComponent (frame),
        //                                  ModalCallbackFunction::forComponent (itemOptionMenuCallback, this) );
        
        if (itemToSelect != nullptr)
        {
            itemSource = itemToSelect;
            
            if (itemSource != nullptr)
            {
                connect = true;
                
                lineOX = itemSource->getX() + itemSource->getWidth() * 0.5;
                lineOY = itemSource->getY() + itemSource->getHeight() * 0.5;
                
                DBG("ORIGIN: " + String(lineOX) + " " + String(lineOY));
            }
        }
    }
    else
    {
        getEditMenu(&buttonsAndMenusLAF, 0, true).showMenuAsync(PopupMenu::Options().withTargetComponent(getMouseFrame()),
                                                                ModalCallbackFunction::forComponent(editMenuCallback, this));
    }
    
#endif
}

void BKConstructionSite::mouseDown (const MouseEvent& eo)
{
    if (edittingComment)
    {
        BKItem* anItem = dynamic_cast<BKItem*> (eo.originalComponent->getParentComponent());
        if (anItem == nullptr)
        {
            graph->deselectAll();
        }
        edittingComment = false;
        return;
    }
    
    MouseEvent e = eo.getEventRelativeTo(this);
    
#if JUCE_IOS
    TouchEvent* t = getTouchEvent(e.source);
    
    if (t == nullptr)
    {
        t = new TouchEvent(e.source);
        t->path.startNewSubPath(e.position);
        touches.add(t);
    }
    
    t->pushPoint(e.position, e.mods);
    
    repaint();
#endif

    
    itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
    
    held = false;
    
    lastX = e.x; lastY = e.y;
    
    mouseClicked(lastX, lastY, e.eventTime);
    
    if (itemToSelect != nullptr && !itemToSelect->resizing)
    {
        setMouseDownOnItem(true);
        
        processor.gallery->setGalleryDirty(true);
        
        if (e.mods.isShiftDown())
        {
            // also select this item
            if (itemToSelect != nullptr)
            {
                if (!itemToSelect->getSelected())   graph->select(itemToSelect);
                else                                graph->deselect(itemToSelect);
            }
            
            for (auto item : graph->getSelectedItems())
            {
                prepareItemDrag(item, e, true);
            }
            
        }
        else if (e.mods.isAltDown())
        {
            // Copy and drag
            
            itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
            
            lastX = e.x; lastY = e.y;
            
            
        }
        else
        {
            itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
            
            for (auto item : graph->getSelectedItems())
            {
                prepareItemDrag(item, e, true);
            }
        }
        
        if (e.mods.isCommandDown())
        {
            itemSource = itemToSelect;
            
            if (itemSource != nullptr)
            {
                connect = true;
                
                DBG("ORIGIN: " + String(lineOX) + " " + String(lineOY));
                lineOX = e.x;
                lineOY = e.y;
            }
        }
    }
    else
    {
        if (!e.mods.isShiftDown())
        {
            graph->deselectAll();
        }

        selected.deselectAll();
        
        addAndMakeVisible(lasso = new LassoComponent<BKItem*>());
        
        lasso->setAlpha(0.5);
        lasso->setColour(LassoComponent<BKItem*>::ColourIds::lassoFillColourId, Colours::lightgrey);
        lasso->setColour(LassoComponent<BKItem*>::ColourIds::lassoOutlineColourId, Colours::antiquewhite);
        
        lasso->beginLasso(eo, this);
    }

    getParentComponent()->grabKeyboardFocus();
}

void BKConstructionSite::mouseUp (const MouseEvent& eo)
{
    if (edittingComment) return;
    
    MouseEvent e = eo.getEventRelativeTo(this);
    
    touches.removeObject (getTouchEvent(e.source));
    
    mouseReleased();

    if (itemToSelect == nullptr) lasso->endLasso();
    
    if (selected.getNumSelected())
    {
        if (!eo.mods.isShiftDown()) graph->deselectAll();
        
        for (auto item : selected)  graph->select(item);
        
        selected.deselectAll();
        
        return;
    }
    
    if (connect)
    {
        int X = e.x;
        int Y = e.y;
        
        itemTarget = getItemAtPoint(X, Y);
        
        if (itemTarget != nullptr)
        {
            graph->connect(itemSource, itemTarget);
        }
        
        connect = false;
    }
    
    for (auto item : graph->getSelectedItems())
    {
        if (item->isDragging)
        {
            item->isDragging = false;
        }
    }
    
    repaint();
    
    getParentComponent()->grabKeyboardFocus();
    
}

void BKConstructionSite::mouseDrag (const MouseEvent& e)
{
    if (edittingComment) return;
    
#if JUCE_IOS
    MouseEvent eo = (e.eventComponent != this) ? e.getEventRelativeTo(this) : e;
    
    TouchEvent* t = getTouchEvent(eo.source);
    
    if (t == nullptr)
    {
        t = new TouchEvent(eo.source);
        t->path.startNewSubPath(eo.position);
        touches.add(t);
    }
    
    t->pushPoint(eo.position, eo.mods);
    
    repaint();
#endif
    
    mouseDragged();
    
    if (itemToSelect == nullptr) lasso->dragLasso(e);
    
    if (!connect && !e.mods.isShiftDown())
    {
        for (auto item : graph->getSelectedItems())
        {
            item->performDrag(e);
        }
    }
    
    lineEX = e.getEventRelativeTo(this).x;
    lineEY = e.getEventRelativeTo(this).y;
    
    repaint();

}


void BKConstructionSite::idDidChange(void)
{
    if (currentItem == NULL) return;
    
    BKPreparationType type = currentItem->getType();
    
    // GET NEW ID
    int newId = -1;
    
    if (type == PreparationTypeKeymap)              newId = processor.updateState->currentKeymapId;
    else if (type == PreparationTypeDirect)         newId = processor.updateState->currentDirectId;
    else if (type == PreparationTypeNostalgic)      newId = processor.updateState->currentNostalgicId;
    else if (type == PreparationTypeSynchronic)     newId = processor.updateState->currentSynchronicId;
    else if (type == PreparationTypeTempo)          newId = processor.updateState->currentTempoId;
    else if (type == PreparationTypeTuning)         newId = processor.updateState->currentTuningId;
    else if (type == PreparationTypeDirectMod)      newId = processor.updateState->currentModDirectId;
    else if (type == PreparationTypeNostalgicMod)   newId = processor.updateState->currentModNostalgicId;
    else if (type == PreparationTypeSynchronicMod)  newId = processor.updateState->currentModSynchronicId;
    else if (type == PreparationTypeTuningMod)      newId = processor.updateState->currentModTuningId;
    else if (type == PreparationTypeTempoMod)       newId = processor.updateState->currentModTempoId;
    
    currentItem->setId(newId);

    processor.currentPiano->configure();
}

BKItem* BKConstructionSite::getItemAtPoint(const int X, const int Y)
{
    BKItem* theItem = nullptr;
    
    if (itemSource != nullptr)
    {
        int which = 0;
        
        for (auto item : graph->getItems())
        {
            int left = item->getX(); int right = left + item->getWidth();
            int top = item->getY(); int bottom = top + item->getHeight();
            
            if (X >= left && X <= right && Y >= top && Y <= bottom) // is in item
            {
                theItem = dynamic_cast<BKItem*> (item);
                break;
            }
            
            which++;
        }
    }
    
    return theItem;
}

void BKConstructionSite::findLassoItemsInArea (Array <BKItem*>& itemsFound,
                                  const Rectangle<int>& area)
{
    int areaX = area.getX(); int areaY = area.getY(); int areaWidth = area.getWidth(); int areaHeight = area.getHeight();
    
    // psuedocode determine if collide: if (x1 + w1) - x2 >= 0 and (x2 + w2) - x1 >= 0
    
    for (auto item : graph->getItems())
    {
        int itemX = item->getX(); int itemWidth = item->getWidth();
        int itemY = item->getY(); int itemHeight = item->getHeight();
        
        if (((itemX + itemWidth - areaX) >= 0) && ((areaX + areaWidth - itemX) >= 0) &&
            ((itemY + itemHeight - areaY) >= 0) && ((areaY + areaHeight - itemY) >= 0))
            itemsFound.add(item);
    }
    
    /*
    for (int x = area.getX(); x < area.getRight(); x++)
    {
        for (int y = area.getY(); y < area.getBottom(); y++)
        {
            BKItem* item = getItemAtPoint(x,y);
            
            if (item != nullptr) itemsFound.add(item);
        }
    }
     */
}

SelectedItemSet<BKItem*>& BKConstructionSite::getLassoSelection(void)
{
    return selected;
}

