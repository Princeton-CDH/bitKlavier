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

BKConstructionSite::BKConstructionSite(BKAudioProcessor& p, /*Viewport* vp,*/ BKItemGraph* theGraph):
altDown(false),
processor(p),
graph(theGraph),
connect(false),
lastX(10),
lastY(10)/*,
viewport(vp)*/
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
    g.setColour(Colours::burlywood.withMultipliedBrightness(0.25));
    g.fillAll();
    
    if (connect)
    {
        g.setColour(Colours::lightgrey);
        
        
        g.drawLine(lineOX, lineOY, lineEX, lineEY, (platform == BKIOS) ? 2 : 3);
        
    }
    
    for (auto line : graph->getLines())
    {
        g.setColour(Colours::goldenrod);

        g.drawLine(line.getStartX(), line.getStartY(), line.getEndX(), line.getEndY(), (platform == BKIOS) ? 1 : 2);
    }
}

bool BKConstructionSite::itemOutsideBounds(Rectangle<int> bounds)
{
    for (auto item : graph->getItems())
    {
        Point<int> xy = item->getPosition();
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
        float X = item->getX();
        float Y = item->getY();
        
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
        float X = (left ? mostLeft : (right ? mostRight : item->getX()));
        float Y = (top ? mostTop : (bottom ? mostBottom : item->getY()));
        
        item->setTopLeftPosition(X, Y);
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
        Point<float>pos(X,Y);
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
    
    if (type != PreparationTypeGenericMod)
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
    
#if JUCE_IOS
    toAdd->setTopLeftPosition(lastX, lastY);
#endif
    
#if JUCE_MAC
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

void BKConstructionSite::mouseMove (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    if (e.x != lastEX) lastX = e.x;
    
    if (e.y != lastEY) lastY = e.y;
    
    lastEX = e.x;
    lastEY = e.y;
}

PopupMenu BKConstructionSite::getNewMenu(void)
{
    PopupMenu newMenu;
    newMenu.setLookAndFeel(&buttonsAndMenusLAF);
    
    newMenu.addItem(KEYMAP_ID, "Keymap");
    newMenu.addItem(DIRECT_ID, "Direct");
    newMenu.addItem(NOSTALGIC_ID, "Nostalgic");
    newMenu.addItem(SYNCHRONIC_ID, "Synchronic");
    newMenu.addItem(TUNING_ID, "Tuning");
    newMenu.addItem(TEMPO_ID, "Tempo");
    newMenu.addItem(MODIFICATION_ID, "Modification");
    newMenu.addItem(PIANOMAP_ID, "Piano Map");
    newMenu.addItem(RESET_ID, "Reset");
    
    return newMenu;
}

PopupMenu BKConstructionSite::getConstructionOptionMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
    
    menu.addItem(PASTE_ID, "Paste");
    menu.addSeparator();
    menu.addItem(UNDO_ID, "Undo");
    menu.addItem(REDO_ID, "Redo");
    menu.addSeparator();
    menu.addSubMenu("Add...", getNewMenu());
    
    return menu;
}

PopupMenu BKConstructionSite::getItemOptionMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);

    if (graph->getSelectedItems().size() == 1)
    {
        BKPreparationType type = graph->getSelectedItems().getFirst()->getType();
        if (type <= PreparationTypeTempoMod)
        {
            menu.addItem(EDIT_ID, "Edit");
            menu.addSeparator();
        }
    }
    
    if (graph->getSelectedItems().size() > 1)
    {
        menu.addItem(ALIGN_VERTICAL, "Align (vertical)");
        menu.addItem(ALIGN_HORIZONTAL, "Align (horizontal)");
        menu.addSeparator();
    }
    
    menu.addItem(COPY_ID, "Copy");
    menu.addItem(CUT_ID, "Cut");
    menu.addItem(PASTE_ID, "Paste");
    menu.addSeparator();
    menu.addItem(UNDO_ID, "Undo");
    menu.addItem(REDO_ID, "Redo");
    menu.addSeparator();
    menu.addItem(DELETE_ID, "Delete");
    
    return menu;
}

void BKConstructionSite::itemOptionMenuCallback(int result, BKConstructionSite* vc)
{
    if (result == EDIT_ID)
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
    else if (result == UNDO_ID)
    {
        
    }
    else if (result == REDO_ID)
    {
        
    }
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
}

void BKConstructionSite::constructionOptionMenuCallback(int result, BKConstructionSite* vc)
{
    if (result == KEYMAP_ID)
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
    else if (result == UNDO_ID)
    {
        
    }
    else if (result == REDO_ID)
    {
        
    }
    else if (result == PASTE_ID)
    {
        vc->paste();
    }
}

void BKConstructionSite::mouseDoubleClick(const MouseEvent& eo)
{
    
}

void BKConstructionSite::mouseHold(Component* frame, bool onItem)
{
    frame->setTopLeftPosition(frame->getX()+20, frame->getY());
    
    if (onItem)
    {
        getItemOptionMenu().showMenuAsync(PopupMenu::Options().withTargetComponent (frame),
                                          ModalCallbackFunction::forComponent (itemOptionMenuCallback, this) );
    }
    else
    {
        getConstructionOptionMenu().showMenuAsync(PopupMenu::Options().withTargetComponent (frame),
                                                  ModalCallbackFunction::forComponent (constructionOptionMenuCallback, this) );
    }
}

void BKConstructionSite::mouseDown (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
    
    lastX = e.x; lastY = e.y;
    
    mouseClicked(lastX, lastY, e.eventTime);
    
    if (itemToSelect != nullptr)
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
        lasso->setColour(LassoComponent<BKItem*>::ColourIds::lassoOutlineColourId, Colours::white);
        
        lasso->beginLasso(eo, this);

    }

    getParentComponent()->grabKeyboardFocus();
    
}

void BKConstructionSite::mouseUp (const MouseEvent& eo)
{
    
    MouseEvent e = eo.getEventRelativeTo(this);
    
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
    lastX = e.x; lastY = e.y;

    
    
    mouseDragged();
    
    if (itemToSelect == nullptr) lasso->dragLasso(e);
    
    if (!connect && !e.mods.isShiftDown())
    {
        for (auto item : graph->getSelectedItems())
        {
            item->performDrag(e);
            
            //if (item->)
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

