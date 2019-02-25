// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   DigestWriterSpec.cxx

#include <vector>
#include <fstream>

#include "TTree.h"

#include "MFTTestwf/DigestWriterSpec.h"
#include "MFTTestwf/DigitDigestSpec.h"

#include "Framework/ControlService.h"

using namespace o2::framework;

namespace o2
{
namespace MFT
{

void DigestWriter::init(InitContext& ic)
{
  auto filename = ic.options().get<std::string>("mft-digest-outfile");
  mFile = std::make_unique<TFile>(filename.c_str(), "RECREATE");
  if (!mFile->IsOpen()) {
    LOG(ERROR) << "Cannot open the " << filename.c_str() << " file !";
    mState = 0;
    return;
  }
  
  auto logfilename = ic.options().get<std::string>("mft-digest-logfile");
  mLogFile = std::make_unique<std::ofstream>(logfilename.c_str(), std::ofstream::out);
  if (!mLogFile->is_open()) {
    LOG(ERROR) << "Cannot open the " << logfilename.c_str() << " log file !";
    mState = 0;
    return;
  }
  LOG(INFO) << "Open the log file " << logfilename.c_str();

  mState = 1;
}

void DigestWriter::run(ProcessingContext& pc)
{
  if (mState != 1)
    return;

  auto dd = pc.inputs().get<Digest>("digitdigest");

  LOG(INFO) << "DigitDigest: inputCount = " << dd.inputCount << " digitsCount = " << dd.digitsCount;

  *mLogFile << "DigitDigest: inputCount = " << dd.inputCount << " digitsCount = " << dd.digitsCount << '\n';
  
  mLogFile->close();

  mFile->Close();

  //std::ofstream ofs { "mft-digest-logfile-test" };
  //ofs << "DigitDigest: inputCount = " << dd.inputCount << " digitsCount = " << dd.digitsCount << '\n';
  //ofs.close();
  
  mState = 2;
  pc.services().get<ControlService>().readyToQuit(true);
}

DataProcessorSpec getDigestWriterSpec()
{
  return DataProcessorSpec{
    "mft-digest-writer",
    Inputs{
      InputSpec{ "digitdigest", "MFT", "DIGITDIGEST" } },
    Outputs{},
    AlgorithmSpec{ adaptFromTask<DigestWriter>() },
    Options{
      { "mft-digest-outfile", VariantType::String, "mft_digest.root", { "Name of the output file" } },
      { "mft-digest-logfile", VariantType::String, "mft_digest.log", { "Name of the output log file" } } }
  };
}

} // namespace MFT
} // namespace o2
