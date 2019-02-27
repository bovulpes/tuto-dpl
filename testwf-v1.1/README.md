Add the content of the file:

```bash
O2Dependencies.cmake
```

to the file:

```bash
O2/cmake/O2Dependencies.cmake
```

Place the other files like this:

```bash
O2/Detectors/ITSMFT/MFT/testwf/CMakeLists.txt
O2/Detectors/ITSMFT/MFT/testwf/include/MFTTestwf/DigitReaderSpec.h
O2/Detectors/ITSMFT/MFT/testwf/src/TestWorkflow.cxx
O2/Detectors/ITSMFT/MFT/testwf/src/DigitReaderSpec.cxx
```

Build and run:

```bash
mft-test-workflow -b
```
