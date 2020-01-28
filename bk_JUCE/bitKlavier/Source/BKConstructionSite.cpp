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

BKConstructionSite::BKConstructionSite(BKAudioProcessor& p, BKItemGraph* theGraph):
altDown(false),
edittingComment(false),
processor(p),
graph(theGraph),
connect(false),
lastX(10),
lastY(10),
held(false)
{
    addAndMakeVisible(clickFrame);
    clickFrame.setSize(5,5);
    
    setWantsKeyboardFocus(false);
    
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
    
    graph->deselectAll();
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
    
    redraw();
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

void BKConstructionSite::startConnection(int x, int y)
{
    itemSource = graph->getSelectedItems().getFirst();
    
    if (itemSource != nullptr)
    {
        connect = true;
        
        lineOX = x;
        lineOY = y;
        DBG("ORIGIN: " + String(lineOX) + " " + String(lineOY));
    }
}

void BKConstructionSite::makeConnection(int x, int y, bool doAnother)
{
    if (connect)
    {
        itemTarget = getItemAtPoint(x, y);
        
        if (itemTarget != nullptr)
        {
            graph->connect(itemSource, itemTarget);
        }
        
        connect = doAnother;
    }
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
#else 
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
    
    // SHOULD ALSO SAVE VALUE TREE OF STATE WITH ITEMs in CLIPBOARD
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
        
        newItem->setContent(processor.getPreparationState(item->getType(), item->getId()).createXml());
        
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
    
    BKItem::PtrArr newItems;
    
    if (!clipboard.size() || (upperLeftest == nullptr)) return;
    
    pastemap.clear();
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        pastemap.add(new HashMap<int,int>());
    }
    
    graph->deselectAll();
    
    int offsetX = upperLeftest->getX(); int offsetY = upperLeftest->getY();
    
    for (auto item : clipboard)
    {
        BKItem::Ptr newItem = item->duplicate();
        
        BKPreparationType type = newItem->getType();
        int oldId = item->getId();
        
        // duplicate (all we need to do is set to new Id and add to gallery)
        int newId = processor.gallery->addCopy(type, newItem->getContent().get(), oldId);
        
        newItem->print();
    
        pastemap[type]->set(oldId, newId);

        newItem->setSelected(true);
        
        newItem->setTopLeftPosition((newItem->getX()-offsetX) + lastEX, (newItem->getY()-offsetY) + lastEY);
        
        processor.currentPiano->add(newItem, false); // make sure not to configure yet or bad ids will cause a crash
        
        newItems.add(newItem);
    }
    
    for (int i = 0; i < newItems.size(); i++)
    {
        BKItem* oldItem = clipboard[i];
        BKItem* newItem = newItems[i];
        
        for (auto connection : oldItem->getConnections())
        {
            BKPreparationType connectionType = connection->getType();
            int connectionId = connection->getId();
            
            BKItem* connectionItem = nullptr;
            
            for (auto item : newItems)
            {
                if (item->getType() == connectionType && item->getId() == connectionId) connectionItem = item;
            }
            
            if (connectionItem != nullptr)
            {
                newItem->addConnection(connectionItem);
                connectionItem->addConnection(newItem);
            }
        }

    }
    
    // NOW set new Id for item. didnt want to do it before so that
    // ^ I'm assuming it's so that the connection ids match up
    for (auto item : newItems)
    {
        int oldId = item->getId();
        int newId = pastemap[item->getType()]->getReference(item->getId());
        DBG(String(cPreparationTypes[item->getType()]) + " old: " + String(oldId) + " new: " + String(newId));
        item->setId(newId);
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
    
    if (connect)
    {
        lineEX = e.getEventRelativeTo(this).x;
        lineEY = e.getEventRelativeTo(this).y;
        
        repaint();
    }
}

void BKConstructionSite::editMenuCallback(int result, BKConstructionSite* vc)
{
    BKAudioProcessor& processor = vc->processor;
    
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
    else if (result == ALIGN_VERTICAL)
    {
        vc->align(0);
    }
    else if (result == ALIGN_HORIZONTAL)
    {
        vc->align(3);
    }
    // Make a connection cable
    else if (result == CONNECTION_ID)
    {
        int x = vc->currentItem->getX() + (vc->currentItem->getWidth() * 0.5f);
        int y = vc->currentItem->getY() + (vc->currentItem->getHeight() * 0.5f);
        vc->startConnection(x, y);
    }
    // Maximally connect selected items
    else if (result == CONNECT_ALL_ID)
    {
        for (int i = 0; i < vc->graph->getSelectedItems().size()-1; ++i)
        {
            for (int j = i+1; j < vc->graph->getSelectedItems().size(); ++j)
            {
                vc->graph->connect(vc->graph->getSelectedItems()[i],
                                   vc->graph->getSelectedItems()[j]);
            }
        }
        vc->repaint();
    }
    // Disconnect a single item or multiple selected items from any other connected items
    else if (result == DISCONNECT_FROM_ID)
    {
        // For each selected item
        for (BKItem::Ptr selectedItem : vc->graph->getSelectedItems())
        {
            // Check each connected item
            for (BKItem::Ptr connectedItem : selectedItem->getConnections())
            {
                // If the connected item is not also selected, disconnect it
                if (!vc->graph->getSelectedItems().contains(connectedItem))
                {
                    vc->graph->disconnect(selectedItem, connectedItem);
                }
            }
        }
        vc->repaint();
    }
    // Disconnect multiple selected items from each other
    else if (result == DISCONNECT_BETWEEN_ID)
    {
        for (int i = 0; i < vc->graph->getSelectedItems().size()-1; ++i)
        {
            for (int j = i+1; j < vc->graph->getSelectedItems().size(); ++j)
            {
                vc->graph->disconnect(vc->graph->getSelectedItems()[i],
                                      vc->graph->getSelectedItems()[j]);
            }
        }
        vc->repaint();
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
    else if (result == BLENDRONIC_ID)
    {
        vc->addItem(PreparationTypeBlendronic, true);
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
    else if (result == BLENDRONIC_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeBlendronic);
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
    else if (result == BLENDRONICMOD_EDIT_ID)
    {
        processor.updateState->setCurrentDisplay(PreparationTypeBlendronicMod);
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

void BKConstructionSite::mouseHold(Component* frame, bool onItem)
{
#if JUCE_IOS
    frame->setTopLeftPosition(frame->getX()+20, frame->getY());
    
    if (onItem)
    {
        if (itemToSelect != nullptr)
        {
            itemSource = itemToSelect;
            
            if (itemSource != nullptr)
            {
                if ((itemSource->getType() != PreparationTypeComment) &&
                    (itemSource->getType() != PreparationTypePianoMap))
                {
                    connect = true;
                    
                    lineOX = itemSource->getX() + itemSource->getWidth() * 0.5;
                    lineOY = itemSource->getY() + itemSource->getHeight() * 0.5;
                    
                    DBG("ORIGIN: " + String(lineOX) + " " + String(lineOY));
                }
                
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
    DBG("mousedown");
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
    
    // This must happen before the right-click menu or the menu will close
    getParentComponent()->grabKeyboardFocus();
    
    // Clicking on an item
    if (itemToSelect != nullptr && !itemToSelect->resizing)
    {
        setMouseDownOnItem(true);
        
        processor.gallery->setGalleryDirty(true);
        
        // Right mouse click (must be first so right click overrides other mods)
        if (e.mods.isRightButtonDown())
        {
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
            getEditMenu(&buttonsAndMenusLAF, graph->getSelectedItems().size(), false, true).showMenuAsync
            (PopupMenu::Options().withTargetScreenArea(Rectangle<int>(Desktop::getMousePosition(), Desktop::getMousePosition())),
             ModalCallbackFunction::forComponent (editMenuCallback, this) );
        }
        // Control click (same as right click on Mac)
#if JUCE_MAC
        else if (e.mods.isCtrlDown())
        {
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
            getEditMenu(&buttonsAndMenusLAF, graph->getSelectedItems().size(), false, true).showMenuAsync
            (PopupMenu::Options().withTargetScreenArea(Rectangle<int>(Desktop::getMousePosition(), Desktop::getMousePosition())),
             ModalCallbackFunction::forComponent (editMenuCallback, this) );
        }
#endif
        else if (e.mods.isShiftDown())
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
            // make a connection and look to make another
            if (connect) makeConnection(e.x, e.y, true);
            
            // Copy and drag
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
        }
        // Command click
        else if (e.mods.isCommandDown())
        {
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
            if (connect) makeConnection(e.x, e.y);
            else startConnection(e.x, e.y);
        }
        // Unmodified left mouse click
        else
        {
            if (connect) makeConnection(e.x, e.y);

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
    }
    // Clicking on blank graph space
    else
    {
        if (e.mods.isRightButtonDown())
        {
            getEditMenu(&buttonsAndMenusLAF, graph->getSelectedItems().size(), true, true).showMenuAsync
            (PopupMenu::Options().withTargetScreenArea(Rectangle<int>(Desktop::getMousePosition(), Desktop::getMousePosition())),
             ModalCallbackFunction::forComponent (editMenuCallback, this) );
        }
#if JUCE_MAC
        else if (e.mods.isCtrlDown())
        {
            getEditMenu(&buttonsAndMenusLAF, graph->getSelectedItems().size(), true, true).showMenuAsync
            (PopupMenu::Options().withTargetScreenArea(Rectangle<int>(Desktop::getMousePosition(), Desktop::getMousePosition())),
             ModalCallbackFunction::forComponent (editMenuCallback, this) );
        }
#endif
        else
        {
            if (!e.mods.isShiftDown())
            {
                graph->deselectAll();
            }
            
            selected.deselectAll();
        }
        lasso = std::make_unique<LassoComponent<BKItem*>>();
        addAndMakeVisible(*lasso);
        
        lasso->setAlpha(0.5);
        lasso->setColour(LassoComponent<BKItem*>::ColourIds::lassoFillColourId, Colours::lightgrey);
        lasso->setColour(LassoComponent<BKItem*>::ColourIds::lassoOutlineColourId, Colours::antiquewhite);
        
        lasso->beginLasso(eo, this);
        
        // Stop trying to make a connection on blank space click
        connect = false;
    }
}

void BKConstructionSite::mouseUp (const MouseEvent& eo)
{
    if (edittingComment) return;
    
    MouseEvent e = eo.getEventRelativeTo(this);
    
    // Do nothing on right click mouse up
    if (e.mods.isRightButtonDown()) return;
#if JUCE_MAC
    // Do nothing on ctrl click mouse up
    if (e.mods.isCtrlDown()) return;
#endif
    
    touches.removeObject (getTouchEvent(e.source));
    
    mouseReleased();
    
    getParentComponent()->grabKeyboardFocus();

    if (itemToSelect == nullptr) lasso->endLasso();
    
    if (selected.getNumSelected())
    {
        if (!eo.mods.isShiftDown()) graph->deselectAll();
        
        for (auto item : selected)  graph->select(item);
        
        selected.deselectAll();
        
        return;
    }
    
    // Uncomment for press-hold-release connection behavior when using CMD shortcut
    // Otherwise make connections with CMD+click and click on target
//  if (connect) makeConnection(e.x, e.y, e.mods.isAltDown());

    
    for (auto item : graph->getSelectedItems())
    {
        if (item->isDragging)
        {
            item->isDragging = false;
        }
    }
    
    repaint();
}

void BKConstructionSite::mouseDrag (const MouseEvent& e)
{
    if (edittingComment) return;
    
    // Do nothing on right click drag
    if (e.mods.isRightButtonDown()) return;
    
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
    
    if (e.getDistanceFromDragStart() > 50) mouseDragged();
    
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
    else if (type == PreparationTypeBlendronic)     newId = processor.updateState->currentBlendronicId;
    else if (type == PreparationTypeTempo)          newId = processor.updateState->currentTempoId;
    else if (type == PreparationTypeTuning)         newId = processor.updateState->currentTuningId;
    else if (type == PreparationTypeDirectMod)      newId = processor.updateState->currentModDirectId;
    else if (type == PreparationTypeNostalgicMod)   newId = processor.updateState->currentModNostalgicId;
    else if (type == PreparationTypeSynchronicMod)  newId = processor.updateState->currentModSynchronicId;
    else if (type == PreparationTypeBlendronicMod)  newId = processor.updateState->currentModBlendronicId;
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
