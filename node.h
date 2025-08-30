#ifndef CONSOLEGO_NODE_H
#define CONSOLEGO_NODE_H

#endif // CONSOLEGO_NODE_H

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <algorithm>

#include "board.h"
#include "utils.h"

struct Node:  public std::enable_shared_from_this<Node>{
    // e.g. ["B" "dd"] ["TR", "dd", "fj", "np"]
    std::vector<std::vector<std::string>> props;

    std::vector<std::shared_ptr<Node>> children;

    std::weak_ptr<Node> parent;

    std::shared_ptr<Board> board;

    Node() = default;

    Node(const Node &) = delete;

    Node &operator=(const Node &) = delete;

    ~Node() = default;

    Node(std::shared_ptr<Node> parent) {
        this->parent = parent;
        parent->children.push_back(std::shared_ptr<Node>(this));
    }
    std::shared_ptr<Node> Copy() {
        auto ret = std::make_shared<Node>();
        ret->props = this->props;
        ret->children = this->children;
        ret->parent = this->parent;
        ret->board = this->board;
        return ret;
    };
    // Write the node in SGF format to an io.Writer.
    // This method instantiates io.WriterTo for no particularly good reason.
    std::string WriteTo() {
        std::string node = ";";
        for (auto &prop: this->props) {
            if (prop.empty())
                continue;
            node += prop[0];
            for (size_t i = 1; i < prop.size(); i++) {
                node += "[";
                node += prop[i];
                node += "]";
            }
        }
        return node;
    }

    int key_index(std::string key) {
        for (size_t i = 0; i < this->props.size(); i++) {
            if (this->props[i][0] == key) {
                return i;
            }
        }
        return -1;
    };


    // AddValue adds the specified string as a value for the given key. If the value
    // already exists for the key, nothing happens.
    void AddValue(std::string key, std::string val) {
        this->board->mutor_check(key);
        auto ki = this->key_index(key);
        if (ki == -1) {
            this->props.push_back(std::vector<std::string>{key, val});
            return;
        };
        for (size_t i = 1; i < this->props[ki].size(); i++) {
            if (this->props[ki][i] == val) {
                return;
            }
        }
        this->props[ki].push_back(val);
    }

