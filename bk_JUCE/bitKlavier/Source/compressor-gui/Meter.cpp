/*
Copyright (C) 2020 Phillip Lamp
This file is part of the CTAGDRC project <https://github.com/p-hlp/CTAGDRC>.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "include/Meter.h"
#include "include/Constants.h"


Meter::Meter(ComboBox* selectBox) :
selectBox(selectBox)
{
    //Init vars
    startAngle = static_cast<float>(5.0f / 3.0f * MathConstants<float>::pi);
    endAngle = static_cast<float>(7.0f / 3.0f * MathConstants<float>::pi);
    meterBg.prepare(startAngle, endAngle);
    meterBg.setMode(GR);
    needle.prepare(startAngle, endAngle);
    needle.setMode(GR);
    addAndMakeVisible(modeBox);
    modeBox.addItem("Input", 1);
    modeBox.addItem("Output", 2);
    modeBox.addItem("Gain Reduction", 3);
    modeBox.setJustificationType(Justification::centred);
    modeBox.setSelectedId(3);
    
    modeBox.onChange = [this] { modeBoxChanged(); };
    addAndMakeVisible(modeBox);
    addAndMakeVisible(meterBg);
    addAndMakeVisible(needle);

    addAndMakeVisible(selectBox);
    backgroundDarkGrey = Colour(Constants::Colors::bg_DarkGrey);
}

void Meter::paint(Graphics& g)
{
    g.setColour(backgroundDarkGrey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3);
}

void Meter::resized()
{
    auto bounds = getLocalBounds();
    auto boxHeight = (bounds.getHeight() / 10) * 2;
    boxArea = bounds.removeFromTop(boxHeight);
    // const FlexItem::Margin comboMargin = FlexItem::Margin(3.0f, 8.0f, 3.0f, 8.0f);
    auto modeBoxArea = boxArea.removeFromLeft(boxArea.getWidth()/2);
    modeBoxArea.reduce(2,2);
    modeBox.setBounds(modeBoxArea);
    boxArea.reduce(2,2);
    selectBox->setBounds(boxArea);
//    mBox.flexWrap = FlexBox::Wrap::noWrap;
//    mBox.flexDirection = FlexBox::Direction::row;
//    mBox.justifyContent = FlexBox::JustifyContent::flexStart;
//    mBox.items.add(FlexItem(modeBox).withFlex(1).withMargin(comboMargin));
//    mBox.items.add(FlexItem(*selectBox).withFlex(1).withMargin(comboMargin));
//    mBox.performLayout(boxArea.toFloat());

    meterBg.setBounds(bounds);
    needle.setBounds(bounds);
}

void Meter::setMode(int m)
{
    meterBg.setMode(m);
    needle.setMode(m);
    meterMode = m;
}

void Meter::modeBoxChanged()
{
    setMode(modeBox.getSelectedId());
}

void Meter::update(const float& val)
{
    if (val != valueInDecibel)
    {
        needle.update(val);
    }
}

int Meter::getMode()
{
    return meterMode;
}

float Meter::getValue()
{
    return valueInDecibel;
}

void Meter::setGUIEnabled(bool state)
{
    needle.setNeedleEnabled(state);
    meterBg.setIndicatorEnabled(state);
}
