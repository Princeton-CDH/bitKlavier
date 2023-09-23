# juce-draggableListBox
In July 2018, Charles Schiermeyer (aka *matkatmusic*) submitted a [query to the JUCE Forum](https://forum.juce.com/t/listbox-drag-to-reorder-solved/28477), asking for help implementing drag-to-reorder functionality in the JUCE *ListBox* class. Shortly afterward, the forum post was marked "Solved", and Charles was kind enough to provide a [full example implementation hosted on BitBucket](https://bitbucket.org/MatkatMusic/listboxreorder/).

This is a revised version of Charles's code, in which I have separated the app-specific code from the more generic code, expanded support for a custom list-element Component class, and added support for list-management operations such as deleting items and adding new ones. (See the commit history for the refactoring steps involved.)

To implement a JUCE list-box with drag-to-reorder capability:
1. Include *DraggableListBox.h* and *DraggableListBox.cpp* in your JUCE project.
2. Define your own app-specific class for your actual list data, which inherits from *DraggableListBoxItemData*, ensuring that you override the following member functions:
   - *getNumItems()* should return the number of list elements.
   - *paintContents()* is a callback to render one specific list item.
   - *moveBefore()* and *moveAfter()* are calbacks which re-order the list, moving the dragged item either before or after the item it was dropped onto.
3. Define a custom list-item Component which inherits from *DraggableListBoxItem*. This may contain any sub-components you need, e.g., the example version includes a "Delete" button to trigger deletion of a list item, and an "Action" button to trigger some action on a single item. (The corresponding *doItemAction()* function is implemented in the app-specific item data class.)
4. Define a custom *model* class which inherits from *DraggableListBoxModel*, which overrides *refreshComponentForRow()* to ensure that your new custom list-item Component class is used.
5. Declare your list-box widget using class *DraggableListBox*, plus two companion objects:
   - one object of the app-specific data class you defined at step 2.
   - one object of the customized model class you defined at step 4.
6. Initialize your model object with references to your *DraggableListBox* and your app-specific data object.
7. Set your *DraggableListBox*'s model to be your model object (by calling the former's *setModel()* member).

In this example, all custom list-related classes are defined in *MyListComponent.h/.cpp*, and the surrounding GUI is defined in *MainContentComponent.h/.cpp*. (*Main.cpp* is a Projucer-generated file and is completely generic.)

**Note** I have used a *juce::OwnedArray* as my app-specific data container. If you prefer to use a *std::vector* (or any of several other container classes in the C++ STL), you can use the function templates found in *ReorderFunctions.h* (from Charles's original code, but not used in mine) to implement the *moveBefore()/moveAfter()* operations.
