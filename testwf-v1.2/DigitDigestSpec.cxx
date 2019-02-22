// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   DigitDigestSpec.cxx

#include <vector>

#include "MFTTestwf/DigitDigestSpec.h"

#include "TTree.h"
#include "Framework/ControlService.h"
#include "ITSMFTBase/Digit.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DataFormatsITSMFT/ROFRecord.h"

using namespace o2::framework;
using namespace o2::ITSMFT;

namespace o2
{
namespace MFT
{

void DigitDigest::init(InitContext& ic)
{
  mState = 1;
}

void DigitDigest::run(ProcessingContext& pc)
{
  if (mState != 1)
    return;

  auto mftDigest = pc.outputs().make<Digest>(OutputRef{"digitdigest"}, 1);
  mftDigest.at(0).inputCount = pc.inputs().size();
	
  auto digits = pc.inputs().get<const std::vector<o2::ITSMFT::Digit>>("digits");
  mftDigest.at(0).digitsCount = digits.size();

  mState = 2;
  pc.services().get<ControlService>().readyToQuit(true);
}

DataProcessorSpec getDigitDigestSpec()
{
  return DataProcessorSpec{
    "mft-digit-digest",
    Inputs{
      InputSpec{ "digits", "MFT", "DIGITS"} },
    Outputs{
      OutputSpec{ {"digitdigest"}, "MFT", "DIGITDIGEST" } },
      AlgorithmSpec{
      /*
      [](ProcessingContext& ctx) {
        auto mftDigest = ctx.outputs().make<Digest>(OutputRef{"digitdigest"}, 1);
	mftDigest.at(0).inputCount = ctx.inputs().size();
      }
      */
      adaptFromTask<DigitDigest>()
      },
    Options{}
  };
}

} // namespace MFT
} // namespace o2
