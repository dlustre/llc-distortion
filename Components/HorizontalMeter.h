#pragma once

#include <JuceHeader.h>

namespace Gui {
	class HorizontalMeter : public juce::Component {
	public: 
		void paint(Graphics& g) override {
			auto bounds = getLocalBounds().toFloat();

			g.setColour(Colours::white.withBrightness(0.4f));
			g.fillRoundedRectangle(bounds, 5.f);

			gradient = ColourGradient{
				juce::Colour(39, 68, 156),
				bounds.getBottomLeft(),
				juce::Colour(67, 113, 250),
				bounds.getBottomRight(),
				false
			};
			
			gradient.addColour(0.5, juce::Colour(65, 105, 225));

			//g.setColour(Colours::white);
			g.setGradientFill(gradient);

			const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getWidth()));
			g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 5.f);

		}
		void setLevel(const float value) { level = value; }
	private: 
		float level = -60.f;
		juce::ColourGradient gradient{};
	};
}