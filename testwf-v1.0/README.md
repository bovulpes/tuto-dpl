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
O2/Detectors/ITSMFT/MFT/testwf/include/MFTTestwf/TestWorkflow.h
O2/Detectors/ITSMFT/MFT/testwf/TestWorkflow.cxx
O2/Detectors/ITSMFT/MFT/testwf/mft-test-workflow.cxx
```

Build and run:

```bash
mft-test-workflow
```
