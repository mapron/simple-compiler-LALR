#include "stdafx.h"
#include <bits/stl_pair.h>
const int keywordsCount=9;
const int tokenCount=43;

int baseTokenCount=0;

QString keywords[keywordsCount]={
  "int",
  "float",
  "print",
  "scan",
  "wait",
  "if",
  "else",
  "while",
  "prints"

};
int keywordsValues[keywordsCount]={
  TOKEN_TYPE_INT,
  TOKEN_TYPE_FLOAT,
  TOKEN_KEYWORD_PRINT,
  TOKEN_KEYWORD_SCAN,
  TOKEN_KEYWORD_WAIT,
  TOKEN_KEYWORD_IF,
  TOKEN_KEYWORD_ELSE,
  TOKEN_KEYWORD_WHILE,
  TOKEN_KEYWORD_PRINTS,

};
QString tokenNames[tokenCount+1]={
    "END",

    // базовые
    "TOKEN_IDENT",   // qwerty2
    "TOKEN_NUMBER",  // 567
    "TOKEN_STRING", // "sdf"
    "TOKEN_SEMICOLON", // ;

    // типы
     "TOKEN_TYPE_INT",   // int
     "TOKEN_TYPE_FLOAT", // float


    // ключевые слова
    "TOKEN_KEYWORD_PRINT", // print
    "TOKEN_KEYWORD_SCAN",  // scan
    "TOKEN_KEYWORD_WAIT",  // wait
    "TOKEN_KEYWORD_IF",    // if
    "TOKEN_KEYWORD_ELSE",  // else
    "TOKEN_KEYWORD_WHILE", // while
    "TOKEN_KEYWORD_PRINTS", // print

    //операторы
    "TOKEN_OPERATOR_SKOBKA_F_L", // {
    "TOKEN_OPERATOR_SKOBKA_F_R", // }
    "TOKEN_OPERATOR_SKOBKA_R_L", // (
    "TOKEN_OPERATOR_SKOBKA_R_R", // )
    "TOKEN_OPERATOR_SKOBKA_K_L", // [
    "TOKEN_OPERATOR_SKOBKA_K_R", // ]

    "TOKEN_OPERATOR_INCREMENT", // ++
    "TOKEN_OPERATOR_DECREMENT", // --
    "TOKEN_OPERATOR_COMMA", // ,

    "TOKEN_OPERATOR_PLUS", //+
    "TOKEN_OPERATOR_MINUS", // -
    "TOKEN_OPERATOR_EXCLAMATION", // !

    "TOKEN_OPERATOR_MULT", // *
    "TOKEN_OPERATOR_DIVIDE", // /
    "TOKEN_OPERATOR_REMAIN", // %

    "TOKEN_OPERATOR_SHIFTLEFT", // <<
    "TOKEN_OPERATOR_SHIFTRIGHT", // >>

    "TOKEN_OPERATOR_LOWER", // <
    "TOKEN_OPERATOR_GREATER" , // >
    "TOKEN_OPERATOR_LOWEREQUAL", // <=
    "TOKEN_OPERATOR_GREATEREQUAL", // >=

    "TOKEN_OPERATOR_EQUAL", // ==
    "TOKEN_OPERATOR_NOTEQUAL", // !=

    "TOKEN_OPERATOR_AND", // &
    "TOKEN_OPERATOR_XOR", // ^
    "TOKEN_OPERATOR_OR", // |

    "TOKEN_OPERATOR_AND_LOGICAL", // &&
    "TOKEN_OPERATOR_OR_LOGICAL", // ||
    "TOKEN_OPERATOR_ASSIGN", // =
    "TOKEN_OPERATOR_ASSIGN_ADD" // +=
};


QString opcodeNames[]={
    "",
    "OC_PRINT",
    "OC_PRINTS",
    "OC_SCAN",
    "OC_WAIT",
    "OC_ASSIGNMENT",
    "OC_DECLARATION",

    "OC_PLUS",
    "OC_MINUS",
    "OC_MULT",
    "OC_DIVID",
    "OC_UPLUS",
    "OC_UMINUS",
    "OC_NEGATION",

    "OC_ADDRESS",
    "OC_VALUE",
    "OC_CONSTANT",
    "OC_INT2FLOAT",
    "OC_FLOAT2INT",

};
std::deque<QString> tokens;
std::deque<int> token2semantic;


VarTable varTable;
ConstTable constTable;
Logger logger;


QByteArray token::dump()
{
    QByteArray rt;
    QString s="";

    QString types="?";
    if (type>=0 && type< (int)tokens.size()) types=tokens[type];
    else types="type:"+QString::number(type);
    s=s + types;
    if (id>0) s=s+"(id:" + QString::number(id)+")";
    s=s+"\r\n";
    rt.append(s);
    return rt;
}

bool token::isTerminal()
{
    return (type>0 && type <=tokenCount);
}

bool token::isBase()
{
    return (type>0 && type <baseTokenCount);
}
parser::parser()
{
data="";debug="";
position=0;
}

/*
  1- a-zA-z
  2- 0-9 
  3- пробельные
  4- спецсимволы ()-=-.,
  */
int charType(char c)
{
if ((c>='a' && c <='z') || (c>='A' && c <='Z') || c=='_') return 1;
if (c>='0' && c <='9') return 2;
if (c=='\r' || c =='\n' || c=='\t' ||c==' ') return 3;
if (c=='\"') return 5;
QString ops="(){}[]+-*/;.,<>=!&|^%";
if (ops.contains(c)) return 4;


return 0;
}

/*
  1  sldf4353l3245   идентификаторы
  2  123.5656       числа
  -345
  3 \r\n\t
  4 (){}=-+* /   операторы
  *5 "lkjfd фдорвап lsjkfg" строковые константы пока не делаем

  */
#define CURSOR data[position]
#define NEXT position++;
#define PREV position--;

