#pragma once

#include <string>
#include <functional>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <random>

enum class SettingType{
	Bool,
	Number,
	Range,
	Enum,
	String,
};

class Setting
{
public:
    virtual ~Setting() = default;

    std::string mName;
    std::string mDescription;
    SettingType mType;
    bool mDisplay = false;
    bool* mHideOnClickGUI = nullptr;
    std::function<bool()> mIsVisible = []() { return true; };

    Setting(std::string name, std::string description, SettingType type)
        : mName(std::move(name)), mDescription(std::move(description)), mType(type)
    {}

    //had to do dis sorry :/
    float sliderEase = 0;
    float boolScale = 0;
    bool isDragging = false;

    bool enumExtended = false;
    bool colourExtended = false; // I am a fucking sigma
    float enumSlide = 0;
    float colourSlide = 0; // to lerp it

    virtual int getEnumIndex() const { return -1; }
    virtual void setEnumIndex(int idx) {}
    virtual const std::vector<std::string>* getEnumValues() const { return nullptr; }
};

// Define visible condition
#define VISIBILITY_CONDITION(setting, condition) setting.mIsVisible = std::function<bool()>([&]() { return condition; });

class BoolSetting : public Setting
{
public:
    bool mValue = false;
    int mKey;

    BoolSetting(std::string name, std::string description, bool value, int key = -1)
        : Setting(std::move(name), std::move(description), SettingType::Bool), mValue(value), mKey(key)
    {

    }

    void setValue(bool value)
    {
        mValue = value;
    }

    std::string getName()
    {
        return mName;
    }

    // Operator overloading for easy access
    explicit operator bool() const
    {
        return mValue;
    }
};

class NumberSetting : public Setting
{
public:
    float mValue = 0.0f;
    float mMin = 0.0f;
    float mMax = 0.0f;
    float mStep = 0.0f;

    NumberSetting(std::string name, std::string description, float value, float min, float max, float step)
        : Setting(std::move(name), std::move(description), SettingType::Number), mValue(value), mMin(min), mMax(max), mStep(step)
    {

    }

    void setValue(float value)
    {
        mValue = std::round(value / mStep) * mStep;
    }
    float get()  {
        return mValue;
	}
    
    template <typename T>
    T as() const
    {
        return static_cast<T>(mValue);
    }
};

class RangeSetting : public Setting
{
public:
    float mMin = 0.0f;
    float mMax = 0.0f;
    float mValueMin = 0.0f;
    float mValueMax = 0.0f;
    float mStep = 0.0f;

    RangeSetting(std::string name, std::string description, float valuemin,float valuemax, float min, float max, float step)
        : Setting(std::move(name), std::move(description), SettingType::Range), mValueMin(valuemin), mValueMax(valuemax), mMin(min), mMax(max), mStep(step)
    {

    }

    void setValue(float min,float max)
    {
        mValueMin = std::round(min / mStep) * mStep;
        mValueMax = std::round(max / mStep) * mStep;
    }

    float getMinValue()
    {
        return mValueMin;
	}

    float getMaxValue()
    {
        return mValueMax;
	}

    float getRandom() {
        if (mValueMax <= mValueMin) {
            return mValueMin;
        }
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(mValueMin, mValueMax);
        return dist(rng);
    }


};

class EnumSetting : public Setting
{
public:
    int mValue = 0;
    std::vector<std::string> mValues;

    EnumSetting(std::string name, std::string description, int index, std::vector<std::string> values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(index), mValues(std::move(values))
    {

    }

    template <typename IndexType, typename... Args>
    EnumSetting(std::string name, std::string description, IndexType index, Args... values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(static_cast<int>(index))
    {
        mValues = { values... };
    }

    void setValue(int value)
    {
        mValue = value;
    }

    template <typename T>
    explicit operator T() const
    {
        return static_cast<T>(mValue);
    }

    template <typename T>
    T as() const
    {
        return static_cast<T>(mValue);
    }
};

// EnumSetting, but the mValue is a custom type (should always be an enum)
template <typename T>
class EnumSettingT : public Setting
{
public:
    T mValue;
    std::vector<std::string> mValues;

    EnumSettingT(std::string name, std::string description, T index, std::vector<std::string> values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(index), mValues(std::move(values))
    {

    }

    void setValue(T value)
    {
        mValue = value;
    }

    template <typename... Args>
    EnumSettingT(std::string name, std::string description, T index, Args... values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(index)
    {
        mValues = { values... };
    }

    template <typename type>
    type as() const
    {
        return static_cast<type>(mValue);
    }

    int getEnumIndex() const override {
        return static_cast<int>(mValue);
    }

    void setEnumIndex(int idx) override {
        mValue = static_cast<T>(idx);
    }

    const std::vector<std::string>* getEnumValues() const override {
        return &mValues;
    }
};

