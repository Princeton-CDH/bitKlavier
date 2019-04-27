stk_module
==========

JUCE module wrapper for the STK library.

This wrapper has all the real-time classes (audio-in, threading, sockets etc.) removed to reduce external dependancies and because JUCE provides all of this functionality.

This module is designed as a quick way to add effects and generators to a JUCE based audio project. To use simply add the module to the JUCE modules folder, create a new project in the Introjucer and add the module. All the classes are in the stk namespace.

There are a few amendments to the STK codebase in order to remove some compiler warnings.