token parser::nextToken()
{
    token rt;
    QString value="";
    // сейчас мы тут пропустим пробельные символы
    while (charType(CURSOR)==3){
        position++;
    }
    if (data[position]=='/' && data[position+1]=='*'){
        while (!(data[position-1]=='*' && data[position]=='/')) NEXT
        NEXT NEXT
    }
    int ctype=charType(CURSOR);
    if (ctype==1){
        rt.type=TOKEN_IDENT;

        while (charType(CURSOR)==1 || charType(CURSOR)==2){
            value+=CURSOR;
            NEXT
        }
        for (int i=0;i<keywordsCount;i++){
            if (keywords[i]==value){

                rt.type=keywordsValues[i];
                return rt;
            }
        }

        rt.id=varTable.add(value);
        // logger.log("value: "+value+" rt.id:"+QString::number(rt.id)+"\r\n");
        return rt;
    }
    if (ctype==4){
        switch (CURSOR){
            case ';': rt.type=TOKEN_SEMICOLON;NEXT return rt; break;
            case '=':rt.type=TOKEN_OPERATOR_ASSIGN;NEXT
                if (CURSOR=='=') {
                    rt.type=TOKEN_OPERATOR_EQUAL;NEXT
                }
                 return rt; break;
            case '(':rt.type=TOKEN_OPERATOR_SKOBKA_R_L;NEXT   return rt; break;
            case ')':rt.type=TOKEN_OPERATOR_SKOBKA_R_R;NEXT   return rt; break;
            case '{':rt.type=TOKEN_OPERATOR_SKOBKA_F_L;NEXT   return rt; break;
            case '}':rt.type=TOKEN_OPERATOR_SKOBKA_F_R;NEXT   return rt; break;
            case '[':rt.type=TOKEN_OPERATOR_SKOBKA_K_L;NEXT   return rt; break;
            case ']':rt.type=TOKEN_OPERATOR_SKOBKA_K_R;NEXT   return rt; break;
            case '+':rt.type=TOKEN_OPERATOR_PLUS;NEXT
                if (CURSOR=='+') {
                    rt.type=TOKEN_OPERATOR_INCREMENT;NEXT
                }else if (CURSOR=='=') {
                    rt.type=TOKEN_OPERATOR_ASSIGN_ADD;NEXT
                }
                 return rt; break;
            case '-':rt.type=TOKEN_OPERATOR_MINUS;NEXT
                if (CURSOR=='-') {
                    rt.type=TOKEN_OPERATOR_DECREMENT;NEXT
                }
                 return rt; break;
            case ',':rt.type=TOKEN_OPERATOR_COMMA;NEXT   return rt; break;
            case '!':rt.type=TOKEN_OPERATOR_EXCLAMATION;NEXT
                if (CURSOR=='=') {
                    rt.type=TOKEN_OPERATOR_NOTEQUAL;NEXT
                }
                 return rt; break;
            case '*':rt.type=TOKEN_OPERATOR_MULT  ;NEXT return rt; break;
            case '/':rt.type=TOKEN_OPERATOR_DIVIDE;NEXT return rt; break;
            case '%':rt.type=TOKEN_OPERATOR_REMAIN;NEXT return rt; break;

            case '<':rt.type=TOKEN_OPERATOR_LOWER;NEXT
                if (CURSOR=='<') {
                    rt.type=TOKEN_OPERATOR_SHIFTLEFT;NEXT
                }
                if (CURSOR=='=') {
                    rt.type=TOKEN_OPERATOR_LOWEREQUAL;NEXT
                }
                 return rt; break;
           case '>':rt.type=TOKEN_OPERATOR_GREATER;NEXT
                if (CURSOR=='>') {
                    rt.type=TOKEN_OPERATOR_SHIFTRIGHT;NEXT
                }
                if (CURSOR=='=') {
                    rt.type=TOKEN_OPERATOR_GREATEREQUAL;NEXT
                }
                 return rt; break;
            case '&':rt.type=TOKEN_OPERATOR_AND;NEXT
                if (CURSOR=='&') {
                    rt.type=TOKEN_OPERATOR_AND_LOGICAL;NEXT
                }
                return rt; break;
            case '|':rt.type=TOKEN_OPERATOR_OR;NEXT
                if (CURSOR=='|') {
                    rt.type=TOKEN_OPERATOR_OR_LOGICAL;NEXT
                }
                return rt; break;
             case '^':rt.type=TOKEN_OPERATOR_XOR;NEXT   return rt; break;

        }
    }
    if (ctype==2){
        rt.type=TOKEN_NUMBER;
        while (charType(CURSOR)==2 || CURSOR=='.') {
            value+=CURSOR;
            NEXT
        }
        rt.id=constTable.add(value);

        return rt;
    }
    if (ctype==5){// строка
        NEXT
        rt.type=TOKEN_STRING;
        value="";
        while (charType(CURSOR)!=5) {
            value+=CURSOR;
            NEXT
        }
        NEXT
        rt.id=constTable.add(value);

        return rt;
    }
    return rt;
}


tokenList parser::parse(QByteArray in)
{

    tokenList rt;
    data=in+"  ";
    //QByteArray rt="";
    token current_token;
    do
    {
        current_token=this->nextToken();
        rt.push_back(current_token);
        logger.log(current_token.dump(),5);

    }while (current_token);
    rt.pop_back();
    return rt;

}

int VarTable::add(QString name)
{
    unsigned int sz=list.size();
    for (unsigned int i=0;i<sz;i++) {
        if (list[i].name ==name) return i;
    }
    VarTableRecord r;
    r.name=name;
    r.type=0;
    list.push_back(r);
    return (int)sz;
}

int ConstTable::add(QString name)
{
    unsigned int sz=list.size();
    for (unsigned int i=0;i<sz;i++) {
        if (list[i].name ==name) return i;
    }
    ConstTableRecord r;
    r.name=name;
    r.type=0;
    list.push_back(r);
    return (int)sz;
}

/*
void Alfabet::add(QString s, int n){
        std::pair<QString,int> m;
        m.first=s;
        m.second=n;
        data.insert(m);
    }
int Alfabet::operator [] (QString s)
{
    return data[s];
}*/


QString Rule::dump(std::deque<QString> &tokens)
{
        QString rt="";
         if (from<0 || from>=(int)tokens.size()) return "offset error";
        rt=tokens[from];
        rt+=" -> ";        
        for (unsigned int i=0;i<to.size();i++) {
            unsigned int ind=to[i];
            if (ind>=tokens.size()) rt+="offset error";
            else  rt+=tokens[ind]+" ";
        }
        return rt;
}


Grammar::Grammar()
{

 terminalCount=tokenCount+1;
}

void Grammar::load(QByteArray data)
{
    QStringList lines;
    QString data2=data;
    lines= data2.split("\n");
    Rule current_rule;
    rules.clear();
    for (int i = 0; i < lines.size(); ++i){
         QString s=lines.at(i);
         bool tab=s[0]=='\t';
         s=s.trimmed();
         if (s==""&&!tab) continue;
//         logger.log(s+"\r\n");
         //    continue;
         if (tab) {
             QStringList parts;

             parts=s.split(" ");
             for (int j = 0; j < parts.size(); ++j){
                 QString part=parts.at(j).trimmed();
                 if (part=="") continue;
                 int tokenid=this->getid(part);
                 current_rule.to.push_back(tokenid);
             }
             //current_rule.to
         }else{
             QStringList parts;
             parts=s.split(" ");
             s=parts[0];
             int sema=0;

             if (parts.size()>1) sema=parts[1].toInt();


             int id=this->getid(s);
             token2semantic.resize(tokens.size());
             token2semantic[id]=sema;
             if (sema)
                 logger.log(tokens[id]+"=>"+QString::number(sema)+"\r\n");

             current_rule.from=id;
             continue;
         }
         rules.push_back(current_rule);
         current_rule.to.clear();
       // logger.log(s+"\r\n");
         //logger.log(current_rule.dump()+"\r\n");


    }
   // token2semantic.pop_back();
    //if (current_rule.from)
    //rules.push_back(current_rule);
/*
    tokens[TOKEN_IDENT]="a";
    tokens[TOKEN_OPERATOR_PLUS]="+";
    tokens[TOKEN_OPERATOR_MINUS]="-";
    tokens[TOKEN_OPERATOR_SKOBKA_R_L]="(";
    tokens[TOKEN_OPERATOR_SKOBKA_R_R]=")";
    tokens[terminalCount]="S";
*/
    //for (unsigned int i=0;i<token2semantic.size();i++){
    // logger.log(tokens[i]+" => "+opcodeNames[token2semantic[i]]+"\r\n");
    //}
    baseTokenCount=tokens.size();
    printRules();

}

