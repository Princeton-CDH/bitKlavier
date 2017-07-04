/*
  ==============================================================================

    BKConstructionSite.cpp
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKConstructionSite.h"

#define AUTO_DRAW 1
#define NUM_COL 8

BKConstructionSite::BKConstructionSite(BKAudioProcessor& p, BKItemGraph* theGraph):
BKDraggableComponent(false,false,false),
processor(p),
graph(theGraph),
connect(false),
lastX(10),
lastY(10),
altDown(false)
{
    addKeyListener(this);
    
    setWantsKeyboardFocus(true);
    
    graph->deselectAll();
    
    redraw();
}

BKConstructionSite::~BKConstructionSite(void)
{
    
}

void BKConstructionSite::redraw(void)
{
    BKItem::RCArr items = graph->getAllItems();
    
    for (auto item : items)
    {
        graph->removeUI(item);
    }
    
    removeAllChildren();
    
    graph->reconstruct();
    
    graph->deselectAll();
    
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

void BKConstructionSite::deleteSelected(void)
{
    
    BKItem::PtrArr selectedItems = graph->getSelectedItems();
    
    for (int i = selectedItems.size(); --i >= 0;)
    {
        deleteItem(selectedItems[i]);
    }
    
    selected.deselectAll();
    
    repaint();
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

void BKConstructionSite::resized()
{
    repaint();
}

void BKConstructionSite::paint(Graphics& g)
{
    //g.setColour(Colours::gold);
    g.setColour(Colours::burlywood.withMultipliedBrightness(0.25));
    g.fillAll();
    
    
    g.setColour(Colours::white);
    g.drawRect(getLocalBounds(), 1);
    
    if (itemIsHovering)
    {
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds(), 3);
    }
    
    if (connect)
    {
        g.setColour(Colours::goldenrod);
        g.drawLine(lineOX, lineOY, lineEX, lineEY, 3);
    }
    
    for (auto line : graph->getLines())
    {
        g.setColour(Colours::goldenrod);
        g.drawLine(line.getStartX(), line.getStartY(), line.getEndX(), line.getEndY(), 3);
    }
}

void BKConstructionSite::itemIsBeingDragged(BKItem* thisItem, Point<int> startPosition)
{
    repaint();
}

void BKConstructionSite::pianoMapDidChange(BKItem* thisItem)
{
    graph->update(PreparationTypePianoMap,thisItem->getId());
}

void BKConstructionSite::draw(void)
{
    int keymapCount = 0, prepCount = 0, otherCount = 0, modCount = 0;
    for (auto item : graph->getAllItems())
    {
        BKPreparationType type = item->getType();
        int which = item->getId();
        
        Point<int> xy = processor.currentPiano->configuration->getXY(type, which);
        
        int x = xy.x; int y = xy.y;
        
        DBG("DRAW X: " + String(x) + " Y: " + String(y));
        
        if (type == PreparationTypeKeymap)
        {
#if AUTO_DRAW
            int col = (int)(keymapCount / NUM_COL);
            int row = keymapCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 50 + (col * 25);
            
            item->setTopLeftPosition(X, Y);
            
            keymapCount++;
#else
            
            item->setTopLeftPosition(x, y);
#endif
            
        }
        else if (type <= PreparationTypeNostalgic || type == PreparationTypePianoMap || type == PreparationTypeGenericMod)
        {
#if AUTO_DRAW
            int col = (int)(prepCount / NUM_COL);
            int row = prepCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 200 + (col * 25);
            
            item->setTopLeftPosition(X, Y);
            
            prepCount++;
#else
            item->setTopLeftPosition(x, y);
#endif
            
        }
        else if (type > PreparationTypeKeymap)
        {
#if AUTO_DRAW
            int col = (int)(modCount / NUM_COL);
            int row = modCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 400 + (col * 25);
            
            
            item->setTopLeftPosition(X, Y);
            
            modCount++;
#else
            item->setTopLeftPosition(x, y);
#endif
        }
        else
        {
#if AUTO_DRAW
            int col = (int)(otherCount / NUM_COL);
            int row = otherCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 350 + (col * 25);
            
            item->setTopLeftPosition(X, Y);
            
            otherCount++;
#else
            item->setTopLeftPosition(x, y);
#endif
            
        }
        
        addAndMakeVisible(item);
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
    graph->remove(item);
    removeChildComponent(item);
}

int BKConstructionSite::getUnusedId(BKPreparationType type, int which)
{
    int thisId = which;
    
    if (type <= PreparationTypeKeymap)
    {
        int count = processor.gallery->getNum(type);
        
        if (count < 0)
        {
            thisId = 0;
        }
        else
        {
            while (thisId < count)
            {
                if (!graph->containsItemWithTypeAndId(type, thisId))
                    break;
                
                thisId++;
            }
        }
        
        if (thisId >= count)
            processor.gallery->add(type);
        
    }
    
    return thisId;
}


void BKConstructionSite::addItem(BKPreparationType type)
{
    int thisId = -1;
    
    if (type != PreparationTypeGenericMod)
    {
        thisId = processor.gallery->getNewId(type);
        
        processor.gallery->addTypeWithId(type, thisId);
    }
    
    BKItem::Ptr toAdd = new BKItem(type, thisId, processor);
    
    toAdd->setTopLeftPosition(lastX, lastY);
    
    lastX += 10; lastY += 10;
    
    graph->add(toAdd);
    
    addAndMakeVisible(toAdd);
}

void BKConstructionSite::copy(void)
{
    graph->updateClipboard();
}

void BKConstructionSite::addItemsFromClipboard(void)
{
    graph->deselectAll();
    
    int which = 0;
    int firstX, firstY;
    for (auto item : graph->clipboard)
    {
        int thisId = getUnusedId(item->getType(), item->getId());
        
        BKItem::Ptr toAdd = new BKItem(item->getType(), thisId, processor);
        
        if (which == 0)
        {
            toAdd->setTopLeftPosition(lastX, lastY);
            firstX = item->position.x; firstY = item->position.y;
        }
        else
        {
            toAdd->setTopLeftPosition(lastX+item->position.x-firstX, lastY+item->position.y-firstY);
        }
        
        graph->add(toAdd);
        
        for (auto connection : toAdd->getConnections())
            graph->connect(toAdd, connection);
        
        addAndMakeVisible(toAdd);
        
        which++;
        
        graph->select(toAdd);
    }
}

void BKConstructionSite::paste(void)
{
    addItemsFromClipboard();
}

void BKConstructionSite::cut(void)
{
    graph->updateClipboard();
    deleteSelected();
    
}

void BKConstructionSite::mouseMove (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    lastX = e.x;
    lastY = e.y;
}

void BKConstructionSite::mouseDown (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
    
    lastX = e.x; lastY = e.y;
    
    if (itemToSelect != nullptr)
    {
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
            
            graph->updateClipboard();
            
            lastX = e.x; lastY = e.y;
            
            addItemsFromClipboard();
            
            
            
            
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
        graph->deselectAll();
        
        selected.deselectAll();
        
        addAndMakeVisible(lasso = new LassoComponent<BKItem*>());
        
        lasso->beginLasso(eo, this);
    }
    
    
}

void BKConstructionSite::mouseDrag (const MouseEvent& e)
{
    lastX = e.x; lastY = e.y;
    
    if (itemToSelect == nullptr) lasso->dragLasso(e);
    
    if (!e.mods.isCommandDown())
    {
        bool resizeX = false, resizeY = false;
        for (auto item : graph->getSelectedItems())
        {
            item->performDrag(e);
        }
    }
    
    lineEX = e.getEventRelativeTo(this).x;
    lineEY = e.getEventRelativeTo(this).y;
    
    DBG("DRAG: "+String(lineEX) + " " +String(lineEY));
    
    repaint();
}

void BKConstructionSite::mouseUp (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    connect = false;
    
    if (itemToSelect == nullptr) lasso->endLasso();
    
    if (selected.getNumSelected())
    {
        graph->deselectAll();
        
        for (auto item : selected)  graph->select(item);
        
        return;
    }
    
    if (e.mods.isCommandDown())
    {
        
        int X = e.x;
        int Y = e.y;
        
        itemTarget = getItemAtPoint(X, Y);
        
        if (itemTarget != nullptr)
        {
            graph->connect(itemSource, itemTarget);
            graph->drawLine(lineOX, lineOY, X, Y);
        }
    }
    
    for (auto item : graph->getSelectedItems())
    {
        if (item->isDragging)
        {
            int X = item->getX(); int Y = item->getY();
            DBG("SET X: " + String(X) + " Y: " + String(Y));
            processor.currentPiano->configuration->setItemXY(item->getType(), item->getId(), X, Y);
            item->isDragging = false;
        }
    }
    
    repaint();
    
}

void BKConstructionSite::reconfigureCurrentItem(void)
{
    BKPreparationType type = currentItem->getType();
    
    BKItem::PtrArr connections;
    
    for (auto item : currentItem->getConnections())
    {
        connections.add(item);
        
        graph->disconnect(currentItem, item);
    }
    
    for (auto item : connections)   graph->connect(currentItem, item);
}

void BKConstructionSite::idDidChange(void)
{
    BKPreparationType type = currentItem->getType();
    
    processor.updateState->removeActive(type, currentItem->getId());
    
    BKItem::PtrArr connections;
    
    for (auto item : currentItem->getConnections())
    {
        connections.add(item);
        
        graph->disconnect(currentItem, item);
    }
    
    int newId = -1;
    
    if (type == PreparationTypeKeymap)          newId = processor.updateState->currentKeymapId;
    else if (type == PreparationTypeDirect)     newId = processor.updateState->currentDirectId;
    else if (type == PreparationTypeNostalgic)  newId = processor.updateState->currentNostalgicId;
    else if (type == PreparationTypeSynchronic) newId = processor.updateState->currentSynchronicId;
    else if (type == PreparationTypeTempo)      newId = processor.updateState->currentTempoId;
    else if (type == PreparationTypeTuning)     newId = processor.updateState->currentTuningId;
    else if (type == PreparationTypeGenericMod)
    {
        ModificationMapper::Ptr thisMapper = currentItem->getMapper();
        BKPreparationType modType = thisMapper->getType();
    
        if (modType == PreparationTypeDirect)
        {
            currentItem->getMapper()->setId(processor.updateState->currentModDirectId);
        }
        else if (modType == PreparationTypeSynchronic)
        {
            currentItem->getMapper()->setId(processor.updateState->currentModSynchronicId);
        }
        else if (modType == PreparationTypeNostalgic)
        {
            currentItem->getMapper()->setId(processor.updateState->currentModNostalgicId);
        }
        else if (modType == PreparationTypeTuning)
        {
            currentItem->getMapper()->setId(processor.updateState->currentModTuningId);
        }
        else if (modType == PreparationTypeTempo)
        {
            currentItem->getMapper()->setId(processor.updateState->currentModTempoId);
        }
        
    }
    
    currentItem->setId(newId);
    
    processor.updateState->addActive(type, newId);
    
    for (auto item : connections)   graph->connect(currentItem, item);
    
}


bool BKConstructionSite::keyPressed (const KeyPress& e, Component*)
{
    
}

BKItem* BKConstructionSite::getItemAtPoint(const int X, const int Y)
{
    BKItem* theItem = nullptr;
    
    if (itemSource != nullptr)
    {
        int which = 0;
        
        for (auto item : graph->getAllItems())
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
    
    for (auto item : graph->getAllItems())
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

