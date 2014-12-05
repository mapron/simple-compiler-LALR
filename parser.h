#ifndef PARSER_H
#define PARSER_H
#include "stdafx.h"
//using namespace std;



class token
{
public:
    int type;
    int id;
    token() {type=0;id=0;}
    operator bool () {return type>0;}
    bool isTerminal();
    bool isBase();
    QByteArray dump() ;
};
typedef std::deque<token> tokenList;

class parser
{
    QByteArray data;
    int position;
public:
    QByteArray debug;
    parser();
    tokenList parse(QByteArray in);
    token nextToken();


};

struct VarTableRecord
{
    QString name;
    int type;
    bool declarated;
    VarTableRecord()
    {
       name="";type=0;declarated=false;
    }
};

class VarTable
{
   public:
  std::deque<VarTableRecord> list;

  int add(QString name);
  VarTable() {VarTableRecord a;a.name="";a.type=0;a.declarated=false;list.push_back(a);}
  VarTableRecord* get(int id){return &list[id];}
  QString dump(){
      QString rt="";
      for (unsigned int i=0;i<list.size();i++){
          QString dc=list[i].declarated?"declarated": "NOT declarated";
          rt+=QString("%1 %2 %3\r\n").arg(i).arg(list[i].name).arg(dc);
      }
      return rt;
  }
};


struct ConstTableRecord
{
    QString name;
    int type;
};

class ConstTable
{
  std::deque<ConstTableRecord> list;
  public:
  int add(QString value);
  ConstTableRecord* get(int id){return &list[id];}
  ConstTable() {ConstTableRecord a;a.name="";a.type=0;list.push_back(a);}
};


enum tokenTypes {
    // базовые
    TOKEN_IDENT=1,
    TOKEN_NUMBER,
    TOKEN_STRING, // string
    TOKEN_SEMICOLON, // ;

    // типы
     TOKEN_TYPE_INT,   // int
     TOKEN_TYPE_FLOAT, // float    

    // ключевые слова
    TOKEN_KEYWORD_PRINT, // print
    TOKEN_KEYWORD_SCAN,  // scan
    TOKEN_KEYWORD_WAIT,  // wait
    TOKEN_KEYWORD_IF,    // if
    TOKEN_KEYWORD_ELSE,  // else
    TOKEN_KEYWORD_WHILE, // while
    TOKEN_KEYWORD_PRINTS, // prints

    //операторы
    TOKEN_OPERATOR_SKOBKA_F_L, // {
    TOKEN_OPERATOR_SKOBKA_F_R, // }
    TOKEN_OPERATOR_SKOBKA_R_L, // (
    TOKEN_OPERATOR_SKOBKA_R_R, // )
    TOKEN_OPERATOR_SKOBKA_K_L, // [
    TOKEN_OPERATOR_SKOBKA_K_R, // ]

    TOKEN_OPERATOR_INCREMENT, // ++
    TOKEN_OPERATOR_DECREMENT, // --
    TOKEN_OPERATOR_COMMA, // ,

    TOKEN_OPERATOR_PLUS, //+
    TOKEN_OPERATOR_MINUS, // -
    TOKEN_OPERATOR_EXCLAMATION, // !

    TOKEN_OPERATOR_MULT, // *
    TOKEN_OPERATOR_DIVIDE, // /
    TOKEN_OPERATOR_REMAIN, // %

    TOKEN_OPERATOR_SHIFTLEFT, // <<
    TOKEN_OPERATOR_SHIFTRIGHT, // >>

    TOKEN_OPERATOR_LOWER, // <
    TOKEN_OPERATOR_GREATER , // >
    TOKEN_OPERATOR_LOWEREQUAL, // <=
    TOKEN_OPERATOR_GREATEREQUAL, // >=

    TOKEN_OPERATOR_EQUAL, // ==
    TOKEN_OPERATOR_NOTEQUAL, // !=

    TOKEN_OPERATOR_AND, // &
    TOKEN_OPERATOR_XOR, // ^
    TOKEN_OPERATOR_OR, // |

    TOKEN_OPERATOR_AND_LOGICAL, // &&
    TOKEN_OPERATOR_OR_LOGICAL, // ||
    TOKEN_OPERATOR_ASSIGN, // =
    TOKEN_OPERATOR_ASSIGN_ADD // +=
    };


class Alfabet
{
    std::map<QString,int> data;
public:
    Alfabet()
    {
    }
    void add(QString s, int n);
    int operator [] (QString s);
};