void Grammar::printRules()
{
    logger.log("\r\nGRAMMAR Rules:\r\n",5);
    for (unsigned int i=0;i<rules.size();i++) logger.log(rules[i].dump(tokens)+"\r\n",5);
}


int Grammar::getid(QString name)
{
    int sz=tokens.size();
    for (int i=0;i<sz;i++) {
        if (tokens[i]==name) return i;
    }
    tokens.push_back(name);
    return sz;
}


Chain Grammar::subChain(Chain &in,int length,int start=0)
{
    Chain rt;

    for (int i=0;i<length;i++){
        rt.push_back(in[start+i]);
    }
    return rt;
}

bool Grammar::isPureTerminal(Chain &c)
{

    for (unsigned int s=0;s<c.size();s++){
        if (c[s]>=terminalCount) return false;
    }
    return true;
}



bool Grammar::createFirstList()
{
    logger.log("\r\n FIRST:\r\n",6);
    int sz=tokens.size();

    first.clear();
    first.resize(sz-terminalCount);
    for (unsigned int i=0;i<rules.size();i++) {
        //int firstSymbol=rules[i].to[0];
        if (rules[i].to.size()==0) continue;
        Chain firstChain=subChain(rules[i].to,1);

        //int ind=firstSymbol-terminalCount;
        int from1=rules[i].from-terminalCount;

        if (from1>=0){
            if (!inSet(firstChain,first[from1]))
                first[from1].push_back(firstChain);
        }
    }



    bool nextIteration;
    int iteration=0;
    do
    {
        nextIteration=false;
        iteration++;
        logger.log("Iteration:"+QString::number(iteration)+"\r\n",6);
        for (unsigned int i=0;i<first.size();i++) {// перебираем все нетерминалы в FIRST(K,terminal)
            for (unsigned int j=0;j<first[i].size();j++) {//перебираем все цепочки
                for (unsigned int s=0;s<first[i][j].size();s++) {//перебираем все символы в цепочке

                    int ind=first[i][j][s]-terminalCount ;
                    if (ind>=0){// нашли нетерминал
                        ChainArray first2=first[ind];
                        for (unsigned int y=0;y<first2.size();y++){
                            if (!inSet(first2[y],first[i])){
                                first[i].push_back(first2[y]);
                                nextIteration=true;
                            }
                        }
                    }
                }
            }
        }
        if (iteration>10) {
            logger.log("\r\n ERROR:iteration limit reached!\r\n");
            break;
        }
    }while(nextIteration);




    logger.log("\r\n FIRST - SUCCESS:\r\n",6);
    return true;
}

void Grammar::printFollow(QString adding="")
{

    logger.log("\r\n FOLLOW"+adding+":\r\n",6);
    for (unsigned int i=0; i<follow.size(); i++) {
        if (follow[i].size()==0) continue;
        QString s=tokens[i + terminalCount] + " -> ";
        s=s+chainsToStr(follow[i]);
        logger.log(s+"\r\n",6);
    }
}

bool Grammar::createFollowList()
{
    /*
               '           ''
0
s   )         ),e         ),e
t   r         R,+,-       R,+,-
r   0         0           0

1
s   ),e        ),e         ),e
t   R,+,-      R,+,-       R,+,-,),e
r   ),e        ),e         ),e

2
s   ),e         ),e           ),e
t   R,+,-,),e   R,+,-,),e     R,+,-,),e
r   ),e         ),e           ),e


s   ),e
r   ),e
t   +,-,),e
Шаг 3. Для всех А∈VN вычислить:
   FOLLOW′i(1,A)=FOLLOWi(1,A)∪FIRST(1,B),∀B∈(FOLLOWi(1,A)∩VN).
   FOLLOWi(1,A)u= FIRST(1,B), B- нетерминалы из FOLLOWi(1,A)
Шаг 4. Для всех А∈VN положить:
   FOLLOW′′i(1, A)=FOLLOW′i(1, A)∪FOLLOW′i(1, B),
     ∀B∈(FOLLOW′i(1, A)∩VN), если ∃ правило B→ε.
Шаг 5. Для всех А∈VN определить:
     FOLLOWi+1(1, A) = FOLLOW′′i(1, A)∪FOLLOW′′i(1, B),
     для всех нетерминальных символов B∈VN, имеющих правило вида
     B→αA, α∈(VT∪VN)*.

      */
    //return true;
    logger.log("\r\n FOLLOW:\r\n",6);
    int sz=tokens.size();
    follow.clear();
    follow.resize(sz-terminalCount);
    for (unsigned int i=0;i<rules.size();i++) {
        int rsz=rules[i].to.size();
        for (int j=0;j<rsz-1;j++) {            
            Chain following=subChain(rules[i].to,1,j+1); // цепочка после A
            int follows=rules[i].to[j]; // A

            int ind=follows-terminalCount;

            if (ind>=0){ // если А - терминал
                if (!inSet(following,follow[ind])) //logger.log("!\r\n");
                    follow[ind].push_back(following);
            }
        }
    }


    // program -> {e}
    Chain emptyChain;
    follow[0].push_back(emptyChain);

    bool nextIteration;
    int iteration=0;
    do
    {
        nextIteration=false;

        logger.log("Iteration:"+QString::number(iteration)+"\r\n",6);
        iteration++;
       // printFollow();
        //FOLLOW′i(1,A)=FOLLOWi(1,A)∪FIRST(1,B),∀B∈(FOLLOWi(1,A)∩VN).
        for (unsigned int i=0;i<follow.size();i++) {// перебираем нетерминалы
            for (unsigned int j=0;j<follow[i].size();j++) { // перебираем цепочки

                for (unsigned int s=0;s<follow[i][j].size();s++) {//перебираем все символы в цепочке
                    int ind=follow[i][j][s]-terminalCount ;
                    if (ind>=0){


                         ChainArray first2=first[ind];
                         for (unsigned int y=0;y<first2.size();y++){
                            if (!inSet(first2[y],follow[i])){
                                follow[i].push_back(first2[y]);
                                nextIteration=true;
                            }
                        }
                    }
                }


            }
        }
       // printFollow("\'");
        //  FOLLOW′′i(1, A)=FOLLOW′i(1, A)∪FOLLOW′i(1, B),  ∀B∈(FOLLOW′i(1, A)∩VN), если ∃ правило B→ε.
        // берем все нетерминалы B из FOLLOW, для которых есть правило B→ε, и
        // добавляем их в
        for (unsigned int i=0;i<follow.size();i++) {// перебираем нетерминалы
            for (unsigned int j=0;j<follow[i].size();j++) { // перебираем цепочки

                for (unsigned int s=0;s<follow[i][j].size();s++) {//перебираем все символы в FOLLOW цепочке
                    int ind=follow[i][j][s]-terminalCount ;
                    if (ind>=0){// B: если нетерминал, B∈(FOLLOW′i(1, A)∩VN),     R,+,- :  R =>НТ

                         ChainArray follow2=follow[ind];  //

                         if (inSet(emptyChain,follow2)){ //    ∃ правило B→ε.

                             for (unsigned int y=0;y<follow2.size();y++){
                                if (!inSet(follow2[y],follow[i])){
                                    follow[i].push_back(follow2[y]);
                                    nextIteration=true;
                                }
                             }
                         }

                    }
                }


            }
        }


      //  printFollow("\'\'");

        //FOLLOWi+1(1, A) = FOLLOW′′i(1, A)∪FOLLOW′′i(1, B), B∈VN  B→αA, α∈(VT∪VN)*.
        for (unsigned int i=0;i<rules.size();i++) {
            int sz=rules[i].to.size();
            if (!sz) continue;
            int A=rules[i].to[sz-1];
            int B=rules[i].from;
            int indA=A-terminalCount;
            int indB=B-terminalCount;

            if (indB>=0 && indA>=0){
                ChainArray followB=follow[indB];
                for (unsigned int y=0;y<followB.size();y++){

                     if (!inSet(followB[y],follow[indA])){
                            follow[indA].push_back(followB[y]);
                            nextIteration=true;

                    }
                }
            }
        }


        if (iteration>30) {
            logger.log("\r\n ERROR:iteration limit reached!\r\n",6);
            break;
        }
    }while(nextIteration);




    logger.log("\r\n FOLLOW - SUCCESS:\r\n",6);
    return true;
}



