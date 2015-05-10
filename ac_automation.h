
#pragma once

#include <string>
#include <vector>

namespace tool
{
class ACAutomation
{
public:
    ACAutomation();
    ~ACAutomation();
    void preprocess(const std::vector<std::string> &keywords);
    int32_t search(const std::string &sentence);

private:
    struct TrieNode
    {
        int32_t status;
        TrieNode *fail;  // fail node
        std::vector<TrieNode *> vec_next;  // goto nodes
        std::vector<std::string> vec_keywords;  // keywords end with this character

        TrieNode() : status(0), fail(NULL), vec_next(_S_ALPHABET_SIZE, NULL){}
    };

    void insert(const std::string &keyword);
    void build_fail_tbl();
    void clear(TrieNode *node);
    void print_trie();

    const static int32_t _S_ALPHABET_SIZE = 256;
    const static int32_t _S_SHIFT = 128;
    static int32_t _s_state;
    TrieNode *_root;
};
}  // namespace
