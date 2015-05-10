
#include <queue>
#include <iostream>

#include "ac_automation.h"

namespace tool
{
int32_t ACAutomation::_s_state = 0;

ACAutomation::ACAutomation() : _root(new TrieNode())
{
    _root->status = _s_state++;
}

ACAutomation::~ACAutomation()
{
    clear(_root);
    _root = NULL;
}

void ACAutomation::preprocess(const std::vector<std::string> &keywords)
{
    for(std::vector<std::string>::const_iterator cit = keywords.begin();
            cit != keywords.end();
            cit++)
    {
        insert(*cit);
    }
    build_fail_tbl();
    //print_trie();
}

int32_t ACAutomation::search(const std::string &sentence)
{
    if(_root == NULL)
    {
        return 0;
    }

    int32_t res = 0;
    TrieNode *node = _root;
    for(std::string::const_iterator cit = sentence.begin();
            cit != sentence.end();
            cit++)
    {
        // find next valid position
        while(node->vec_next.at(*cit + _S_SHIFT) == NULL && node != _root)
        {
            node = node->fail;
        }
        node = node->vec_next.at(*cit + _S_SHIFT);
        if(node == NULL)
        {
            node = _root;
        }

        // find word
        if(node->vec_keywords.size())
        {
            res += node->vec_keywords.size();
        }
    }
    return res;
}

void ACAutomation::insert(const std::string &keyword)
{
    TrieNode *node = _root;
    for(std::string::const_iterator cit = keyword.begin();
            cit != keyword.end();
            cit++)
    {
        // new inexistent node
        if(node->vec_next.at(*cit + _S_SHIFT) == NULL)
        {
            node->vec_next.at(*cit + _S_SHIFT) = new TrieNode();
            node->vec_next.at(*cit + _S_SHIFT)->status = _s_state++;
        }
        node = node->vec_next.at(*cit + _S_SHIFT);
    }
    node->vec_keywords.push_back(keyword);
}

void ACAutomation::build_fail_tbl()
{
    if(_root == NULL)
    {
        return;
    }

    // get fail node for nodes whose depth is 1
    std::queue<TrieNode *> queue_nodes;
    for(std::vector<TrieNode *>::const_iterator cit = _root->vec_next.begin();
            cit != _root->vec_next.end();
            cit++)
    {
        if(*cit != NULL)
        {
            (*cit)->fail = _root;
            queue_nodes.push(*cit);
        }
    }

    // get fail node for nodes whose depth is more than 1
    while(queue_nodes.size())
    {
        // get next node
        TrieNode *node = queue_nodes.front();
        queue_nodes.pop();

        int32_t index = 0;
        for(std::vector<TrieNode *>::const_iterator cit = node->vec_next.begin();
                cit != node->vec_next.end();
                cit++, index++)
        {
            if(*cit != NULL)
            {
                queue_nodes.push(*cit);  // push child node

                // get fail node for *cit
                TrieNode *temp = node->fail;
                TrieNode *next = temp->vec_next.at(index);
                while(temp != _root && next == NULL)
                {
                    temp = temp->fail;
                }
                if(next != NULL)
                {
                    (*cit)->fail = next;temp->vec_next.at(index);
                    (*cit)->vec_keywords.insert((*cit)->vec_keywords.end(),
                            next->vec_keywords.begin(),
                            next->vec_keywords.end());
                }
                else
                {
                    (*cit)->fail = _root;
                }
            }
        }
    }
}

void ACAutomation::clear(TrieNode *node)
{
    if(node == NULL)
        return;

    for(std::vector<TrieNode *>::const_iterator cit = node->vec_next.begin();
            cit != node->vec_next.end();
            cit++)
    {
        clear(*cit);
    }
    delete node;
}

void ACAutomation::print_trie()
{
    if(_root == NULL)
    {
        return;
    }

    std::cout << _root << std::endl;

    std::queue<TrieNode *> queue_nodes;
    queue_nodes.push(_root);
    // get fail node for nodes whose depth is more than 1
    while(queue_nodes.size())
    {
        // get next node
        TrieNode *node = queue_nodes.front();
        queue_nodes.pop();

        int32_t index = 0;
        for(std::vector<TrieNode *>::const_iterator cit = node->vec_next.begin();
                cit != node->vec_next.end();
                cit++, index++)
        {
            if(*cit != NULL)
            {
                queue_nodes.push(*cit);  // push child node
                char ch = index;
                std::cout << ch << ":" << *cit << ":" << (*cit)->status << ":" << (*cit)->fail << "  ";
            }
        }
        std::cout << std::endl;
    }
}
}  // namespace
