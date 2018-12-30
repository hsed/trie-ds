#include <iostream>
#include <string>
#include <stdexcept>
#include <vector> //only used for debugging
#include <fstream>

#include <assert.h>
#include <time.h>

using namespace std;

class radixTree {
    public:
        radixTree(bool debug = false){
            debugMode = debug;
            rootPtr = NULL;
            _createNode(rootPtr, "");
            
        }
        ~radixTree(){
            _deleteTree(rootPtr);

        }
        void addWord(string word){
            // function to add word to tree

            // first make to upper
            _strToUpperInPlace(word);
            _strRevInPlace(word);

            _addWordToTree(word, rootPtr, debugMode);
        }

        void printTree(){
            _printTree(rootPtr);
        }

        vector<pair<int, string>> getTreeNodesVect(){
            vector<pair<int, string>> resultVect;
            _getTreeNodesVect(rootPtr, 0, resultVect);

            return resultVect;
        }

        vector<string> getWordsVec(){
            vector<string> words;
            string buf; // initial buffer is empty
            
            _getWordsVec(rootPtr, buf, words);

            return words;
        }

        string findRhymingWord(string inputStr, int randSeed = -1){
            if (randSeed < 0){
                srand(time(NULL));
            }
            else {
                srand(randSeed);
            }
            //we have have 3 args pass by ref
            //first is initial input this is pruned as you loop-in recursively.
            //second is the pointer to currentParent this will end up being pointer to '$' node.
            //last items is all recovered words currently starting from root that either exactly
            //match or are chosen at random

            _strToUpperInPlace(inputStr);
            _strRevInPlace(inputStr);
            inputStr += '$';    //to signal end of strs

            if (rootPtr->children[inputStr[0] - 'A'] == NULL){
                //not in dictionary at all return empty string
                return "";
            }

            else {
                string finalStr = "";
                _findRhymingWord(inputStr, rootPtr, finalStr, debugMode);
                _strRevInPlace(finalStr); //reverse back
                return finalStr;
            }
        }


    private:
        struct node {
            string val;
            node** children; //note this is eventually a pointer to an array-of-pointers!
        };
        //each child elem then is a pointer to a node or to null
        typedef node* nodePtr;
        nodePtr rootPtr;

        static const uint8_t MAX_NODES = 27; //alphabet size + 1 for end-of-str
        bool debugMode = false;



        // create a new node pointed by childPtr (passed-by-ref) which must be null initially
        static void _createNode(nodePtr& childPtr, string val){
            if (childPtr != NULL) {
                throw runtime_error("A new node cannot be created for a non-null pointer.");
            }
            childPtr = new node;
            childPtr->val = val;
            
            //should be array of nodeptr to null of max_array_size 27
            // the brackets initialises to null pointers
            childPtr->children = new nodePtr [MAX_NODES]();
        }

        static void _createEndNode(nodePtr& childPtr, string val = "$"){
            if (childPtr != NULL) {
                throw runtime_error("A new node cannot be created for a non-null pointer.");
            }
            childPtr = new node;
            childPtr->val = val;
            
            //end ptr doesn't need any children
            childPtr->children = NULL;
        }


        // class method
        static void _strToUpperInPlace(string& str){
            auto beginPtr = str.begin();
            auto endPtr = str.end();

            while(beginPtr != endPtr){
                *beginPtr = toupper(*beginPtr);
                ++beginPtr;
            }
        }

        // class method
        static void _strRevInPlace(string& str){
            auto beginPtr = str.begin();
            auto lastPtr = str.end() - 1;

            //depending on even or odd chars the pointers can jump over
            // so we use 'less than' rather than 'not equal'
            while(beginPtr < lastPtr){
                auto tempChar = *beginPtr;
                *beginPtr = *lastPtr;
                *lastPtr = tempChar;
                ++beginPtr;
                --lastPtr;
            }
        }

