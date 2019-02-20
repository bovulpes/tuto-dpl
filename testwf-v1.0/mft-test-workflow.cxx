// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "MFTTestwf/TestWorkflow.h"

using namespace o2::framework;

// we need to add workflow options before including Framework/runDataProcessing
void customize(std::vector<o2::framework::ConfigParamSpec>& workflowOptions)
{

  int wfopt1_val = -9999;
  std::string wfopt1_help("MFT workflow test option 1 (int value)");
  workflowOptions.push_back(
    ConfigParamSpec{ "mft-opt-1", VariantType::Int, wfopt1_val, { wfopt1_help } });

  std::string wfopt2_help("MFT workflow test option 2 (default is all)");
  workflowOptions.push_back(
    ConfigParamSpec{ "mft-opt-2", VariantType::String, "all", { wfopt2_help } });
}

#include "Framework/runDataProcessing.h"

WorkflowSpec defineDataProcessing(ConfigContext const& configcontext)
{

  auto wfopt1_val = configcontext.options().get<int>("mft-opt-1");
  LOG(INFO) << "MFT workflow test option 1 = " << wfopt1_val;

  return std::move(o2::MFT::TestWorkflow::getWorkflow());
}
