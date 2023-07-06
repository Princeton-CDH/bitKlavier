#include "DraggableListBox.h"

DraggableListBoxItemData::~DraggableListBoxItemData() {};

void DraggableListBoxItem::paint(Graphics& g)
{
   
   
    modelData.paintContents(rowNum, g, getLocalBounds());
    
    if (insertAfter)
    {
        g.setColour(Colours::red);
        g.fillRect(0, getHeight() - 3, getWidth(), 3);
    }
    else if (insertBefore)
    {
        g.setColour(Colours::red);
        g.fillRect(0, 0, getWidth(), 3);
    }

    if(listBox.isRowSelected(rowNum))
    {
        g.setColour(Colours::yellow);
        g.drawRect(getLocalBounds(),5);
    }
 
}

void DraggableListBoxItem::mouseEnter(const MouseEvent&)
{
    savedCursor = getMouseCursor();
    setMouseCursor(MouseCursor::DraggingHandCursor);
}

void DraggableListBoxItem::mouseExit(const MouseEvent&)
{
    setMouseCursor(savedCursor);
}

void DraggableListBoxItem::mouseDrag(const MouseEvent &e)
{
    if (DragAndDropContainer* container = DragAndDropContainer::findParentDragContainerFor(this))
    {
        container->startDragging("DraggableListBoxItem", this);
        listBox.dragSelectedRows(e, rowNum, modelData.getNumItems());
    }
}

    
void DraggableListBoxItem::mouseDragSelected(const MouseEvent &e)
{
    if (DragAndDropContainer* container = DragAndDropContainer::findParentDragContainerFor(this))
    {
        container->startDragging("DraggableListBoxItem", this);
        //listBox.dragSelectedRows(e, rowNum, modelData.getNumItems());
    }
}
void DraggableListBoxItem::mouseDown(const MouseEvent &e)
{
    if(e.mods.isCtrlDown())
    {
        listBox.selectRow(rowNum, false, false);
    } else
    {
        listBox.selectRow(rowNum, false, true);
        listBox.sendChangeMessage();
    }
   
}
void DraggableListBoxItem::mouseUp(const MouseEvent &e)
{
   
}

void DraggableListBoxItem::updateInsertLines(const SourceDetails &dragSourceDetails)
{
    if (dragSourceDetails.localPosition.y < getHeight() / 2)
    {
        insertBefore = true;
        insertAfter = false;
    }
    else
    {
        insertAfter = true;
        insertBefore = false;
    }
    repaint();
}

void DraggableListBoxItem::hideInsertLines()
{
    insertBefore = false;
    insertAfter = false;
}

void DraggableListBoxItem::itemDragEnter(const SourceDetails& dragSourceDetails)
{
    updateInsertLines(dragSourceDetails);
}

void DraggableListBoxItem::itemDragMove(const SourceDetails& dragSourceDetails)
{
    updateInsertLines(dragSourceDetails);
}

void DraggableListBoxItem::itemDragExit(const SourceDetails& /*dragSourceDetails*/)
{
    hideInsertLines();
}

void DraggableListBoxItem::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    if (DraggableListBoxItem* item = dynamic_cast<DraggableListBoxItem*>(dragSourceDetails.sourceComponent.get()))
    {
        
        if (dragSourceDetails.localPosition.y < getHeight() / 2)
        {
            //listBox.dropSelectedRows(modelData.getNumItems(), rowNum, true);
            std::vector<int> rows;
            for (int i = 0; i < modelData.getNumItems(); i++)
            {
                if(listBox.isRowSelected(i))
                {
                    rows.push_back(i);
                }
                    //dynamic_cast<DraggableListBoxItem*>(getComponentForRowNumber(i))->itemDroppedSelected( rowToDrop, before);
            }
//            if (rowNum == 0)
//            {
//                rowNum = 1;
//            }
            
            int newIndex = modelData.moveToIndex(rows, rowNum);
            listBox.deselectAllRows();
          

                listBox.selectRangeOfRows(newIndex, newIndex + rows.size()-1);
            
//            modelData.moveBefore(item->rowNum, rowNum);
//            if (listBox.isRowSelected(item->rowNum))
//            {
//                listBox.deselectRow(item->rowNum);
//                listBox.selectRow(rowNum - 1);
//            }
        }
        else
        {
            //listBox.dropSelectedRows(modelData.getNumItems(), rowNum, true);
            std::vector<int> rows;
            for (int i = 0; i < modelData.getNumItems(); i++)
            {
                if(listBox.isRowSelected(i))
                {
                    rows.push_back(i);
                }
                    //dynamic_cast<DraggableListBoxItem*>(getComponentForRowNumber(i))->itemDroppedSelected( rowToDrop, before);
            }
            int newIndex = modelData.moveToIndex(rows, rowNum + 1);
            listBox.deselectAllRows();
          
            listBox.selectRangeOfRows(newIndex, newIndex + rows.size() - 1);
           // modelData.moveAfter(item->rowNum, rowNum, false);
//            if (listBox.isRowSelected(item->rowNum))
//            {
//                listBox.deselectRow(item->rowNum);
//                listBox.selectRow(rowNum);
//            }
        }
        
    }
    hideInsertLines();
    
}
void DraggableListBoxItem::itemDroppedSelected(int dropRow, bool before)
{
     if(before)
        modelData.moveBefore(rowNum, dropRow);
     else
        modelData.moveAfter(rowNum, dropRow);
                
    listBox.deselectRow(rowNum);
            
        
        //hideInsertLines();
        
}

Component* DraggableListBoxModel::refreshComponentForRow(int rowNumber,
                                                         bool /*isRowSelected*/,
                                                         Component *existingComponentToUpdate)
{
    std::unique_ptr<DraggableListBoxItem> item(dynamic_cast<DraggableListBoxItem*>(existingComponentToUpdate));
    if (isPositiveAndBelow(rowNumber, modelData.getNumItems()))
    {
        item = std::make_unique<DraggableListBoxItem>(listBox, modelData, rowNumber);
    }
    return item.release();
}
