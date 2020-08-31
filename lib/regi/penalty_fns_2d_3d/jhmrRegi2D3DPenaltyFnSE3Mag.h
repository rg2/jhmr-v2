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

#ifndef JHMRREGI2D3DPENALTYFNSE3MAG_H_
#define JHMRREGI2D3DPENALTYFNSE3MAG_H_

#include "jhmrRegi2D3DPenaltyFn.h"

namespace jhmr
{

// Forward declarations:
class Dist;

class Regi2D3DPenaltyFnSE3Mag : public Regi2D3DPenaltyFn
{
public:
  using DistPtr = std::shared_ptr<Dist>;

  std::vector<DistPtr>   rot_pdfs_per_obj;
  std::vector<DistPtr> trans_pdfs_per_obj;

  std::vector<bool> apply_reg_for_obj;

  void compute(const ListOfFrameTransformLists& cams_wrt_objs,
               const size_type num_projs,
               const CamList& cams,
               const CamAssocList& cam_assocs,
               const std::vector<bool>& intermediate_frames_wrt_vol,
               const FrameTransformList& intermediate_frames,
               const FrameTransformList& regi_xform_guesses,
               const ListOfFrameTransformLists* xforms_from_opt) override;
};

}  // jhmr

#endif


