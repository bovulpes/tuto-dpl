Place the files like this:

```bash
O2/Detectors/ITSMFT/MFT/testwf/CMakeLists.txt
O2/Detectors/ITSMFT/MFT/testwf/include/MFTTestwf/DigitDigestSpec.h
O2/Detectors/ITSMFT/MFT/testwf/src/TestWorkflow.cxx
O2/Detectors/ITSMFT/MFT/testwf/src/DigitDigestSpec.cxx
```

Comment in DigitReaderSpec.cxx the line:

//pc.services().get<ControlService>().readyToQuit(true);

Build and run:

```bash
mft-test-workflow -b
```
