/**
 * @file utils_tests.cpp
 * @author Kartik Dutt
 *
 * Tests for various functionalities of models.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#define BOOST_TEST_DYN_LINK
#include <utils/utils.hpp>
#include <dataloader/datasets.hpp>
#include <dataloader/dataloader.hpp>
#include <models/lenet/lenet.hpp>
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(ModelTest);

template<
    typename OptimizerType,
    typename OutputLayerType = mlpack::ann::NegativeLogLikelihood<>, 
    typename InitializationRuleType = mlpack::ann::RandomInitialization,
    class MetricType = mlpack::metric::SquaredEuclideanDistance,
    typename InputType = arma::mat,
    typename OutputType = arma::mat
>
void CheckFFNWeights(mlpack::ann::FFN<OutputLayerType, InitializationRuleType>& model,
    const std::string& datasetName, const double threshold, const bool takeMean,
    const OptimizerType& optimizer)
{
  DataLoader<InputType, OutputType> dataloader(datasetName, true);
  // Train the model. Note: Callbacks such as progress bar and loss aren't used in testing.
  // Training the model for few epochs ensures that a user can use the pretrained model
  // on any other dataset.
  model.Train(dataloader.TrainX(), dataloader.TrainY(), optimizer);
  // Verify viability of model on validation datset.
  OutputType predictions;
  model.Predict(dataloader.ValidX(), predictions);
  double error = MetricType::Evaluate(predictions, dataloader.ValidY());
  if (takeMean)
  {
    error = error / predictions.n_elem;
  }

  BOOST_REQUIRE_LE(error, threshold);
}
/**
 * Test for sequential model.
 *
 * @param layer Sequential layer that contains the model.
 * @param datasetName Dataset which will be used for training and
 *                    validation.
 * @param threshold Maximum error for given metric.
 * @param takeMean Determines whether or not to take mean in error.
 * @param optimizer Optimizer that will be used for training.
 *
 * @tparam OptimizerType Optimizer type from ensmallen.
 * @tparam OutputLayerType The output layer type used to evaluate the network.
 * @tparam InitializationRuleType Rule used to initialize the weight matrix.
 * @tparam MetricType Metric that will be used 
 */
template<
    typename OptimizerType,
    typename OutputLayerType = mlpack::ann::NegativeLogLikelihood<>, 
    typename InitializationRuleType = mlpack::ann::RandomInitialization,
    class MetricType = mlpack::metric::SquaredEuclideanDistance,
    typename InputType = arma::mat,
    typename OutputType = arma::mat
>
void CheckSequentialModel(mlpack::ann::Sequential<>* layer,
    const std::string& datasetName, const double threshold,
    const bool takeMean, const OptimizerType& optimizer)
{
  // We can run two tests for sequential layer.
  // 1. Can it be attached to other models.
  // 2. Used as an FFN for training / inference.
  mlpack::ann::FFN<OutputLayerType, InitializationRuleType> model;
  model.Add<mlpack::ann::IdentityLayer<>>();
  model.Add(layer);
  CheckFFNWeights<OptimizerType, OutputLayerType, InitializationRuleType,
      MetricType, InputType, OutputType>(model, datasetName, threshold,
      takeMean, optimizer);
  // Using Layer in another model.
}

/**
 * Simple test for Le-Net model.
 */
BOOST_AUTO_TEST_CASE(LeNetModelTest)
{
  // Add tests here.
}

BOOST_AUTO_TEST_SUITE_END();
