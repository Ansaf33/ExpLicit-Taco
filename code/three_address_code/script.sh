
cd optimizer
lex lexopt.l
yacc -d parseopt.y
gcc lex.yy.c y.tab.c ../gentac.c ../../symbol_table/Gsymbol.c ../../symbol_table/varList.c ../instruction_set.c ../../reghandling.c ../map_implementation/map.c ../set_implementation/set.c -o program
./program ../tac.txt ../optimized.txt

cd ..
lex lextac.l
yacc -d parsetac.y
gcc lex.yy.c y.tab.c gentac.c ../symbol_table/Gsymbol.c ../symbol_table/varList.c instruction_set.c ../reghandling.c map_implementation/map.c set_implementation/set.c -o program
./program optimized.txt assembly_tac.xsm

cd ../label_translation
./scriptTAC.sh

cd ../../xsm_expl
./xsm -l library.lib -e ../code/three_address_code/proper_assembly.xsm


