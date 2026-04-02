#include "core/ConfiguredItem.hpp"

ConfiguredItem::ConfiguredItem(const QString &name, ConfiguredItemType type)
    : name_(name),
      type_(type)
{
}

const QString &ConfiguredItem::name() const
{
    return name_;
}

void ConfiguredItem::setName(const QString &name)
{
    name_ = name;
}

ConfiguredItemType ConfiguredItem::type() const
{
    return type_;
}

void ConfiguredItem::setType(ConfiguredItemType type)
{
    type_ = type;
}

const QString &ConfiguredItem::description() const
{
    return description_;
}

void ConfiguredItem::setDescription(const QString &description)
{
    description_ = description;
}

const QString &ConfiguredItem::parameterKey() const
{
    return parameterKey_;
}

void ConfiguredItem::setParameterKey(const QString &key)
{
    parameterKey_ = key;
}

const QString &ConfiguredItem::parameterValue() const
{
    return parameterValue_;
}

void ConfiguredItem::setParameterValue(const QString &value)
{
    parameterValue_ = value;
}

const QString &ConfiguredItem::parameterUnit() const
{
    return parameterUnit_;
}

void ConfiguredItem::setParameterUnit(const QString &unit)
{
    parameterUnit_ = unit;
}

bool ConfiguredItem::required() const
{
    return required_;
}

void ConfiguredItem::setRequired(bool required)
{
    required_ = required;
}

ConfiguredItem *ConfiguredItem::parent()
{
    return parent_;
}

const ConfiguredItem *ConfiguredItem::parent() const
{
    return parent_;
}

void ConfiguredItem::addChild(std::unique_ptr<ConfiguredItem> child)
{
    if (!child)
    {
        return;
    }

    child->parent_ = this;
    children_.push_back(std::move(child));
}

bool ConfiguredItem::removeChild(ConfiguredItem *childToRemove)
{
    if (!childToRemove)
    {
        return false;
    }

    for (auto it = children_.begin(); it != children_.end(); ++it)
    {
        if (it->get() == childToRemove)
        {
            children_.erase(it);
            return true;
        }
    }

    return false;
}

std::vector<std::unique_ptr<ConfiguredItem>> &ConfiguredItem::children()
{
    return children_;
}

const std::vector<std::unique_ptr<ConfiguredItem>> &ConfiguredItem::children() const
{
    return children_;
}

bool ConfiguredItem::isParameter() const
{
    return type_ == ConfiguredItemType::Parameter;
}