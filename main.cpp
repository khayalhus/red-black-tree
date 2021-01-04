#include <fstream> // ifstream
#include <iostream> // cout, cerr
#include <string> // getline, stoi

using namespace std;

// since there was a constraint of using only a single file, here are the order of functions:
// struct declerations and their constructor/destructor, 

struct Tops {
    int * stats; // holds max_points, max_assists, max_rebounds
    string * holders; // holds the corresponding players' names

    Tops();
    ~Tops();

    void checkTops(struct Node * node); // checks new/updated player's stats
};

Tops::Tops() {
    // dynamically allocate for 3 top stats on construction
    stats = new int[3];
    holders = new string[3];
}

Tops::~Tops() {
    // free allocated memory on destruction
    delete [] stats;
    delete [] holders;
}

struct Node {
    string season;
    string full_name;
    string team;
    int rebound;
    int assist;
    int point;

    struct Node* parent; // parent node
    struct Node* left; // left child
    struct Node* right; // right child
    string color; // red or black

    Node();
    ~Node() {}
};

Node::Node() {
    this->parent = NULL;
    this->left = NULL;
    this->right = NULL;
    this->color = "RED"; // newly inserted node color assigned red
}

struct RBT {
    struct Node* head; // points to root of the red-black tree

    RBT() { this->head = NULL; }
    ~RBT();

    struct Node* find(string full_name); // traverses tree and finds
    void insert(struct Node* newnode, Tops* maxes); // traverses tree and inserts or updates
    void print(string current_season, Tops* maxes, ofstream& outfile); // prints seasonal data
    void print_rec(string current_season, struct Node* node, int height, ofstream& outfile); // prints red-black tree's nodes that have latest season 
    void remove_all(struct Node* node); // recursively removes all nodes
};

RBT::~RBT() {
    remove_all(this->head);
}

void Tops::checkTops(struct Node * newnode) {
    // checks newly added/updated player's stats against all time top stats
    if (this->stats[0] < newnode->point) {
        this->stats[0] = newnode->point;
        this->holders[0] = newnode->full_name;
    }
    if (this->stats[1] < newnode->assist) {
        this->stats[1] = newnode->assist;
        this->holders[1] = newnode->full_name;
    }
    if (this->stats[2] < newnode->rebound) {
        this->stats[2] = newnode->rebound;
        this->holders[2] = newnode->full_name;
    }
    return;
}

void RBT::remove_all(struct Node* node) {
    // recursively removes all nodes from the red-black tree
    if (node == NULL) {
        return;
    }
    remove_all(node->left);
    remove_all(node->right);
    delete node;
}

void RBT::insert(struct Node* newnode, Tops* maxes) {
    // traverses tree and inserts or updates
    if (this->head == NULL) {
        // insert as root
        head = newnode;
        newnode->color = "BLACK"; // color of head node should be black
        maxes->checkTops(newnode);
        return;
    }
    struct Node* traverse = head;
    struct Node* tail = NULL;
    int comparison = newnode->full_name.compare(traverse->full_name);
    while (traverse != NULL) {
        comparison = newnode->full_name.compare(traverse->full_name);
        if (comparison < 0) { // new player full name is alphabetically smaller than traverse node
            // traverse left child
            tail = traverse;
            traverse = traverse->left;
        }
        else if (comparison > 0) { // new player full name is alphabetically bigger than traverse node
            // traverse right child
            tail = traverse;
            traverse = traverse->right;
        }
        else { // player already exists
            // copy and update data
            traverse->season = newnode->season;
            traverse->full_name = newnode->full_name;
            traverse->team = newnode->team;
            traverse->rebound += newnode->rebound;
            traverse->assist += newnode->assist;
            traverse->point += newnode->point;
            maxes->checkTops(traverse);
            delete newnode; // free newnode
            return;
        }
    }
    if (comparison < 0) {
        // add as left child of a node
        tail->left = newnode;
        newnode->parent = tail;
    }
    else if (comparison > 1) {
        // add as right child of a node
        tail->right = newnode;
        newnode->parent = tail;
    }
    maxes->checkTops(newnode);
    return;
}

void RBT::print(string current_season, Tops* maxes, ofstream& outfile) {
    // prints seasonal data and player full names
    outfile << "End of the " << current_season << " Season" << '\n';
    outfile << "Max Points " << maxes->stats[0] << " - Player Name: " << maxes->holders[0] << '\n';
    outfile << "Max Assists " << maxes->stats[1] << " - Player Name: " << maxes->holders[1] << '\n';
    outfile << "Max Rebs " << maxes->stats[2] << " - Player Name: " << maxes->holders[2] << '\n';
    print_rec(current_season, this->head, 0, outfile);
}

void RBT::print_rec(string current_season, struct Node* node, int height, ofstream& outfile) {
    // prints red-black tree's nodes that have latest season 
    if (node == NULL) {
        return;
    }
    if (node->season.compare(current_season) == 0) {
        for (int i = 0; i < height; i++) {
            outfile << "-";
        }
        outfile << "(" << node->color << ") " << node->full_name << '\n';
    }
    print_rec(current_season, node->left, height+1, outfile); // traverse left node
    print_rec(current_season, node->right, height+1, outfile); // traverse right noded
}



int main(int argc, char** argv) {

    string file_name;

    if (argc > 1) {
        file_name = argv[1]; // get number N from console argument (./a.out N)
    }
    else {
        file_name = "euroleague.csv";
        cout << "File name was not specified as an argument" << endl;
        cout << "Automatically setting file name as euroleague.csv" << endl;
    }

    ifstream infile;
    infile.open(file_name);

    ofstream outfile;
    outfile.open("output.txt");

    if (!infile) {
        cerr << "File cannot be opened!" << endl;
        exit(1);
    }

    string input;

    getline(infile, input); // reading the header line
    string header = input;

    RBT* tree = new RBT; // holds red-black tree
    string current_season = ""; // holds current season value
    Tops* maxes = new Tops; // holds top stats

    while (infile.peek() != EOF) {
        Node* newnode = new Node;
        
        getline(infile, input, ',');
        newnode->season = input;
        
        if (current_season.compare("") == 0) {
            // initialize season value
            current_season = input;
        }
        else if (current_season.compare(input) != 0) {
            // if season value changed print
            tree->print(current_season, maxes, outfile);
            current_season = input;
        }

        getline(infile, input, ',');
        newnode->full_name = input;
        getline(infile, input, ',');
        newnode->team = input;
        getline(infile, input, ',');
        newnode->rebound = stoi(input);
        getline(infile, input, ',');
        newnode->assist = stoi(input);
        getline(infile, input, '\n');
        newnode->point = stoi(input);

        tree->insert(newnode, maxes); // add or update player
    }
    tree->print(current_season, maxes, outfile); // print last season

    infile.close();
    outfile.close();

    delete maxes;
    delete tree;

}