QString Grammar::chainToStr(Chain &c)
{
QString rt="";
if (c.size()==0) rt="e";
else{
    rt=tokens[ c[0] ];
    for (unsigned int j=1;j<c.size();j++) rt=rt+" "+tokens[ c[j] ];
}
return rt;
}

QString Grammar::chainsToStr(ChainArray &a)
{
QString rt="";
if (a.size()==0) rt="()";
else{
    rt="("+chainToStr(a[0] );
    for (unsigned int j=1;j<a.size();j++) rt=rt+","+chainToStr(a[j] );
    rt+=")";
}
return rt;
}

bool Grammar::clearFirstFollow()
{
   // for (unsigned int k=0; k<first.size(); k++) {
        for (unsigned int i=0; i<first.size(); i++) {

            ChainArray a;

            for (unsigned int j=0;j<first[i].size();j++){

                if (isPureTerminal(first[i][j]))
                    a.push_back(first[i][j]);
            }
            first[i]=a;
        }
   // }

   //  for (unsigned int k=0; k<follow.size(); k++) {
        for (unsigned int i=0; i<follow.size(); i++) {
            ChainArray a;
            for (unsigned int j=0;j<follow[i].size();j++){
                if (isPureTerminal(follow[i][j]))
                    a.push_back(follow[i][j]);
            }
            follow[i]=a;
        }
   // }


    logger.log("\r\nCleared FIRST:\r\n",4);
    //for (unsigned int k=0; k<first.size(); k++) {
        //logger.log("k: "+QString::number(k)+"\r\n");
        for (unsigned int i=0; i<first.size(); i++) {
            if (first[i].size()==0) continue;
            QString s=tokens[i + terminalCount] + " -> ";
            s=s+chainsToStr(first[i]);
            logger.log(s+"\r\n",4);
        }
    //}

    logger.log("\r\nCleared FOLLOW:\r\n",4);
    //for (unsigned int k=0; k<follow.size(); k++) {
        //logger.log("k: "+QString::number(k)+"\r\n");
        for (unsigned int i=0; i<follow.size(); i++) {
            if (follow[i].size()==0) continue;
            QString s=tokens[i + terminalCount] + " -> ";
            s=s+chainsToStr(follow[i]);
            logger.log(s+"\r\n",4);
        }
   // }
    return true;

}

bool Grammar::testFirstFollow()
{
    bool rt=true;
    intersections.clear();
    int sz=tokens.size();
    intersections.resize(sz-terminalCount);
    for (unsigned int i=0; i<follow.size(); i++) {

        for (unsigned int j=0;j<follow[i].size();j++){
            Chain c=follow[i][j];
             for (unsigned int y=0;y<first[i].size();y++){
                 if (first[i][y]==c) intersections[i].push_back(c);
             }
        }
        if (intersections[i].size()>0){
            rt=false;
            QString s="intersection:" + tokens[i + terminalCount] + " -> ";
            s=s+chainsToStr(intersections[i]);
            logger.log(s+"\r\n");
        }
    }

    return rt;
}

