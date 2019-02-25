// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   DigestWriterSpec.h

#ifndef O2_MFT_DIGESTWRITER_H_
#define O2_MFT_DIGESTWRITER_H_

#include "TFile.h"

#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"

using namespace o2::framework;
namespace o2
{
namespace MFT
{

class DigestWriter : public Task
{
 public:
  DigestWriter() = default;
  ~DigestWriter() = default;
  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;

 private:
  int mState = 0;
  std::unique_ptr<TFile> mFile = nullptr;
  std::unique_ptr<std::ofstream> mLogFile = nullptr;
};

/// create a processor spec
/// write ITS tracks a root file
framework::DataProcessorSpec getDigestWriterSpec();

} // namespace MFT
} // namespace o2

#endif /* O2_MFT_DIGESTWRITER */
