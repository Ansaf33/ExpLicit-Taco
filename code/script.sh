lex lexical.l
echo "Parsing input file"
yacc -d parser.y
gcc AST.c reghandling.c symbol_table/Gsymbol.c symbol_table/varList.c operators/optrans.c three_address_code/gentac.c three_address_code/instruction_set.c y.tab.c lex.yy.c -o program
./program input.txt assembly_code.xsm

cd label_translation
./script.sh



