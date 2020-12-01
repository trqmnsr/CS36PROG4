//
// Created by Tareq Mansour on 11/29/20.
//
#include <iostream>
#include <fstream>
#include <queue>
#include "json.hpp"
#include "AVL.h"


AVLNode::AVLNode(int key) :
        key_(key),
        height_(0),
        balanceFactor_(0),
        parent_(std::weak_ptr<AVLNode>()),
        left_(nullptr),
        right_(nullptr){}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
        key_(key),
        height_(0),
        balanceFactor_(0),
        parent_(parent),
        left_(nullptr),
        right_(nullptr){
    this->parent_.lock()->height_ ++;
}

bool AVLNode::IsLeaf() const {
    return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
    return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
    return right_ != nullptr;
}

int AVLNode::GetBalance() {
    int right = HasRightChild() ? right_->height_ : -1;
    int left = HasLeftChild() ? left_->height_ : -1;
    this->balanceFactor_ = right - left;
    return balanceFactor_;
}

int AVLNode::SetHeight() {
    int leftHeight = this->HasLeftChild() ? this->left_->height_ : -1;
    int rightHeight = this->HasRightChild() ? this->right_->height_ : -1;
    this->height_ = std::max(leftHeight, rightHeight) + 1;
    return this->height_;
}

void AVLNode::DeleteChild(std::shared_ptr<AVLNode> v) {
    if (left_ == v) {
        left_ = nullptr;
    } else if (right_ == v) {
        right_ = nullptr;
    } else {
        std::cerr << "AVLNode::DeleteChild Error: non-child passed as argument\n";
        exit(EXIT_FAILURE);
    }
}

void AVLNode::ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u) {
    if (left_ == u || right_ == u) {
        std::cerr << "AVLNode::ReplaceChild Error: child passed as replacement\n";
    }
    if (left_ == v) {
        left_ = u;
        u->parent_ = v->parent_;
    } else if (right_ == v) {
        right_ = u;
        u->parent_ = v->parent_;
    } else {
        std::cerr << "AVLNode::ReplaceChild Error: non-child passed as argument\n";
        exit(EXIT_FAILURE);
    }
}

AVLTree::AVLTree(): root_(nullptr), size_(0) {}

AVLTree::AVLTree(std::string JSONFileLocation): root_(nullptr), size_(0) {
    AVLTree();
    InsertJSON(JSONFileLocation);
}

void AVLTree::Insert(int key) {

    /* If root is null place in root*/
    if (root_ == nullptr) {
        root_ = std::make_shared<AVLNode>(key);
        size_++;
        return;
    }
    std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;
    while (currentNode != nullptr) {
        lastNode = currentNode;
        currentNode = (key < currentNode->key_) ?
                      currentNode->left_ : currentNode->right_;
    }
    if (key < lastNode->key_) {
        lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
        currentNode = lastNode->left_;
    } else {
        currentNode = lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
    }
    size_++;
    std::shared_ptr<AVLNode> child = nullptr;
    std::shared_ptr<AVLNode> grandChild = nullptr;
    do{
        currentNode->SetHeight();
        int balanceFactor = currentNode->GetBalance();

        if (balanceFactor < -1 && grandChild == child->left_){
            currentNode = RightRotation(currentNode);
            child = currentNode->right_;
        } else if(balanceFactor < -1 && grandChild == child->right_) {
            child = LeftRotation(child);
            currentNode = RightRotation(currentNode);
        }else if(balanceFactor > 1 && grandChild == child->right_) {
            currentNode = LeftRotation(currentNode);
            child = currentNode->left_;
        }
        else if(balanceFactor > 1 && grandChild == child->left_) {
            child = RightRotation(child);
            currentNode = LeftRotation(currentNode);
        }
        if (currentNode->parent_.lock() == nullptr) root_ = currentNode;
        grandChild = child;
        child = currentNode;
        currentNode = currentNode->parent_.lock();
    } while (currentNode != nullptr);
}

