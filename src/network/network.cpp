#include "network/network.h"
#include "network/node.h"
#include "network/link.h"
#include <vector>

Network::Network(int width, int height) : width(width), height(height) {
    createNodes();
    createLinks();
}

void Network::initializeTopology() {
}

void Network::createNodes() {
    nodes.resize(height);
    for (int y = 0; y < height; ++y) {
        nodes[y].resize(width);
        for (int x = 0; x < width; ++x) {
            Node* node = new Node(x, y);
            nodes[y][x] = node;
        }
    }
}

void Network::createLinks() {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Node* currentNode = getNode(i, j);
            
            if (i < width - 1) {
                Node* rightNode = getNode(i + 1, j);
                Link* linkPositiveX = new Link(currentNode, rightNode, Direction::POSITIVE_X);
                links.push_back(linkPositiveX);
                currentNode->addLink(linkPositiveX);
                rightNode->addLink(linkPositiveX);
            }
            
            if (j < height - 1) {
                Node* downNode = getNode(i, j + 1);
                Link* linkPositiveY = new Link(currentNode, downNode, Direction::POSITIVE_Y);
                links.push_back(linkPositiveY);
                currentNode->addLink(linkPositiveY);
                downNode->addLink(linkPositiveY);
            }
        }
    }
}

Node* Network::getNode(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return nodes[y][x];
    }
    return nullptr;
}

std::vector<Link*> Network::getLinks() {
    return links;
}

Network::~Network() {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            delete nodes[i][j];
        }
    }
    for (Link* link : links) {
        delete link;
    }
}