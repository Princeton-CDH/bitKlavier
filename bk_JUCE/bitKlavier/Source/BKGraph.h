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
class BKConstructionSite;

class BKItem : public ItemMapper, public BKDraggableComponent, public BKListener, private Timer
{
public:
    typedef ReferenceCountedArray<BKItem, CriticalSection>      PtrArr;
    typedef ReferenceCountedObjectPtr<BKItem>                   Ptr;
    
    BKItem(BKPreparationType type, int Id, BKAudioProcessor& p);
    
    uint64 time, synchronicNotePlayTime;
    void timerCallback(void) override;
    
    ~BKItem(void);

    BKItem* duplicate(void);
    
    void mouseDown(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    
    void paint(Graphics& g) override;
    
    void resized(void) override;
    
    void itemIsBeingDragged(const MouseEvent&) override;
    
    void keyPressedWhileSelected(const KeyPress&) override;

    void setItemType(BKPreparationType type, bool create);
    
    inline void setSelected(bool select)
    {
        isSelected = select;
        repaint();
    }
    inline bool getSelected(void) { return isSelected;}
    
    inline void setConnections(BKItem::PtrArr newConnections)
    {
        connections = newConnections;
    }
    
    inline void addConnection(BKItem::Ptr item)
    {
        if (!isConnectedTo(item))   connections.add(item);
    }
    
    inline void addConnections(BKItem::PtrArr theseItems)
    {
        for (auto item : theseItems)
        {
            if (!isConnectedTo(item)) connections.add(item);
        }
    }
    
    
    inline bool removeConnection(BKPreparationType type, int Id)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if ((connections.getUnchecked(i)->getType() == type) && (connections.getUnchecked(i)->getId() == Id))
            {
                connections.remove(i);
                return true;
            }
        }
        return false;
    }
    
    inline bool removeConnection(BKItem::Ptr thisItem)
    {
        int index = 0;
        for (auto item : connections)
        {
            if (item == (BKItem *)thisItem)
            {
                connections.remove(index);
                return true;
            }
            
            index++;
        }
        return false;
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
        for (auto item : connections)
        {
            if ((item->getType() >= PreparationTypeDirect && item->getType() <= PreparationTypeTempo) ||
                item->getType() == PreparationTypeBlendronic)
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
            
            s = s + cPreparationTypes[type]+":";
            
            for (auto item : connex)
            {
                s += " " + String(item->getId());
            }
            
            connex.clear();
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
    juce::Point<float> origin;
    
    Image image;
    RectanglePlacement placement;
    
    //void textEditorReturnKeyPressed     (TextEditor&)           override;
    void bkTextFieldDidChange           (TextEditor&)           override {};
    //void bkTextFieldReturnKeyPressed    (TextEditor&)           override;
    void bkComboBoxDidChange            (ComboBox*)             override;
    void bkButtonClicked                (Button* b)             override {};
    void bkMessageReceived              (const String& message) override {};
    
    juce::Point<int> lastClick;
    
    juce::Point<int> position;
    
    void setImage(Image newImage);
    
    void configurePianoCB(void);
    void configureComment(void);

    BKItem::PtrArr connections;
    
    bool resizing, wasJustDragged;
    
    void setCommentText(String text) { comment.setText(text);}
    String getCommentText(void) { return comment.getText();}
    
    std::shared_ptr<XmlElement> getContent(void) { return content;}
    void setContent(std::shared_ptr<XmlElement> xml) {content = xml;}
    
private:
    
    BKAudioProcessor& processor;
    Label label;
    
    std::unique_ptr<ComponentBoundsConstrainer> constrain;
    
    std::unique_ptr<ResizableCornerComponent> resizer;
    
    // Piano menu
    BKComboBox menu;
    
    // Comment
    TextEditor comment;
    
    // UI stuff
    Component fullChild;
    
    std::shared_ptr<XmlElement> content;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKItem)
};

class BKItemGraph
{
public:
    BKItemGraph(BKAudioProcessor& p);
    
    ~BKItemGraph(void);

    BKItem::Ptr get(BKPreparationType type, int Id);

    void addItem(BKItem* thisItem);
    void removeItem(BKItem* thisItem);
    
    bool contains(BKItem* thisItem);
    bool contains(BKPreparationType type, int Id);
    
    void clear(void);
    void clearItems(void);
    
    bool connect(BKPreparationType type1, int id1, BKPreparationType type2, int id2);
    bool connect(BKItem* item1, BKItem* item2);
    bool disconnect(BKItem* item1, BKItem* item2);
    
    bool isValidConnection(BKPreparationType type1, BKPreparationType type2);
    
    void reconstruct(void);
    
    
    BKPreparationType getModType(BKPreparationType type);
    
    void select(BKItem* item);
    
    void deselect(BKItem* item);
    
    void deselectAll(void);
    
    void selectAll(void);
    
    BKItem::PtrArr getSelectedItems(void);
    
    Array<int> getPreparationIds(BKItem::PtrArr theseItems);
    
    void print(void);
    
    
    BKItem::PtrArr getItems(void);

    
    Array<Line<int>> getLines(void);
    
private:
    BKAudioProcessor& processor;
    
    JUCE_LEAK_DETECTOR(BKItemGraph)
};





#endif  // BKGRAPH_H_INCLUDED