std::shared_ptr<AVLNode> AVLTree::RightRotation(const std::shared_ptr<AVLNode>& x) {
    std::shared_ptr<AVLNode> y = x->left_;
    std::shared_ptr<AVLNode> u = x->parent_.lock();
    x->left_ = y->right_;
    if (x->HasLeftChild()) x->left_->parent_ = x;
    y->parent_ = x->parent_;
    if (u != nullptr) {
        if (u->left_ == x) u->left_ = y;
        else if (u->right_ == x) u->right_ = y;
    }
    x->parent_ = std::weak_ptr<AVLNode>(y);
    y->right_ = x;
    x->SetHeight();
    x->GetBalance();
    y->SetHeight();
    y->GetBalance();
    return y;
}

std::shared_ptr<AVLNode> AVLTree::LeftRotation(const std::shared_ptr<AVLNode> x) {
    std::shared_ptr<AVLNode> y = x->right_;
    std::shared_ptr<AVLNode> u = x->parent_.lock();
    x->right_ = y->left_;
    if (x->HasRightChild()) x->right_->parent_ = x;
    y->parent_ = x->parent_;
    if (u != nullptr) {
        if (u->left_ == x) u->left_ = y;
        else if (u->right_ == x) u->right_ = y;
    }
    x->parent_ = std::weak_ptr<AVLNode>(y);
    y->left_ = x;
    x->SetHeight();
    x->GetBalance();
    y->SetHeight();
    y->GetBalance();
    return y;
}

void AVLTree::InsertJSON(std::string fileLocation){
    //nodes_ = std::vector<KeyValuePair>(); // initialize nodes array
    std::ifstream file; // create file stream
    try {
        // open file at specified first argument file name
        file.open(fileLocation);
    } catch (std::invalid_argument & e) { // catch if either loadfile throws error
        throw e.what(); // feed thrown error
    }
    // initialize jsonObject to receive json file
    nlohmann::json jsonObject = nullptr;
    // if file argument is not available display error and close
    if (!file.is_open()) {
        throw std::domain_error("ERROR: FILE" + fileLocation + " NOT FOUND!");
    }
    // try/catch will catch error if the json file is parse incorrectly
    try {
        // stream file to jsonObject
        file >> jsonObject;
    } catch (nlohmann::json::parse_error& e) {
        // send error to cerr and terminate
        std::cerr << __PRETTY_FUNCTION__ << " : " << __LINE__ << " : "
                  << e.what() << '\n';
    }
    // close file
    file.close();

    for (auto itr = jsonObject.begin(); itr !=jsonObject.end(); itr++) {
        if (itr.key() != "metadata"){
            Insert(itr.value()["key"]);
        }
    }
}
//bool AVLTree::Delete(int key) {}
//bool AVLTree::Find(int key) const {}
std::string AVLTree::JSON() const {
    nlohmann::json result;
    std::queue< std::shared_ptr<AVLNode> > nodes;
    if (root_ != nullptr) {
        result["root"] = root_->key_;
        result["height"] = root_->height_;
        result["size"] = size_;
        nodes.push(root_);
        while (!nodes.empty()) {
            auto v = nodes.front();
            nodes.pop();
            std::string key = std::to_string(v->key_);
            result[key]["balance factor"] = v->balanceFactor_;
            result[key]["height"] = v->height_;
            if (v->left_ != nullptr) {
                result[key]["left"] = v->left_->key_;
                nodes.push(v->left_);
            }
            if (v->right_ != nullptr) {
                result[key]["right"] = v->right_->key_;
                nodes.push(v->right_);
            }
            if (v->parent_.lock() != nullptr) {
                result[key]["parent"] = v->parent_.lock()->key_;
            } else {
                result[key]["root"] = true;
            }
        }
    }
    result["size"] = size_;
    return result.dump(2) + "\n";
}


size_t AVLTree::size() const {
    return size_;
}
bool AVLTree::empty() const {
    return size_ == 0;
}
//int AVLTree::DeleteMin() {}
