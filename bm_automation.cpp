
#include <algorithm>
#include <iostream>

#include "bm_automation.h"

namespace tool
{
BMAutomation::BMAutomation() {}

BMAutomation::~BMAutomation() {}

void BMAutomation::preprocess(const std::string &keyword)
{
    _keyword = keyword;
    bad_character_shift();
    good_suffix_shift();
}

int32_t BMAutomation::search(const std::string &sentence)
{
    if(sentence.size() < _keyword.size())
    {
        return 0;
    }

    int32_t res = 0;
    int32_t shift = 0;
    for(int32_t i = 0; i <= sentence.size() - _keyword.size(); i += shift)
    {
        // compare from end to begin
        int32_t j = _keyword.size() - 1;
        while(j >= 0 && sentence.at(i + j) == _keyword.at(j))
        {
            j--;
        }

        if(j < 0)  // find word
        {
            res++;
            shift = 1;
        }
        else
        {
            int32_t good_suffix_shift = _vec_good_suffix_shift.at(j);
            int32_t bad_character_shift
                = _vec_bad_character_shift.at(sentence.at(i + j) + _S_SHIFT) - (_keyword.size() - 1 - j);
            shift = std::max(bad_character_shift, good_suffix_shift);
            std::cout << "bc is: " << static_cast<char>(sentence.at(i + j)) << " "
                << "bc: " << bad_character_shift << " "
                << "gs :" << good_suffix_shift << " "
                << "shift: " << shift << std::endl;
        }
    }
    return res;
}

void BMAutomation::bad_character_shift()
{
    _vec_bad_character_shift.resize(_S_ALPHABET_SIZE, _keyword.size());
    int32_t index = _keyword.size();
    for(std::string::const_iterator cit = _keyword.begin();
            cit != _keyword.end();
            cit++)
    {
        _vec_bad_character_shift.at(*cit + _S_SHIFT) = --index;
        std::cout << static_cast<char>(*cit) << ": " << index + 1 << std::endl;
    }
}

void BMAutomation::good_suffix_shift()
{
    // generate suffix for keyword
    std::vector<int32_t> vec_suffix(_keyword.size(), _keyword.size());
    for(int32_t i = _keyword.size() - 2; i >= 0; i--)
    {
        int32_t j = i;
        while(j >= 0 && _keyword.at(j) == _keyword.at(_keyword.size() - 1 - i + j))
        {
            j--;
        }
        vec_suffix.at(i) = i - j;
        std::cout << i << ": " << i - j << std::endl;
    }

    // calculate suffix shift
    _vec_good_suffix_shift.resize(_keyword.size(), _keyword.size());  // initialize

    // calculate suffix shift according to prefix
    int32_t j = 0;
    for(int32_t i = _keyword.size() - 1; i >= 0; i--)
    {
        if(vec_suffix.at(i) == i + 1)
        {
            for(; j < _keyword.size() - 1 - i; j++)
            {
                if(_vec_good_suffix_shift.at(j) == _keyword.size())
                {
                    _vec_good_suffix_shift.at(j) = _keyword.size() - 1 - i;
                }
            }
        }
    }

    // calculate suffix shift that match
    for(int32_t i = 0; i <= _keyword.size() - 2; i++)
    {
        _vec_good_suffix_shift.at(_keyword.size() - 1 - vec_suffix.at(i)) = _keyword.size() - 1 - i;
    }
}
}  // namespace
