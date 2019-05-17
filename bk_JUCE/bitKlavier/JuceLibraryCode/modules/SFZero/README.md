# SFZero, the Juce module version

This is a fork of the [original SFZero by Steve Folta](https://github.com/stevefolta/SFZero), with the following changes:

* has been converted to a Juce module, so you can easily consume it from your own projects (you still get the sample player plugin, but it now includes that module)
* requires Juce 4.1 or higher
* supports Juce 4.1 module format
* now also supports new Juce 4.2 module format (thanks to Loki Davison)
* conveniently sits within its own `sfzero::` namespace
* has a tidied-up code base, so it now builds with as few warnings as possible on all platforms and on both 32/64 bit architectures. I also simplified logging, added support for synchronous sample loading, and fixed a few bugs.

For more information, please see also this [blog article](http://www.mucoder.net/blog/2016/03/24/sfzero.html)

Please note that, in order to build, SFZero requires [Juce](http://www.juce.com).

Before building the plugin, it's necessary to

* copy the modules/SFZero folder as a childfolder to your Juce modules folder.
* load `plugin/SFZero.jucer` into your IntroJucer tool and save the project again. This should regenerate the project build definitions with the proper links to your Juce module location.

You can find this fork's source code at: [https://github.com/altalogix/SFZero](https://github.com/altalogix/SFZero)

