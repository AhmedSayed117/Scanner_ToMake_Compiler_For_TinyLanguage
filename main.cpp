#include "bits/stdc++.h"
#include "iostream"
using namespace std;
#define endl "\n"
// Strings /////////////////////////////////////////////////////////////////////////
bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

////////////////////////////////////////////////////////////////////////////////////
// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;
    char line_buf[MAX_LINE_LENGTH]{};
    int cur_ind, cur_line_size;

    explicit InFile(const char* str)
    {
        file=nullptr;
        if(str) file=fopen(str, "r");
        cur_line_size=0; cur_ind=0; cur_line_num=0;
    }
    ~InFile()
    {
        if(file) fclose(file);
    }
    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }
    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}
            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }
    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }
    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {
            if(!GetNewLine())
                return nullptr;
            SkipSpaces();
        }
        return &line_buf[cur_ind];
    }
    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    explicit OutFile(const char* str)
    {
        file=nullptr;
        if(str) file=fopen(str, "w");
    }
    ~OutFile()
    {
        if(file) fclose(file);
    }
    void Out(const char* s) const
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};
////////////////////////////////////////////////////////////////////////////////////
// Scanner /////////////////////////////////////////////////////////////////////////
InFile inFile("input.txt");
OutFile outFile("output.txt");


#define MAX_TOKEN_LEN 40
enum TokenType{
            IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
            ASSIGN, EQUAL, LESS_THAN,
            PLUS, MINUS, TIMES, DIVIDE, POWER,
            SEMI_COLON,
            LEFT_PAREN, RIGHT_PAREN,
            LEFT_BRACE, RIGHT_BRACE,
            ID, NUM,
            ENDFILE, ERROR
};

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
        {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "Num",
                "EndFile", "Error"
        };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1]{};

    Token()
    {
        str[0]=0;
        type=ERROR;
    }
    Token(TokenType _type, const char* _str)
    {
        type=_type;
        Copy(str, _str);
    }
};

const Token reserved_words[]=
{
        Token(IF, "if"),
        Token(THEN, "then"),
        Token(ELSE, "else"),
        Token(END, "end"),
        Token(REPEAT, "repeat"),
        Token(UNTIL, "until"),
        Token(READ, "read"),
        Token(WRITE, "write")
};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
        Token(ASSIGN, ":="),
        Token(EQUAL, "="),
        Token(LESS_THAN, "<"),
        Token(PLUS, "+"),
        Token(MINUS, "-"),
        Token(TIMES, "*"),
        Token(DIVIDE, "/"),
        Token(POWER, "^"),
        Token(SEMI_COLON, ";"),
        Token(LEFT_PAREN, "("),
        Token(RIGHT_PAREN, ")"),
        Token(LEFT_BRACE, "{"),
        Token(RIGHT_BRACE, "}")
};
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}



void writeonfile(const string& str,const string& Token){
    char *pr = new char [100];

    string s = "[";s+= to_string(inFile.cur_line_num);s+="] ";
    s+=str;s+=" ";s+="(" ;s+=Token;s+=")";

    Copy(pr, s.c_str());
    outFile.Out(pr);
    delete[] pr;
}

void checkAssignmentOperator(deque<char>&d){
    if (d.front()=='<'){
        writeonfile(symbolic_tokens[2].str,TokenTypeStr[symbolic_tokens[2].type]);
        d.pop_front();
    }
    else if (d.front()=='='){
        writeonfile(symbolic_tokens[1].str,TokenTypeStr[symbolic_tokens[1].type]);
        d.pop_front();
    }
    else if (d.front()=='*'){
        writeonfile(symbolic_tokens[5].str,TokenTypeStr[symbolic_tokens[5].type]);
        d.pop_front();
    }
    else if (d.front()=='-'){
        writeonfile(symbolic_tokens[4].str,TokenTypeStr[symbolic_tokens[4].type]);
        d.pop_front();
    }else if (d.front()=='+'){
        writeonfile(symbolic_tokens[3].str,TokenTypeStr[symbolic_tokens[3].type]);
        d.pop_front();
    }else if (d.front()=='/'){
        writeonfile(symbolic_tokens[6].str ,TokenTypeStr[symbolic_tokens[6].type]);
        d.pop_front();
    }else if (d.front()=='^'){
        writeonfile(symbolic_tokens[7].str,TokenTypeStr[symbolic_tokens[7].type]);
        d.pop_front();
    }
}//+ * - / ^ < > =

