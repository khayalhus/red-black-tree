#include <fstream>
#include <iostream>
#include <string>

using namespace std;

struct Node {
    string season;
    string full_name;
    string team;
    int rebound;
    int assist;
    int point;

    struct Node * parent;
    struct Node * left;
    struct Node * right;

    Node();
    ~Node() {}
};

Node::Node() {

}

int main(int argc, char** argv) {

    string file_name;

	if (argc > 1) {
		file_name = argv[1]; // get number N from console argument (./a.out N)
	} else {
		file_name = "euroleague.csv";
		cout << "File name was not specified as an argument" << endl;
		cout << "Automatically setting file name as euroleague.csv" << endl;
	}
	
	ifstream file;
	file.open(file_name);
	
	if (!file){
		cerr << "File cannot be opened!";
		exit(1);
	}

	string input;
    
	getline(file, input); // reading the header line

	string header = input;

    Node * head;
    
    while(file.peek() != EOF) {
        Node * newnode = new Node;
        getline(file, input, ',');
        newnode->season = input;
        getline(file, input, ',');
        newnode->full_name = input;
        getline(file, input, ',');
        newnode->team = input;
        getline(file, input, ',');
        newnode->rebound = stoi(input);
        getline(file, input, ',');
        newnode->assist = stoi(input);
        getline(file, input, '\n');
        newnode->point = stoi(input);
        getline(file, input, '\n'); //this is for reading the \n character into dummy variable.
    }
}