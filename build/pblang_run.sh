FILE=$1
OUTPUT=$2
if test -f "$FILE"; then
  ./pbl-compiler.o "$FILE"
  llvm-dis "module_one" -o "module_two.ll"
  clang "module_two.ll" -o "$OUTPUT"
  ./"$OUTPUT"
else
  echo "$FILE does not exist"
fi

