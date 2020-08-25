/*
 * MIT License
 *
 * Copyright (c) 2020 Robert Grupp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef JHMRIMGSIMMETRIC2DGRADNCCOCL_H_
#define JHMRIMGSIMMETRIC2DGRADNCCOCL_H_

#include "jhmrImgSimMetric2DGradImgOCL.h"
#include "jhmrImgSimMetric2DNCCOCL.h"

namespace jhmr
{

class ImgSimMetric2DGradNCCOCL : public ImgSimMetric2DGradImgOCL
{
public:
  ImgSimMetric2DGradNCCOCL();

  explicit ImgSimMetric2DGradNCCOCL(const boost::compute::device& dev);

  ImgSimMetric2DGradNCCOCL(const boost::compute::context& ctx,
                           const boost::compute::command_queue& queue);

  void allocate_resources() override;

  void compute() override;

protected:
  void process_mask() override;

private:
  ImgSimMetric2DNCCOCL grad_x_sim_;
  ImgSimMetric2DNCCOCL grad_y_sim_;
};

}  // jhmr

#endif