void validateid(const string& str){//Num ID
    string letter,number;

    deque<char>d;
    for (auto i:str) {
        if (i!=' ' && (IsDigit(i) || IsLetterOrUnderscore(i)))d.push_back(i);
        if (i=='<' )d.push_back(i);
        else if (i=='>' )d.push_back(i);
        else if (i=='=' )d.push_back(i);
        else if (i=='*' )d.push_back(i);
        else if (i=='-' )d.push_back(i);
        else if (i=='+' )d.push_back(i);
        else if (i=='/' )d.push_back(i);
        else if (i=='^' )d.push_back(i);
     }
    let:
    while (!d.empty()){
        if((d.front()=='>' || d.front()=='<' || d.front()=='=' || d.front()=='^' || d.front()=='/' || d.front()=='*'|| d.front()=='-'|| d.front()=='+')){
            if (!letter.empty()){
                writeonfile(letter,TokenTypeStr[ID]);//ID
                letter="";
            }
            checkAssignmentOperator(d);
            letter="";
        }

        if (IsLetterOrUnderscore(d.front())){
            letter+=d.front();
            d.pop_front();
        }else{
            if (!letter.empty()){
                writeonfile(letter,TokenTypeStr[ID]);//ID
                letter="";
            }
            goto num;
        }
        if (d.empty()){
            writeonfile(letter,TokenTypeStr[ID]);//ID
            letter="";
        }
    }
    num:
    while (!d.empty()){
        if((d.front()=='>' || d.front()=='<' || d.front()=='=' || d.front()=='^' || d.front()=='/' || d.front()=='*'|| d.front()=='-'|| d.front()=='+')){
            if (!number.empty()){
                writeonfile(number,TokenTypeStr[NUM]);//number
                number="";
            }
            checkAssignmentOperator(d);
            number="";
        }

        if (IsDigit(d.front())){
            number+=d.front();
            d.pop_front();
        }else{
            if (!number.empty()){
                writeonfile(number,TokenTypeStr[NUM]);//number
                number="";
            }
            goto let;
        }
        if (d.empty()){
            writeonfile(number,TokenTypeStr[NUM]);//number
            number="";
            goto let;
        }
    }
}
bool error(char c,char c2){
    if ((c!=' ' && (!IsLetterOrUnderscore(c) && !IsDigit(c))) && ( c!='>' && c!=';' && c!='<' && c!='=' && c!='^' && c!='/' && c!='*'&& c!='-'&& c!='+')){
        if (c==':' && c2=='=')return false;
        writeonfile(to_string(c),TokenTypeStr[ERROR]);//error
        return true;
    }
    return false;
}