bool Grammar::normalizeGrammar()
{
    /*
  S->b
 S->Sa
 ==
S->bZ
Z->aZ
Z->e


postfix-expression -> primary-expression
postfix-expression -> no-recursion
postfix-expression -> postfix-expression TOKEN_OPERATOR_SKOBKA_K_L expression TOKEN_OPERATOR_SKOBKA_K_R
postfix-expression -> postfix-expression TOKEN_OPERATOR_SKOBKA_R_L expression-list TOKEN_OPERATOR_SKOBKA_R_R
postfix-expression -> postfix-expression TOKEN_OPERATOR_INCREMENT
postfix-expression -> postfix-expression TOKEN_OPERATOR_DECREMENT
===
postfix-expression -> primary-expression Z
postfix-expression -> no-recursion Z
Z -> TOKEN_OPERATOR_SKOBKA_K_L expression TOKEN_OPERATOR_SKOBKA_K_R  Z
Z -> TOKEN_OPERATOR_SKOBKA_R_L expression-list TOKEN_OPERATOR_SKOBKA_R_R  Z
Z -> TOKEN_OPERATOR_INCREMENT  Z
Z -> TOKEN_OPERATOR_DECREMENT  Z
Z ->e


statement-seq -> statement
statement-seq -> statement-seq statement
===
statement-seq -> statement  Z
Z -> statement  Z
Z -> e


S->a S
S->a
===
S->a Z
Z->S
Z->e

      */
    bool rt=false;
    logger.log("\r\nNormalizing grammar.\r\n",5);
    std::vector< std::deque<Rule*> > rulesp;
    unsigned int sz=tokens.size();
    rulesp.clear();
    rulesp.resize(sz);

    logger.log("\r\nLeft recursion...\r\n",5);

    unsigned int rsz=rules.size();
    for (unsigned int j=0;j<rsz;j++) {
        int litera=rules[j].from;
        rulesp[litera].push_back(&rules[j]);
    }
    for (unsigned int i=0;i<rulesp.size();i++){
        bool is_recursive=false;
        for (unsigned int j=0;j<rulesp[i].size();j++) {
            if (rulesp[i][j]->to.size() && rulesp[i][j]->to[0]==rulesp[i][j]->from){
                is_recursive=true;
            }

        }
        if (is_recursive){// левая рекурсия

            tokens.push_back(tokens[i]+"\'");
            for (unsigned int j=0;j<rulesp[i].size();j++) {
                if (rulesp[i][j]->to.size() && rulesp[i][j]->to[0]==rulesp[i][j]->from){
                    rulesp[i][j]->to.pop_front();
                    rulesp[i][j]->to.push_back(sz);
                    rulesp[i][j]->from=sz;
                }else{ // не рекурсия, добавляем в конец Z
                    rulesp[i][j]->to.push_back(sz);
                }
            }
            Rule r;
            r.from=sz;
            rules.push_back(r);
            sz++;
            rt=true;
        }


    }

    logger.log("\r\nLeft factorization...\r\n",5);
    /*
    for (unsigned int j=0;j<rules.size();j++) {
        if (rules[j].to.size()==0) continue;
        int litera=rules[j].to[0];
        rulesp[litera].push_back(&rules[j]);

    }

    for (unsigned int i=0; i<rulesp.size(); i++) {// перебираем все символы
        if (rulesp[i].size()>1){
            rt=true;
            logger.log("token "+tokens[i]+" has more than 1 rule \r\n");
            tokens.push_back("dummy_"+QString::number(sz));

            Rule r;

            for (unsigned int j=0;j<rulesp[i].size();j++) {
                r.from= rulesp[i][j]->from;
                rulesp[i][j]->from=sz;
                rulesp[i][j]->to.pop_front();


            }
            r.to.push_back(i);
            r.to.push_back(sz);

            rules.push_back(r);

            sz++;
        }

    }*/
    return rt;
}

QString Grammar::printStack(std::stack<int> &magazine,bool shorted=false)
{
    QString mstack="";
    std::stack<int> magazine1=magazine;
    while (!magazine1.empty()) {
       int mval=magazine1.top();
       magazine1.pop();
       mstack+=tokens[mval]+" ";
       if (!shorted) mstack+="< ";
    }
    return mstack;
}

void Grammar::lpush(std::stack<int> &magazine,int val)
{
    magazine.push(val);    
    logger.log(" >> "+tokens[val]+", memory stack: "+printStack(magazine)+"\r\n",6);

}

int  Grammar::lpop(std::stack<int> &magazine)
{
    int val= magazine.top();
    magazine.pop();    
    logger.log(" << "+tokens[val]+", memory stack: "+printStack(magazine)+"\r\n",6);
    return val;
}

// int a; =>  TYPE_INT IDENT SEMICOLON    => DEACLARATION
bool Grammar::build(tokenList program,Tree &tree)
{
    //K=K+1;// K=1 => LL(1)

    logger.log("Starting syntax analys\r\n");

    std::stack<int> magazine; // рабочий стек.
  //  tokenList p1;
    int pc=0; // указатель на токен в программе.

    lpush(magazine,terminalCount);// magazine={S}

    tree.root.val.type = terminalCount;
    Node *cur_node=&tree.root;
    int psize=program.size();
    //Chain emptyChain;
    QString log="";
    int iterations=0;

    QString stack_trace="";
    bool rt=true;
    while(iterations<150 && pc<psize)
    {
        iterations++;
        int top=magazine.top();
        int curs=program[pc].type;// переменная хранит в себе "курсор" в программе.

        stack_trace+=printStack(magazine,true)+"\r\n";

        Chain pcurs;
        pcurs.push_back(curs);

        logger.log("------------ I:"+QString::number(iterations)+"\r\n");
       // logger.log("stack top:"+tokens[top]+"\r\n");



        int env_min=pc-2<0?0:pc-2;
        int env_max=pc+2>=psize?psize-1:pc+2;
        QString penv="";
        for (int p=env_min;p<=env_max;p++){
            QString pt=tokens[program[p].type];
            if (p==pc) penv+=" >"+pt+"<";
            else       penv+=" "+pt+"";
        }
        logger.log("programm env:"+penv+"\r\n");
        logger.log(tree.dump(),7);

        int topInd=top -terminalCount;
        if ( topInd>=0){ // в вершине стека - нетерминал




            bool c_in_first=false;

            for (unsigned int i=0; i<rules.size(); i++){
                QString first_str="";
                if (rules[i].from!=top) continue;
                if (!rules[i].to.size()) continue;

                //Chain first_all;
               // for (unsigned int j=0;  j<rules[i].to.size(); j++){
                    // для первого символа в правиле, например в S->TR это T
                    // а в  "T -> a"   это "a"
                    int right_c=rules[i].to[0];
                    int ind=right_c-terminalCount;
                    if (ind>=0){ // нетерминал в правой части правила

                        if (inSet(pcurs,first[ind])){
                            first_str=chainsToStr(first[ind]);
                            c_in_first=true;//break;
                        }
                    }else{
                        if (right_c==curs){
                            first_str="symbol:"+tokens[right_c];
                            c_in_first=true;//break;
                        }
                    }
                //}
                if (c_in_first){// применяем свёртку A -> (to), e.g.   S->TR

                    lpop(magazine);

                    log+="T";
                    logger.log("A -> (x): "+rules[i].dump(tokens)+" ; first:"+first_str+" \r\n");
                    unsigned int sz=rules[i].to.size();
                    for (unsigned int j=sz-1;  j>0; j--) lpush(magazine,rules[i].to[j]);
                    if (sz) lpush(magazine,rules[i].to[0]);
                    for (unsigned int j=0;  j<sz-1; j++){
                        cur_node->adds(rules[i].to[j]);
                    }
                    if (sz) {
                        cur_node->adds(rules[i].to[sz-1],true);
                        cur_node=cur_node->first();
                    }else{
                        cur_node=cur_node->next;

                    }

                    break;
                }
            }

            if (c_in_first) continue;

            bool c_in_follow=false;
            for (unsigned int i=0; i<rules.size(); i++){
                if (rules[i].from!=top) continue;
                ChainArray followsA=follow[topInd];

                for (unsigned int j=0;  j<followsA.size(); j++){
                    if (pcurs==followsA[j]){
                        c_in_follow=true;break;
                    }

                }
                if (c_in_follow){// применяем свёртку A -> (e)
                    cur_node=cur_node->next;
                    logger.log("A -> (e) \r\n");
                    log+="E";
                    lpop(magazine);                   
                    break;
                }
            }
            if (c_in_follow) continue;

            /*
            bool simple_rule=false;
            Rule srule;
            for (unsigned int i=0; i<rules.size(); i++){
                if (rules[i].to.size()==1 && rules[i].to[0]==curs && rules[i].from==top){ // свёртка T => NT
                    srule=rules[i];
                    simple_rule=true;
                    break;
                }
            }
            if (simple_rule) {
                log+="S";
                logger.log("T => NT:"+srule.dump(tokens)+"\r\n");
                lpop(magazine);

                int sruleto=srule.to[0];

                lpush(magazine,sruleto);
                continue;
            }*/

        }
        if (curs==top){ // выброс
            cur_node->val= program[pc];
            if (cur_node->next==NULL){
                logger.log("next is NULL! pointer error! \r\n");
                logger.log("node:"+ cur_node->dump(0));
                break;
            }else{
                cur_node=cur_node->next;
            }
            log+="U";
            logger.log("unshift \r\n");
            lpop(magazine);
            pc++;
            continue;
        }
        if ( topInd>=0){
            Rule srule;
            bool void_rule=false;
            for (unsigned int i=0; i<rules.size(); i++){
                if (rules[i].to.size()==0 && rules[i].from==top){ // свёртка T => e
                    srule=rules[i];
                    void_rule=true;
                    break;
                }
            }
            if (void_rule) {
                cur_node=cur_node->next;
                log+="V";
                logger.log("T => e:"+srule.dump(tokens)+"\r\n");
                lpop(magazine);
                continue;
            }
        }
        rt=false;
        logger.log("parsing error! compile stopped... \r\n");
        break;
    };
    // logger.log("\r\n stack trace: \r\n"+stack_trace);
    tree.root.cleaning();
    tree.root.optimize();
    logger.log(tree.dump());
    return rt;
}




