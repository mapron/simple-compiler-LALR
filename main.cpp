#include "stdafx.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile filein("in.txt");
    QFile fileout("out.com");
    //QFile filedebug("debug.txt");
    logger.loglevel=5;

    QFile grammarfile("grammar");
    QByteArray s;
    QByteArray out="";
    QByteArray g;

    for (int i=0;i<=tokenCount;i++) {
        tokens.push_back(tokenNames[i]);
        token2semantic.push_back(0);
    }

    if (filein.open(QFile::ReadOnly) && grammarfile.open(QFile::ReadOnly)  ) {
        s = filein.readAll();
        g = grammarfile.readAll();
        if (fileout.open(QFile::WriteOnly) && logger.init() ) {


            try{
                parser theParser;
                tokenList ltokens;
                ltokens= theParser.parse(s);
                logger.log(theParser.debug);

                //Alfabet al;

                Grammar grammar;

                // загружаем грамматику
                grammar.load(g);


                for (int i=0;i<7;i++){
                    grammar.createFirstList();
                    grammar.createFollowList();
                    grammar.clearFirstFollow();                    
                    if (!grammar.normalizeGrammar()) break;
                    grammar.printRules();
                }
                //grammar.testFirstFollow();
                grammar.printRules();

                // пишем данные
                out.append("\xB4\x00\xCD\x16\xC3",5);

                Tree tree;
                if (grammar.build(ltokens,tree)){

                    Compiler compiler;
                    if (compiler.compile(tree)){
                        logger.log("\r\n Variable table:\r\n");
                        logger.log(varTable.dump());
                        out=compiler.build();

                    }else{
                        logger.log("compiler error:"+compiler.error+"\r\n");
                    }
                }




                fileout.write(out);

                // закрываем файлы.
                filein.close();
                grammarfile.close();
                fileout.close();
                logger.close();

            }catch(...){
                logger.log("ERROR during compilation.");
            }


        }

    }

    return 0;
}