    // DeleteKey deletes the given key and all of its values.
    void DeleteKey(std::string key) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return;
        }
        this->board->mutor_check(key);
        this->props.erase(this->props.begin() + ki);
    }

    // DeleteValue checks if the given key in this node has the given value, and
    // removes that value, if it does.
    void DeleteValue(std::string key, std::string val) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return;
        }
        this->board->mutor_check(key);
        for (size_t i = 1; i < this->props[ki].size(); i++) {
            if (this->props[ki][i] == val) {
                this->props[ki].erase(this->props[ki].begin() + i);
                return;
            }
        }
        if (this->props[ki].size() < 2) {
            this->props.erase(this->props.begin() + ki);
        }
    }

    // GetValue returns the first value for the given key, if present, in which case
    // ok will be true. Otherwise it returns "" and false.
    std::string GetValue(std::string key) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return "";
        }
        return this->props[ki][1];
    }

    // SetValue sets the specified string as the first and only value for the given
    // key.
    void SetValue(std::string key, std::string val) {
        this->DeleteKey(key);
        this->AddValue(key, val);
    }

    // SetValues sets the values of the key to the values provided. The original
    // slice remains safe to modify.
    void SetValues(std::string key, std::vector<std::string> values) {
        this->DeleteKey(key);
        for (auto &val: values) {
            this->AddValue(key, val);
        }
    }

    // KeyCount returns the number of keys a node has.
    int KeyCount() { return this->props.size(); }

    // ValueCount returns the number of values a key has.
    int ValueCount(std::string key) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return 0;
        }
        return this->props[ki].size() - 1;
    }

    // AllKeys returns a new slice of strings, containing all the keys that the node
    // has.
    std::vector<std::string> AllKeys() {
        std::vector<std::string> ret;
        for (auto &slice: this->props) {
            ret.push_back(slice[0]);
        }
        return ret;
    }

    // AllValues returns a new slice of strings, containing all the values that a
    // given key has in this node.
    std::vector<std::string> AllValues(std::string key) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return std::vector<std::string>();
        }
        std::vector<std::string> ret;
        for (size_t i = 1; i < this->props[ki].size(); i++) {
            ret.push_back(this->props[ki][i]);
        }
        return ret;
    }

    // GetLastMove returns the last move in the node, if present, in which case
    // ok will be true. Otherwise it returns "" and false.
    std::tuple<int, int, std::string, bool> GetLastMove() {
        auto lastChild = this->LastChild();
        if (lastChild == nullptr) {
            return std::make_tuple(0, 0, "", false);
        }
        auto lastMove = lastChild->AllValues("B");
        if (lastMove.size() == 1) {
            auto [x, y, onboard] = ParsePoint(lastMove[0], this->board->size);
            if (onboard) {
                return std::make_tuple(x, y, "B", true);
            }
        }
        lastMove = lastChild->AllValues("W");
        if (lastMove.size() == 1) {
            auto [x, y, onboard] = ParsePoint(lastMove[0], this->board->size);
            if (onboard) {
                return std::make_tuple(x, y, "W", true);
            }
        }
        return std::make_tuple(0, 0, "", false);
    }

    // Parent returns the parent of a node. This will be nil if the node is the root
    // of the tree.
    std::shared_ptr<Node> Parent() { return this->parent.lock(); }

    // Children returns a new slice of pointers to all the node's children.
    std::vector<std::shared_ptr<Node>> Children() { return this->children; }

    // MainChild returns the first child a node has. If the node has zero children,
    // nil is returned.
    std::shared_ptr<Node> MainChild() {
        if (this->children.size() == 0) {
            return nullptr;
        }
        return this->children[0];
    }

    std::shared_ptr<Node> LastChild() {
        if (this->children.size() == 0) {
            return nullptr;
        }
        return this->children[this->children.size() - 1];
    }

    std::string LastColor() {
        if (this->children.size() == 0) {
            auto props = this->AllKeys();
            if (props.size() >= 1) {
                if (props[0] == "B") {
                    return "W";
                } else if (props[0] == "W") {
                    return "B";
                } else {
                    return "B";
                }
            } else {
                return "B";
            }
        } else {
            auto props = this->children[0]->LastChild()->AllKeys();
            if (props.size() >= 1) {
                if (props[0] == "B") {
                    return "W";
                } else if (props[0] == "W") {
                    return "B";
                } else {
                    return "B";
                }
            } else {
                return "B";
            }
        }
    }

    // SetParent sets a node's parent. The node is also removed from the original
    // parent's list of children, and added to the new parent's list. SetParent
    // panics if a cyclic tree is created.
    void SetParent(std::shared_ptr<Node> new_parent) {
        // Remove from old parent's children
        if (auto old_parent = parent.lock()) {
            old_parent->children.erase(
                std::remove_if(
                    old_parent->children.begin(),
                    old_parent->children.end(),
                    [this](const std::shared_ptr<Node>& node) {
                        return node.get() == this;
                    }
                ),
                old_parent->children.end()
            );
        }
    
        // Set new parent
        parent = new_parent;
    
        // Add to new parent's children if parent exists
        if (new_parent) {
            // Check for cycles
            auto current = new_parent.get();
            while (current) {
                if (current == this) {
                    throw std::runtime_error("Cycle detected in node hierarchy");
                }
                current = current->parent.lock().get();
            }
            
            // Add to children
            new_parent->children.push_back(shared_from_this());
        }
        // TODO: complete this
        // clean_board_cache_recursive()
    }

    // DeleteChildren deletes all children of a node. This is useful for
    // clearing the children of a node when it is no longer needed.
    void DeleteChildren() {
        children.clear();
    }
    // ToString returns a string representation of the node for debugging
    std::string ToString() {
        if (!this) {
            return "<nil>";
        }

        std::string noun = "children";
        if (children.size() == 1) {
            noun = "child";
        }

        std::vector<std::string> keys = this->AllKeys();
        std::sort(keys.begin(), keys.end());

        std::ostringstream oss;
        oss << "Node " << this
            << ": depth " << (int)this->GetLine().size() - 1
            << ", " << children.size() << " " << noun
            << ", subtree size " << this->SubtreeSize()
            << ", keys [";
        for (size_t i = 0; i < keys.size(); ++i) {
            oss << keys[i];
            if (i + 1 < keys.size()) oss << " ";
        }
        oss << "]";
        return oss.str();
    }

    // GetLine returns the line representation of the node
    std::vector<std::shared_ptr<Node>> GetLine() const {
        std::vector<std::shared_ptr<Node>> ret;
        auto node = std::const_pointer_cast<Node>(shared_from_this());
        while (node) {
            ret.push_back(node);
            node = node->parent.lock();
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
    // SubtreeSize returns the number of nodes in the subtree rooted at this node
    int SubtreeSize() {
        int size = 1;  // Count this node
        for (const auto& child : children) {
            size += child->SubtreeSize();
        }
        return size;
    }

    // Validate checks a node for obvious problems;
    // found as an error, other wise it returns nil.
    void Validate() {
        auto all_b = this->AllValues("B");
        auto all_w = this->AllValues("W");
        auto all_ab = this->AllValues("AB");
        auto all_aw = this->AllValues("AW");
        auto all_ae = this->AllValues("AE");
        if (all_b.size() + all_w.size() > 1) {
            throw std::runtime_error("Too many moves (B or W values)");
        }
        if(all_b.size() + all_w.size() >0 && all_ab.size() + all_aw.size() + all_ae.size() >0){
            throw std::runtime_error("Mix of move and setup properties");
        }
        auto parent = this->parent.lock().get();
        if (parent) {
            auto board = parent->board.get();
            if(all_b.size() >0) {
                auto mv = all_b[0];
                if(!ValidPoint(mv, board->size)) {
                    board->LegalColour(mv, board->size);
                } else if (mv != "" && mv != "tt") {
                    throw std::runtime_error("Invalid B move point: " + mv);
                }
            }
            if(all_w.size() >0) {
                auto mv= all_w[0];
                if(!ValidPoint(mv, board->size)) {
                    board->LegalColour(mv, board->size);
                } else if (mv != "" && mv != "tt") {
                    throw std::runtime_error("Invalid W move point: " + mv);
                }
            }
        }
    }

};