        // recursive function to search and decide position to add word
        // pointers are passed by ref at each recursive call
        //string is const and by-ref pass to avoid copying
        static void _addWordToTree(const string& word, nodePtr& currentPtr, const bool debug = false){
            auto charIdx = word[0] - 'A';   //assume word is upper case

            if (currentPtr->children[charIdx] == NULL) {
                // for this case create children array all pointing to null
                // only one of them set to non null based on data
                // the root itself has empty string as value
  
                _createNode(currentPtr->children[charIdx], word);

                //create end node additional
                _createEndNode(currentPtr->children[charIdx]->children[MAX_NODES - 1]);

            }
            else {
                // start from one-step back to end iterator properly
                auto currIter = currentPtr->children[charIdx]->val.begin() - 1;
                auto newIter = word.begin() - 1;

                const auto currBeginIter = currentPtr->children[charIdx]->val.begin();
                const auto currIterEnd = currentPtr->children[charIdx]->val.end();
                const auto newIterEnd = word.end();

                //at least one char will be matched
                //increment iterators while matched chars
                //finds common prefix in newWord and stored word
                do {
                    ++currIter, ++newIter;
                } 
                while(currIter != currIterEnd && newIter != newIterEnd && *currIter == *newIter);
                //now both iterators are 1 char ahead of last matched chars
                

                //now append range of string to newStr -- parentWord
                //now we have two new childs to create
                //they will always be 1 or 2
                //note internally there is copying of chars here can use string view in future
                auto parentWord = string(currBeginIter, currIter);
                auto child1Word = string(currIter, currIterEnd);
                auto child2Word = string(newIter, word.end());


                if (debug) cout << "ParentWord: " << parentWord << "\tChildWords: "
                                                << child1Word << ", " << child2Word << endl;

                if (child1Word.empty() && !child2Word.empty()){
                    // tree item completely consumed so now we search in child
                    // we dont need to update parent value
                    //we only add word once
                    _addWordToTree(child2Word, currentPtr->children[charIdx], debug);
                }
                else if (!child1Word.empty() && child2Word.empty()){
                    //in this case we set parent to common form
                    //and also set its lastChild to '$' as common form is an end-word now
                    //and add remaining word to sub-tree

                    /// this is wrong we need to append whole end tree
                    //currentPtr->children[charIdx]->val = parentWord;

                    //_addWordToTree(child1Word, currentPtr->children[charIdx]);


                    currentPtr->children[charIdx]->val = parentWord;

                    nodePtr tmpNode = NULL;
                    _createNode(tmpNode, child1Word); //create tmpnode with child1
                    delete tmpNode->children; //save mem

                    //copy with dollar i.e. in this case the only non-null child is '$'
                    // which we copy over
                    tmpNode->children = currentPtr->children[charIdx]->children;
                    
                    //create new null children
                    currentPtr->children[charIdx]->children = new nodePtr [MAX_NODES]();

                    //add end char as this is also a word
                    _createEndNode(currentPtr->children[charIdx]->children[MAX_NODES - 1]);
                    
                    //append all nested children to parent's child index by first char of new child
                    currentPtr->children[charIdx]->children[child1Word[0] - 'A'] = tmpNode;
                }
                else if (child1Word.empty() && child2Word.empty()){
                    //we need to catch for special case where string ends e.g. eating; eaten and eat.
                    //need an additional char for that may be?

                    //do this directly
                    //dollars are only added for subwords that are added to dict
                    //otherwise they are not needed or used

                    //only createNode if end symbol doesn't already exist
                    if (currentPtr->children[charIdx]->children[MAX_NODES - 1] == NULL){
                        _createEndNode(currentPtr->children[charIdx]->children[MAX_NODES - 1]);
                    }

                    //throw runtime_error("This is a special case, not yet implemented eating;eaten ->eat");
                }
                else {
                    currentPtr->children[charIdx]->val = parentWord;

                    nodePtr tmpNode = NULL;
                    _createNode(tmpNode, child1Word);
                    delete tmpNode->children; //save mem

                    //copy with dollar
                    tmpNode->children = currentPtr->children[charIdx]->children;
                    
                    currentPtr->children[charIdx]->children = new nodePtr [MAX_NODES]();
                    currentPtr->children[charIdx]->children[child1Word[0] - 'A'] = tmpNode;
                    
                    // usually these are different so shuld be fine
                    _addWordToTree(child2Word, currentPtr->children[charIdx], debug);
                }

               

                
                
                //throw runtime_error("Not Implemented");

                //recursively call function to create node
            }
        }


