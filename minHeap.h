//
// Created by Paul Kolsun on 11/2/25.
//

#ifndef COP3530_PROJECT2_MINHEAP_H
#define COP3530_PROJECT2_MINHEAP_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

// Each city and its weight
struct Node {
    string cityName;
    int weight; //weight (score) = increment score by 1 every time a city has an event
    // ignore cities with score 0 (cities missed during data collection)

    Node(string name, int w = 0) {
        cityName = name;
        weight = w;
    }
};

// Makes cities ordered by least severe weather
class MinHeap {
private:
    vector<Node> heap;  // Internal heap array to store all city nodes

    // Get parent and child indices for a given node
    int parent(int i) { return (i - 1) / 2; }
    int left(int i)   { return 2 * i + 1; }
    int right(int i)  { return 2 * i + 2; }


    // Restore heap property by moving a node up
    void heapifyUp(int i) {
        while (i != 0 && heap[parent(i)].weight > heap[i].weight) {
            swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }


    // Restore heap property by moving a node down

    void heapifyDown(int i) {
        int smallest = i;
        int l = left(i);
        int r = right(i);

        if (l < heap.size() && heap[l].weight < heap[smallest].weight)
            smallest = l;
        if (r < heap.size() && heap[r].weight < heap[smallest].weight)
            smallest = r;

        if (smallest != i) {
            swap(heap[i], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:

    // Insert into the heap

    void insert(const string &cityName, int weight) {
        Node newNode(cityName, weight);
        heap.push_back(newNode);
        heapifyUp(heap.size() - 1);
    }


    // Remove and return the city with the smallest weight

    Node extractMin() {
        if (heap.empty()) {
            throw runtime_error("Heap is empty");
        }

        Node root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);

        return root;
    }


    // Return the city with the smallest weight without removing

    Node getMin() {
        if (heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        return heap[0];
    }


    // Change a city's weight and reheapify

    void setWeight(const string &cityName, int newWeight) {
        for (int i = 0; i < heap.size(); ++i) {
            if (heap[i].cityName == cityName) {
                heap[i].weight = newWeight;
                heapifyUp(i);
                heapifyDown(i);
                return;
            }
        }
    }


    // Gets the  current weight for a given city

    int getWeight(const string &cityName) const {
        for (const auto &node : heap) {
            if (node.cityName == cityName)
                return node.weight;
        }
        return -1;
    }


    vector<Node> getAll() const {
        return heap;
    }



    int size() const {
        return heap.size();
    }



    bool empty() const {
        return heap.empty();
    }

    vector<Node> getTop(int x){
        int lastweight = 0;
        vector<Node> output;
        output.push_back(getMin());
        lastweight = output[0].weight;
        int pos = 0;
        for (int i = 1; i < x; i++) {
            for (int j = pos; j<size()-1; j++) {
                if (heap[j].weight > lastweight) {
                    lastweight = heap[j].weight;
                    output.push_back(heap[j]);
                    break;
                }
            }
        }
        return output;
    }

    void printTopX(int x) {
        vector<Node> out = getTop(x);
        for (auto i : out) {
            std::cout << i.cityName << " : " << i.weight << std::endl;
        }
    }
};

#endif //COP3530_PROJECT2_MINHEAP_H

