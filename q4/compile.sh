clear
echo "COMPILING..."
gcc -o reader_writer reader_writer.c -lpthread -lrt
echo
echo "COMPILED..."
echo "RUNNING..."
echo
./reader_writer