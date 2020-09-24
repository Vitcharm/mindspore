/**
 * Copyright 2019 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GRAD_BN_GRAD_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GRAD_BN_GRAD_H_

#include <vector>
#include "src/lite_kernel.h"

namespace mindspore::kernel {

class BNGradCPUKernel : public LiteKernel {
 public:
  explicit BNGradCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                           const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx,
                           const mindspore::lite::PrimitiveC *primitive)
      : LiteKernel(parameter, inputs, outputs, ctx, primitive), workspace(nullptr), workspace_size(0) {}
  ~BNGradCPUKernel() override {
    if (workspace) delete[] workspace;
  }

  int Init() override;
  int ReSize() override;
  int Run() override;

 private:
  float *workspace;
  int workspace_size;
};
}  // namespace mindspore::kernel
#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GRAD_BN_GRAD_H_
