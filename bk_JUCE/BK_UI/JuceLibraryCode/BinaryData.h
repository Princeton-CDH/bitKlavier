/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   direct_icon_png;
    const int            direct_icon_pngSize = 7648;

    extern const char*   icon_png;
    const int            icon_pngSize = 38622;

    extern const char*   keymap_icon_png;
    const int            keymap_icon_pngSize = 3135;

    extern const char*   mod_icon_png;
    const int            mod_icon_pngSize = 2536;

    extern const char*   nostalgic_icon_png;
    const int            nostalgic_icon_pngSize = 11489;

    extern const char*   piano_icon_png;
    const int            piano_icon_pngSize = 3337;

    extern const char*   reset_icon_png;
    const int            reset_icon_pngSize = 2039;

    extern const char*   synchronic_icon_png;
    const int            synchronic_icon_pngSize = 22367;

    extern const char*   tempo_icon_png;
    const int            tempo_icon_pngSize = 16810;

    extern const char*   tuning_icon_png;
    const int            tuning_icon_pngSize = 12404;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 10;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
