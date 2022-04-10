// Copyright (c) 2011 and onwards The OpenVanilla Project (MIT License).
// All possible vChewing-specific modifications are of:
// (c) 2021 and onwards The vChewing Project (MIT-NTL License).
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

2. No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements above.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef BLOCKREADINGBUILDER_H_
#define BLOCKREADINGBUILDER_H_

#include <string>
#include <vector>

#include "Grid.h"
#include "LanguageModel.h"

namespace Gramambular
{

class BlockReadingBuilder
{
  public:
    explicit BlockReadingBuilder(LanguageModel *lm);
    void clear();

    size_t length() const;
    size_t cursorIndex() const;
    void setCursorIndex(size_t newIndex);
    void insertReadingAtCursor(const std::string &reading);
    bool deleteReadingBeforeCursor(); // backspace
    bool deleteReadingAfterCursor();  // delete

    bool removeHeadReadings(size_t count);

    void setJoinSeparator(const std::string &separator);
    const std::string joinSeparator() const;

    std::vector<std::string> readings() const;

    Grid &grid();

  protected:
    void build();

    static const std::string Join(std::vector<std::string>::const_iterator begin,
                                  std::vector<std::string>::const_iterator end, const std::string &separator);

    // 規定最多可以組成的詞的字數上限為 10
    static const size_t MaximumBuildSpanLength = 10;

    size_t m_cursorIndex;
    std::vector<std::string> m_readings;

    Grid m_grid;
    LanguageModel *m_LM;
    std::string m_joinSeparator;
};

inline BlockReadingBuilder::BlockReadingBuilder(LanguageModel *lm) : m_LM(lm), m_cursorIndex(0)
{
}

inline void BlockReadingBuilder::clear()
{
    m_cursorIndex = 0;
    m_readings.clear();
    m_grid.clear();
}

inline size_t BlockReadingBuilder::length() const
{
    return m_readings.size();
}

inline size_t BlockReadingBuilder::cursorIndex() const
{
    return m_cursorIndex;
}

inline void BlockReadingBuilder::setCursorIndex(size_t newIndex)
{
    m_cursorIndex = newIndex > m_readings.size() ? m_readings.size() : newIndex;
}

inline void BlockReadingBuilder::insertReadingAtCursor(const std::string &reading)
{
    m_readings.insert(m_readings.begin() + m_cursorIndex, reading);

    m_grid.expandGridByOneAtLocation(m_cursorIndex);
    build();
    m_cursorIndex++;
}

inline std::vector<std::string> BlockReadingBuilder::readings() const
{
    return m_readings;
}

inline bool BlockReadingBuilder::deleteReadingBeforeCursor()
{
    if (!m_cursorIndex)
    {
        return false;
    }

    m_readings.erase(m_readings.begin() + m_cursorIndex - 1, m_readings.begin() + m_cursorIndex);
    m_cursorIndex--;
    m_grid.shrinkGridByOneAtLocation(m_cursorIndex);
    build();
    return true;
}

inline bool BlockReadingBuilder::deleteReadingAfterCursor()
{
    if (m_cursorIndex == m_readings.size())
    {
        return false;
    }

    m_readings.erase(m_readings.begin() + m_cursorIndex, m_readings.begin() + m_cursorIndex + 1);
    m_grid.shrinkGridByOneAtLocation(m_cursorIndex);
    build();
    return true;
}

inline bool BlockReadingBuilder::removeHeadReadings(size_t count)
{
    if (count > length())
    {
        return false;
    }

    for (size_t i = 0; i < count; i++)
    {
        if (m_cursorIndex)
        {
            m_cursorIndex--;
        }
        m_readings.erase(m_readings.begin(), m_readings.begin() + 1);
        m_grid.shrinkGridByOneAtLocation(0);
        build();
    }

    return true;
}

inline void BlockReadingBuilder::setJoinSeparator(const std::string &separator)
{
    m_joinSeparator = separator;
}

inline const std::string BlockReadingBuilder::joinSeparator() const
{
    return m_joinSeparator;
}

inline Grid &BlockReadingBuilder::grid()
{
    return m_grid;
}

inline void BlockReadingBuilder::build()
{
    if (!m_LM)
    {
        return;
    }

    size_t begin = 0;
    size_t end = m_cursorIndex + MaximumBuildSpanLength;

    if (m_cursorIndex < MaximumBuildSpanLength)
    {
        begin = 0;
    }
    else
    {
        begin = m_cursorIndex - MaximumBuildSpanLength;
    }

    if (end > m_readings.size())
    {
        end = m_readings.size();
    }

    for (size_t p = begin; p < end; p++)
    {
        for (size_t q = 1; q <= MaximumBuildSpanLength && p + q <= end; q++)
        {
            std::string combinedReading = Join(m_readings.begin() + p, m_readings.begin() + p + q, m_joinSeparator);
            if (!m_grid.hasNodeAtLocationSpanningLengthMatchingKey(p, q, combinedReading))
            {
                std::vector<Unigram> unigrams = m_LM->unigramsForKey(combinedReading);

                if (unigrams.size() > 0)
                {
                    Node n(combinedReading, unigrams, std::vector<Bigram>());
                    m_grid.insertNode(n, p, q);
                }
            }
        }
    }
}

inline const std::string BlockReadingBuilder::Join(std::vector<std::string>::const_iterator begin,
                                                   std::vector<std::string>::const_iterator end,
                                                   const std::string &separator)
{
    std::string result;
    for (std::vector<std::string>::const_iterator iter = begin; iter != end;)
    {
        result += *iter;
        ++iter;
        if (iter != end)
        {
            result += separator;
        }
    }
    return result;
}
} // namespace Gramambular

#endif