void analysis(string str)
{
    string tmp;
    bool isRead = false;
    bool isIf = false;
    bool isWrite = false;
    bool isAssign = false;
    bool isUntil = false;

    cout<<str.size()-2<<endl;
    for (int i=0;i<=str.size()-2;i++) {
        //not contain spaces
        if (str[i]!=' ') tmp+=str[i];
        else continue;

        //comment
        if (str[i]=='{'){
            writeonfile(symbolic_tokens[11].str,TokenTypeStr[symbolic_tokens[11].type]);
            inFile.SkipUpto("}");
            writeonfile(symbolic_tokens[12].str,TokenTypeStr[symbolic_tokens[12].type]);
            tmp="";
            break;
        }
        if (str[i]=='('){
            str[i]=' ';
            writeonfile(symbolic_tokens[9].str,TokenTypeStr[symbolic_tokens[9].type]);
            continue;
        }if (str[i]==')'){
            str[i]=' ';
            writeonfile(symbolic_tokens[10].str,TokenTypeStr[symbolic_tokens[10].type]);
            continue;
        }
        //if condition
        if (tmp=="if" && ((str[i+1]==' ') || (!IsDigit(str[i+1]) && !IsLetterOrUnderscore(str[i+1]))  )    ){
            writeonfile(reserved_words[0].str,TokenTypeStr[reserved_words[0].type]);
            tmp="";
            isIf = true;
            continue;
        }
        //
        //after x
        if (isIf && !tmp.empty() && str[i+1]!=' '){
            continue;
        }

        if (isIf && !tmp.empty()){
            isIf= false;

            validateid(tmp);

            tmp="";
            continue;
        }
        if (tmp=="then"){
            writeonfile(reserved_words[1].str,TokenTypeStr[reserved_words[1].type]);//then
            tmp="";
            continue;
        }
        if (tmp=="until"){
            writeonfile(reserved_words[5].str,TokenTypeStr[reserved_words[5].type]);//until
            tmp="";
            isUntil= true;
            continue;
        }
        if (isUntil && tmp.find(';')!=string::npos){

            validateid(tmp.erase(tmp.size()-1));

            writeonfile(symbolic_tokens[8].str,TokenTypeStr[symbolic_tokens[8].type]);//;

            tmp="";
            isUntil= false;
            continue;
        }

        //read input
        if (tmp=="read" && ((str[i+1]==' ') || (!IsDigit(str[i+1]) && !IsLetterOrUnderscore(str[i+1]))  )  ){
            writeonfile(reserved_words[6].str,TokenTypeStr[reserved_words[6].type]);//read
            tmp="";
            isRead = true;
            continue;
        }
        //validate id

        if (isRead && !tmp.empty() && tmp.find(';')==string::npos){
            continue;
        }
        //id

        if (isRead && !tmp.empty() && tmp.find(';')!=string::npos){
            isRead= false;

            validateid(tmp.erase(tmp.size()-1));

            //semi column
            writeonfile(symbolic_tokens[8].str,TokenTypeStr[symbolic_tokens[8].type]);//;
            tmp="";
            continue;
        }

        //write file
        if (tmp=="write" && ((str[i+1]==' ') || (!IsDigit(str[i+1]) && !IsLetterOrUnderscore(str[i+1]))  )  ){
            writeonfile(reserved_words[7].str,TokenTypeStr[reserved_words[7].type]);//write
            tmp="";
            isWrite= true;
            continue;
        }

        if (tmp=="repeat" && ((str[i+1]==' ') || (!IsDigit(str[i+1]) && !IsLetterOrUnderscore(str[i+1]))  )  ){
            writeonfile(reserved_words[4].str,TokenTypeStr[reserved_words[4].type]);//repeat
            tmp="";
            continue;
        }

        //semi column
        if (tmp==";"){
            writeonfile(symbolic_tokens[8].str,TokenTypeStr[symbolic_tokens[8].type]);//;
            tmp="";
            continue;
        }

        if(tmp.find(":=")!=string::npos){
            writeonfile(tmp.erase(tmp.size()-2),TokenTypeStr[21]);//ID

            writeonfile(symbolic_tokens[0].str,TokenTypeStr[symbolic_tokens[0].type]);//assignment
            isAssign= true;
            tmp="";
            continue;
        }

        if (isWrite && tmp.find(';')!=string::npos){
            validateid(tmp.erase(tmp.size()-1));
            tmp="";
            isWrite= false;

            //semi column
            writeonfile(symbolic_tokens[8].str,TokenTypeStr[symbolic_tokens[8].type]);//;

            continue;
        }
        if (isAssign && tmp.find(';')!=string::npos){
            validateid(tmp.erase(tmp.size()-1));
            tmp="";
            isAssign= false;

            //semi column
            writeonfile(symbolic_tokens[8].str,TokenTypeStr[symbolic_tokens[8].type]);//;
            continue;
        }
        if (tmp=="end")
        {
            writeonfile(tmp,TokenTypeStr[3]);

            tmp="";
            continue;
        }
        //error
//        if (i>0 && str[i-1]==' ' && str[i+1]==' ' && error(str[i],str[i+1])){
//
//        }
    }
    cout<<str;
}

int main(){
    while (inFile.GetNewLine()){
        analysis(inFile.GetNextTokenStr());
    }
    writeonfile(TokenTypeStr[23],TokenTypeStr[ENDFILE]);//EndOfFile
    return 0;
}