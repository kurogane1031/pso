#include "pso.h"
#include <gtest/gtest.h>

TEST(TestName, objective_float_add){
  std::vector<float> var = { 1.0, 2.001, 3.8459, 6.0123, 7.9991, 8.4312 };
  float sum = var[0] * var[0] + var[1] * var[1] + var[2] * var[2] +
              var[3] * var[3] + var[4] * var[4] + var[5] * var[5];
  ASSERT_FLOAT_EQ(objective_sphere(var), sum);
}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