QString Node::dump(int level)
{

   QString tabs="";
   for (int i=0;i<level;i++) tabs+="\t";
   QString rt=tabs+val.dump();
   for (unsigned int i=0;i<childs.size();i++){
       rt+=childs[i].dump(level+1);
   }
   return rt;
}


void Node::adds(int symbol,bool last)
{
    Node n;
    n.parent=this;
    token t;
    t.type=symbol;
    n.val=t;
    if (last){
        n.next=next;
    }


    childs.push_back(n);
    int ccount=childs.size();

    if (ccount>=2){
        childs[ccount-2].next= &childs[ccount-1];
    }

}

Node* Node::first()
{
    int ccount=childs.size();
    if (ccount>=1){
        return &childs[0];
    }
    return NULL;
}

bool Node::cleaning()
{
    // спрева чистим всех детей.   

    for (unsigned int i=0;i<childs.size();i++){
        childs[i].cleaning();
    }

    std::deque<Node> nchilds;
    // вырежем пустых детей
    for (unsigned int i=0;i<childs.size();i++){
        if (childs[i].val.isTerminal() || childs[i].childs.size()!=0){
            nchilds.push_back(childs[i]);
        }
    }
    childs=nchilds;
    /*
    if (childs.size()==1 && childs[0].childs.size()==0){
        this->val=childs[0].val;
        childs.clear();

    }*/
    if (childs.size()==1 ){
        this->val=childs[0].val;
         std::deque<Node> nchilds=childs[0].childs;
         childs.clear();
        this->childs=nchilds;


    }



    return true;
}

bool Node::optimize()
{
    // спрева чистим всех детей.
    std::deque<Node> nchilds;
    for (unsigned int i=0;i<childs.size();i++){
        childs[i].optimize();
        if (!childs[i].val.isBase()){
            for (unsigned int j=0;j<childs[i].childs.size();j++){
                nchilds.push_back(childs[i].childs[j]);
            }
        }else{
             nchilds.push_back(childs[i]);
        }
    }
    childs=nchilds;
    return true;
}


bool Compiler::compile(Tree &tree)
{
   // for (unsigned int i=0;i<token2semantic.size();i++){
    // logger.log(tokens[i]+" => "+opcodeNames[token2semantic[i]]+"\r\n");
    //}

    return compileNode(tree.root);
}



void Compiler::insertValue(token &val)
{
    BCod newcode;
    if (val.type==TOKEN_IDENT){
        newcode.operation=OC_VALUE;
        newcode.op1=val.id;
        int t=constTable.get(val.id)->type;
        newcode.op3=t==1?"I":"F";
        bcod.push_back(newcode);
    }
    if (val.type==TOKEN_NUMBER){// fld

        newcode.operation=OC_CONSTANT;
        float fval=constTable.get(val.id)->name.toFloat();
        newcode.op2= fval;
       // logger.log("constant:"+QString::number(fval)+"\r\n");
        bcod.push_back(newcode);
    }
}

void Compiler::insertOpcode(OperationCodes cd)
{
     BCod newcode;
     newcode.operation=cd;
     bcod.push_back(newcode);
}

