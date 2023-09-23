#pragma once
#include "JuceHeader.h"

template<typename C>
void MoveItemBefore(C& container, size_t currentIndex, size_t indexOfItemToPlaceBefore)
{
    if (currentIndex == indexOfItemToPlaceBefore) return;

    jassert(isPositiveAndBelow((int)currentIndex, (int)container.size()));
    jassert(isPositiveAndBelow((int)indexOfItemToPlaceBefore, (int)container.size()));

    if (currentIndex < indexOfItemToPlaceBefore)
    {
        std::rotate(container.begin() + currentIndex,
            container.begin() + currentIndex + 1,
            container.begin() + indexOfItemToPlaceBefore);
    }
    else
    {
        std::rotate(container.begin() + indexOfItemToPlaceBefore,
            container.begin() + currentIndex,
            container.begin() + currentIndex + 1);
    }
}

template<typename C>
void MoveItemAfter(C& container, size_t currentIndex, size_t indexOfItemToPlaceAfter)
{
    if (currentIndex == indexOfItemToPlaceAfter) return;

    jassert(isPositiveAndBelow((int)currentIndex, (int)container.size()));
    jassert(isPositiveAndBelow((int)indexOfItemToPlaceAfter, (int)container.size()));

    if (currentIndex < indexOfItemToPlaceAfter)
    {
        std::rotate(container.begin() + currentIndex,
            container.begin() + currentIndex + 1,
            container.begin() + indexOfItemToPlaceAfter + 1);
    }
    else
    {
        std::rotate(container.begin() + indexOfItemToPlaceAfter + 1,
            container.begin() + currentIndex,
            container.begin() + currentIndex + 1);
    }
}
