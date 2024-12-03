//
// Created by alexj on 12/3/2024.
//

#ifndef PROJECT_3_DSA_MAX_HEAP_H
#define PROJECT_3_DSA_MAX_HEAP_H

#include <iostream>
#include <vector>
#include "SalesData.h"
using namespace std;

// sorted by total profit
class max_heap {
private:
    vector<SalesData> heap;
    // heapify up
    void heapifyUp(int index) {
        if (index <= 0) return; // root has no parent
        int parentIndex = (index - 1) / 2;
        if (heap[index].totalProfit > heap[parentIndex].totalProfit) {
            swap(heap[index], heap[parentIndex]);
            heapifyUp(parentIndex);
        }
    }
    // heapity down
    void heapifyDown(int index) {
        int leftChildIndex = 2 * index + 1;
        int rightChildIndex = 2 * index + 2;
        int largest = index;
        if (leftChildIndex < heap.size() && heap[leftChildIndex].totalProfit > heap[largest].totalProfit) {
            largest = leftChildIndex;
        }
        if (rightChildIndex < heap.size() && heap[rightChildIndex].totalProfit > heap[largest].totalProfit) {
            largest = rightChildIndex;
        }
        if (largest != index) {
            swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    void insert(SalesData &value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    pair<string, SalesData> extractMax() {
        if (heap.empty()) {
            throw out_of_range("Heap is empty");
        }
        SalesData max = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return make_pair(max.orderID,max);
    }

    void display() const {
        for (int i = 0; i < heap.size(); ++i) {
            cout << heap[i].totalProfit << " ";
        }
        cout << endl;
    }

    int size() const {
        return heap.size();
    }

    bool isEmpty() const {
        return heap.empty();
    }
};
#endif //PROJECT_3_DSA_MAX_HEAP_H
