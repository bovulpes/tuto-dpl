// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   RecoWorkflow.cxx

#include "MFTTestwf/TestWorkflow.h"

#include "MFTTestwf/DigitReaderSpec.h"
#include "MFTTestwf/DigitDigestSpec.h"
#include "MFTTestwf/DigestWriterSpec.h"
#include "MFTTestwf/ClustererSpec.h"
#include "MFTTestwf/ClusterWriterSpec.h"

namespace o2
{
namespace MFT
{

namespace TestWorkflow
{

framework::WorkflowSpec getWorkflow()
{
  framework::WorkflowSpec specs;

  specs.emplace_back(o2::MFT::getDigitReaderSpec());
  specs.emplace_back(o2::MFT::getDigitDigestSpec());
  specs.emplace_back(o2::MFT::getDigestWriterSpec());
  specs.emplace_back(o2::MFT::getClustererSpec());
  specs.emplace_back(o2::MFT::getClusterWriterSpec());

  return specs;
}

} // namespace TestWorkflow

} // namespace MFT
} // namespace o2
  
