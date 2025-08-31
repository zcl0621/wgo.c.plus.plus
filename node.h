#ifndef CONSOLEGO_NODE_H
#define CONSOLEGO_NODE_H


#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "board.h"
#include "utils.h"
#include "colour.h"

const std::vector<std::string> mutors = {"B", "W", "AB", "AW", "AE", "PL", "SZ"};


struct Node : public std::enable_shared_from_this<Node> {
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
        this->mutorCheck(key);
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
        this->mutorCheck(key);
        this->props.erase(this->props.begin() + ki);
    }

    // DeleteValue checks if the given key in this node has the given value, and
    // removes that value, if it does.
    void DeleteValue(std::string key, std::string val) {
        auto ki = this->key_index(key);
        if (ki == -1) {
            return;
        }
        this->mutorCheck(key);
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

    Colour LastColor() {
        if (this->children.size() == 0) {
            auto props = this->AllKeys();
            if (props.size() >= 1) {
                if (props[0] == "B") {
                    return Colour::WHITE;
                } else if (props[0] == "W") {
                    return Colour::BLACK;
                } else {
                    return Colour::BLACK;
                }
            } else {
                return Colour::BLACK;
            }
        } else {
            auto props = this->children[0]->LastChild()->AllKeys();
            if (props.size() >= 1) {
                if (props[0] == "B") {
                    return Colour::WHITE;
                } else if (props[0] == "W") {
                    return Colour::BLACK;
                } else {
                    return Colour::BLACK;
                }
            } else {
                return Colour::BLACK;
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
                    std::remove_if(old_parent->children.begin(), old_parent->children.end(),
                                   [this](const std::shared_ptr<Node> &node) { return node.get() == this; }),
                    old_parent->children.end());
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
        this->clearBoardCacheRecursive();
    }

    // DeleteChildren deletes all children of a node. This is useful for
    // clearing the children of a node when it is no longer needed.
    void DeleteChildren() { children.clear(); }
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
        oss << "Node " << this << ": depth " << (int) this->GetLine().size() - 1 << ", " << children.size() << " "
            << noun << ", subtree size " << this->SubtreeSize() << ", keys [";
        for (size_t i = 0; i < keys.size(); ++i) {
            oss << keys[i];
            if (i + 1 < keys.size())
                oss << " ";
        }
        oss << "]";
        return oss.str();
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
        if (all_b.size() + all_w.size() > 0 && all_ab.size() + all_aw.size() + all_ae.size() > 0) {
            throw std::runtime_error("Mix of move and setup properties");
        }
        auto parent = this->parent.lock().get();
        if (parent) {
            auto board = parent->board.get();
            if (all_b.size() > 0) {
                auto mv = all_b[0];
                if (!ValidPoint(mv, board->size)) {
                    board->LegalColour(mv, Colour::BLACK);
                } else if (mv != "" && mv != "tt") {
                    throw std::runtime_error("Invalid B move point: " + mv);
                }
            }
            if (all_w.size() > 0) {
                auto mv = all_w[0];
                if (!ValidPoint(mv, board->size)) {
                    board->LegalColour(mv, Colour::WHITE);
                } else if (mv != "" && mv != "tt") {
                    throw std::runtime_error("Invalid W move point: " + mv);
                }
            }
        }
    }
    // Play attempts to play the specified move at the node. The argument should be
    // an SGF coordinate, e.g. "dd". The colour is determined intelligently.
    //
    // If successful, a new node is created, and attached as a child. That child is
    // then returned and the error is nil. However, if the specified move already
    // existed in a child, that child is returned instead and no new node is
    // created; the error is still nil. On failure, the original node is returned,
    // along with an error. Failure indicates the move was illegal.
    //
    // Note that passes cannot be played with Play.
    std::shared_ptr<Node> Play(std::string move) { return this->PlayColour(move, this->board->player, true); }

    // PlayColour is like Play, except the colour is specified rather than being
    // automatically determined.
    std::shared_ptr<Node> PlayColour(std::string move, Colour colour, bool checkLegal) {
        if (checkLegal) {
            auto legal = this->board->LegalColour(move,colour);
            if (!legal) {
                throw std::runtime_error("Illegal move: " + move);
            }
        }
        auto key = "B";
        if (colour == Colour::WHITE) {
            key = "W";
        }
        auto child = this->children;
        for (auto i = 0; i < child.size(); i++) {
            if (child[i]->ValueCount(key) == 1) {
                auto mv = child[i]->GetValue(key);
                if (mv == move) {
                    return child[i];
                }
            }
        }
        auto newNode = std::make_shared<Node>(shared_from_this());
        newNode->SetValue(key, move);
        return newNode;
    }
    // Pass passes. The colour is determined intelligently. Normally, a new node is
    // created, attached as a child, and returned. However, if the specified pass
    // already existed in a child, that child is returned instead and no new node is
    // created.
    std::shared_ptr<Node> Pass() { return this->PassColour(this->board->player); }

    // PassColour is like Pass, except the colour is specified rather than being
    // automatically determined.
    std::shared_ptr<Node> PassColour(Colour colour) {
        if (colour != Colour::WHITE && colour != Colour::BLACK) {
            throw std::runtime_error("Invalid colour: " + std::to_string(static_cast<int>(colour)));
        }
        auto key = (colour == Colour::WHITE) ? "W" : "B";
        for (const auto &child: this->children) {
            if (child->ValueCount(key) == 1) {
                auto mv = child->GetValue(key);
                if (!ValidPoint(mv, this->board->size)) {
                    return child;
                }
            }
        }
        auto newNode = std::make_shared<Node>(shared_from_this());
        newNode->SetValue(key, "");
        return newNode;
    }

    // Tree functions

    // GetRoot travels up the tree, examining each node's parent until it finds the
    // root node, which it returns.
    std::shared_ptr<Node> GetRoot() {
        auto root = shared_from_this();
        while (root->parent.lock()) {
            root = root->parent.lock();
        }
        return root;
    }

    // GetEnd travels down the tree from the node, until it reaches a node with zero
    // children. It returns that node. Note that, if GetEnd is called on a node that
    // is not on the main line, the result will not be on the main line either, but
    // will instead be the end of the current branch.
    std::shared_ptr<Node> GetEnd() {
        auto node = shared_from_this();
        while (!node->children.empty()) {
            node = node->children.back();
        }
        return node;
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

    // MakeMainLine adjusts the tree structure so that the main line leads to this
    // node.
    void MakeMainLine() {
        auto node = shared_from_this();
        while (auto parent = node->parent.lock()) {
            // Find node in parent's children
            auto it = std::find_if(parent->children.begin(), parent->children.end(),
                                   [node](const std::shared_ptr<Node> &child) { return child == node; });
            if (it != parent->children.end() && it != parent->children.begin()) {
                // Move node to front
                parent->children.erase(it);
                parent->children.insert(parent->children.begin(), node);
            }
            node = parent;
        }
    }

    // SubtreeSize returns the number of nodes in the subtree rooted at this node
    int SubtreeSize() {
        int size = 1; // Count this node
        for (const auto &child: children) {
            size += child->SubtreeSize();
        }
        return size;
    }

    // TreeSize returns the number of nodes in the whole tree.
    int TreeSize() { return this->GetRoot()->SubtreeSize(); }

    // SubtreeNodes returns a slice of every node in a node's subtree, including
    // itself.
    std::vector<std::shared_ptr<Node>> SubtreeNodes() {
        std::vector<std::shared_ptr<Node>> nodes;
        nodes.push_back(shared_from_this());
        for (const auto &child: children) {
            auto childNodes = child->SubtreeNodes();
            nodes.insert(nodes.end(), childNodes.begin(), childNodes.end());
        }
        return nodes;
    }

    // TreeNodes returns a slice of every node in the tree, including itself.
    std::vector<std::shared_ptr<Node>> TreeNodes() {
        std::vector<std::shared_ptr<Node>> nodes;
        auto root = GetRoot();
        if (root) {
            nodes = root->SubtreeNodes();
        }
        return nodes;
    }

    // SubTreeKeyValueCount returns the number of keys and values in a node's
    // subtree, including itself.
    std::pair<int, int> SubTreeKeyValueCount() {
        auto keyCount = this->KeyCount();
        auto valueCount = 0;
        for (auto &key: this->AllKeys()) {
            valueCount += this->ValueCount(key);
        }
        for (auto &child: this->children) {
            auto [childKeys, childValues] = child->SubTreeKeyValueCount();
            keyCount += childKeys;
            valueCount += childValues;
        }
        return {keyCount, valueCount};
    }
    // TreeKeyValueCount returns the number of keys and values in the whole tree.
    std::pair<int, int> TreeKeyValueCount() {
        auto [keyCount, valueCount] = this->GetRoot()->SubTreeKeyValueCount();
        return {keyCount, valueCount};
    }

    // RootBoardSize travels up the tree to the root, and then finds the board size,
    // which it returns as an integer. If no SZ property is present, it returns 19.
    int RootBoardSize() {
        auto root = this->GetRoot();
        auto sz = root->GetValue("SZ");
        if (sz == "") {
            return 19;
        }
        try {
            return std::stoi(sz);
        } catch (...) {
            return 19;
        }
    }

    // RootKomi travels up the tree to the root, and then finds the komi, which it
    // returns as a float64. If no KM property is present, it returns 0.
    float RootKomi() {
        auto root = this->GetRoot();
        auto km = root->GetValue("KM");
        if (km == "") {
            return 0;
        }
        try {
            return std::stod(km);
        } catch (...) {
            return 0;
        }
    }
    // Dyer returns the Dyer Signature of the entire tree.
    std::string Dyer() {
        std::map<int, std::string> vals = {{20, "??"}, {40, "??"}, {60, "??"}, {31, "??"}, {51, "??"}, {71, "??"}};
        auto moveCount = 0;
        std::shared_ptr<Node> node = this->GetRoot();
        auto size = this->RootBoardSize();
        bool breaked = false;
        std::vector<std::string> loopKeys = {"B", "W"};
        while (breaked == false) {
            for (auto &key: loopKeys) {
                try {
                    auto mv = node->GetValue(key);
                    moveCount += 1;
                    if (moveCount == 20 || moveCount == 40 || moveCount == 60 || moveCount == 31 || moveCount == 51 ||
                        moveCount == 71) {
                        if (ValidPoint(mv, size)) {
                            vals[moveCount] = mv;
                        }
                    }
                } catch (...) {
                }
            }
            node = node->MainChild();
            if (node == nullptr || moveCount > 71) {
                breaked = true;
            }
        }
        return std::to_string(size) + vals[20] + vals[40] + vals[60] + vals[31] + vals[51] + vals[71];
    }

    // clear_board_cache_recursive() needs to be called whenever a node's board cache becomes invalid.
    // This can be due to:
    //
    //		* Changing a board-altering property.
    //		* Changing the identity of its parent.
    void clearBoardCacheRecursive() {
        if (this->board.get() == nullptr) {
            return;
        }
        this->board = nullptr;
        for (auto &child: this->children) {
            child->clearBoardCacheRecursive();
        }
    }

    void mutorCheck(std::string key) {
        for (auto &s: mutors) {
            if (s == key) {
                this->clearBoardCacheRecursive();
                break;
            }
        }
    }
    // TODO: complete this function
    std::shared_ptr<Board> GetBoard() { return this->board; }

    // Save saves the entire game tree to the specified file. It does not need to be
    // called from the root node, but can be called from any node in an SGF tree -
    // the whole tree is always saved.

    std::string Save() { return SaveCollection(std::vector<std::shared_ptr<Node>>{shared_from_this()}); }

    std::string SaveCollection(std::vector<std::shared_ptr<Node>> nodes) {
        std::string sgf;
        std::vector<std::shared_ptr<Node>> roots;
        for (auto &node: nodes) {
            if (node) {
                roots.push_back(node);
            }
        }
        if (roots.size() == 0) {
            return "()";
        }
        for (auto &root: roots) {
            sgf = root->writeTree();
        }
        return sgf;
    }
    std::string writeTree() {
        std::string sgf = "(";
        sgf += this->WriteTo();
        if (children.size() == 0) {
            // leaf node
            sgf += ")";
            return sgf;
        } else if (children.size() == 1) {
            // main line, no branch, 递归主干
            sgf += children[0]->writeTree().substr(1, children[0]->writeTree().size() - 2); // 去掉子树的外层括号
        } else {
            // 分支，每个分支都递归包裹
            for (auto &child: children) {
                sgf += child->writeTree();
            }
        }
        sgf += ")";
        return sgf;
    }
};

#endif // CONSOLEGO_NODE_H
