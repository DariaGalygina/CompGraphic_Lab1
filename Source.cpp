#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>

struct Node {
    long long value;
    Node* parent;
    std::string operation;

    Node(long long val, Node* p = nullptr, const std::string& op = "")
        : value(val), parent(p), operation(op) {}
};

class StateSearch {
private:
    std::unordered_set<long long> visited;
    std::queue<Node*> queue;

    void clearMemory(Node* node) {
        if (node) {
            clearMemory(node->parent);
            delete node;
        }
    }

    void reconstructPath(Node* node, std::vector<std::string>& path) {
        if (node->parent) {
            reconstructPath(node->parent, path);
            path.push_back(node->operation);
        }
    }

public:
    ~StateSearch() {
        while (!queue.empty()) {
            Node* node = queue.front();
            queue.pop();
            clearMemory(node);
        }
    }

    // Прямой поиск: +3, *2
    std::vector<std::string> forwardSearch(long long start, long long target) {
        visited.clear();
        std::queue<Node*> empty;
        std::swap(queue, empty);

        Node* root = new Node(start);
        queue.push(root);
        visited.insert(start);

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            if (current->value == target) {
                std::vector<std::string> path;
                reconstructPath(current, path);
                return path;
            }

            // Операция +3
            long long newValue1 = current->value + 3;
            if (newValue1 <= target * 2 && visited.find(newValue1) == visited.end()) {
                Node* newNode = new Node(newValue1, current, "+3");
                queue.push(newNode);
                visited.insert(newValue1);
            }

            // Операция *2
            long long newValue2 = current->value * 2;
            if (newValue2 <= target * 2 && visited.find(newValue2) == visited.end()) {
                Node* newNode = new Node(newValue2, current, "*2");
                queue.push(newNode);
                visited.insert(newValue2);
            }
        }

        return {};
    }

    // Прямой поиск: +3, *2, -2
    std::vector<std::string> forwardSearchWithSubtract(long long start, long long target) {
        visited.clear();
        std::queue<Node*> empty;
        std::swap(queue, empty);

        Node* root = new Node(start);
        queue.push(root);
        visited.insert(start);

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            if (current->value == target) {
                std::vector<std::string> path;
                reconstructPath(current, path);
                return path;
            }

            // Операция +3
            long long newValue1 = current->value + 3;
            if (newValue1 > 0 && newValue1 <= target * 2 && visited.find(newValue1) == visited.end()) {
                Node* newNode = new Node(newValue1, current, "+3");
                queue.push(newNode);
                visited.insert(newValue1);
            }

            // Операция *2
            long long newValue2 = current->value * 2;
            if (newValue2 > 0 && newValue2 <= target * 2 && visited.find(newValue2) == visited.end()) {
                Node* newNode = new Node(newValue2, current, "*2");
                queue.push(newNode);
                visited.insert(newValue2);
            }

            // Операция -2
            long long newValue3 = current->value - 2;
            if (newValue3 > 0 && visited.find(newValue3) == visited.end()) {
                Node* newNode = new Node(newValue3, current, "-2");
                queue.push(newNode);
                visited.insert(newValue3);
            }
        }

        return {};
    }

    // Обратный поиск: +3, *2
    std::vector<std::string> backwardSearch(long long start, long long target) {
        visited.clear();
        std::queue<Node*> empty;
        std::swap(queue, empty);

        Node* root = new Node(target);
        queue.push(root);
        visited.insert(target);

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            if (current->value == start) {
                std::vector<std::string> path;
                reconstructPath(current, path);
                std::reverse(path.begin(), path.end());
                // Преобразуем обратные операции в прямые
                for (auto& op : path) {
                    if (op == "-3") op = "+3";
                    else if (op == "/2") op = "*2";
                    else if (op == "+2") op = "-2";
                }
                return path;
            }

            // Обратная операция для +3: -3
            if (current->value - 3 >= start / 2) {
                long long newValue1 = current->value - 3;
                if (newValue1 >= start / 2 && visited.find(newValue1) == visited.end()) {
                    Node* newNode = new Node(newValue1, current, "-3");
                    queue.push(newNode);
                    visited.insert(newValue1);
                }
            }

            // Обратная операция для *2: /2 (только если делится нацело)
            if (current->value % 2 == 0) {
                long long newValue2 = current->value / 2;
                if (newValue2 >= start / 2 && visited.find(newValue2) == visited.end()) {
                    Node* newNode = new Node(newValue2, current, "/2");
                    queue.push(newNode);
                    visited.insert(newValue2);
                }
            }
        }

        return {};
    }

    // Двунаправленный поиск
    std::vector<std::string> bidirectionalSearch(long long start, long long target) {
        if (start == target) return {};

        std::unordered_map<long long, Node*> forwardVisited;
        std::unordered_map<long long, Node*> backwardVisited;

        std::queue<Node*> forwardQueue;
        std::queue<Node*> backwardQueue;

        Node* forwardRoot = new Node(start);
        Node* backwardRoot = new Node(target);

        forwardQueue.push(forwardRoot);
        backwardQueue.push(backwardRoot);
        forwardVisited[start] = forwardRoot;
        backwardVisited[target] = backwardRoot;

        while (!forwardQueue.empty() && !backwardQueue.empty()) {
            // Обрабатываем прямое направление
            if (!forwardQueue.empty()) {
                Node* current = forwardQueue.front();
                forwardQueue.pop();

                // Операция +3
                long long newValue1 = current->value + 3;
                if (processBidirectionalNode(current, newValue1, "+3", forwardVisited, backwardVisited,
                    forwardQueue, target)) {
                    return getBidirectionalPath(forwardVisited[newValue1], backwardVisited[newValue1]);
                }

                // Операция *2
                long long newValue2 = current->value * 2;
                if (processBidirectionalNode(current, newValue2, "*2", forwardVisited, backwardVisited,
                    forwardQueue, target)) {
                    return getBidirectionalPath(forwardVisited[newValue2], backwardVisited[newValue2]);
                }
            }

            // Обрабатываем обратное направление
            if (!backwardQueue.empty()) {
                Node* current = backwardQueue.front();
                backwardQueue.pop();

                // Обратная операция для +3: -3
                long long newValue1 = current->value - 3;
                if (newValue1 >= start &&
                    processBidirectionalNode(current, newValue1, "-3", backwardVisited, forwardVisited,
                        backwardQueue, start, true)) {
                    return getBidirectionalPath(forwardVisited[newValue1], backwardVisited[newValue1]);
                }

                // Обратная операция для *2: /2
                if (current->value % 2 == 0) {
                    long long newValue2 = current->value / 2;
                    if (newValue2 >= start &&
                        processBidirectionalNode(current, newValue2, "/2", backwardVisited, forwardVisited,
                            backwardQueue, start, true)) {
                        return getBidirectionalPath(forwardVisited[newValue2], backwardVisited[newValue2]);
                    }
                }
            }
        }

        return {};
    }

