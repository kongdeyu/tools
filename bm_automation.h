
#pragma once

#include <string>
#include <vector>

namespace tool
{
class BMAutomation
{
public:
    BMAutomation();
    ~BMAutomation();
    void preprocess(const std::string &keyword);
    int32_t search(const std::string &sentence);

private:
    void bad_character_shift();
    void good_suffix_shift();

    const static int32_t _S_ALPHABET_SIZE = 256;
    const static int32_t _S_SHIFT = 128;

    std::string _keyword;
    std::vector<int32_t> _vec_bad_character_shift;
    std::vector<int32_t> _vec_good_suffix_shift;
};
}  // namespace
