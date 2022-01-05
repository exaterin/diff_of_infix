#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

vector <string> tokens;
bool terminated = false;
 
struct Node{
    string data;
    Node *left, *right;
    string evaluation;

    Node(string data){
        this->data = data;
        this->left = this->right = nullptr;
    };

    Node(string data, Node *left, Node *right){
        this->data = data;
        this->left = left;
        this->right = right;
    };
};

bool if_number(string str){
    bool first = true;
        int i = 0;
    for(auto it = str.cbegin(); it != str.cend(); ++it){
        if(isdigit(*it)) {}
        else 
            if(*it == '.' && first && (i != 0))
                first = false;
        else
            return false; 
        ++i;
    }
    return true;
}
 
bool is_operator(char c){ 
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int priority(const string &c){
    if(c == "*" || c == "/")
        return 2;
    if(c== "+" || c == "-")
        return 1;
    else
        return 0;
}

vector <string> infix_to_postfix(string line){
    vector<string> tokens;
    stringstream buffer(line);
    string token;
    
    while(buffer >> token)
        tokens.push_back(token);

    vector<string> outputList;
    stack<string> s;

    for(int i = 0; i < tokens.size(); ++i){
        if(if_number(tokens[i]) || tokens[i] == "x") 
            outputList.push_back(tokens[i]);

        if(tokens[i] == "(")
            s.push(tokens[i]);
      
        if(tokens[i] == ")"){
            while(!s.empty() && s.top() != "("){
                outputList.push_back(s.top());
                s.pop();
            }
            s.pop();
        }
        if(is_operator(tokens[i][0]) == true) {
            while(!s.empty() && priority(s.top()) >= priority(tokens[i])) {
                outputList.push_back(s.top());
                s.pop();
            }
            s.push(tokens[i]);
        }
    }
    while(!s.empty()){
        outputList.push_back(s.top());
        s.pop();
    }
    return outputList;
}

string string_without_zeros(double r){
    string str = std::to_string (r);
    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
    str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
    return str;
}

string simplify(string left, string op, string right){

    switch(op[0]){
        case '+':
            if(if_number(left) && if_number(right)){
                double r = stod(left) + stod(right);
                return string_without_zeros(r);
            } else if(if_number(left) && stod(left) == 0){ //0 + b = b
                    return right;
            } else if(if_number(right) && stod(right) == 0){ //b + 0 = b
                    return left;
            } else
                return "(" + left + " + " + right + ")";         
        break;

        case '-':
            if(if_number(left) && if_number(right)){
                double r = stod(left) - stod(right);
                return string_without_zeros(r);
            } else if(if_number(right) && stod(right) == 0){ //b - 0 = b
                    return left;
            } else
                return "(" + left + " - " + right + ")";         
        break;
        
        case '*':
            if(if_number(left) && if_number(right)){
                auto r = stod(left) * stod(right);
                return string_without_zeros(r);
            } else if(if_number(left) && stod(left) == 0){ //0 * b = 0
                    return "0";
            } else if(if_number(right) && stod(right) == 0){ //b * 0 = 0
                    return "0";
            } else if(if_number(left) && stod(left) == 1){ //1 * b = b
                    return right;
            } else if(if_number(right) && stod(right) == 1){ //b * 1 = b
                    return left;
            } else
                return "(" + left + " * " + right + ")";         
        break;
        
        case '/':
            if(if_number(left) && if_number(right)){
                auto r = stod(left) / stod(right);
                return string_without_zeros(r);
            } else if(if_number(left) && stod(left) == 0){ //0 / b = 0
                    return "0";
            } else if(if_number(right) && stod(right) == 1){ //b / 1 = b
                    return left;
            } else
                return "(" + left + " / " + right + ")";
    };
}

string evaluate(Node* root){
    if (root->left == nullptr && root->right == nullptr) 
        return root -> data;
    else 
        return (simplify(evaluate(root->left), root ->data, evaluate(root->right)));
}


string differentiate(Node* root){
    //if it is a leaf
    if(root -> left == nullptr && root -> right == nullptr)
        if (root->data == "x")
            return "1"; //x' = 1
        else
            return "0"; //c' = 0 (konstanta)

    else{
        string left_d = differentiate(root->left); string right_d = differentiate(root->right);
        string left_ev = evaluate(root->left); string right_ev = evaluate(root->right);

        switch (root->data[0]) {
            case '+': //(a + b)' = a' + b'
                return simplify(left_d, "+" ,right_d);
                break;

            case '-': //(a - b)' = a' - b'
                return simplify(left_d, "-" ,right_d);
                break;

            case '*': //(a * b)' = a' * b + a * b'
                return(simplify (simplify(left_d, "*",right_ev), "+", simplify(right_d, "*", left_ev)));
                break;
            
            case '/': //(a / b)' = (a' * b - a * b') / (b * b)
                return(simplify (simplify (simplify(left_d, "*",right_ev), "-", simplify(right_d, "*", left_ev) ) , "/", simplify(right_ev, "*", right_ev)));
                break;

            default:
                break;
        }
    }
}

Node* construct(vector <string> postfix){
    if (postfix.size() == 0) 
        return nullptr;

    stack<Node*> s;
    for (int i = 0; i < postfix.size(); ++i){
        if (is_operator(postfix[i][0])){
            if(s.size() < 2){
                cout << "Input error" << endl;
                terminated = true;
            } else {
                Node* x = s.top();
                s.pop();
                Node* y = s.top();
                s.pop();
                Node* node = new Node(postfix[i], y, x);
                s.push(node);
            }   
        } else
            s.push(new Node(postfix[i]));
    }
    return s.top();
}
 
bool correctness(string line){
    int p_left = 0;
    int p_rigth = 0;

    for (char c: line){
        if(!is_operator(c) && c != 'x' && c != ')' && c != '(' && !isdigit(c) && c != '.' && c != ' ')
            return false;
        if(c == '(')
            ++p_left;
        if(c == ')')
            ++p_rigth;
    }
    if(p_rigth != p_left)
        return false;
  
    return true;
}

int main(){
    string s;
    while(getline(cin,s)){
        terminated = false;
        if(!s.empty()){
            if(correctness(s)){
                terminated = false;
                vector <string> postfix = infix_to_postfix(s);;
                Node* root = construct(postfix);
                
                if(!terminated){
                    string diff = differentiate(root);
                    cout << diff << endl;
                }
            }
            else
                cout << "Input error" << endl;
        }
    }
    return 0;
}
