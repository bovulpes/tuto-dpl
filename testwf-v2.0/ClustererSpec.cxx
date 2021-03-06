// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   ClustererSpec.cxx

#include <vector>

#include "MFTTestwf/ClustererSpec.h"

#include "MFTBase/GeometryTGeo.h"

#include "ITSMFTBase/Digit.h"
#include "ITSMFTReconstruction/ChipMappingMFT.h"
#include "ITSMFTReconstruction/DigitPixelReader.h"

#include "Framework/ControlService.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DetectorsBase/GeometryManager.h"
#include "DataFormatsITSMFT/CompCluster.h"
#include "DataFormatsITSMFT/Cluster.h"
#include "DataFormatsITSMFT/ROFRecord.h"

using namespace o2::framework;

namespace o2
{
namespace MFT
{

void ClustererDPL::init(InitContext& ic)
{
  o2::Base::GeometryManager::loadGeometry(); // for generating full clusters
  o2::MFT::GeometryTGeo* geom = o2::MFT::GeometryTGeo::Instance();
  geom->fillMatrixCache(o2::utils::bit2Mask(o2::TransformType::T2L));

  mClusterer = std::make_unique<o2::ITSMFT::Clusterer>();
  mClusterer->setGeometry(geom);
  //mClusterer->setNChips(o2::ITSMFT::ChipMappingMFT::getNChips()); // FIXME !
  mClusterer->setNChips(geom->getNumberOfChips());

  //mClusterer->setMaskOverflowPixels(false);

  auto filename = ic.options().get<std::string>("mft-dictionary-file");
  mFile = std::make_unique<std::ifstream>(filename.c_str(), std::ios::in | std::ios::binary);
  if (mFile->good()) {
    mClusterer->loadDictionary(filename);
    LOG(INFO) << "MFTClusterer running with a provided dictionary: " << filename.c_str();
    mState = 1;
  } else {
    LOG(INFO) << "MFTClusterer running without a dictionary";
    mState = 1;
    //LOG(WARNING) << "Cannot open the " << filename.c_str() << " file !";
    //mState = 0;
  }

  mClusterer->print();
}

void ClustererDPL::run(ProcessingContext& pc)
{
  if (mState > 1)
    return;

  auto digits = pc.inputs().get<const std::vector<o2::ITSMFT::Digit>>("digits");
  auto labels = pc.inputs().get<const o2::dataformats::MCTruthContainer<o2::MCCompLabel>*>("labels");
  auto rofs = pc.inputs().get<const std::vector<o2::ITSMFT::ROFRecord>>("ROframes");
  auto mc2rofs = pc.inputs().get<const std::vector<o2::ITSMFT::MC2ROFRecord>>("MC2ROframes");

  LOG(INFO) << "MFTClusterer pulled " << digits.size() << " digits, "
            << labels->getIndexedSize() << " MC label objects, in "
            << rofs.size() << " RO frames and "
            << mc2rofs.size() << " MC events";

  o2::ITSMFT::DigitPixelReader reader;
  reader.setDigits(&digits);
  reader.setDigitsMCTruth(labels.get());
  reader.init();

  std::vector<o2::ITSMFT::CompClusterExt> compClusters;
  std::vector<o2::ITSMFT::Cluster> clusters;
  o2::dataformats::MCTruthContainer<o2::MCCompLabel> clusterLabels;
  std::vector<o2::ITSMFT::ROFRecord> clusterROframes;                  // To be filled in future
  std::vector<o2::ITSMFT::MC2ROFRecord>& clusterMC2ROframes = mc2rofs; // Simply, replicate it from digits ?

  mClusterer->process(reader, &clusters, &compClusters, &clusterLabels);

  LOG(INFO) << "MFTClusterer pushed " << clusters.size() << " clusters, in "
            << clusterROframes.size() << " RO frames and "
            << clusterMC2ROframes.size() << " MC events";

  pc.outputs().snapshot(Output{ "MFT", "COMPCLUSTERS", 0, Lifetime::Timeframe }, compClusters);
  pc.outputs().snapshot(Output{ "MFT", "CLUSTERS", 0, Lifetime::Timeframe }, clusters);
  pc.outputs().snapshot(Output{ "MFT", "CLUSTERSMCTR", 0, Lifetime::Timeframe }, clusterLabels);
  pc.outputs().snapshot(Output{ "MFT", "MFTClusterROF", 0, Lifetime::Timeframe }, clusterROframes);
  pc.outputs().snapshot(Output{ "MFT", "MFTClusterMC2ROF", 0, Lifetime::Timeframe }, clusterMC2ROframes);

  mState = 2;
  //pc.services().get<ControlService>().readyToQuit(true);
}

DataProcessorSpec getClustererSpec()
{
  return DataProcessorSpec{
    "mft-clusterer",
    Inputs{
      InputSpec{ "digits", "MFT", "DIGITS", 0, Lifetime::Timeframe },
      InputSpec{ "labels", "MFT", "DIGITSMCTR", 0, Lifetime::Timeframe },
      InputSpec{ "ROframes", "MFT", "MFTDigitROF", 0, Lifetime::Timeframe },
      InputSpec{ "MC2ROframes", "MFT", "MFTDigitMC2ROF", 0, Lifetime::Timeframe } },
    Outputs{
      OutputSpec{ "MFT", "COMPCLUSTERS", 0, Lifetime::Timeframe },
      OutputSpec{ "MFT", "CLUSTERS", 0, Lifetime::Timeframe },
      OutputSpec{ "MFT", "CLUSTERSMCTR", 0, Lifetime::Timeframe },
      OutputSpec{ "MFT", "MFTClusterROF", 0, Lifetime::Timeframe },
      OutputSpec{ "MFT", "MFTClusterMC2ROF", 0, Lifetime::Timeframe } },
    AlgorithmSpec{ adaptFromTask<ClustererDPL>() },
    Options{
      { "mft-dictionary-file", VariantType::String, "complete_dictionary.bin", { "Name of the cluster-topology dictionary file" } } }
  };
}

} // namespace MFT
} // namespace o2