typedef std::deque<int> Chain;
typedef std::deque<Chain> ChainArray;
typedef std::vector<ChainArray> ChainTable;

class Rule
{
    public:
    int from;
    Chain to;
    QString dump(std::deque<QString> &tokens);
    Rule() {from=0;to.clear();}
};


class Node
{
    public:
    Node *next;
    Node *parent;
    std::deque<Node> childs;
    token val;
    //void add(token t);
    void adds(int symbol,bool last=false);
    Node *first();
    QString dump(int level);
    Node(){next=NULL;parent=NULL;}
    bool cleaning();
    bool optimize();
};

class Tree
{
    public:
    Node root;
    QString dump() {return root.dump(0);}
};



class Grammar
{
    std::deque<Rule> rules;    
    ChainTable first;
    ChainTable follow;
    ChainTable intersections;
    int terminalCount;
public:   
    void load(QByteArray data);
    void addtoken(QString name,int val);
    int getid(QString name);
    bool build(tokenList program,Tree &tree);
    bool createFirstList();
    bool createFollowList();
    bool clearFirstFollow();
    bool testFirstFollow();
    bool normalizeGrammar();
    bool inSet(Chain &val,ChainArray &set){
        for (unsigned int i=0;i<set.size();i++) if (set[i]==val) return true;
        return false;
    }
    Grammar();

    void printRules();
    void printFollow(QString adding);

    Chain subChain(Chain &in,int length,int start);
    bool isPureTerminal(Chain &c);
    QString chainToStr(Chain &c);
    QString chainsToStr(ChainArray &a);


    void lpush(std::stack<int> &magazine,int val);
    QString printStack(std::stack<int> &magazine,bool shorted);


    int lpop(std::stack<int> &magazine);
};




class Logger
{
public:
    QFile *filedebug;
    void log(QString msg,int level=0)
    {
        QByteArray a;
        a.append(msg);
        if (level<=loglevel){
            filedebug->write(a);
            filedebug->flush();
        }
    }

    int loglevel;
    bool init()
    {
        filedebug=new QFile("debug.txt");
        return filedebug->open(QFile::WriteOnly);
    }
    void close()
    {
       filedebug->close();
    }
};

class BCod
{
public:
    int operation;
    int op1;
    float op2;
    QString op3;
    BCod() {operation=0;op1=0;op2=0;op3="";}
};

enum OperationCodes{
    OC_PRINT=1,
    OC_PRINTS,
    OC_SCAN,
    OC_WAIT,
    OC_ASSIGNMENT,
    OC_DECLARATION,

    OC_PLUS,
    OC_MINUS,
    OC_MULT,
    OC_DIVID,
    OC_UPLUS,
    OC_UMINUS,
    OC_NEGATION,

    OC_ADDRESS,
    OC_VALUE,
    OC_CONSTANT,
    OC_INT2FLOAT,
    OC_FLOAT2INT,
};

enum MovType{
    MOV_CONST=0,
    MOV_IN,
    MOV_OUT
};


class Compiler
{
    static const int org_offset=0x100;
    int var_offset;
    std::deque<BCod> bcod;
    QByteArray out;

public:
    QString error;
    bool compile(Tree &tree);
    bool compileNode(Node &tree);
    QByteArray& build();

   // void insertAddress(token &val);
    void insertValue(token &val);
    void insertOpcode(OperationCodes cd);

    int float2int(float f);
    float int2float(int f);
    void outputI4(unsigned int i);
    void outputI2(unsigned int i);
    void outputI1(unsigned char i);
    void makeJump(int jl);
    void makeMov(QString reg,unsigned int c,MovType type=MOV_CONST);
    void makeInt(unsigned char i);
    void makeFLD();
    void makeFsumm();
    void makeFST(unsigned short address,bool isfloat);
    void makeFSTAndPush();
    int calcOffset(int i){
        return (i)*4+var_offset;
    }
};

enum SemanticTypes {
    ST_NONE=0,
    ST_PRINT,
    ST_PRINTS,
    ST_SCAN,
    ST_WAIT,
    ST_ASSIGNMENT,
    ST_DECLARATION,
    ST_ADDITIVE, //=7 additive-expression
    ST_MULTIPLICATIVE,
    ST_UNARY
};



extern QString tokenNames[];
extern const int tokenCount;
extern VarTable varTable;
extern ConstTable constTable;
extern Logger logger;
extern std::deque<QString> tokens;
extern std::deque<int> token2semantic;
#endif // PARSER_H