        static void _findRhymingWord(string& inputWord, nodePtr& currPtr, string& finalWord, const bool debug = false){
            //cases:
            /*
                first char not found in root's children => return none -- this is handled in higher order fun

                distinction: if inputWord == "" then do random searches

                few chars match => 
                    - add matching chars as finalWord
                    - recurse with remaining chars in input and further depth
                    - search for remain substr as index, if '$' as input then search for 
                      '$' in current children
                    - if not found... repeat current search DO NOT CHANGE DEPTH but 
                      make input emptyString i.e. ""
                    
            */
        
            if (inputWord.empty()){
                //do randomised searches until you reach currPtr val as '$'

                if (currPtr->val[0] != '$'){
                    //do stuff when not $, at $ it automatically stops
                    //throw runtime_error("Randomised searching not implemented");


                    vector<uint8_t> nonNullChildIdx;
                    for (auto i = 0; i < MAX_NODES; i++){
                        if (currPtr->children[i] != NULL) {
                            nonNullChildIdx.push_back(i);
                        }
                    }
                    //there has to be atleast one non-null child,
                    //if no A-Z char then it'll be the '$' symbol
                    //randomly choose int between {0,..,N-1} (inclusive) for N non-null children
                    //use that to select random child
                    auto childNode = currPtr->children[nonNullChildIdx[(rand() % (nonNullChildIdx.size()))]];

                    finalWord += (childNode->val[0] != '$' ? childNode->val : ""); //only append if non-end-str
                    if (debug) cout << "randFinalWord: " << finalWord << endl;
                    _findRhymingWord(inputWord, childNode, finalWord, debug);
                }
            }
            else {
                //input word is non-empty, search by first char
                auto charIdx = inputWord[0] - 'A';
                auto childNode = currPtr->children[charIdx]; //pointer to matched child node

                if (charIdx + 'A' == '$') {
                    // catch this case then check if '$' exists in child
                    //need to keep track of special case when input word[0] is '$'
                    //for this just search if the lastChild is nonNull
                    
                    //here we make an assumption that even if its a full match i.e.
                    //pad in dictionary and value of current node and also pad is searchstring
                    //we still look for other random alternatives because all of them will still
                    //have the same number of matching chars.
                    inputWord = "";
                    _findRhymingWord(inputWord, currPtr, finalWord, debug);

                    //in other case all is well simply exit recursion
                    // because it will not hit any other if statement
                }
                else if (childNode == NULL) {
                    //repeat call exactly with input removed for randomisation
                    inputWord = "";
                    _findRhymingWord(inputWord, currPtr, finalWord, debug);
                }
                else {
                    //we can do some matching

                    // start from one-step back to end iterator properly
                    //curr -> in dict ; new -> word searched
                    auto currIter = childNode->val.begin() - 1;
                    auto newIter = inputWord.begin() - 1;

                    const auto currIterEnd = childNode->val.end();
                    const auto newIterBegin = inputWord.begin();
                    const auto newIterEnd = inputWord.end();

                    do {
                        ++currIter, ++newIter;
                    } 
                    while(currIter != currIterEnd && newIter != newIterEnd && *currIter == *newIter);

                    auto matchedChars = string(newIterBegin, newIter);
                    auto remChars = string(newIter, inputWord.end());


                    inputWord = remChars;
                    finalWord += childNode->val;//this sshuld be full label of node to avoid partial wors

                    if (debug) cout << "matchedChars: " << matchedChars << "\tremChars: " << remChars << "\tfinalWord: " << finalWord << endl;
                    _findRhymingWord(inputWord, childNode, finalWord, debug);

                }
            }
        
        }



        static void _getTreeNodesVect(const nodePtr& currPtr, int currDepth, vector<pair<int, string>>& vect){ 
            vect.push_back(pair<int, string>(currDepth, currPtr->val));
            
            // A dollar node is a special type with no children at all, no even null array
            // for this we will can't loop so be careful!
            if (currPtr->val != "$"){
                for (auto i = 0 ; i < MAX_NODES; i++){
                    if (currPtr->children[i] != NULL){
                        _getTreeNodesVect(currPtr->children[i], currDepth+1, vect);
                    }
                    
                }
            }
        }

        // default depth is 1 because anything meaningful is always allocated to children
        static void _printTree(const nodePtr currPtr, int currDepth = 0){
            vector<pair<int, string>> resultVect;
            _getTreeNodesVect(currPtr, currDepth, resultVect);

            auto prevDepth = 0;

            for(const pair<int, string>& val : resultVect)
            {
                if (val.first > prevDepth) { cout << "\n"; }
                else if (val.first < prevDepth) { cout << "\n\n"; }
                cout << "@" << val.first << ": " << val.second << "; ";

                prevDepth = val.first;
            }
            
            
        }

        //assume buffer already has empty string
        static void _getWordsVec(const nodePtr& currPtr, string& buffer, vector<string>& wordsVec){
            
            if (currPtr->val[0] == '$'){
                // one of the children is '$' (end char)
                // we can directly compare first char here so only single comparison
                // this is base case
                //because of pass_by_ref, direct reverse in giving incorrect results
                //when recursive loop is unrolled
                string tmp = buffer; 
                _strRevInPlace(tmp); //reverse to store in correct order for better printing
                wordsVec.push_back(tmp);
            }
            
            else {
                // for every node if child exists traverse through
                for (auto i = 0 ; i < MAX_NODES; i++){
                    if (currPtr->children[i] != NULL){
                        //need to traverse tree here so store parent value
                        buffer.append(currPtr->val);
                        _getWordsVec(currPtr->children[i], buffer, wordsVec);

                        //make sure to remove str when you go up the tree
                        //first arg is startPos; secondArg is noOfChars
                        buffer.erase(buffer.length() - currPtr->val.length(), currPtr->val.length());
                    }
                    
                }
            }
        }

        
        static void _deleteTree(nodePtr& currPtr){
            // in base case (end-node) this is always false
            if (currPtr->val[0] != '$') {
                // for every node if child exists traverse through
                for (auto i = 0 ; i < MAX_NODES; i++){
                    if (currPtr->children[i] != NULL){
                        //need to traverse tree until end node reached
                        _deleteTree(currPtr->children[i]);
                    }
                    
                }
            }

            //all children (if existed) now deleted must now delete itself
            delete currPtr;
        }


        
};