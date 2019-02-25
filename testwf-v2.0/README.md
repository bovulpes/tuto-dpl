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
O2/Detectors/ITSMFT/MFT/testwf/include/MFTTestwf/ClustererSpec.h
O2/Detectors/ITSMFT/MFT/testwf/include/MFTTestwf/ClusterWriterSpec.h
O2/Detectors/ITSMFT/MFT/testwf/src/ClustererSpec.cxx
O2/Detectors/ITSMFT/MFT/testwf/src/ClusterWriterSpec.cxx
O2/Detectors/ITSMFT/MFT/testwf/src/TestWorkflow.cxx
```

Build and run:

```bash
mft-test-workflow -b
```
