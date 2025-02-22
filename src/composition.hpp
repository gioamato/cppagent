//
// Copyright Copyright 2009-2019, AMT – The Association For Manufacturing Technology (“AMT”)
// All rights reserved.
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#pragma once

#include "globals.hpp"

#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace mtconnect
{
  class Composition
  {
   public:
    class Description
    {
     public:
      Description(std::string body, std::string manufacturer, std::string model,
                  std::string serialNumber, std::string station)
          : m_body(std::move(body)),
            m_manufacturer(std::move(manufacturer)),
            m_model(std::move(model)),
            m_serialNumber(std::move(serialNumber)),
            m_station(std::move(station)),
            m_hasAttributes(false)
      {
      }

      Description(const Description &another)
          : m_body(another.m_body),
            m_manufacturer(another.m_manufacturer),
            m_model(another.m_model),
            m_serialNumber(another.m_serialNumber),
            m_station(another.m_station),
            m_hasAttributes(false)
      {
      }

      Description(std::string body, const std::map<std::string, std::string> &attributes)
          : m_body(std::move(body)), m_hasAttributes(false)
      {
        if (attributes.count("manufacturer") > 0)
          m_manufacturer = attributes.at("manufacturer");

        if (attributes.count("model") > 0)
          m_model = attributes.at("model");

        if (attributes.count("serialNumber") > 0)
          m_serialNumber = attributes.at("serialNumber");

        if (attributes.count("station") > 0)
          m_station = attributes.at("station");
      }

      const std::map<std::string, std::string> &getAttributes() const
      {
        if (!m_hasAttributes)
        {
          auto *self = const_cast<Description *>(this);

          if (!m_manufacturer.empty())
            self->m_attributes["manufacturer"] = m_manufacturer;

          if (!m_model.empty())
            self->m_attributes["model"] = m_model;

          if (!m_serialNumber.empty())
            self->m_attributes["serialNumber"] = m_serialNumber;

          if (!m_station.empty())
            self->m_attributes["station"] = m_station;

          self->m_hasAttributes = true;
        }

        return m_attributes;
      }

      const std::string &getBody() const
      {
        return m_body;
      }

     protected:
      std::map<std::string, std::string> m_attributes;

      std::string m_body;
      std::string m_manufacturer;
      std::string m_model;
      std::string m_serialNumber;
      std::string m_station;
      bool m_hasAttributes;
    };

   public:
    Composition(std::string aId, std::string aType, std::string aName, std::string aUuid)
        : m_id(std::move(aId)),
          m_uuid(std::move(aUuid)),
          m_name(std::move(aName)),
          m_type(std::move(aType)),
          m_hasAttributes(false)
    {
    }

    Composition(const Composition &another)
        : m_id(another.m_id),
          m_uuid(another.m_uuid),
          m_name(another.m_name),
          m_type(another.m_type),
          m_hasAttributes(false)
    {
      if (another.m_description)
        m_description = std::make_unique<Description>(*another.m_description);
    }

    Composition(const std::map<std::string, std::string> &attributes) : m_hasAttributes(false)
    {
      m_id = attributes.at("id");
      m_type = attributes.at("type");

      if (attributes.count("uuid") > 0)
        m_uuid = attributes.at("uuid");

      if (attributes.count("name") > 0)
        m_name = attributes.at("name");
    }

    const std::map<std::string, std::string> &getAttributes() const
    {
      if (!m_hasAttributes)
      {
        auto *self = const_cast<Composition *>(this);

        self->m_attributes["id"] = m_id;
        self->m_attributes["type"] = m_type;

        if (!m_uuid.empty())
          self->m_attributes["uuid"] = m_uuid;

        if (!m_name.empty())
          self->m_attributes["name"] = m_name;

        self->m_hasAttributes = true;
      }

      return m_attributes;
    }

    const Description *getDescription() const
    {
      return m_description.get();
    }

    void setDescription(Description *aDescription)
    {
      m_description.reset(aDescription);
    }

   protected:
    std::string m_id;
    std::string m_uuid;
    std::string m_name;
    std::string m_type;

    std::map<std::string, std::string> m_attributes;
    bool m_hasAttributes;

    std::unique_ptr<Description> m_description;
  };
}  // namespace mtconnect