bool Compiler::compileNode(Node &node)
{
   // QString name=tokens[node.type];
    //if (name=="assignment-expression")
    BCod newcode,code2;
    VarTableRecord* var;
    ConstTableRecord* con;
    std::deque<int> ids;
    int vtype;

    if (node.val.isTerminal()){
        if (node.val.type==TOKEN_IDENT ||node.val.type==TOKEN_NUMBER){
            insertValue(node.val);
            return true;
        }
        if (node.val.type==TOKEN_SEMICOLON) return true;
        error=node.val.dump();
        return false;
    }
    switch (token2semantic[node.val.type]){
        case ST_PRINT:
            newcode.operation=OC_PRINT;
            //newcode.op1=node.childs[2].val.id;
            var=varTable.get(node.childs[2].val.id);
            if (var->declarated){
                insertValue(node.childs[2].val);
                //if (var->type==2)//float
                insertOpcode(OC_FLOAT2INT);
                //logger.log(var->name+" is declarated\r\n");
                bcod.push_back(newcode);
            }else{
                error="undefined variable "+var->name;
                return false;
            }
        break;
        case ST_PRINTS:
            newcode.operation=OC_PRINTS;


            con=constTable.get(node.childs[2].val.id);
            newcode.op3=con->name ;
           // logger.log("op3:"+newcode.op3);
            bcod.push_back(newcode);

        break;
        case ST_SCAN:
            newcode.operation=OC_SCAN;
            newcode.op1=node.childs[2].val.id;
            var=varTable.get(node.childs[2].val.id);
            if (var->type==2){
                error="float scan of "+var->name+" is not supported.";
                return false;
            }if (var->declarated){
                //insertAddress(node.childs[2].val);
                bcod.push_back(newcode);
            }else{
                error="undefined variable "+var->name;
                return false;
            }
        break;
        case ST_WAIT:            
            insertOpcode(OC_WAIT);
        break;
        case ST_ASSIGNMENT:
            newcode.operation=OC_ASSIGNMENT;
            if (!compileNode(node.childs[2])) return false;
            if (node.childs[1].val.type==TOKEN_OPERATOR_ASSIGN_ADD){
                insertValue(node.childs[0].val);
                insertOpcode(OC_PLUS);
            }
            var=varTable.get(node.childs[0].val.id);
            newcode.op1=node.childs[0].val.id;
            if (var->type==1) newcode.op3="I";
            if (var->type==2) newcode.op3="F";
            //insertAddress(node.childs[0].val);
            bcod.push_back(newcode);

        break;
        case ST_DECLARATION:
            //newcode.operation=ST_DECLARATION;
            logger.log(" declaration\r\n");
            if (node.childs[1].childs.size()==0){
                ids.push_back(node.childs[1].val.id);
            }else{
                for (unsigned int i=0;i<node.childs[1].childs.size();i++){
                    if (node.childs[1].childs[i].val.type==TOKEN_IDENT){
                        ids.push_back(node.childs[1].childs[i].val.id);
                    }
                }
            }
            vtype=1;
            if (node.childs[0].val.type==TOKEN_TYPE_FLOAT) vtype=2;
            for (unsigned int i=0;i<ids.size();i++){

                var=varTable.get(ids[i]);
                var->declarated=true;
                var->type=vtype;
                logger.log(var->name+" declarating..\r\n");
            }

        break;
        case ST_ADDITIVE: // a - b,  push(b) push(a)


            if (!compileNode(node.childs[2])) return false;
            if (!compileNode(node.childs[0])) return false;
            newcode.operation=OC_PLUS;
            if (node.childs[1].val.type==TOKEN_OPERATOR_MINUS) newcode.operation=OC_MINUS;
            bcod.push_back(newcode);

        break;
        case ST_MULTIPLICATIVE: // a / b,  push(b) push(a)


            if (!compileNode(node.childs[2])) return false;
            if (!compileNode(node.childs[0])) return false;
            newcode.operation=OC_MULT;
            if (node.childs[1].val.type==TOKEN_OPERATOR_DIVIDE) newcode.operation=OC_DIVID;
            bcod.push_back(newcode);

        break;

        case ST_UNARY: // -a

            if (!compileNode(node.childs[1])) return false;
            newcode.operation=OC_UMINUS;
            if (node.childs[0].val.type==TOKEN_OPERATOR_PLUS) newcode.operation=OC_UPLUS;
            if (node.childs[0].val.type==TOKEN_OPERATOR_EXCLAMATION) newcode.operation=OC_NEGATION;
            bcod.push_back(newcode);

        break;

        default:
        for (unsigned int i=0;i<node.childs.size();i++){
            if (!compileNode(node.childs[i])) return false;
        }
    }



    return true;
}

void Compiler::outputI4(unsigned int i)
{
    const char * offsetp=(const char *)&i;
    out.append(offsetp+0,1);
    out.append(offsetp+1,1);
    out.append(offsetp+2,1);
    out.append(offsetp+3,1);
}

int Compiler::float2int(float f)
{
    return *((int *)&f);
}

float Compiler::int2float(int f)
{
    return *((float *)&f);
}

void Compiler::outputI2(unsigned int i)
{
    const char * offsetp=(const char *)&i;
    out.append(offsetp+0,1);
    out.append(offsetp+1,1);
}

void Compiler::outputI1(unsigned char i)
{
    out.append(i);
}

void Compiler::makeJump(int jl)
{
    out.append("\xE9",1);
   // jl+=3;
    outputI2((unsigned int)jl);
}

void Compiler::makeMov(QString reg,unsigned int val,MovType type)
        // type=0 REG<-const, type=1 REG<-ad, type=2 ad<-REG
{
    //logger.log(reg+" ");
    int csize=0;
    reg=reg.toUpper();
    if (reg[0]=='E') {out.append('\x66');reg=reg.right(2);csize=4;}
    unsigned char reg_index=0;
    if (reg[1]=='X') {reg=reg.left(1);if (!csize) csize=2;reg_index+=8;}
    if (reg[1]=='H') {reg=reg.left(1);if (!csize) csize=1;reg_index+=4;}
    if (reg[1]=='L') {reg=reg.left(1);if (!csize) csize=1;reg_index+=0;}

    unsigned char opcode='\xB0';
    if (type==MOV_CONST) {// const

        if (reg[0]=='A') reg_index+=0;
        if (reg[0]=='C') reg_index+=1;
        if (reg[0]=='D') reg_index+=2;
        if (reg[0]=='B') reg_index+=3;
        opcode='\xB0'+reg_index;
    }else if (type==MOV_IN) {// REG<-ad
        opcode='\xA1';
        if (csize==1) opcode='\xA0';
        if (csize==4) csize=2;

    }else if (type==MOV_OUT) {// ad<-REG
        opcode='\xA3';
        if (csize==1) opcode='\xA2';
        if (csize==4) csize=2;
    }


    if (csize) out.append(opcode);
    //logger.log("mov "+reg+" "+QString::number(csize)+"\r\n");
    if (csize==1){
        outputI1(val);
    }else if (csize==2){
        outputI2(val);
    }else if (csize==4){
        outputI4(val);
    }

}

void Compiler::makeInt(unsigned char i)
{
     out.append("\xCD",1);
     outputI1(i);// int 21h
}

void Compiler::makeFLD()
{
    out.append("\x66\x58",2); //pop eax
    makeMov("eax",var_offset,MOV_OUT);// ax->
    out.append("\x9B\xD9\x06",3);// FLD
    outputI2(var_offset);//  выводим адрес числа.

}


void Compiler::makeFST(unsigned short address,bool isfloat)
{
    // 0042  9B DB 1E 0000 R                FISTP   SHORT_INTEGER
    // 0030  9B D9 16 0000 R                FST     SHORT_REAL


    if (isfloat){
        out.append("\x9B\xD9\x16",3);
    }else{
        out.append("\x9B\xDB\x1E",3);
    }
    outputI2(address);

}

void Compiler::makeFSTAndPush()
{
     makeFST(var_offset,true);
     makeMov("ax",var_offset,MOV_IN);// ax<-
     out.append("\x66\x50",2); //push eax,
}


