#if !defined(__CLING__) || defined(__ROOTCLING__)

#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <memory>
#include "FairLogger.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "ITSMFTBase/Digit.h"
#include "SimulationDataFormat/RunContext.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "SimulationDataFormat/MCCompLabel.h"

void read_digits(std::string path = "./",
		 std::string digiFName = "mftdigits.root",
		 std::string runContextFName = "collisioncontext.root",
		 std::string mctruthFName = "o2sim.root")
{
  if (path.back() != '/') {
    path += '/';
  }

  // open file with digits
  std::unique_ptr<TFile> digiFile(TFile::Open((path + digiFName).c_str()));
  if (!digiFile || digiFile->IsZombie()) {
    LOG(ERROR) << "Failed to open input digits file " << (path + digiFName) << FairLogger::endl;
    return;
  }

  // open file with run context
  std::unique_ptr<TFile> rcFile(TFile::Open((path + runContextFName).c_str()));
  if (!rcFile || rcFile->IsZombie()) {
    LOG(ERROR) << "Failed to open runContext file " << (path + runContextFName) << FairLogger::endl;
    return;
  }

  // get tree with digits
  TTree* digiTree = (TTree*)digiFile->Get("o2sim");
  if (!digiTree) {
    LOG(ERROR) << "Failed to get digits tree" << FairLogger::endl;
    return;
  }
  printf("MFTdigit entries = %lld \n",digiTree->GetEntries());

  // read the digits through a vector
  std::vector<o2::ITSMFT::Digit>* dv = nullptr;
  digiTree->SetBranchAddress("MFTDigit", &dv);
  printf("MFTDigit size = %zu \n",dv->size());

  // read the composed labels through a vector container stored in a tree
  o2::dataformats::MCTruthContainer<o2::MCCompLabel>* labels = nullptr;
  digiTree->SetBranchAddress("MFTDigitMCTruth", &labels);
  printf("MFTDigitMCTruth IndexedSize %zu NElements %zu \n",labels->getIndexedSize(),labels->getNElements());

  // ROF record entries in the digit tree
  auto rofRecVec = reinterpret_cast<vector<o2::ITSMFT::ROFRecord>*>(digiFile->GetObjectChecked("MFTDigitROF", "vector<o2::ITSMFT::ROFRecord>"));
  if (!rofRecVec) {
    LOG(ERROR) << "Failed to get MFT digits ROF Records" << FairLogger::endl;
    return;
  }
  printf("MFTDigitROF size = %zu \n",rofRecVec->size());

  // MCEvID -> ROFrecord references
  auto mc2rofVec = reinterpret_cast<vector<o2::ITSMFT::MC2ROFRecord>*>(digiFile->GetObjectChecked("MFTDigitMC2ROF", "vector<o2::ITSMFT::MC2ROFRecord>"));
  if (!mc2rofVec) {
    LOG(WARNING) << "Did not find MCEvent to ROF records references" << FairLogger::endl;
    return;
  }
  printf("MFTDigitMC2ROF size = %zu \n",mc2rofVec->size());

  // MC collisions record
  auto runContext = reinterpret_cast<o2::steer::RunContext*>(rcFile->GetObjectChecked("RunContext", "o2::steer::RunContext"));
  if (!runContext) {
    LOG(WARNING) << "Did not find RunContext" << FairLogger::endl;
    return;
  }

  auto intRecordsVec = runContext->getEventRecords(); // interaction record
  auto evPartsVec = runContext->getEventParts();      // event parts
  int nEvents = runContext->getNCollisions();

  printf("event records %zu, event parts %zu, N collisions %d \n\n",intRecordsVec.size(),evPartsVec.size(),nEvents);

  // digits, stored in the tree o2sim
  digiTree->GetEvent(0);
  Int_t nDigits = dv->size();
  printf("MFTDigit size = %d \n",nDigits);

  UShort_t chipID, dCol, dRow;
  UInt_t dROF;
  Int_t dCh;
  Int_t nReadDigits1 = 0;
  Int_t nReadDigits2 = 0;
  const UShort_t maxROF = 1000;
  UInt_t nDigROF1[maxROF]; // signal
  UInt_t nDigROF2[maxROF]; // noise
  for (Int_t i = 0; i < maxROF; i++) {
    nDigROF1[i] = 0;
    nDigROF2[i] = 0;
  }
  UInt_t maxRunROF = 0;
  for (vector<o2::ITSMFT::Digit>::const_iterator d = dv->begin(); d != dv->end(); ++d) {
    chipID = d->getChipIndex();
    dCol   = d->getColumn();
    dRow   = d->getRow();
    dCh    = d->getCharge();
    dROF   = d->getROFrame();
    if (chipID >= 920) {
      printf("Chip ID = %d >= 920 !!! col/row %d/%d ch %d ROF %d \n",chipID,dCol,dRow,dCh,dROF);
      continue;
    }
    maxRunROF = (maxRunROF > dROF) ? maxRunROF : dROF;
    if (dCh > 165) { // is signal
      nReadDigits1++;
      nDigROF1[dROF]++;
    } else { // is noise
      nReadDigits2++;
      nDigROF2[dROF]++;
    } // end signal,noise
  } // end digit loop
  printf("maxRunROF = %d \n",maxRunROF);
  return;
  
  // signal
  UInt_t nROF1 = 0;
  UInt_t nDigROF1_0_13 = 0;
  for (UInt_t iROF = 0; iROF < (maxRunROF+1); iROF++) {
    if (nDigROF1[iROF] > 0) {
      nROF1++;
      //printf("ROF %3d  NDigits %3d \n",iROF,nDigROF1[iROF]);
      if (iROF <= 13) {
	nDigROF1_0_13 += nDigROF1[iROF];
      }
    }
  }
  printf("S: read %d digits in %d ROFrames. \n",nReadDigits1,nROF1);
  printf("S: Read %d digits in ROFrames 0 to 13. \n",nDigROF1_0_13);
  
  // noise
  UInt_t nROF2 = 0;
  UInt_t nDigROF2_0_13 = 0;
  for (UInt_t iROF = 0; iROF < (maxRunROF+1); iROF++) {
    if (nDigROF2[iROF] > 0) {
      nROF2++;
      //printf("ROF %3d  NDigits %3d \n",iROF,nDigROF2[iROF]);
      if (iROF <= 13) {
	nDigROF2_0_13 += nDigROF2[iROF];
      }
    }
  }
  printf("N: read %d digits in %d ROFrames. \n",nReadDigits2,nROF2);
  printf("N: read %d digits in ROFrames 0 to 13. \n",nDigROF2_0_13);
  
  // ROFrames, stored in a vector
  Int_t nROFRec = 0;
  for (vector<o2::ITSMFT::ROFRecord>::const_iterator rofrec = rofRecVec->begin(); rofrec != rofRecVec->end(); ++rofrec) {
    const auto& bcdata = rofrec->getBCData(); // InteractionRecord
    //cout << "ROFRecord _____ " << nROFRec << "\n";
    //cout << "     Time from start of run " << bcdata.timeNS << " [ns] \n";
    //cout << "     Bunch crossing ID " << bcdata.bc << "\n";
    //cout << "     LHC orbit " << bcdata.orbit << "\n";
    const auto rofEntry = rofrec->getROFEntry(); // EvIndex
    const auto nROFEntries = rofrec->getNROFEntries();
    const auto roFrame = rofrec->getROFrame(); // the iteration
    const auto rofEntryEvent = rofEntry.getEvent();
    const auto rofEntryIndex = rofEntry.getIndex();
    //cout << "ROF entries " << nROFEntries << " roFrame " << roFrame << "\n";
    //cout << "     ROF entry event " << rofEntryEvent << " index " << rofEntryIndex << "\n";
    nROFRec++;
  }
  printf("Read %d ROFRecords \n",nROFRec);  

  // MC to ROFRecord, stored in a vector
  Int_t nMC2ROFRec = 0;
  Int_t nDigSignal = 0;
  for (vector<o2::ITSMFT::MC2ROFRecord>::const_iterator mc2rof = mc2rofVec->begin(); mc2rof != mc2rofVec->end(); ++mc2rof) {
    auto minROF = mc2rof->minROF;  // init    0xffffffff
    auto maxROF = mc2rof->maxROF;  // init to 0
    auto eventRecordID = mc2rof->eventRecordID;
    auto rofRecordID = mc2rof->rofRecordID;
    cout << "Event record ID " << eventRecordID << " ROF record ID " << rofRecordID << " , min ROF " << minROF << " maxROF " << maxROF << "\n";
    for (auto rof = minROF; rof <= maxROF; rof++) {
      const auto& rofrec = (*rofRecVec)[rofRecordID];
      const auto rofEntry = rofrec.getROFEntry(); // EvIndex
      const auto nROFEntries = rofrec.getNROFEntries();
      const auto roFrame = rofrec.getROFrame(); // the iteration
      const auto rofEntryEvent = rofEntry.getEvent();
      const auto rofEntryIndex = rofEntry.getIndex();
      cout << "ROF entries " << nROFEntries << " roFrame " << roFrame << "\n";
      cout << "     ROF entry event " << rofEntryEvent << " index " << rofEntryIndex << "\n";
      auto dgid0 = rofEntryIndex;
      auto dgid1 = rofEntryIndex + nROFEntries -1;
      cout << "     digit ID from " << dgid0 << " to " << dgid1 << "\n";
      for (auto dgid = dgid0; dgid <= dgid1; dgid++) {
	const auto& labs = labels->getLabels(dgid);
	auto mccl = labs[0];
	if (mccl.getTrackID() >= 0 && (mccl.getEventID() == eventRecordID)) {
	  nDigSignal++;
	  printf("     digit %d track ID %d event ID %d source ID %d \n",dgid,mccl.getTrackID(),mccl.getEventID(),mccl.getSourceID());
	}
      }
      //
      rofRecordID++;
    }
    nMC2ROFRec++;
  }
  printf("Read %d MC2ROFRecords with %d digits from signal \n",nMC2ROFRec,nDigSignal);  
  
  // MC digit labels, stored in the tree o2sim
  Int_t dMCT_ISize = labels->getIndexedSize();
  Int_t dMCT_NElem = labels->getNElements();
  printf("MFTDigitMCTruth IndexedSize %d NElements %d \n",dMCT_ISize,dMCT_NElem);

  Int_t trackID;
  for (Int_t iLab = 0; iLab < dMCT_ISize; iLab++) {
    const auto& labs = labels->getLabels(iLab);
    //cout << "size = " << labs.size() << '\n';
    // see std::ostream& operator<<(std::ostream& os, const o2::MCCompLabel& c)
    // in MCCompLabel.cxx
    //cout << labs[0] << '\n';
    auto mccl = labs[0];
    //printf("Label %d track ID %d event ID %d source ID %d \n",iLab,mccl.getTrackID(),mccl.getEventID(),mccl.getSourceID());
  }

}

#endif
