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

/// Example of accessing the digits of MCEvent digitized with continous readout

void readMFTDigits(std::string path = "./",
                   std::string digiFName = "mftdigits.root",
                   std::string runContextFName = "collisioncontext.root",
                   std::string mctruthFName = "o2sim.root")
{
  if (path.back() != '/') {
    path += '/';
  }

  std::unique_ptr<TFile> digiFile(TFile::Open((path + digiFName).c_str()));
  if (!digiFile || digiFile->IsZombie()) {
    LOG(ERROR) << "Failed to open input digits file " << (path + digiFName) << FairLogger::endl;
    return;
  }
  std::unique_ptr<TFile> rcFile(TFile::Open((path + runContextFName).c_str()));
  if (!rcFile || rcFile->IsZombie()) {
    LOG(ERROR) << "Failed to open runContext file " << (path + runContextFName) << FairLogger::endl;
    return;
  }

  TTree* digiTree = (TTree*)digiFile->Get("o2sim");
  if (!digiTree) {
    LOG(ERROR) << "Failed to get digits tree" << FairLogger::endl;
    return;
  }
  printf("MFTdigit entries = %lld \n",digiTree->GetEntries());

  std::vector<o2::ITSMFT::Digit>* dv = nullptr;
  digiTree->SetBranchAddress("MFTDigit", &dv);
  //printf("MFTDigit size = %zu \n",dv->size());
  
  o2::dataformats::MCTruthContainer<o2::MCCompLabel>* labels = nullptr;
  digiTree->SetBranchAddress("MFTDigitMCTruth", &labels);
  //printf("MFTDigitMCTruth IndexedSize %zu NElements %zu \n",labels->getIndexedSize(),labels->getNElements());

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

  //return;
  
  for (int iev = 0; iev < nEvents; iev++) {
    
    const auto& collision = intRecordsVec[iev];
    const auto& evParts = evPartsVec[iev];

    int nmixed = evParts.size();
    /*
    printf("MCEvent %d made of %d MC parts: ", iev, (int)evParts.size());
    for (auto evp : evParts) {
      printf(" [src%d entry %d]", evp.sourceID, evp.entryID);
    }
    printf("\n");
    */
    if (int(mc2rofVec->size()) <= iev || (*mc2rofVec)[iev].eventRecordID < 0) {
      LOG(WARNING) << "Event was not digitized" << FairLogger::endl;
      continue;
    }
    
    const auto& m2r = (*mc2rofVec)[iev];
    printf("Digitized to ROF %d - %d, entry %d in ROFRecords\n\n", m2r.minROF, m2r.maxROF, m2r.rofRecordID);

    int dgid0, dgid1;
    int rofEntry = m2r.rofRecordID;
    for (auto rof = m2r.minROF; rof <= m2r.maxROF; rof++) {
      
      const auto& rofrec = (*rofRecVec)[rofEntry];

      printf("ROF %d ROFentry %d event %d \n",rof,rofEntry,rofrec.getROFEntry().getEvent());
      //rofrec.print();
      
      digiTree->GetEntry(rofrec.getROFEntry().getEvent());
            
      // read 1st and last digit of concerned rof
      
      dgid0 = rofrec.getROFEntry().getIndex();
      dgid1 = rofrec.getROFEntry().getIndex() + rofrec.getNROFEntries() - 1;

      printf("digits %d to %d \n",dgid0,dgid1);

      for (int dgid = dgid0; dgid <= dgid1; dgid++) {
	const auto& digit = (*dv)[dgid];
        printf("digit %d chip %d row/col %d/%d q %d \n",dgid,digit.getChipIndex(),digit.getRow(),digit.getColumn(),digit.getCharge());
	const auto& labs = labels->getLabels(dgid);
	printf("MC labels size %ld \n",labs.size());
	for (long ilab = 0; ilab < labs.size(); ilab++) {
	  printf("MC track ID %d event ID %d source ID %d \n",labs[ilab].getTrackID(),labs[ilab].getEventID(),labs[ilab].getSourceID());
	}
      }
      
      const auto& digit0 = (*dv)[dgid0];
      const auto& labs0 = labels->getLabels(dgid0);
      //printf("First digit of this ROF (Entry: %6d) : ", dgid0);
      //digit0.print(std::cout);
      //printf(" MCinfo: ");
      //labs0[0].print();

      const auto& digit1 = (*dv)[dgid1];
      const auto& labs1 = labels->getLabels(dgid1);
      //printf("Last digit of this ROF (Entry: %6d) : ", dgid1);
      //digit1.print(std::cout);
      //printf(" MCinfo: ");
      //labs1[0].print();
      
      //
      
      rofEntry++;

    }

    printf("\n");

    if (iev == 0) break;
    
  } // end event loop
  
}

#endif