QByteArray& Compiler::build()
{

    //QByteArray out="";
    //unsigned int offset=0;
    unsigned int tableLength=varTable.list.size();
    unsigned int jumplength=tableLength*4;
    //offset=org_offset+tableLength*4+3;
    //logger.log("start offset:"+QString::number(offset,16)+"\r\n");
    //offset=tableLength;
    QByteArray printProc="";
    printProc.append("\x3D\x00\x00\x75\x09\x50\xB0\x30\xB4\x0E"
                     "\xCD\x10\x58\xC3\x60\xBA\x00\x00\x3D\x00"
                     "\x00\x74\x12\xBB\x0A\x00\xF7\xF3\xE8\xEF"
                     "\xFF\x8B\xC2\x04\x30\xB4\x0E\xCD\x10\xEB"
                     "\x00\x61\xC3",43);
    printProc.append("\x52\x50\x56\xEB\x03\x00\x0A\x00\xB9\x00"
                     "\x00\x2E\xC6\x06\xB6\x01\x00\x8A\x04\x46"
                     "\x3C\x00\x75\x02\xEB\x21\x3C\x2D\x75\x08"
                     "\x2E\xC6\x06\xB6\x01\x01\xEB\xEB\x50\x8B"
                     "\xC1\x2E\xF7\x26\xB7\x01\x8B\xC8\x58\x2C"
                     "\x30\xB4\x00\x8B\xD1\x03\xC8\xEB\xD6\x2E"
                     "\x80\x3E\xB6\x01\x00\x74\x02\xF7\xD9\x5E"
                     "\x58\x5A\xC3",73);

    jumplength+=printProc.length();
    var_offset=3+printProc.length()+org_offset;

    makeJump(jumplength);

    out.append(printProc);
    for (unsigned int i=0;i<tableLength;i++){
        out.append("\xC3\xC3\xC3\xC3",4);
    }
    // var_offset=out.length()+org_offset;

    foreach (BCod b,bcod ){
        QString l;
        l=opcodeNames[b.operation];
        if (b.op1) l+=" "+QString::number(b.op1);
        if (b.op2) l+=" "+QString::number(b.op2);
        if (b.op3.length()) l+=" "+(b.op3);
        logger.log(l+"\r\n");

        //--------
        if (b.operation==OC_WAIT){
             makeMov("ah",0);
             makeInt(0x16);
        }else if (b.operation==OC_PRINTS){

            QString outs=""+b.op3+"\r\n$";
            unsigned int len=outs.length();           
            makeJump(len);

            unsigned int soff=out.length()+org_offset;
            out.append(outs);
            makeMov("dx",soff);
            makeMov("ah",9);
            makeInt(0x21);// int 21h


        }else if (b.operation==OC_CONSTANT){
           //
           // unsigned int soff=out.length()+org_offset;
            makeMov("eax",float2int(b.op2));
           // outputI4(float2int(b.op2));
            out.append("\x66\x50",2); //push eax,

        }else if (b.operation==OC_PLUS){
//            out.append("\x66\x58",2); //pop eax
//            int procRel= -out.length()-3+3;
//            out.append("\xE8",1);// call
//            outputI2(procRel);
//
//            out.append("\x66\x58",2); //pop eax
//            procRel= -out.length()-3+3;
//            out.append("\xE8",1);// call
//            outputI2(procRel);
//
//
//            makeMov("eax",float2int(111.0));
//            out.append("\x66\x50",2); //push eax,
            makeFLD();
            makeFLD();
            out.append("\x9B\xD8\xC1",3);
            makeFSTAndPush();
        }else if (b.operation==OC_MINUS){
            makeFLD();
            makeFLD();
            out.append("\x9B\xD8\xC1",3);
            makeFSTAndPush();
        }else if (b.operation==OC_MULT){
            makeFLD();
            makeFLD();
            out.append("\x9B\xD8\xC1",3);
            makeFSTAndPush();
        }else if (b.operation==OC_DIVID){
            makeFLD();
            makeFLD();
            out.append("\x9B\xD8\xC1",3);
            makeFSTAndPush();

        }

        else if (b.operation==OC_ASSIGNMENT){
            int off=calcOffset(b.op1);
            makeFLD();
            makeFST(off,b.op3=="F");
        }else if (b.operation==OC_VALUE){

            int off=calcOffset(b.op1);           

            if (b.op3=="F"){
                out.append("\x9B\xD9\x06",3);// FLD float
            }else{
                out.append("\x9B\xDB\x06",3);// FLD int
            }
            outputI2(off);//  выводим адрес числа.


            out.append("\x9B\xD9\x16",3);// FST float
            outputI2(var_offset);

            makeMov("ax",var_offset,MOV_IN);// ax<-
            out.append("\x66\x50",2); //push eax, в стеке теперь - число вещественное!
        }else if (b.operation==OC_FLOAT2INT){

            out.append("\x66\x58",2); //pop eax
            makeMov("ax",var_offset,MOV_OUT);// ax->

            out.append("\x9B\xD9\x06",3);// FLD float
            outputI2(var_offset);//  выводим адрес числа.

            out.append("\x9B\xD9\x16",3);// FST float
            outputI2(var_offset);

            makeMov("ax",var_offset,MOV_IN);// ax<-
            out.append("\x66\x50",2); //push eax, в стеке теперь - число целое!



        }else if (b.operation==OC_PRINT){
            out.append("\x66\x58",2); //pop eax
            int procRel= -out.length()-3+3;
            out.append("\xE8",1);// call
            outputI2(procRel);
            /*
            makeMov("ah",0xe);
            makeMov("al",'\r');
            makeInt(0x10);
            makeMov("al",'\n');
            makeInt(0x10);*/
        }else if (b.operation==OC_SCAN){
            // out.append("\x66\x58",2); //pop eax
            makeJump(8);// делаем прыжок вперед, перепрыгивая через число

            unsigned int buffer_offset=out.length()+org_offset;
            outputI4(0);outputI4(0);

            makeMov("dx",buffer_offset);// mov dx, offset msg1
            makeMov("ah",0xA);
            makeInt(0x21);// int 21h


            out.append("\xC6\x87",2);// mov buffer_offset+7,0
            outputI2(buffer_offset+7);
            outputI1(0);

            out.append("\xBE",1);//lea    si, buffer + 2  ;
            outputI2(buffer_offset+2);

            int procRel= -out.length()-3+46;
            out.append("\xE8",1);// call
            outputI2(procRel);

            int off=calcOffset(b.op1);

            out.append("\x8B\xC1",2);// mov ax,cx
            makeMov("ax",off,MOV_OUT); // [off]=ax

        }
    }
   // makeInt(0x20);
    outputI1(0xC3);// ret
    return out;
}

/*
program
    statement-seq
        statement

        statement
        statement
        statement
    END

TOKEN_TYPE_INT TOKEN_IDENT(id:1) TOKEN_OPERATOR_COMMA TOKEN_IDENT(id:2) TOKEN_SEMICOLON
TOKEN_IDENT(id:1) TOKEN_OPERATOR_ASSIGN TOKEN_NUMBER(id:1) TOKEN_SEMICOLON
TOKEN_KEYWORD_PRINT TOKEN_OPERATOR_SKOBKA_R_L TOKEN_IDENT(id:1) TOKEN_OPERATOR_SKOBKA_R_R
TOKEN_SEMICOLON TOKEN_KEYWORD_WAIT TOKEN_SEMICOLON

END
*/
