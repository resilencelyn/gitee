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

#include "AssociatedPhrases.h"
#include "vChewing-Swift.h"
#include <fcntl.h>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "KeyValueBlobReader.h"
#include "LMConsolidator.h"

namespace vChewing
{

AssociatedPhrases::AssociatedPhrases() : fd(-1), data(0), length(0)
{
}

AssociatedPhrases::~AssociatedPhrases()
{
    if (data)
    {
        close();
    }
}

const bool AssociatedPhrases::isLoaded()
{
    if (data)
    {
        return true;
    }
    return false;
}

bool AssociatedPhrases::open(const char *path)
{
    if (data)
    {
        return false;
    }

    LMConsolidator::FixEOF(path);
    LMConsolidator::ConsolidateContent(path, true);

    fd = ::open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("open:: file not exist");
        return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        printf("open:: cannot open file");
        return false;
    }

    length = (size_t)sb.st_size;

    data = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    if (!data)
    {
        ::close(fd);
        return false;
    }

    KeyValueBlobReader reader(static_cast<char *>(data), length);
    KeyValueBlobReader::KeyValue keyValue;
    KeyValueBlobReader::State state;
    while ((state = reader.Next(&keyValue)) == KeyValueBlobReader::State::HAS_PAIR)
    {
        keyRowMap[keyValue.key].emplace_back(keyValue.key, keyValue.value);
    }
    // 下面這一段或許可以做成開關、來詢問是否對使用者語彙採取寬鬆策略（哪怕有行內容寫錯也會放行）
    if (state == KeyValueBlobReader::State::ERROR)
    {
        // close();
        if (mgrPrefs.isDebugModeEnabled)
            syslog(LOG_CONS, "AssociatedPhrases: Failed at Open Step 5. On Error Resume Next.\n");
        // return false;
    }
    return true;
}

void AssociatedPhrases::close()
{
    if (data)
    {
        munmap(data, length);
        ::close(fd);
        data = 0;
    }

    keyRowMap.clear();
}

const std::vector<std::string> AssociatedPhrases::valuesForKey(const std::string &key)
{
    std::vector<std::string> v;
    auto iter = keyRowMap.find(key);
    if (iter != keyRowMap.end())
    {
        const std::vector<Row> &rows = iter->second;
        for (const auto &row : rows)
        {
            std::string_view value = row.value;
            v.push_back({value.data(), value.size()});
        }
    }
    return v;
}

const bool AssociatedPhrases::hasValuesForKey(const std::string &key)
{
    return keyRowMap.find(key) != keyRowMap.end();
}

}; // namespace vChewing
