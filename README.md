# bitKlavier: the prepared digital piano

Developed by Dan Trueman and Michael Mulshine.

bitKlavier takes inspiration from John Cage's *prepared piano*, but instead of screws and erasers we place a reconfigurable collection of digital machines between the virtual strings of the digital piano. Learn more at the [bitKlavier website](http://bitklavier.com).

bitKlavier is built with [JUCE](http://juce.com>), the C++ audio programming framework ([available under GPLv3 license](https://github.com/WeAreROLI/JUCE)).

Development on bitKlavier is sponsored by the [Center for Digital Humanities @ Princeton University](https://cdh.princeton.edu/).  See the [CDH project page](https://cdh.princeton.edu/projects/bitklavier/) for more details.

## License

This project is made available under the [GPLv3 license](https://www.gnu.org/licenses/quick-guide-gplv3.en.html)

## Installation

bitKlavier is currently in beta. See [releases](https://github.com/Princeton-CDH/bitKlavier/releases)
to download the latest version.  The current version requires that you also
download a [resource package with samples and galleries](http://manyarrowsmusic.com/bitKlavier/bitKlavier_ModelB/bitKlavier_resources.zip).  Currently, this resource folder must be placed in your
Documents folder.

## Development setup and instructions

Branches in this git repository are named based on [git flow](https://github.com/nvie/gitflow) branching
conventions; *master* contains code for the current release, new feature development
for the next release is in *dev*.

Download [JUCE and Projucer](https://juce.com/get-juce/download) for your platform,
install the [appropriate compiler and devlopment tools](https://www.juce.com/learn/getting-started),
and open the project in Projucer.  Code is structured in the standard
Model-View-Controller design pattern.

To compile bitKlavier as a VST plugin, you will also need
[Steinberg's VST-SDK](https://www.steinberg.net/en/company/developers.html).

To run your locally compiled version of bitKlavier, you will also need the
resource folder (see Installation above).


