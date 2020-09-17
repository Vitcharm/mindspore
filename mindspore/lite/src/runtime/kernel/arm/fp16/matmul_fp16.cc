/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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

#include "src/runtime/kernel/arm/fp16/matmul_fp16.h"
#include "nnacl/fp16/matmul_fp16.h"
#include "nnacl/fp16/cast_fp16.h"
#include "src/runtime/runtime_api.h"
#include "include/errorcode.h"
#include "src/kernel_registry.h"

using mindspore::lite::KernelRegistrar;
using mindspore::lite::RET_ERROR;
using mindspore::lite::RET_INPUT_TENSOR_ERROR;
using mindspore::lite::RET_MEMORY_FAILED;
using mindspore::lite::RET_OK;
using mindspore::schema::PrimitiveType_MatMul;

namespace mindspore::kernel {
MatmulFP16CPUKernel::~MatmulFP16CPUKernel() { FreeTmpBuffer(); }

void MatmulFP16CPUKernel::FreeTmpBuffer() {
  if (a_pack_ptr_ != nullptr) {
    ctx_->allocator->Free(a_pack_ptr_);
    a_pack_ptr_ = nullptr;
  }
  if (b_pack_ptr_ != nullptr) {
    ctx_->allocator->Free(b_pack_ptr_);
    b_pack_ptr_ = nullptr;
  }
  if (bias_ptr_ != nullptr) {
    ctx_->allocator->Free(bias_ptr_);
    bias_ptr_ = nullptr;
  }
  if (output_ptr_ != nullptr) {
    ctx_->allocator->Free(output_ptr_);
    output_ptr_ = nullptr;
  }
}

int MatmulFP16CPUKernel::ReSize() {
  FreeTmpBuffer();
  int batch = 1;
  auto a_shape = in_tensors_[0]->shape();
  auto c_shape = out_tensors_[0]->shape();
  if (in_tensors_.size() == 3) {
    auto bias_shape = in_tensors_[2]->shape();
    if (bias_shape[bias_shape.size() - 1] != c_shape[c_shape.size() - 1]) {
      MS_LOG(ERROR) << "The bias' dimension is not equal with column";
      return RET_INPUT_TENSOR_ERROR;
    }
  }

  for (size_t i = 0; i < a_shape.size() - 2; ++i) {
    batch *= a_shape[i];
  }
  params_->batch = batch;
  params_->row_ = c_shape[c_shape.size() - 2];
  params_->col_ = c_shape[c_shape.size() - 1];
  params_->deep_ = params_->a_transpose_ ? a_shape[a_shape.size() - 2] : a_shape[a_shape.size() - 1];
  params_->row_16_ = UP_ROUND(params_->row_, C16NUM);
  params_->col_8_ = UP_ROUND(params_->col_, C8NUM);
  thread_count_ = MSMIN(thread_count_, UP_DIV(params_->col_, C8NUM));
  thread_stride_ = UP_DIV(UP_DIV(params_->col_, C8NUM), thread_count_) * C8NUM;

  a_pack_ptr_ = reinterpret_cast<float16_t *>(
    ctx_->allocator->Malloc(params_->batch * params_->row_16_ * params_->deep_ * sizeof(float16_t)));
  if (a_pack_ptr_ == nullptr) {
    FreeTmpBuffer();
    return RET_MEMORY_FAILED;
  }
  memset(a_pack_ptr_, 0, params_->batch * params_->row_16_ * params_->deep_ * sizeof(float16_t));

  b_pack_ptr_ = reinterpret_cast<float16_t *>(
    ctx_->allocator->Malloc(params_->batch * params_->col_8_ * params_->deep_ * sizeof(float16_t)));
  if (b_pack_ptr_ == nullptr) {
    FreeTmpBuffer();
    return RET_MEMORY_FAILED;
  }
  memset(b_pack_ptr_, 0, params_->batch * params_->col_8_ * params_->deep_ * sizeof(float16_t));

  params_->a_const_ = (in_tensors_[0]->MutableData() != nullptr);
  params_->b_const_ = (in_tensors_[1]->MutableData() != nullptr);
  if (params_->a_const_ == true) {
    if (in_tensors_[0]->data_type() == kNumberTypeFloat32) {
      InitMatrixA(reinterpret_cast<float *>(in_tensors_[0]->MutableData()), a_pack_ptr_);
    } else {
      InitMatrixA(reinterpret_cast<float16_t *>(in_tensors_[0]->MutableData()), a_pack_ptr_);
    }
  }
  if (params_->b_const_ == true) {
    InitMatrixB(reinterpret_cast<float *>(in_tensors_[1]->MutableData()), b_pack_ptr_);
  }

  if (in_tensors_.size() == 3) {
    bias_ptr_ = reinterpret_cast<float16_t *>(ctx_->allocator->Malloc(params_->col_8_ * sizeof(float16_t)));
    if (bias_ptr_ == nullptr) {
      FreeTmpBuffer();
      return RET_MEMORY_FAILED;
    }
    memset(bias_ptr_, 0, params_->col_8_ * sizeof(float16_t));
    Float32ToFloat16(reinterpret_cast<float *>(in_tensors_[2]->MutableData()), bias_ptr_, params_->col_);
  }

  if (out_tensors_[0]->data_type() == kNumberTypeFloat32) {
    output_ptr_ = reinterpret_cast<float16_t *>(
      ctx_->allocator->Malloc(params_->batch * params_->row_ * params_->col_ * sizeof(float16_t)));
  }
  return RET_OK;
}

void MatmulFP16CPUKernel::InitMatrixA(float *a_ptr, float16_t *a_pack_ptr) {
  for (int i = 0; i < params_->batch; i++) {
    float *src = a_ptr + i * params_->deep_ * params_->row_;
    float16_t *dst = a_pack_ptr + i * params_->deep_ * params_->row_16_;
    if (params_->a_transpose_) {
      Fp32RowMajor2Fp16Row16Major(src, dst, params_->deep_, params_->row_);
    } else {
      Fp32RowMajor2Fp16Col16Major(src, dst, params_->row_, params_->deep_);
    }
  }
}

void MatmulFP16CPUKernel::InitMatrixA(float16_t *a_ptr, float16_t *a_pack_ptr) {
  for (int i = 0; i < params_->batch; i++) {
    float16_t *src = a_ptr + i * params_->deep_ * params_->row_;
    float16_t *dst = a_pack_ptr + i * params_->deep_ * params_->row_16_;
    if (params_->a_transpose_) {
      Fp16RowMajor2Fp16Row16Major(src, dst, params_->deep_, params_->row_);
    } else {
      Fp16RowMajor2Fp16Col16Major(src, dst, params_->row_, params_->deep_);
    }
  }
}

void MatmulFP16CPUKernel::InitMatrixB(float *b_ptr, float16_t *b_pack_ptr) {
  for (int i = 0; i < params_->batch; i++) {
    float *src = b_ptr + i * params_->deep_ * params_->col_;
    float16_t *dst = b_pack_ptr + i * params_->deep_ * params_->col_8_;
    if (params_->b_transpose_) {
      Fp32RowMajor2Fp16Col8Major(src, dst, params_->col_, params_->deep_);
    } else {
      Fp32RowMajor2Fp16Row8Major(src, dst, params_->deep_, params_->col_);
    }
  }
}

int MatmulFP16CPUKernel::Init() {
  if (!InferShapeDone()) {
    return RET_OK;
  }
  return ReSize();
}

int MatmulFP16CPUKernel::RunImpl(int task_id) {
  int cur_stride = params_->col_ - task_id * thread_stride_;
  int cur_oc = MSMIN(thread_stride_, cur_stride);
  if (cur_oc <= 0) {
    return RET_OK;
  }
  auto b = current_b_ + task_id * thread_stride_ * params_->deep_;
  auto bias = (bias_ptr_ == nullptr) ? nullptr : bias_ptr_ + thread_stride_ * task_id;
  auto c = current_c_ + task_id * thread_stride_;
  MatMulFp16(current_a_, b, c, bias, ActType_No, params_->deep_, params_->row_, cur_oc, params_->col_, true);

  return RET_OK;
}

int MatmulFP16Run(void *cdata, int task_id) {
  auto op = reinterpret_cast<MatmulFP16CPUKernel *>(cdata);
  auto error_code = op->RunImpl(task_id);
  if (error_code != RET_OK) {
    MS_LOG(ERROR) << "MatmulFp32Run error task_id[" << task_id << "] error_code[" << error_code << "]";
    return RET_ERROR;
  }
  return RET_OK;
}

int MatmulFP16CPUKernel::Run() {
  auto prepare_ret = Prepare();
  if (prepare_ret != RET_OK) {
    MS_LOG(ERROR) << "Prepare fail!ret: " << prepare_ret;
    return prepare_ret;
  }
  auto b = reinterpret_cast<float *>(in_tensors_[1]->MutableData());
  auto out_tensor = out_tensors_[0];
  float16_t *c_ptr;
  if (out_tensor->data_type() == kNumberTypeFloat32) {
    c_ptr = output_ptr_;
  } else {
    c_ptr = reinterpret_cast<float16_t *>(out_tensor->MutableData());
  }
  if (params_->a_const_ == false) {
    if (in_tensors_[0]->data_type() == kNumberTypeFloat32) {
      InitMatrixA(reinterpret_cast<float *>(in_tensors_[0]->MutableData()), a_pack_ptr_);
    } else {
      InitMatrixA(reinterpret_cast<float16_t *>(in_tensors_[0]->MutableData()), a_pack_ptr_);
    }
  }
  if (params_->b_const_ == false) {
    InitMatrixB(b, b_pack_ptr_);
  }
  for (int i = 0; i < params_->batch; ++i) {
    current_a_ = a_pack_ptr_ + i * params_->row_16_ * params_->deep_;
    current_b_ = b_pack_ptr_ + i * params_->deep_ * params_->col_8_;
    current_c_ = c_ptr + i * params_->row_ * params_->col_;
    ParallelLaunch(this->context_->thread_pool_, MatmulFP16Run, this, thread_count_);
  }
  if (out_tensor->data_type() == kNumberTypeFloat32) {
    auto size = out_tensor->ElementsNum();
    auto out_tensor_data = reinterpret_cast<float *>(out_tensor->MutableData());
    Float16ToFloat32(output_ptr_, out_tensor_data, size);
  }
  return RET_OK;
}

kernel::LiteKernel *CpuMatmulFp16KernelCreator(const std::vector<lite::Tensor *> &inputs,
                                               const std::vector<lite::Tensor *> &outputs, OpParameter *opParameter,
                                               const lite::InnerContext *ctx, const kernel::KernelKey &desc,
                                               const mindspore::lite::PrimitiveC *primitive) {
  auto *kernel = new (std::nothrow) MatmulFP16CPUKernel(opParameter, inputs, outputs, ctx, primitive);
  if (kernel == nullptr) {
    MS_LOG(ERROR) << "kernel is nullptr.";
    return nullptr;
  }
  auto ret = kernel->Init();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "Init kernel failed, name: " << opParameter->name_ << ", type: "
                  << schema::EnumNamePrimitiveType(static_cast<schema::PrimitiveType>(opParameter->type_));
    delete kernel;
    return nullptr;
  }
  return kernel;
}

REG_KERNEL(kCPU, kNumberTypeFloat16, PrimitiveType_MatMul, CpuMatmulFp16KernelCreator)
}  // namespace mindspore::kernel