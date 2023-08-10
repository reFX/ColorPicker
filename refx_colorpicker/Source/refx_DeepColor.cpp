namespace reFX
{

HSB rgbToHsb (const RGB& rgb)
{
    auto maxVal = std::max ({rgb.r, rgb.g, rgb.b});
    auto minVal = std::min ({rgb.r, rgb.g, rgb.b});
    auto delta = maxVal - minVal;

    auto h = 0.0f;
    auto s = 0.0f;
    auto b = maxVal;

    if (delta == 0)
    {
        h = 0.0f;
        s = 0.0f;
    }
    else
    {
        s = delta / maxVal;

        if (maxVal == rgb.r)
            h = (rgb.g - rgb.b) / delta;
        else if (maxVal == rgb.g)
            h = 2 + (rgb.b - rgb.r) / delta;
        else
            h = 4 + (rgb.r - rgb.g) / delta;

        h *= 60;
        if (h < 0)
            h += 360;
    }

    return { h / 360.0f, s, b };
}

RGB hsbToRgb (const HSB& hsb)
{
	auto h = hsb.h * 360.0f;
    auto c = hsb.b * hsb.s;
    auto x = c * (1 - std::abs (std::fmod (h / 60, 2) - 1));
    auto m = hsb.b - c;

    auto r = 0.0f;
    auto g = 0.0f;
    auto b = 0.0f;

    if (h < 60)
    {
        r = c;
        g = x;
		b = 0;
    }
    else if (h < 120)
    {
        r = x;
        g = c;
		b = 0;
    }
    else if (h < 180)
    {
        r = 0;
        g = c;
		b = x;
    }
    else if (h < 240)
    {
        r = 0;
        g = x;
		b = c;
    }
    else if (h < 300)
    {
        r = x;
        g = 0;
		b = c;
    }
    else
    {
        r = c;
        g = 0;
		b = x;
    }

    return { r + m, g + m, b + m };
}

//==============================================================================
bool DeepColor::operator== (const DeepColor& other) const noexcept
{
    return juce::approximatelyEqual (a, other.a) &&
           juce::approximatelyEqual (hsb.h, other.hsb.h) &&
           juce::approximatelyEqual (hsb.s, other.hsb.s) &&
           juce::approximatelyEqual (hsb.b, other.hsb.b);
}

bool DeepColor::operator!= (const DeepColor& other) const noexcept
{
    return ! (*this == other);
}

//==============================================================================
DeepColor::DeepColor (juce::uint32 col) noexcept
{
	a = (((col >> 24) & 0xff) / 255.0f);
	hsb = rgbToHsb ({(((col >> 16) & 0xff) / 255.0f), (((col >> 8)  & 0xff) / 255.0f), (((col >> 0)  & 0xff) / 255.0f)});
}

DeepColor::DeepColor (const juce::Colour& c)
{
	hsb = rgbToHsb ({c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue()});
    a = c.getFloatAlpha();
}

DeepColor DeepColor::fromRGB (float red, float green, float blue) noexcept
{
    DeepColor c;
    c.a = 1.0f;
	c.hsb = rgbToHsb ({red, green, blue});
    return c;
}

DeepColor DeepColor::fromRGBA (float red, float green, float blue, float alpha) noexcept
{
    DeepColor c;
    c.a = alpha;
	c.hsb = rgbToHsb ({red, green, blue});
    return c;
}

DeepColor DeepColor::fromHSB (float hue, float saturation, float brightness, float alpha) noexcept
{
    DeepColor c;
    c.a = alpha;
    c.hsb = {hue, saturation, brightness};
    return c;
}

//==============================================================================
void DeepColor::getHSB (float& h, float& s, float& b) const noexcept
{
    h = hsb.h;
    s = hsb.s;
    b = hsb.b;
}

float DeepColor::getRed() const noexcept           { return hsbToRgb (hsb).r; }
float DeepColor::getGreen() const noexcept         { return hsbToRgb (hsb).g; }
float DeepColor::getBlue() const noexcept          { return hsbToRgb (hsb).b; }

float DeepColor::getHue() const noexcept           { return hsb.h; }
float DeepColor::getSaturation() const noexcept    { return hsb.s; }
float DeepColor::getBrightness() const noexcept    { return hsb.b; }

juce::Colour DeepColor::getColour () const
{
    return juce::Colour::fromFloatRGBA (getRed(), getGreen(), getBlue(), a);
}

DeepColor DeepColor::withAlpha (float newAlpha) const noexcept
{
	auto c = *this;
	c.a = newAlpha;
	return c;
}

}
