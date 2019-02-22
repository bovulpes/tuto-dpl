#include <fstream>
#include <iostream>

using namespace std;

template<class T> char* as_bytes(T& i)  // needed for binary I/O
{
  void* addr = &i;        // get the address of the first byte
                          // of memory used to store the object
  return static_cast<char*>(addr); // treat that memory as bytes
}

int main()
{

  ifstream ifs {"dpl-out.bin", ios_base::binary};
  if (!ifs) printf("can't open input file \n");

  constexpr int dataHeaderSize = 80;
  constexpr int dataProcHeaderSize = 56;

  struct Digest {
    int inputCount;
    int digitsCount;
  };

  char dh[dataHeaderSize];
  char dph[dataProcHeaderSize];

  Digest data;
  
  ifs.read(as_bytes(dh),  dataHeaderSize);
  ifs.read(as_bytes(dph), dataProcHeaderSize);
  ifs.read(as_bytes(data),sizeof(Digest));

  printf("inputs %d digits %d \n",data.inputCount, data.digitsCount);
  
  ifs.close();

  return 0;
  
}