private:
    bool processBidirectionalNode(Node* current, long long newValue, const std::string& op,
        std::unordered_map<long long, Node*>& currentVisited,
        std::unordered_map<long long, Node*>& oppositeVisited,
        std::queue<Node*>& queue,
        long long limit, bool isBackward = false) {
        if (newValue < 0) return false;

        if (isBackward && newValue < limit) return false;
        if (!isBackward && newValue > limit * 2) return false;

        if (currentVisited.find(newValue) == currentVisited.end()) {
            Node* newNode = new Node(newValue, current, op);
            currentVisited[newValue] = newNode;
            queue.push(newNode);

            // Проверяем, есть ли это значение в противоположном направлении
            if (oppositeVisited.find(newValue) != oppositeVisited.end()) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> getBidirectionalPath(Node* forwardNode, Node* backwardNode) {
        std::vector<std::string> forwardPath;
        std::vector<std::string> backwardPath;

        // Собираем путь от встречной точки до начала
        reconstructPath(forwardNode, forwardPath);

        // Собираем путь от встречной точки до цели (в обратном порядке)
        Node* current = backwardNode;
        while (current->parent) {
            backwardPath.push_back(current->operation);
            current = current->parent;
        }

        // Преобразуем обратные операции в прямые
        for (auto& op : backwardPath) {
            if (op == "-3") op = "+3";
            else if (op == "/2") op = "*2";
            else if (op == "+2") op = "-2";
        }
        std::reverse(backwardPath.begin(), backwardPath.end());

        // Объединяем пути
        forwardPath.insert(forwardPath.end(), backwardPath.begin(), backwardPath.end());
        return forwardPath;
    }
};

void printPath(const std::vector<std::string>& path, long long start, long long target) {
    if (path.empty()) {
        std::cout << "Path not found from " << start << " to " << target << std::endl;
        return;
    }

    std::cout << "Path from " << start << " to " << target << " (" << path.size() << " operations): ";
    long long current = start;
    std::cout << current;

    for (const auto& op : path) {
        if (op == "+3") current += 3;
        else if (op == "*2") current *= 2;
        else if (op == "-2") current -= 2;
        std::cout << " -> " << op << " -> " << current;
    }
    std::cout << std::endl;
}

int main() {
    StateSearch searcher;

    // Tests
    std::vector<std::pair<long long, long long>> testCases = {
        {1, 100},
        {2, 55},
        {2, 100},
        {1, 97},
        {2, 1000},
        {2, 10000001}
    };

    std::cout << "=== Forward Search (+3, *2) ===" << std::endl;
    for (const auto& test : testCases) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto path = searcher.forwardSearch(test.first, test.second);
        auto endTime = std::chrono::high_resolution_clock::now();

        printPath(path, test.first, test.second);
        std::cout << "Time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()
            << " microseconds" << std::endl << std::endl;
    }

    std::cout << "\n=== Forward Search (+3, *2, -2) ===" << std::endl;
    for (const auto& test : testCases) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto path = searcher.forwardSearchWithSubtract(test.first, test.second);
        auto endTime = std::chrono::high_resolution_clock::now();

        printPath(path, test.first, test.second);
        std::cout << "Time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()
            << " microseconds" << std::endl << std::endl;
    }

    std::cout << "\n=== Backward Search (+3, *2) ===" << std::endl;
    for (const auto& test : testCases) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto path = searcher.backwardSearch(test.first, test.second);
        auto endTime = std::chrono::high_resolution_clock::now();

        printPath(path, test.first, test.second);
        std::cout << "Time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()
            << " microseconds" << std::endl << std::endl;
    }

    std::cout << "\n=== Bidirectional Search (+3, *2) ===" << std::endl;
    for (const auto& test : testCases) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto path = searcher.bidirectionalSearch(test.first, test.second);
        auto endTime = std::chrono::high_resolution_clock::now();

        printPath(path, test.first, test.second);
        std::cout << "Time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()
            << " microseconds" << std::endl << std::endl;
    }

    return 0;
}