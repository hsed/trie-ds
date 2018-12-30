#include "radixTree.h"


/*
    A trie structure can include a number of children
    It is a good choice to keep this fixed size and simply init to none
    This way even though memory will be huge as every node will require ALPHABET_SIZE nodes connected
    Thus memory will be ALPHABET_SIZE*
*/

void runSimpleTest(const string dictFile, const string resultFile, const string treeFile){
    radixTree radTree = radixTree();
    ifstream fs;
    string word;
    int depth;
    vector<string> finalWords;
    vector<pair<int, string>> treeVectView;

    //read dict (input) file
    fs.open(dictFile.c_str());
    if (!fs.is_open()) {
		cout << endl << "dict file not found." << endl;
	}
    while(fs >> word){
        radTree.addWord(word);
    }
    fs.close();

    //read results file
    fs.open(resultFile.c_str());
    if (!fs.is_open()) {
		cout << endl << "result file not found." << endl;
	}
    while(fs >> word){
        finalWords.push_back(word);
    }
    fs.close();

    //read tree file
    fs.open(treeFile.c_str());
    if (!fs.is_open()) {
		cout << endl << "tree file not found." << endl;
	}
    fs >> depth; //get first depth
    treeVectView.push_back(pair<int, string>(depth, "")); //test root is ""
    while(fs >> depth >> word){
        treeVectView.push_back(pair<int, string>(depth, word));
    }
    fs.close();

    
    assert(finalWords == radTree.getWordsVec());
    assert(treeVectView == radTree.getTreeNodesVect());

    //debugging -- comment otherwise
    // cout << "Tree Layout: " << endl;
    // for (auto& elem : radTree.getTreeNodesVect()){
    //     cout << elem.first << " " << elem.second << endl;
    // }
    // cout << "\nWords: " << endl;
    // for (auto& word : radTree.getWordsVec()){
    //     cout << word << "; " << endl;
    // }
    // cout << "\nTrue: " << endl;
    // for (auto& word : finalWords){
    //     cout << word << "; " << endl;
    // }

}



int main(){
    ifstream fs;
    string word;
    string fname = "dict.txt"; //make cmd arg

    cout << "Diff between A & G: " << int('G' - 'A') << endl;

    // make this optional later
    runSimpleTest("test_dict.txt", "test_result.txt", "test_tree.txt");

    radixTree radTree = radixTree();

    //read dict (input) file
    fs.open(fname.c_str());
    if (!fs.is_open()) {
		cout << endl << "dict file not found." << endl;
	}
    while(fs >> word){
        radTree.addWord(word);
    }
    fs.close();

    // radTree.printTree();

    cout << "\n\nWordsInDict: " << endl;
    for (auto& word : radTree.getWordsVec()){
        cout << word << "; " << endl;
    }

    //radTree.findRhymingWord("apd")

    string searchStr;
    cout << "Enter word (0 to exit): ";
    cin >> searchStr;
    while (searchStr != "0"){
        cout << "\nRhyming Match: " << radTree.findRhymingWord(searchStr) << endl;
        cout << "Enter word (0 to exit): ";
        cin >> searchStr;
    }
    
    

    return 0;
}