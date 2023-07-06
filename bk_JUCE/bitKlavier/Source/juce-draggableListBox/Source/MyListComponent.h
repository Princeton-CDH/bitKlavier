#pragma once
#include "DraggableListBox.h"
#include "../../Piano.h"
// Application-specific data container
struct MyListBoxItemData : public DraggableListBoxItemData
{
    std::vector<Piano::Ptr> modelData;
    ValueTree getState()
    {
        ValueTree listBoxVt(vtagPianoIterator);
        for (auto piano : modelData)
        {
            ValueTree _piano("_piano");
            _piano.setProperty("id", piano->getId(),0);
            listBoxVt.addChild(_piano,-1,0);
        }
        return listBoxVt;
        
    }
    
    void setState(XmlElement* xml, Piano::PtrArr bkPianos)
    {
        for (auto item : xml->getChildIterator())
        {
            modelData.push_back(bkPianos[item->getStringAttribute("id").getIntValue()]);
        }
            
            
    }
    
    int getNumItems() override
    {
        return int(modelData.size());
    }

    void deleteItem(int indexOfItemToDelete) override
    {
        modelData.erase(modelData.begin()+indexOfItemToDelete);
    }

    void addItemAtEnd() override
    {
        //modelData.add(new String("Yahoo"));
    }
    void duplicate(int rowNum)
    {
        modelData.insert(modelData.begin() + rowNum, modelData.at(rowNum));
        printItemsInOrder();
    }
    void paintContents(int rowNum, Graphics& g, Rectangle<int> bounds) override
    {
        g.fillAll(Colours::lightgrey);
        g.setColour(Colours::black);
        g.drawRect(bounds);
        g.drawText(modelData.at(rowNum)->getName(), bounds, Justification::centred);
    }

    void moveBefore(int indexOfItemToMove, int indexOfItemToPlaceBefore) override
    {
//        DBG("Move item " + String(indexOfItemToMove) + " before item " + String(indexOfItemToPlaceBefore));
//        if (indexOfItemToMove <= indexOfItemToPlaceBefore)
//            modelData.move(indexOfItemToMove, indexOfItemToPlaceBefore - 1);
//        else
//            modelData.move(indexOfItemToMove, indexOfItemToPlaceBefore);
//        printItemsInOrder();
    }

    void moveAfter(int indexOfItemToMove, int indexOfItemToPlaceAfter) override
    {
//        DBG("Move item " + String(indexOfItemToMove) + " after item " + String(indexOfItemToPlaceAfter));
//        if (indexOfItemToMove <= indexOfItemToPlaceAfter)
//            modelData.move(indexOfItemToMove, indexOfItemToPlaceAfter);
//        else
//            modelData.move(indexOfItemToMove, indexOfItemToPlaceAfter + 1);
//        printItemsInOrder();
    }
    
    template <class T> int move_selected(std::vector<T>& v, const std::vector<int>& selection, int index)
    {
        
        std::vector<int> sorted_sel = selection;
        std::sort(sorted_sel.begin(), sorted_sel.end());
        std::vector<int> originalPos;
        for (int i = 0; i < modelData.size(); i++)
        {
            originalPos.push_back(i);
        }
        std::sort(originalPos.begin(), originalPos.end());
        
        // 1) Define checker lambda
        // 'filter' is called only once for every element,
        // all the calls respect the original order of the array
        // We manually keep track of the item which is filtered
        // and this way we can look this index in 'sorted_sel' array
      
        int itemIndex2 = 0;
    
        
        auto filter1 = [&originalPos, &itemIndex2, &sorted_sel](const int& item) {
            return !std::binary_search(
                                   sorted_sel.begin(),
                                   sorted_sel.end(),
                                       std::distance(originalPos.begin(), find(originalPos.begin(), originalPos.end(), item)));
        };

  
        // 2) Move all 'not-selected' indices to the end
        auto end_of_selected_indices = std::stable_partition(
                               originalPos.begin(),
                               originalPos.end(),
                               filter1);
        
        
        originalPos.resize(std::distance(originalPos.begin(), end_of_selected_indices));

        int newIndex = std::distance(originalPos.begin(), find(originalPos.begin(), originalPos.end(), index));
        std::sort(sorted_sel.begin(), sorted_sel.end());

        // Packed (data+flag) array
        struct T_withFlag
        {
            T_withFlag(const T* ref = nullptr, bool sel = false): src(ref), selected(sel) {}
            
            const T* src;
            bool selected;
        };

        std::vector<T_withFlag> v_with_flags;

       // should be like
       //      { {0, true}, {0, true}, {3, false},
       //        {0, true}, {2, false}, {4, false},
       //        {5, false}, {0, true}, {7, false} };
       //  for the input data in main()

        v_with_flags.reserve(v.size());

        // No "beautiful" way to iterate a vector
        // and keep track of element index
        // We need the index to check if it is selected
        // The check takes O(log(n)), so the loop is O(n * log(n))
        int itemIndex = 0;
        for (auto& ii: v)
                v_with_flags.emplace_back(
                    T_withFlag(&ii,
                               std::binary_search(
                                  sorted_sel.begin(),
                                  sorted_sel.end(),
                                  itemIndex++)
                               ));
//        String msg = "\nbeeeeeeeeep: ";
//        for (auto item : v_with_flags) msg << String((int)item.selected) + String(*item.src) << " ";
//        DBG(msg);
            // I. (The bulk of ) Removal algorithm
            //   a) Define checker lambda
            auto filter = [](const T_withFlag& ii) { return !ii.selected; };
            //   b) Move every item marked as 'not-selected'
            //      to the end of an array
            auto end_of_selected = std::stable_partition(
                                       v_with_flags.begin(),
                                       v_with_flags.end(),
                                       filter);
//            //   c) Cut off the end of the std::vector
//        msg = "\nboooooop: ";
//        for (auto item : v_with_flags) msg << String((int)item.selected) + String(*item.src) << " ";
//        DBG(msg);
//
        std::vector<T_withFlag> v_move_flags(end_of_selected, v_with_flags.end());
        std::vector<T> v_move(v_move_flags.size());
        std::transform(
                   // for C++20 you can parallelize this
                   // with 'std::execution::par' as first parameter
                   v_move_flags.begin(),
                   v_move_flags.end(),
                   v_move.begin(),
                   [](const T_withFlag& ii) { return *(ii.src); });
        v_with_flags.resize(std::distance(v_with_flags.begin(), end_of_selected));
        v.resize(std::distance(v_with_flags.begin(), end_of_selected));
            // II. Output
            
            std::transform(
                       // for C++20 you can parallelize this
                       // with 'std::execution::par' as first parameter
                       v_with_flags.begin(),
                       v_with_flags.end(),
                       v.begin(),
                       [](const T_withFlag& ii) { return *(ii.src); });
       
        v.insert(v.begin()+newIndex, v_move.begin(), v_move.end());
        return newIndex;
    }
    
