//
// Created by Tareq Mansour on 11/30/20.
//

#include <iostream>
#include "json.hpp"
#include "AVL.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        // if no arguments error and terminate
        std::cerr << "Usage: " << argv[0] << " file1.json";
        return 1;
    }
    std::string fileLocation = argv[1];
    AVLTree avlTree = AVLTree(fileLocation);

    //AVLTree avlTree = AVLTree();

    std::cout << avlTree.JSON() << '\n';
}
