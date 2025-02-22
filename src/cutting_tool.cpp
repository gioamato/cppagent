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

#include "cutting_tool.hpp"

#include "printer.hpp"

#include <sstream>

using namespace std;

namespace mtconnect
{
  CuttingTool::~CuttingTool() = default;

  void CuttingTool::addValue(const CuttingToolValuePtr value)
  {
    m_content.clear();

    // Check for keys...
    if (value->m_key == "Location")
      m_keys[value->m_key] = value->m_value;

    m_values[value->m_key] = value;
  }

  inline static bool splitKey(string &key, string &sel, string &val)
  {
    auto found = key.find_first_of('@');

    if (found != string::npos)
    {
      sel = key;
      key.erase(found);
      sel.erase(0, found + 1);
      found = sel.find_first_of('=');

      if (found != string::npos)
      {
        val = sel;
        sel.erase(found);
        val.erase(0, found + 1);
        return true;
      }
    }

    return false;
  }

  void CuttingTool::updateValue(const std::string &inputKey, const std::string &value)
  {
    m_content.clear();

    if (inputKey == "Location")
      m_keys[inputKey] = value;

    // Split into path and parts and update the asset bits.
    string key = inputKey, sel, val;

    if (splitKey(key, sel, val))
    {
      if (key == "ToolLife")
      {
        for (const auto &life : m_lives)
        {
          if (life->m_properties.count(sel) > 0 && life->m_properties[sel] == val)
          {
            life->m_value = value;
            break;
          }
        }
      }
      else
      {
        for (const auto &item : m_items)
        {
          if (item->m_identity.count(sel) > 0 && val == item->m_identity[sel])
          {
            if (item->m_values.count(key) > 0)
              item->m_values[key]->m_value = value;
            else if (item->m_measurements.count(key) > 0)
              item->m_measurements[key]->m_value = value;

            break;
          }
        }
      }
    }
    else
    {
      if (inputKey == "CutterStatus")
      {
        m_status.clear();
        istringstream stream(value);
        string val;

        while (getline(stream, val, ','))
        {
          m_status.emplace_back(val);
        }
      }
      else
      {
        if (m_values.count(inputKey) > 0)
          m_values[inputKey]->m_value = value;
        else if (m_measurements.count(inputKey) > 0)
          m_measurements[inputKey]->m_value = value;
      }
    }
  }

  void CuttingTool::addIdentity(const std::string &key, const std::string &value)
  {
    m_content.clear();

    Asset::addIdentity(key, value);

    if (key == "toolId")
      m_keys[key] = value;
  }

  std::string &CuttingTool::getContent(const Printer *aPrinter)
  {
    if (m_content.empty())
    {
      const auto serialNumberPos = m_identity.find("serialNumber");
      if (serialNumberPos == m_identity.end())
        Asset::addIdentity("serialNumber", m_assetId);
      else if (serialNumberPos->second.empty())
        Asset::addIdentity("serialNumber", m_assetId);

      m_content = aPrinter->printCuttingTool(this);
    }

    return m_content;
  }

  CuttingToolValue::~CuttingToolValue() = default;

  CuttingItem::~CuttingItem() = default;
}  // namespace mtconnect
