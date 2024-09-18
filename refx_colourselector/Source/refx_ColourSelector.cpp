namespace reFX
{

//==============================================================================
struct ColourComponentSlider  : public juce::Slider
{
    ColourComponentSlider (const juce::String& name, int max)  : juce::Slider (name)
    {
        setRange (0.0, double (max), 0.0);
    }

    juce::String getTextFromValue (double value) override
    {
        return juce::String ((int) value);
    }

    double getValueFromText (const juce::String& text) override
    {
        return (double) text.getIntValue();
    }
};

//==============================================================================
class ColourSelector::OriginalColourComp : public juce::Component
{
public:
    OriginalColourComp (ColourSelector& cs)
        : owner (cs)
    {
        setWantsKeyboardFocus (true);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds().reduced (1);

        g.fillCheckerBoard (rc.toFloat(), 6.0f, 6.0f,
                            juce::Colour (0xffdddddd),
                            juce::Colour (0xffffffff));

        g.setColour (owner.colour.getColour());
        g.fillRect (rc.removeFromTop (rc.getHeight() / 2));

        g.setColour (owner.originalColour.getColour());
        g.fillRect (rc);
    }

    ColourSelector& owner;
};

//==============================================================================
class ColourSelector::Parameter2D : public Component
{
public:
    Parameter2D (ColourSelector& cs, int edgeSize)
        : owner (cs), edge (edgeSize)
    {
        setWantsKeyboardFocus (true);
        addAndMakeVisible (marker);
        setMouseCursor (juce::MouseCursor::CrosshairCursor);
    }

    void setParameters (Params x_, Params y_)
    {
        xParam = x_;
        yParam = y_;

        updateIfNeeded();
    }

    void paint (juce::Graphics& g) override
    {
        if (colours.isNull())
            updateImage();

        g.setOpacity (1.0f);
        g.drawImageTransformed (colours,
                                juce::RectanglePlacement (juce::RectanglePlacement::stretchToFit)
                                    .getTransformToFit (colours.getBounds().toFloat(),
                                                        getLocalBounds().reduced (edge).toFloat()),
                                false);
    }

    void updateImage()
    {
        auto width = getWidth() / 2;
        auto height = getHeight() / 2;
        colours = juce::Image (juce::Image::RGB, width, height, false);

        juce::Image::BitmapData pixels (colours, juce::Image::BitmapData::writeOnly);

        for (int y = 0; y < height; ++y)
        {
            auto yVal = 1.0f - (float) y / (float) height;

            for (int x = 0; x < width; ++x)
            {
                auto xVal = (float) x / (float) width;

                auto c = owner.colour;

                auto set = [&] (Params param, float val)
                {
                    if (param == Params::hue)
                    {
                        auto hsb = c.getHSB();
                        hsb.h = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (hsb);
                    }
                    else if (param == Params::saturation)
                    {
                        auto hsb = c.getHSB();
                        hsb.s = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (hsb);
                    }
                    else if (param == Params::brightness)
                    {
                        auto hsb = c.getHSB();
                        hsb.b = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (hsb);
                    }
                    else if (param == Params::red)
                    {
                        auto rgb = c.getRGB();
                        rgb.r = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (rgb);
                    }
                    else if (param == Params::blue)
                    {
                        auto rgb = c.getRGB();
                        rgb.b = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (rgb);
                    }
                    else if (param == Params::green)
                    {
                        auto rgb = c.getRGB();
                        rgb.g = juce::jlimit (0.0f, 1.0f, val);
                        c = DeepColour (rgb);
                    }
                    else
                    {
                        jassertfalse;
                    }
                };

                set (xParam, xVal);
                set (yParam, yVal);

                pixels.setPixelColour (x, y, c.getColour());
            }
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        grabKeyboardFocus();
        mouseDrag (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto xVal =        (float) (e.x - edge) / (float) (getWidth()  - edge * 2);
        auto yVal = 1.0f - (float) (e.y - edge) / (float) (getHeight() - edge * 2);

        auto set = [&] (Params param, float val)
        {
            if (param == Params::hue)
            {
                auto hsb = owner.colour.getHSB();
                hsb.h = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
            }
            else if (param == Params::saturation)
            {
                auto hsb = owner.colour.getHSB();
                hsb.s = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
            }
            else if (param == Params::brightness)
            {
                auto hsb = owner.colour.getHSB();
                hsb.b = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
            }
            else if (param == Params::red)
            {
                auto rgb = owner.colour.getRGB();
                rgb.r = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
            }
            else if (param == Params::blue)
            {
                auto rgb = owner.colour.getRGB();
                rgb.b = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
            }
            else if (param == Params::green)
            {
                auto rgb = owner.colour.getRGB();
                rgb.g = juce::jlimit (0.0f, 1.0f, val);
                owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
            }
            else
            {
                jassertfalse;
            }
        };

        set (xParam, xVal);
        set (yParam, yVal);
    }

    void updateIfNeeded()
    {
        colours = {};
        repaint();
        updateMarker();
    }

    void resized() override
    {
        colours = {};
        updateMarker();
    }

private:
    ColourSelector& owner;
    const int edge;
    juce::Image colours;
    Params xParam = Params::hue;
    Params yParam = Params::saturation;

    struct Parameter2DMarker  : public Component
    {
        Parameter2DMarker()
        {
            setInterceptsMouseClicks (false, false);
        }

        void paint (juce::Graphics& g) override
        {
            g.setColour (juce::Colour::greyLevel (0.1f));
            g.drawEllipse (1.0f, 1.0f, (float) getWidth() - 2.0f, (float) getHeight() - 2.0f, 1.0f);
            g.setColour (juce::Colour::greyLevel (0.9f));
            g.drawEllipse (2.0f, 2.0f, (float) getWidth() - 4.0f, (float) getHeight() - 4.0f, 1.0f);
        }
    };

    Parameter2DMarker marker;

    void updateMarker()
    {
        auto markerSize = juce::jmax (14, edge * 2);
        auto area = getLocalBounds().reduced (edge);

        auto get = [&] (Params param)
        {
            if (param == Params::hue)
                return owner.colour.getHSB().h;
            else if (param == Params::saturation)
                return owner.colour.getHSB().s;
            else if (param == Params::brightness)
                return owner.colour.getHSB().b;
            else if (param == Params::red)
                return owner.colour.getRGB().r;
            else if (param == Params::blue)
                return owner.colour.getRGB().b;
            else if (param == Params::green)
                return owner.colour.getRGB().g;
            else
                jassertfalse;
            return 0.0f;
        };

        marker.setBounds (juce::Rectangle<int> (markerSize, markerSize).withCentre (area.getRelativePoint (get (xParam), 1.0f - get (yParam))));
    }

    JUCE_DECLARE_NON_COPYABLE (Parameter2D)
};

//==============================================================================
class ColourSelector::Parameter1D  : public Component
{
public:
    Parameter1D (ColourSelector& cs, int edgeSize)
        : owner (cs), edge (edgeSize)
    {
        setWantsKeyboardFocus (true);
        addAndMakeVisible (marker);
    }

    void setParameter (Params p)
    {
        param = p;

        updateIfNeeded();
    }

    void paint (juce::Graphics& g) override
    {
        juce::ColourGradient cg;
        cg.isRadial = false;
        cg.point1.setXY (0.0f, (float) edge);
        cg.point2.setXY (0.0f, (float) getHeight());

        auto c = owner.colour;
        for (float i = 0.0f; i <= 1.0f; i += 0.02f)
        {
            if (param == Params::hue)
            {
                HSB hsb;
                hsb.h = 1.0f - i;
                hsb.s = 1.0f;
                hsb.b = 1.0f;
                cg.addColour (i, DeepColour (hsb).getColour());
            }
            else if (param == Params::saturation)
            {
                auto hsb = c.getHSB();
                hsb.s = 1.0f - i;
                cg.addColour (i, DeepColour (hsb).getColour());
            }
            else if (param == Params::brightness)
            {
                auto hsb = c.getHSB();
                hsb.b = 1.0f - i;
                cg.addColour (i, DeepColour (hsb).getColour());
            }
            else if (param == Params::red)
            {
                auto rgb = c.getRGB();
                rgb.r = 1.0f - i;
                cg.addColour (i, DeepColour (rgb).getColour());
            }
            else if (param == Params::blue)
            {
                auto rgb = c.getRGB();
                rgb.b = 1.0f - i;
                cg.addColour (i, DeepColour (rgb).getColour());
            }
            else if (param == Params::green)
            {
                auto rgb = c.getRGB();
                rgb.g = 1.0f - i;
                cg.addColour (i, DeepColour (rgb).getColour());
            }
        }

        g.setGradientFill (cg);
        g.fillRect (getLocalBounds().reduced (edge));
    }

    void resized() override
    {
        auto markerSize = juce::jmax (14, edge * 2);
        auto area = getLocalBounds().reduced (edge);

        auto get = [&] ()
        {
            if (param == Params::hue)
                return owner.colour.getHSB().h;
            else if (param == Params::saturation)
                return owner.colour.getHSB().s;
            else if (param == Params::brightness)
                return owner.colour.getHSB().b;
            else if (param == Params::red)
                return owner.colour.getRGB().r;
            else if (param == Params::blue)
                return owner.colour.getRGB().b;
            else if (param == Params::green)
                return owner.colour.getRGB().g;
            else
                jassertfalse;
            return 0.0f;
        };

        marker.setBounds (juce::Rectangle<int> (getWidth(), markerSize).withCentre (area.getRelativePoint (0.5f, 1.0f - get())));
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        grabKeyboardFocus();
        mouseDrag (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto val = 1.0f - (float) (e.y - edge) / (float) (getHeight() - edge * 2);

        if (param == Params::hue)
        {
            auto hsb = owner.colour.getHSB();
            hsb.h = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
        }
        else if (param == Params::saturation)
        {
            auto hsb = owner.colour.getHSB();
            hsb.s = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
        }
        else if (param == Params::brightness)
        {
            auto hsb = owner.colour.getHSB();
            hsb.b = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromHSB (hsb.h, hsb.s, hsb.b, owner.colour.getAlpha()));
        }
        else if (param == Params::red)
        {
            auto rgb = owner.colour.getRGB();
            rgb.r = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
        }
        else if (param == Params::blue)
        {
            auto rgb = owner.colour.getRGB();
            rgb.b = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
        }
        else if (param == Params::green)
        {
            auto rgb = owner.colour.getRGB();
            rgb.g = juce::jlimit (0.0f, 1.0f, val);
            owner.set (DeepColour::fromRGBA (rgb.r, rgb.g, rgb.b, owner.colour.getAlpha()));
        }
        else
        {
            jassertfalse;
        }
    }

    void updateIfNeeded()
    {
        repaint();
        resized();
    }

private:
    ColourSelector& owner;
    const int edge;

    struct Parameter1DMarker  : public Component
    {
        Parameter1DMarker()
        {
            setInterceptsMouseClicks (false, false);
        }

        void paint (juce::Graphics& g) override
        {
            auto cw = (float) getWidth();
            auto ch = (float) getHeight();

            juce::Path p;
            p.addTriangle (1.0f, 1.0f,
                           cw * 0.3f, ch * 0.5f,
                           1.0f, ch - 1.0f);

            p.addTriangle (cw - 1.0f, 1.0f,
                           cw * 0.7f, ch * 0.5f,
                           cw - 1.0f, ch - 1.0f);

            g.setColour (juce::Colours::white.withAlpha (0.75f));
            g.fillPath (p);

            g.setColour (juce::Colours::black.withAlpha (0.75f));
            g.strokePath (p, juce::PathStrokeType (1.2f));
        }
    };

    Parameter1DMarker marker;
    Params param = Params::hue;

    JUCE_DECLARE_NON_COPYABLE (Parameter1D)
};

//==============================================================================
class ColourSelector::SwatchComponent   : public Component
{
public:
    SwatchComponent (ColourSelector& cs, int itemIndex)
        : owner (cs), index (itemIndex)
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto col = owner.getSwatchColour (index);

        g.fillCheckerBoard (getLocalBounds().toFloat(), 6.0f, 6.0f,
                            juce::Colour (0xffdddddd).overlaidWith (col),
                            juce::Colour (0xffffffff).overlaidWith (col));
    }

    void mouseDown (const juce::MouseEvent&) override
    {
        juce::PopupMenu m;
        m.addItem (1, TRANS("Use this swatch as the current colour"));
        m.addSeparator();
        m.addItem (2, TRANS("Set this swatch to the current colour"));

        m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (this),
                         juce::ModalCallbackFunction::forComponent (menuStaticCallback, this));
    }

private:
    ColourSelector& owner;
    const int index;

    static void menuStaticCallback (int result, SwatchComponent* comp)
    {
        if (comp != nullptr)
        {
            if (result == 1)  comp->setColourFromSwatch();
            if (result == 2)  comp->setSwatchFromColour();
        }
    }

    void setColourFromSwatch()
    {
        owner.set (owner.getSwatchColour (index));
    }

    void setSwatchFromColour()
    {
        if (owner.getSwatchColour (index) != owner.getCurrentColour())
        {
            owner.setSwatchColour (index, owner.getCurrentColour());
            repaint();
        }
    }

    JUCE_DECLARE_NON_COPYABLE (SwatchComponent)
};

//==============================================================================
class ColourSelector::ColourPreviewComp  : public Component
{
public:
    ColourPreviewComp (ColourSelector& cs, bool isEditable)
        : owner (cs)
    {
        colourLabel.setFont (labelFont);
        colourLabel.setJustificationType (juce::Justification::centred);

        if (isEditable)
        {
            colourLabel.setEditable (true);

            colourLabel.onEditorShow = [this]
            {
                if (auto* ed = colourLabel.getCurrentTextEditor())
                    ed->setInputRestrictions ((owner.flags & showAlphaChannel) ? 8 : 6, "1234567890ABCDEFabcdef");
            };

            colourLabel.onEditorHide = [this]
            {
                updateColourIfNecessary (colourLabel.getText());
            };
        }

        addAndMakeVisible (colourLabel);
    }

    void updateIfNeeded()
    {
        auto newColour = owner.getCurrentColour();

        if (currentColour != newColour)
        {
            currentColour = newColour;
            auto textColour = (juce::Colours::white.overlaidWith (currentColour).contrasting());

            colourLabel.setColour (juce::Label::textColourId,            textColour);
            colourLabel.setColour (juce::Label::textWhenEditingColourId, textColour);
            colourLabel.setText (currentColour.toDisplayString ((owner.flags & showAlphaChannel) != 0), juce::dontSendNotification);

            labelWidth = juce::GlyphArrangement::getStringWidthInt ( labelFont, colourLabel.getText () );

            repaint();
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.fillCheckerBoard (getLocalBounds().toFloat(), 10.0f, 10.0f,
                            juce::Colour (0xffdddddd).overlaidWith (currentColour),
                            juce::Colour (0xffffffff).overlaidWith (currentColour));
    }

    void resized() override
    {
        colourLabel.centreWithSize (labelWidth + 10, (int) labelFont.getHeight() + 10);
    }

private:
    void updateColourIfNecessary (const juce::String& newColourString)
    {
        auto newColour = juce::Colour::fromString (newColourString);

        if (newColour != currentColour)
            owner.set (newColour);
    }

    ColourSelector& owner;

    juce::Colour currentColour;
    juce::Font labelFont { juce::FontOptions ( 14.0f, juce::Font::bold ) };
    int labelWidth = 0;
    juce::Label colourLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPreviewComp)
};

//==============================================================================
ColourSelector::ColourSelector (int sectionsToShow, int edge, int gapAroundColourSpaceComponent)
    : colour (juce::Colours::white),
      flags (sectionsToShow),
      edgeGap (edge)
{
    setLookAndFeel (&lf);

    // not much point having a selector with no components in it!
    jassert ((flags & (showColourAtTop | showRGBSliders | showHSBSliders | showColourspace)) != 0);

    if ((flags & showColourAtTop) != 0)
    {
        previewComponent.reset (new ColourPreviewComp (*this, (flags & editableColour) != 0));
        addAndMakeVisible (previewComponent.get());
    }

    if ((flags & showHSBSliders) != 0)
    {
        sliders.add (hueSlider = new ColourComponentSlider (TRANS ("H"), 360));
        sliders.add (saturationSlider = new ColourComponentSlider (TRANS ("S"), 100));
        sliders.add (brightnessSlider = new ColourComponentSlider (TRANS ("B"), 100));

        if ((flags & showToggle) != 0)
        {
            toggles.add (new juce::ToggleButton (juce::String (int (Params::hue))));
            toggles.add (new juce::ToggleButton (juce::String (int (Params::saturation))));
            toggles.add (new juce::ToggleButton (juce::String (int (Params::brightness))));
        }
    }
    if ((flags & showRGBSliders) != 0)
    {

        sliders.add (redSlider = new ColourComponentSlider (TRANS ("R"), 255));
        sliders.add (greenSlider = new ColourComponentSlider (TRANS ("G"), 255));
        sliders.add (blueSlider = new ColourComponentSlider (TRANS ("B"), 255));

        if ((flags & showToggle) != 0)
        {
            toggles.add (new juce::ToggleButton (juce::String (int (Params::red))));
            toggles.add (new juce::ToggleButton (juce::String (int (Params::green))));
            toggles.add (new juce::ToggleButton (juce::String (int (Params::blue))));
        }
    }

    if ((flags & showAlphaChannel) != 0)
        sliders.add (alphaSlider = new ColourComponentSlider (TRANS ("A"), 255));

    for (auto& slider : sliders)
    {
        addAndMakeVisible (slider);
        slider->onValueChange = [this, slider] { changeColour (slider); };
    }

    for (auto& toggle : toggles)
    {
        addAndMakeVisible (toggle);
        toggle->setButtonText ({});
        toggle->setRadioGroupId (1);
        toggle->onClick = [this] { updateParameters(); sendChangeMessage(); };
    }

    if (toggles.size() > 0)
        toggles[0]->setToggleState (true, juce::dontSendNotification);

    if ((flags & showColourspace) != 0)
    {
        parameter2D.reset (new Parameter2D (*this, gapAroundColourSpaceComponent));
        parameter1D.reset (new Parameter1D (*this, gapAroundColourSpaceComponent));

        addAndMakeVisible (parameter2D.get());
        addAndMakeVisible (parameter1D.get());
    }

    if ((flags & showHexEdit) != 0)
    {
        hex = std::make_unique<juce::TextEditor>();
        hex->setJustification (juce::Justification::centred);
        hex->onTextChange = [this]
        {
            auto hcol = hex->getText();

            // convert hex3 and hex4 formats to hex6 and hex8
            if ( hcol.length () == 3 )
                hcol = juce::String::formatted ( "%c%c%c%c%c%c", hcol[ 0 ], hcol[ 0 ], hcol[ 1 ], hcol[ 1 ], hcol[ 2 ], hcol[ 2 ] );
            else if ( hcol.length () == 4 )
                hcol = juce::String::formatted ( "%c%c%c%c%c%c%c%c", hcol[ 0 ], hcol[ 0 ], hcol[ 1 ], hcol[ 1 ], hcol[ 2 ], hcol[ 2 ], hcol[ 3 ], hcol[ 3 ] );

            // Add missing alpha
            if ( hcol.length () == 6 )
                hcol += juce::String ( "ff" );

            // Convert rgba to argb (JUCE is weird)
            if ( hcol.length () == 8 )
            {
                colour = juce::Colour::fromString (hcol.substring (6) + hcol.substring (0, 6));
                update (juce::sendNotification);
            }
        };
        hex->onFocusLost = [this]
        {
            update (juce::sendNotification);
        };
        addAndMakeVisible (*hex);
    }

    if ((flags & showOriginalColour) != 0)
    {
        originalColourComponent = std::make_unique<OriginalColourComp> (*this);
        addAndMakeVisible (*originalColourComponent);
    }

    if ((flags & showReset) != 0)
    {
        resetButton = std::make_unique<juce::TextButton> ("reset");
        resetButton->onClick = [this]
        {
            set (originalColour);
        };
        addAndMakeVisible (*resetButton);
    }

    update (juce::dontSendNotification);
    updateParameters();
}

ColourSelector::~ColourSelector()
{
    setLookAndFeel (nullptr);
    dispatchPendingMessages();
    swatchComponents.clear();
}

//==============================================================================
juce::Colour ColourSelector::getCurrentColour() const
{
    return ((flags & showAlphaChannel) != 0) ? colour.getColour() : colour.getColour().withAlpha ((juce::uint8) 0xff);
}

void ColourSelector::setCurrentColour (juce::Colour c, juce::NotificationType notification)
{
    if (DeepColour (c) != colour)
    {
        originalColour = c;
        colour = ((flags & showAlphaChannel) != 0) ? c : c.withAlpha ((juce::uint8) 0xff);
        update (notification);
    }
}

void ColourSelector::setCurrentColour (DeepColour c, juce::NotificationType notification)
{
    if (c != colour)
    {
        originalColour = c;
        colour = ((flags & showAlphaChannel) != 0) ? c : c.withAlpha (1.0f);
        update (notification);
    }
}

void ColourSelector::set (const DeepColour& newColour)
{
    colour = newColour;
    update (juce::sendNotification);
}

//==============================================================================
void ColourSelector::update (juce::NotificationType notification)
{
    if (hueSlider)
    {
        hueSlider->setValue (colour.getHue() * 360,                 juce::dontSendNotification);
        saturationSlider->setValue (colour.getSaturation() * 100,   juce::dontSendNotification);
        brightnessSlider->setValue (colour.getBrightness() * 100,   juce::dontSendNotification);
    }

    if (redSlider)
    {
        redSlider->setValue (colour.getRed() * 255,     juce::dontSendNotification);
        greenSlider->setValue (colour.getGreen() * 255, juce::dontSendNotification);
        blueSlider->setValue (colour.getBlue() * 255,   juce::dontSendNotification);
    }

    if (alphaSlider)
        alphaSlider->setValue (colour.getAlpha() * 255, juce::dontSendNotification);

    if (hex && ! hex->hasKeyboardFocus (true))
        hex->setText (colour.getColour().toDisplayString ((flags & showAlphaChannel) != 0), juce::dontSendNotification);

    if (parameter2D != nullptr)
    {
        parameter2D->updateIfNeeded();
        parameter1D->updateIfNeeded();
    }

    if (originalColourComponent != nullptr)
        originalColourComponent->repaint();

    if (previewComponent != nullptr)
        previewComponent->updateIfNeeded();

    if (notification != juce::dontSendNotification)
        sendChangeMessage();

    if (notification == juce::sendNotificationSync)
        dispatchPendingMessages();
}

//==============================================================================
void ColourSelector::paint (juce::Graphics& g)
{
    g.fillAll (findColour (backgroundColourId));

    if ((flags & showRGBSliders) != 0)
    {
        g.setColour (findColour (labelTextColourId));
        g.setFont (11.0f);

        for (auto& slider : sliders)
        {
            if (slider->isVisible())
                g.drawText (slider->getName() + ":",
                            0, slider->getY(),
                            slider->getX() - 8, slider->getHeight(),
                            juce::Justification::centredRight, false);
        }
    }
}

void ColourSelector::resized()
{
    const int swatchesPerRow = 8;
    const int swatchHeight = 22;

    const float numSliders = sliders.size() + (hueSlider && redSlider ? 0.5f : 0.0f) + (alphaSlider ? 0.5f : 0.0f) + (hex ? 1.0f : 0.0f);
    const int numSwatches = getNumSwatches();

    const int swatchSpace = numSwatches > 0 ? edgeGap + swatchHeight * ((numSwatches + 7) / swatchesPerRow) : 0;
    const int sliderSpace = ((flags & showRGBSliders) != 0)  ? juce::jmin (int (22 * numSliders + edgeGap), proportionOfHeight (0.3f)) : 0;
    const int topSpace = ((flags & showColourAtTop) != 0) ? juce::jmin (30 + edgeGap * 2, proportionOfHeight (0.2f)) : edgeGap;

    if (previewComponent != nullptr)
        previewComponent->setBounds (edgeGap, edgeGap, getWidth() - edgeGap * 2, topSpace - edgeGap * 2);

    int y = topSpace;

    if ((flags & showColourspace) != 0)
    {
        const int hueWidth = juce::jmin (50, proportionOfWidth (0.15f));

        parameter2D->setBounds (edgeGap, y,
                                getWidth() - hueWidth - edgeGap - 4,
                                getHeight() - topSpace - sliderSpace - swatchSpace - edgeGap);

        parameter1D->setBounds (parameter2D->getRight() + 4, y,
                                getWidth() - edgeGap - (parameter2D->getRight() + 4),
                                parameter2D->getHeight());

        y = getHeight() - sliderSpace - swatchSpace - edgeGap;
    }

    if (originalColourComponent != nullptr)
    {
        originalColourComponent->setBounds (edgeGap, y, proportionOfWidth (0.14f), proportionOfWidth (0.2f));

        if (resetButton != nullptr)
            resetButton->setBounds (edgeGap, originalColourComponent->getBottom() + 8, proportionOfWidth (0.14f), 20);
    }
    else if (resetButton != nullptr)
    {
        resetButton->setBounds (edgeGap, y, proportionOfWidth (0.14f), 20);
    }

    auto sliderHeight = juce::jmax (4, int (sliderSpace / numSliders));

    if (sliders.size() > 0)
    {
        for (auto [i, slider] : juce::enumerate (sliders))
        {
            auto rc = juce::Rectangle<int> (proportionOfWidth (0.2f), y, proportionOfWidth (0.72f), sliderHeight - 2);

            auto trc = rc.removeFromLeft (rc.getHeight() + 2);
            if (i < std::ssize (toggles))
                toggles[int (i)]->setBounds (trc.translated (-sliderHeight, 0));

            slider->setBounds (rc);

            y += sliderHeight;

            if (slider == brightnessSlider && redSlider != nullptr)
                y += sliderHeight / 2;

            if (slider == blueSlider && alphaSlider != nullptr)
                y += sliderHeight / 2;
        }
    }

    if (hex)
    {
        auto rc = juce::Rectangle<int> (proportionOfWidth (0.2f) + sliderHeight, y, 80, sliderHeight - 2);
        hex->setBounds (rc);
    }

    if (numSwatches > 0)
    {
        const int startX = 8;
        const int xGap = 4;
        const int yGap = 4;
        const int swatchWidth = (getWidth() - startX * 2) / swatchesPerRow;
        y += edgeGap;

        if (swatchComponents.size() != numSwatches)
        {
            swatchComponents.clear();

            for (int i = 0; i < numSwatches; ++i)
            {
                auto* sc = new SwatchComponent (*this, i);
                swatchComponents.add (sc);
                addAndMakeVisible (sc);
            }
        }

        int x = startX;

        for (int i = 0; i < swatchComponents.size(); ++i)
        {
            auto* sc = swatchComponents.getUnchecked(i);

            sc->setBounds (x + xGap / 2,
                           y + yGap / 2,
                           swatchWidth - xGap,
                           swatchHeight - yGap);

            if (((i + 1) % swatchesPerRow) == 0)
            {
                x = startX;
                y += swatchHeight;
            }
            else
            {
                x += swatchWidth;
            }
        }
    }
}

void ColourSelector::changeColour (juce::Slider* slider)
{
    if (sliders[0] == nullptr)
        return;

    if (hueSlider == slider || saturationSlider == slider || brightnessSlider == slider)
    {
        auto col = DeepColour::fromHSB (float (hueSlider->getValue() / 360.0),
                                        float (saturationSlider->getValue() / 100.0),
                                        float (brightnessSlider->getValue() / 100.0),
                                        float (alphaSlider ? alphaSlider->getValue() / 255.0 : 1.0));

        set (col);
    }
    else
    {
        auto col = DeepColour::fromRGBA (float (redSlider->getValue() / 255.0),
                                         float (greenSlider->getValue() / 255.0),
                                         float (blueSlider->getValue() / 255.0),
                                         float (alphaSlider ? alphaSlider->getValue() / 255.0 : 1.0));

        set (col);
    }
}

void ColourSelector::updateParameters()
{
    auto state = getActiveParam();

    if (state == Params::hue)
    {
        parameter1D->setParameter (Params::hue);
        parameter2D->setParameters (Params::saturation, Params::brightness);
    }
    else if (state == Params::saturation)
    {
        parameter1D->setParameter (Params::saturation);
        parameter2D->setParameters (Params::hue, Params::brightness);
    }
    else if (state == Params::brightness)
    {
        parameter1D->setParameter (Params::brightness);
        parameter2D->setParameters (Params::hue, Params::saturation);
    }
    else if (state == Params::red)
    {
        parameter1D->setParameter (Params::red);
        parameter2D->setParameters (Params::blue, Params::green);
    }
    else if (state == Params::green)
    {
        parameter1D->setParameter (Params::green);
        parameter2D->setParameters (Params::blue, Params::red);
    }
    else if (state == Params::blue)
    {
        parameter1D->setParameter (Params::blue);
        parameter2D->setParameters (Params::red, Params::green);
    }
}

ColourSelector::Params ColourSelector::getActiveParam ()
{
    for (auto t : toggles)
        if (t->getToggleState())
            return (Params) t->getName().getIntValue();

    return Params::hue;
}

void ColourSelector::setActiveParam ( Params p )
{
    for (auto t : toggles)
        t->setToggleState ((Params) t->getName().getIntValue() == p, juce::dontSendNotification);

    updateParameters();
}

//==============================================================================
int ColourSelector::getNumSwatches() const
{
    return 0;
}

juce::Colour ColourSelector::getSwatchColour (int) const
{
    jassertfalse; // if you've overridden getNumSwatches(), you also need to implement this method
    return juce::Colours::black;
}

void ColourSelector::setSwatchColour (int, const juce::Colour&)
{
    jassertfalse; // if you've overridden getNumSwatches(), you also need to implement this method
}

} // namespace juce