    int moveToIndex(std::vector<int> indexesToMove, int indexToMoveTo) override
    {
        printItemsInOrder();
        ///String insertBefore = modelData[indexToMoveTo];
        Array<String> moved;
        Array<int> originalPos;
        for (int i = 0; i < modelData.size(); i++)
        {
            originalPos.add(i);
        }
//        String msg = "\nindexesToMove: ";
//        for (auto item : indexesToMove) msg << String(item) << " ";
//        DBG(msg);
//        msg = "\noriginalPos: ";
//        for (auto item : originalPos) msg << String(item) << " ";
//        DBG(msg);
//        msg = "\nmoved: ";
//        for (auto item : moved) msg << item << " ";
//        DBG(msg);
//        String empty("?");
//        DBG("Before move");
//        for (int i = 0; i < indexesToMove.size(); i++)
//        {
//            int index = indexesToMove.getUnchecked(i);
//            DBG(*modelData.getUnchecked(index));
//            moved.add(*modelData.getUnchecked(index));
//            modelData.set(index, new String(empty));
//            originalPos.set(index, -1);
//        }
        
        int newSelected = move_selected(modelData, indexesToMove, indexToMoveTo);
        DBG("after move");
//        msg = "\nindexesToMove: ";
//        for (auto item : indexesToMove) msg << String(item) << " ";
//        DBG(msg);
//        msg = "\noriginalPos: ";
//        for (auto item : originalPos) msg << String(item) << " ";
//        DBG(msg);
//        msg = "\nmoved: ";
//        for (auto item : moved) msg << item << " ";
//        DBG(msg);
        //printItemsInOrder();
//        Array<int> newIndices;
//        int newIndex = originalPos.indexOf(indexToMoveTo);
//        //newIndices.add(newIndex);
//        for (int i = 0; i < moved.size(); i++)
//        {
//            modelData.insert(newIndex, new String(moved.getUnchecked(i)));
//            newIndices.add(newIndex++);
//        }
//        printItemsInOrder();
//        return newIndices;
        return newSelected;
    }
    // Not required, just something I'm adding for confirmation of correct order after DnD.
    // This is an example of an operation on the entire list.
    void printItemsInOrder()
    {
//        String msg = "\nitems: ";
//        for (auto item : modelData) msg << item << " ";
//        DBG(msg);
    }

    // This is an example of an operation on a single list item.
    void doItemAction(int itemIndex)
    {
        //DBG(modelData[itemIndex]);
    }
};

// Custom list-item Component (which includes item-delete button)
class MyListComponent : public DraggableListBoxItem
{
public:
    MyListComponent(DraggableListBox& lb, MyListBoxItemData& data, int rn);
    ~MyListComponent();

    void paint(Graphics&) override;
    void resized() override;
   
protected:
    Rectangle<int> dataArea;
    TextButton actionBtn, deleteBtn;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyListComponent)
};

// Customized DraggableListBoxModel overrides refreshComponentForRow() to ensure that every
// list-item Component is a MyListComponent.
class MyListBoxModel : public DraggableListBoxModel
{
public:
    MyListBoxModel(DraggableListBox& lb, DraggableListBoxItemData& md)
        : DraggableListBoxModel(lb, md) {}

    //actually gets called
    Component* refreshComponentForRow(int, bool, Component*) override;
    
   
};
