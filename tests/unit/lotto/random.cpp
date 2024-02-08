#include <gtest/gtest.h>

#include <algorithm>
#include <lotto/random.hpp>
#include <vector>

#include "statistics.hpp"
#include "test_parameters.hpp"

class RandomGeneratorTest : public testing::Test {
 protected:
  /// Random generator for testing
  lotto::RandomGenerator generator;
};

TEST_F(RandomGeneratorTest, Construct) {
  // Checks if RandomGenerator can be constructed
}

TEST_F(RandomGeneratorTest, GetSeed) {
  // Checks if get_seed returns the correct value
  for (lotto::UIntType i = 0; i < 100; ++i) {
    generator.reseed_generator(i);
    EXPECT_EQ(generator.get_seed(), i);
  }
}

TEST_F(RandomGeneratorTest, DefaultSeedNotFixed) {
  // Checks that the default seed is not fixed
  lotto::UIntType first_seed = generator.get_seed();
  bool is_seed_different = false;
  int n_attempts = 100;
  for (int i = 0; i < n_attempts; ++i) {
    lotto::RandomGenerator new_generator;
    if (new_generator.get_seed() != first_seed) {
      is_seed_different = true;
      break;
    }
  }
  EXPECT_TRUE(is_seed_different)
      << "Seed value is unchanged after " << n_attempts
      << " attempts, random_device implementation appears to be deterministic";
}

TEST_F(RandomGeneratorTest, IntegerRangeSamples) {
  // Checks that values from sample_integer_range behave as expected
  generator.reseed_generator(TEST_SEED);  // fixed seed for testing
  lotto::UIntType min_value = 0;
  lotto::UIntType max_value = 1000;
  int n_samples = 100000000;
  std::vector<lotto::UIntType> samples(n_samples);
  for (int i = 0; i < n_samples; ++i) {
    samples[i] = generator.sample_integer_range(max_value);
  }
  check_samples_from_uniform_distribution(min_value, max_value, samples);
}

TEST_F(RandomGeneratorTest, UnitIntervalSamples) {
  // Checks that values from sample_unit_interval behave as expected
  generator.reseed_generator(TEST_SEED);  // fixed seed for testing
  double min_value = 0.0;
  double max_value = 1.0;
  int n_samples = 100000000;
  std::vector<double> samples(n_samples);
  for (int i = 0; i < n_samples; ++i) {
    samples[i] = generator.sample_unit_interval();
  }
  check_samples_from_uniform_distribution(min_value, max_value, samples);
}

class RandomGeneratorT_Test : public testing::Test {
 protected:
  /// Random generator for testing
  std::shared_ptr<lotto::RandomGeneratorT<std::minstd_rand>> generator;

  void default_construct() {
    generator = std::make_shared<lotto::RandomGeneratorT<std::minstd_rand>>();
  }

  void construct_with_shared_engine() {
    std::random_device device;
    std::shared_ptr<std::minstd_rand> engine =
        std::make_shared<std::minstd_rand>(device());
    generator =
        std::make_shared<lotto::RandomGeneratorT<std::minstd_rand>>(engine);
  }
};

TEST_F(RandomGeneratorT_Test, DefaultConstruct) {
  // Checks if RandomGeneratorT can be default constructed
  default_construct();
}

TEST_F(RandomGeneratorT_Test, SharedEngineConstruct) {
  // Checks if RandomGeneratorT can be constructed
  // with a shared engine
  construct_with_shared_engine();

  EXPECT_TRUE(generator->get_engine() != nullptr);
}

TEST_F(RandomGeneratorT_Test, IntegerRangeSamples) {
  default_construct();

  // Checks that values from sample_integer_range behave as expected
  generator->reseed_generator(TEST_SEED);  // fixed seed for testing
  lotto::UIntType min_value = 0;
  lotto::UIntType max_value = 1000;
  int n_samples = 100000000;
  std::vector<lotto::UIntType> samples(n_samples);
  for (int i = 0; i < n_samples; ++i) {
    samples[i] = generator->sample_integer_range(max_value);
  }
  check_samples_from_uniform_distribution(min_value, max_value, samples);
}

TEST_F(RandomGeneratorT_Test, UnitIntervalSamples) {
  default_construct();

  // Checks that values from sample_unit_interval behave as expected
  generator->reseed_generator(TEST_SEED);  // fixed seed for testing
  double min_value = 0.0;
  double max_value = 1.0;
  int n_samples = 100000000;
  std::vector<double> samples(n_samples);
  for (int i = 0; i < n_samples; ++i) {
    samples[i] = generator->sample_unit_interval();
  }
  check_samples_from_uniform_distribution(min_value, max_value, samples);
}
