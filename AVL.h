//
// Created by Tareq Mansour on 11/29/20.
//

#include <iostream>

class AVLTree;

class AVLNode {
public:
    AVLNode(int key);
    AVLNode(int key, std::weak_ptr<AVLNode> parent);
    bool IsLeaf() const;
    bool IsMissingChild() const;
    bool HasLeftChild() const;
    bool HasRightChild() const;
    int GetBalance() ;
    void DeleteChild(std::shared_ptr<AVLNode> v);
    void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u);

private:
    int key_;
    int height_;
    int balanceFactor_;
    int SetHeight();
    std::weak_ptr<AVLNode> parent_;
    std::shared_ptr<AVLNode> left_;
    std::shared_ptr<AVLNode> right_;

    int balanceLevel();

    friend AVLTree;
}; // class AVLNode

class AVLTree {

public:

    AVLTree();
    AVLTree(std::string JSONFileLocation);


    void Insert(int key);
    void InsertJSON(std::string fileLocation);
    bool Delete(int key);
    bool Find(int key) const;
    std::string JSON() const;
    size_t size() const;
    bool empty() const;
    int DeleteMin();




private:

//    void Balance(std::shared_ptr<AVLNode> currentNode);
    std::shared_ptr<AVLNode> RightRotation(const std::shared_ptr<AVLNode>& x);
    std::shared_ptr<AVLNode> LeftRotation(const std::shared_ptr<AVLNode> x);
    std::shared_ptr<AVLNode> root_;
    size_t size_;

}; // End class AVLTree
