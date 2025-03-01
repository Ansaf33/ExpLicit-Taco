echo "running label translation.."

lex labels.l
gcc lex.yy.c table.c -o program
./program ../three_address_code/assembly_tac.xsm ../three_address_code/proper_assembly.xsm

echo ".. done"
