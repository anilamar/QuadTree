#include <iostream>
#include <string>    // For std::string, std::to_string
#include <sstream>   // For std::stringstream

// Forward declaration for the QuadTree class
class QuadTree;

// --- Point Structure ---
// Represents a 2D coordinate for the top-left corner of a node.
struct Point {
    int x;
    int y;

    // Default constructor
    Point(int px = 0, int py = 0) : x(px), y(py) {}

    // For easy printing of Point objects
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// --- QuadTree Class ---
// Each QuadTree object represents a node in the QuadTree structure.
class QuadTree {
public:
    // Static member to generate unique IDs for each node
    static int nextId;

    int _id;          // Unique ID for the node
    Point _pos;       // Top-left corner position of this node's region
    int _width;       // Width of this node's region
    int _height;      // Height of this node's region

    // Pointers to the four child quadrants
    QuadTree* _northWest;    // Top-left child
    QuadTree* _northEast;    // Top-right child
    QuadTree* _southWest;    // Bottom-left child
    QuadTree* _southEast;    // Bottom-right child

    // Constructor for the QuadTree node
    // Initializes the node's properties and sets children to nullptr.
    QuadTree(Point pos, int width, int height)
        : _pos(pos), _width(width), _height(height),
          _northWest(nullptr), _northEast(nullptr),
          _southWest(nullptr), _southEast(nullptr) {
        _id = nextId++; // Assign a unique ID
        // std::cout << "Created Node " << _id << ": Pos=" << _pos.toString()
        //           << ", Size=" << _width << "x" << _height << std::endl;
    }

    // Destructor
    // Cleans up dynamically allocated child nodes to prevent memory leaks.
    ~QuadTree() {
        //std::cout << "Deleting Node " << _id << std::endl;
        delete _northWest;
        delete _northEast;
        delete _southWest;
        delete _southEast;
    }

    // Subdivides the current node into four children.
    // This method is recursive and stops when leaf nodes reach a minimum size.
    void subdivide() {
        // Base case: If the node is already at or below the minimum size (2x2),
        // or if it has already been subdivided, stop.
        if (_width <= 15 || _height <= 15 || _northWest != nullptr) {
            std::cout << "Node " << _id << ": Cannot subdivide further (Size="
                      << _width << "x" << _height << "). Marking as leaf." << std::endl;
            return;
        }

        //std::cout << "Node " << _id << ": Subdividing..." << std::endl;

        // Calculate dimensions for children (scale factor 1/2)
        int childWidth = _width / 2;
        int childHeight = _height / 2;

        // Create the four child nodes and assign unique IDs
        _northWest = new QuadTree(Point(_pos.x, _pos.y), childWidth, childHeight);
        _northEast = new QuadTree(Point(_pos.x + childWidth, _pos.y), childWidth, childHeight); // Adjusted width for northEast
        _southWest = new QuadTree(Point(_pos.x, _pos.y + childHeight), childWidth, childHeight); // Adjusted height for southWest
        _southEast = new QuadTree(Point(_pos.x + childWidth, _pos.y + childHeight), childWidth, childHeight); // Adjusted dimensions for southEast

        // Important: If original width/height are not perfectly divisible by 2
        // (e.g., 5x5 divided by 2 gives 2x2 children, but leaves a 1 pixel gap).
        // For sizes that are not powers of 2 leading to 2x2, we need to ensure
        // the remaining space is covered.
        // For simplicity, sticking to exact halves as per "scale factor of 1/2".
        // If 100x100 -> 50x50, perfectly fine.
        // If 25x25 -> 12x12 children. The last subdivision might not reach exactly 2x2.
        // Example: 3 -> 1. Minimum 2x2 will stop here.

        // Recursively subdivide children
        _northWest->subdivide();
        _northEast->subdivide();
        _southWest->subdivide();
        _southEast->subdivide();
    }

    // Prints the QuadTree structure for visualization.
    void printTree(int depth = 0) const {
        std::string indent(depth * 4, ' '); // 4 spaces per depth level
        std::cout << indent << "Node ID: " << _id
                  << ", Pos: " << _pos.toString()
                  << ", Size: " << _width << "x" << _height;

        if (_northWest == nullptr) { // If no children, it's a leaf node
            std::cout << " (Leaf)" << std::endl;
        } else {
            std::cout << " (Internal Node)" << std::endl;
            _northWest->printTree(depth + 1);
            _northEast->printTree(depth + 1);
            _southWest->printTree(depth + 1);
            _southEast->printTree(depth + 1);
        }
    }

    /**
     * @brief Generates a Graphviz DOT language string to visualize the QuadTree.
     * @return A std::string containing the DOT code.
     */
    std::string toDotty() const {
        std::stringstream ss;
        ss << "digraph QuadTree {\n";
        ss << "  node [shape=box, style=filled, fillcolor=lightblue];\n"; // Styling for all nodes
        toDottyRecursive(ss);
        ss << "}\n";
        return ss.str();
    }

private:
    // Recursive helper function to generate DOT code for a node and its children.
    void toDottyRecursive(std::stringstream& ss) const {
        // Define the current node
        ss << "  node_" << _id << " [label=\""
           << "ID: " << _id << "\\n"
           << "Pos: " << _pos.toString() << "\\n"
           << "Size: " << _width << "x" << _height;

        if (_northWest == nullptr) {
            ss << "\\n(Leaf)\", fillcolor=lightgreen];\n"; // Different color for leaf nodes
        } else {
            ss << "\\n(Internal Node)\"];\n";
        }

        // Add edges to children and recursively call for children
        if (_northWest != nullptr) {
            ss << "  node_" << _id << " -> node_" << _northWest->_id << " [label=\"NW\"];\n";
            _northWest->toDottyRecursive(ss);
        }
        if (_northEast != nullptr) {
            ss << "  node_" << _id << " -> node_" << _northEast->_id << " [label=\"NE\"];\n";
            _northEast->toDottyRecursive(ss);
        }
        if (_southWest != nullptr) {
            ss << "  node_" << _id << " -> node_" << _southWest->_id << " [label=\"SW\"];\n";
            _southWest->toDottyRecursive(ss);
        }
        if (_southEast != nullptr) {
            ss << "  node_" << _id << " -> node_" << _southEast->_id << " [label=\"SE\"];\n";
            _southEast->toDottyRecursive(ss);
        }
    }
};

// Initialize the static member variable
int QuadTree::nextId = 0;

// --- Main Function ---
int main() {
    // Create the root of the QuadTree with initial height and width 100x100
    // The position (0,0) represents the top-left corner of the entire region.
    QuadTree* root = new QuadTree(Point(0, 0), 100, 100);

    // Start the subdivision process from the root
    root->subdivide();

    std::cout << "\n--- QuadTree Structure (Console Print) ---\n";
    //root->printTree();
    std::cout << "------------------------------------------\n";

    // Generate and print the Dotty code
    std::cout << "\n--- QuadTree Dotty Code (Graphviz) ---\n";
    std::string dotCode = root->toDotty();
    std::cout << dotCode;
    std::cout << "--------------------------------------\n";

    // To visualize this:
    // 1. Save the output of the "--- QuadTree Dotty Code ---" block to a file, e.g., quadtree.dot
    // 2. Install Graphviz (if you haven't already): https://graphviz.org/download/
    // 3. Run the dot command in your terminal:
    //    dot -Tpng quadtree.dot -o quadtree.png
    // 4. Open quadtree.png to see the visualization.


    // Clean up the memory allocated for the QuadTree
    // The destructor will recursively delete all child nodes.
    delete root;
    root = nullptr; // Good practice to nullify pointer after deletion

    return 0;
}
