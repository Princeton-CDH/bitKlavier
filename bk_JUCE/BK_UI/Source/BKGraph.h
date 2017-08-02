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

#include "ItemMapper.h"

class BKAudioProcessor;


class BKItem : public ItemMapper, public BKDraggableComponent, public BKListener
{
public:
    typedef ReferenceCountedArray<BKItem>       RCArr;
    typedef ReferenceCountedObjectPtr<BKItem>   Ptr;
    typedef Array<Ptr>                          PtrArr;
    
    BKItem(BKPreparationType type, int Id, BKAudioProcessor& p);
    
    ~BKItem(void);
    
    void mouseDown(const MouseEvent& e) override;
    
    void paint(Graphics& g) override;
    
    void resized(void) override;
    
    void itemIsBeingDragged(const MouseEvent&, Point<int>) override;
    
    void keyPressedWhileSelected(const KeyPress&) override;

    void setItemType(BKPreparationType type, bool create);
    
    inline void setSelected(bool select) {isSelected = select; repaint();}
    inline bool getSelected(void) { return isSelected;}
    
    inline void setConnections(BKItem::PtrArr newConnections)
    {
        connections = newConnections;
    }
    
    inline void addConnection(BKItem::Ptr thisItem)
    {
        if (!isConnectedTo(thisItem))   connections.add(thisItem);
    }
    
    inline void addConnections(BKItem::PtrArr theseItems)
    {
        for (auto item : theseItems)
        {
            if (!isConnectedTo(item)) connections.add(item);
        }
    }
    
    
    inline void removeConnection(BKPreparationType type, int Id)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if ((connections.getUnchecked(i)->getType() == type) && (connections.getUnchecked(i)->getId() == Id))
            {
                connections.remove(i);
                break;
            }
        }
    }
    
    inline void removeConnection(BKItem::Ptr thisItem)
    {
        int index = 0;
        for (auto item : connections)
        {
            if (item == thisItem) connections.remove(index);
            
            index++;
        }
    }
    
    inline bool isConnectedTo(BKPreparationType type, int Id)
    {
        for (auto item : connections)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline bool isConnectedTo(BKItem::Ptr thisItem)
    {
        for (auto item : connections)
        {
            if ((item->getType() == thisItem->getType()) && (item->getId() == thisItem->getId()))
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline void changeIdOfConnection(BKPreparationType type, int oldId, int newId)
    {
        for (auto item : connections)
        {
            if (item->getType() == type && item->getId() == oldId)
            {
                item->setId(newId);
                break;
            }
        }
    }
    
    inline bool isConnectedToAnyPreparation(void)
    {
        BKItem::PtrArr theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() >= PreparationTypeDirect && item->getType() <= PreparationTypeTempo)
            {
                return true;
            }
        }
        return false;
    }
    
    inline BKItem::PtrArr getConnectionsOfType(BKPreparationType type)
    {
        BKItem::PtrArr theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() == type) theseItems.add(item);
        }
        
        return theseItems;
    }
    
    inline Array<int> getConnectionIdsOfType(BKPreparationType type)
    {
        Array<int> theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() == type) theseItems.add(item->getId());
        }
        
        return theseItems;
    }
    
    inline BKItem::PtrArr getConnections(void) const noexcept { return connections; }
    
    inline Array<Array<int>> getConnectionIds(void)
    {
        Array<Array<int>> connectionIds;
        
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            Array<int> theseItems = getConnectionIdsOfType((BKPreparationType)i);
            connectionIds.add(theseItems);
        }
        return connectionIds;
    }
    
    
    inline String connectionsToString(void)
    {
        String s = "";
        for (int type = 0; type < BKPreparationTypeNil; type++)
        {
            BKItem::PtrArr connex = getConnectionsOfType((BKPreparationType)type);
            
            s += cPreparationTypes[type]+":";
            
            for (auto item : connex)
            {
                s += " " + String(item->getId());
            }
        }
        return s;
    }
    
    ValueTree getState(void);
    void setState(XmlElement* xml);
    
    inline void clearConnections(void)
    {
        connections.clear();
    }
    
    inline void clearConnectionsOfType(BKPreparationType type)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if (connections.getUnchecked(i)->getType() == type) connections.remove(i);
        }
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
    
    void configurePianoCB(void);
    
private:
    BKAudioProcessor& processor;
    Label label;
    
    // Piano menu
    BKComboBox menu;
    
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
    
    void clear(void);
    
    void connect(BKItem* item1, BKItem* item2);
    void disconnect(BKItem* item1, BKItem* item2);
    
    
    void reconstruct(void);
    
    inline BKPreparationType getModType(BKPreparationType type)
    {
        return (BKPreparationType)(type+6);
    }
    
    bool isValidConnection(BKPreparationType type1, BKPreparationType type2);
    
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
    
private:
    BKAudioProcessor& processor;
    
    BKItem::RCArr items;

    void addPreparationToKeymap(BKPreparationType thisType, int thisId, int keymapId);
    
    JUCE_LEAK_DETECTOR(BKItemGraph)
};





#endif  // BKGRAPH_H_INCLUDED
