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

#include "jhmrH5ProjDataIO.h"

#include <fmt/format.h>

#include "jhmrHDF5.h"
#include "internal/jhmrHDF5Internal.h"
#include "jhmrH5CamModelIO.h"

namespace
{

using namespace jhmr;

const char* kJHMR_PROJ_DATA_ATTR_STR = "proj-data";

template <class tPixelScalar>
void WriteProjDataH5Helper(const std::vector<ProjData<tPixelScalar>>& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  SetStringAttr("jhmr-type", kJHMR_PROJ_DATA_ATTR_STR, h5);

  const size_type num_projs = proj_data.size();

  WriteSingleScalarH5("num-projs", num_projs, h5);

  for (size_type i = 0; i < num_projs; ++i)
  {
    H5::Group proj_g = h5->createGroup(fmt::format("proj-{:03d}", i));

    // only add the image if it is non-null
    if (proj_data[i].img)
    {  
      H5::Group img_g = proj_g.createGroup("img");
      WriteImageH5(proj_data[i].img.GetPointer(), &img_g, compress);
    }

    H5::Group cam_g = proj_g.createGroup("cam");
    WriteCamModelH5(proj_data[i].cam, &cam_g);
  
    const auto& lands = proj_data[i].landmarks;
    if (!lands.empty())
    {
      H5::Group lands_g = proj_g.createGroup("landmarks");
      
      for (const auto& name_and_pt : lands)
      {
        WriteMatrixH5(name_and_pt.first, name_and_pt.second, &lands_g, false);
      }
    }
  }
}

template <class tPixelScalar>
void WriteProjDataH5Helper(const std::vector<CamImgPair<tPixelScalar>>& cam_img_pairs,
                           H5::CommonFG* h5, const bool compress)
{
  WriteProjDataH5Helper(CamImgPairsToProjData(cam_img_pairs), h5, compress);
}

}  // un-named

void jhmr::CopyProjDataH5(const H5::CommonFG& src_proj_h5, H5::CommonFG* dst_proj_h5)
{
  const auto* src_proj_g = dynamic_cast<const H5::IdComponent*>(&src_proj_h5);
  jhmrASSERT(src_proj_g);

  auto* dst_proj_g = dynamic_cast<H5::IdComponent*>(dst_proj_h5);
  jhmrASSERT(dst_proj_g);

  SetStringAttr("jhmr-type", kJHMR_PROJ_DATA_ATTR_STR, dst_proj_h5);
  
  const auto src_id = src_proj_g->getId();
  const auto dst_id = dst_proj_g->getId();

  auto copy_h5 = [&src_id, &dst_id](const std::string& n)
  {
    H5Ocopy(src_id, n.c_str(), dst_id, n.c_str(), H5P_DEFAULT, H5P_DEFAULT);
  };

  copy_h5("num-projs");

  const size_type num_projs = ReadSingleScalarH5ULong("num-projs", src_proj_h5);

  for (size_type i = 0; i < num_projs; ++i)
  {
    copy_h5(fmt::format("proj-{:03d}", i));
  }
}

void jhmr::ReadProjDataFromH5AndWriteToDisk(const H5::CommonFG& h5, const std::string& dst_disk_path)
{
  H5::H5File dst_h5(dst_disk_path, H5F_ACC_TRUNC);
 
  CopyProjDataH5(h5, &dst_h5);

  dst_h5.flush(H5F_SCOPE_GLOBAL);
  dst_h5.close();
}

void jhmr::WriteProjDataH5(const ProjDataF32List& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper(proj_data, h5, compress);
}

void jhmr::WriteProjDataH5(const ProjDataU16List& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper(proj_data, h5, compress);
}

void jhmr::WriteProjDataH5(const ProjDataU8List& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper(proj_data, h5, compress);
}

void jhmr::WriteProjDataH5(const ProjDataF32& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5(ProjDataF32List(1, proj_data), h5, compress);
}

void jhmr::WriteProjDataH5(const ProjDataU16& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5(ProjDataU16List(1, proj_data), h5, compress);
}

void jhmr::WriteProjDataH5(const ProjDataU8& proj_data,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5(ProjDataU8List(1, proj_data), h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairF32List& cam_img_pairs,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<float>(cam_img_pairs, h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairU16List& cam_img_pairs,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<unsigned short>(cam_img_pairs, h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairU8List& cam_img_pairs,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<unsigned char>(cam_img_pairs, h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairF32& cam_img_pair,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<float>(CamImgPairF32List(1, cam_img_pair), h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairU16& cam_img_pair,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<unsigned short>(CamImgPairU16List(1, cam_img_pair), h5, compress);
}

void jhmr::WriteProjDataH5(const CamImgPairU8& cam_img_pair,
                           H5::CommonFG* h5,
                           const bool compress)
{
  WriteProjDataH5Helper<unsigned char>(CamImgPairU8List(1, cam_img_pair), h5, compress);
}

namespace  // un-named
{

template <class tProjData>
void WriteProjDataH5ToDiskHelper(const tProjData& pd, const std::string& path, const bool compress)
{
  H5::H5File h5(path, H5F_ACC_TRUNC);
  
  jhmr::WriteProjDataH5(pd, &h5, compress);
  
  h5.flush(H5F_SCOPE_GLOBAL);
  h5.close();
}

}  // un-named

void jhmr::WriteProjDataH5ToDisk(const ProjDataF32List& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const ProjDataU16List& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const ProjDataU8List& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const ProjDataF32& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const ProjDataU16& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const ProjDataU8& proj_data,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(proj_data, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairF32List& cam_img_pairs,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pairs, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairU16List& cam_img_pairs,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pairs, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairU8List& cam_img_pairs,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pairs, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairF32& cam_img_pair,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pair, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairU16& cam_img_pair,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pair, path, compress);
}

void jhmr::WriteProjDataH5ToDisk(const CamImgPairU8& cam_img_pair,
                                 const std::string& path,
                                 const bool compress)
{
  WriteProjDataH5ToDiskHelper(cam_img_pair, path, compress);
}

namespace  // un-named
{

using namespace jhmr;

template <class tPixelScalar>
std::vector<ProjData<tPixelScalar>>
ReadProjDataHelper(const H5::CommonFG& h5, const bool read_pixels)
{
  using PixelScalar  = tPixelScalar;
  using ProjDataType = ProjData<PixelScalar>;
  using ProjDataList = std::vector<ProjDataType>;

  const size_type num_projs = ReadSingleScalarH5ULong("num-projs", h5);

  ProjDataList projs(num_projs);

  for (size_type i = 0; i < num_projs; ++i)
  {
    const std::string proj_g_path = fmt::format("proj-{:03d}", i);

    const H5::Group proj_g = h5.openGroup(proj_g_path);
    
    projs[i].cam = ReadCamModelH5(proj_g.openGroup("cam"));

    if (read_pixels)
    {
      projs[i].img = detail::ReadNDImageH5Helper<PixelScalar,2>(proj_g.openGroup("img"));
    }

    // Read landmarks if present
    try
    {
      HideH5ExceptionPrints suppress_exception_prints;
      
      const H5::Group lands_g = proj_g.openGroup("landmarks");
      
      const hsize_t num_lands = lands_g.getNumObjs();

      if (num_lands)
      {
        auto& m = projs[i].landmarks;

        m.reserve(num_lands);

        for (hsize_t l = 0; l < num_lands; ++l)
        {
          const std::string land_name = lands_g.getObjnameByIdx(l);

          m.emplace(land_name, ReadMatrixH5CoordScalar(land_name, lands_g));
        }
      }
    }
    catch (H5::GroupIException&)
    { }
  }

  return projs;
}

template <class tPixelScalar>
typename itk::Image<tPixelScalar,2>::Pointer
ReadSingleImgFromProjDataHelper(const H5::CommonFG& h5, const size_type proj_idx)
{
  const size_type num_projs = ReadSingleScalarH5ULong("num-projs", h5);

  jhmrASSERT(proj_idx < num_projs);
  
  return detail::ReadNDImageH5Helper<tPixelScalar,2>(h5.openGroup(fmt::format("proj-{:03d}/img", proj_idx)));
}

template <class tPixelScalar>
std::vector<ProjData<tPixelScalar>>
ReadProjDataH5FromDiskHelper(const std::string& path, const bool read_pixels)
{
  return ReadProjDataHelper<tPixelScalar>(H5::H5File(path, H5F_ACC_RDONLY), read_pixels);
}

template <class tPixelScalar>
typename itk::Image<tPixelScalar,2>::Pointer
ReadSingleImgFromProjDataFromDiskHelper(const std::string& path, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataHelper<tPixelScalar>(H5::H5File(path, H5F_ACC_RDONLY), proj_idx);
}

}  // un-named

jhmr::ProjDataF32List jhmr::ReadProjDataH5F32(const H5::CommonFG& h5, const bool read_pixels)
{
  return ReadProjDataHelper<float>(h5, read_pixels);
}

jhmr::ProjDataU16List jhmr::ReadProjDataH5U16(const H5::CommonFG& h5, const bool read_pixels)
{
  return ReadProjDataHelper<unsigned short>(h5, read_pixels);
}

jhmr::ProjDataU8List jhmr::ReadProjDataH5U8(const H5::CommonFG& h5, const bool read_pixels)
{
  return ReadProjDataHelper<unsigned char>(h5, read_pixels);
}

jhmr::ProjDataF32::ProjPtr jhmr::ReadSingleImgFromProjDataH5F32(const H5::CommonFG& h5, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataHelper<float>(h5, proj_idx);
}

jhmr::ProjDataU16::ProjPtr jhmr::ReadSingleImgFromProjDataH5U16(const H5::CommonFG& h5, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataHelper<unsigned short>(h5, proj_idx);
}

jhmr::ProjDataU8::ProjPtr jhmr::ReadSingleImgFromProjDataH5U8(const H5::CommonFG& h5, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataHelper<unsigned char>(h5, proj_idx);
}

jhmr::ProjDataF32List jhmr::ReadProjDataH5F32FromDisk(const std::string& path, const bool read_pixels)
{
  return ReadProjDataH5FromDiskHelper<float>(path, read_pixels);
}

jhmr::ProjDataU16List jhmr::ReadProjDataH5U16FromDisk(const std::string& path, const bool read_pixels)
{
  return ReadProjDataH5FromDiskHelper<unsigned short>(path, read_pixels);
}

jhmr::ProjDataU8List jhmr::ReadProjDataH5U8FromDisk(const std::string& path, const bool read_pixels)
{
  return ReadProjDataH5FromDiskHelper<unsigned char>(path, read_pixels);
}

jhmr::ProjDataF32::ProjPtr
jhmr::ReadSingleImgFromProjDataH5F32FromDisk(const std::string& path, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataFromDiskHelper<float>(path, proj_idx);
}

jhmr::ProjDataU16::ProjPtr
jhmr::ReadSingleImgFromProjDataH5U16FromDisk(const std::string& path, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataFromDiskHelper<unsigned short>(path, proj_idx);
}

jhmr::ProjDataU8::ProjPtr
jhmr::ReadSingleImgFromProjDataH5U8FromDisk(const std::string& path, const size_type proj_idx)
{
  return ReadSingleImgFromProjDataFromDiskHelper<unsigned char>(path, proj_idx);
}

namespace  // un-named
{

using namespace jhmr;

template <class tDstScalar, class tSrcScalar>
std::vector<ProjData<tDstScalar>>
CastProjData(const std::vector<ProjData<tSrcScalar>>& src_pd)
{
  const size_type num_projs = src_pd.size();

  std::vector<ProjData<tDstScalar>> dst_pd(num_projs);

  return dst_pd;
}

template <class tScalar>
typename ProjData<tScalar>::ProjPtr
DeferredProjReaderHelpler(std::vector<ProjData<tScalar>>* pd_ptr,
                          const size_type proj_idx,
                          const std::string& pd_path_on_disk,
                          const bool cache_imgs)
{
  using Scalar  = tScalar;
  using PD      = ProjData<Scalar>;
  using ProjPtr = typename PD::ProjPtr;

  auto& pd = *pd_ptr;

  jhmrASSERT(proj_idx < pd.size());
  
  ProjPtr to_ret;
  
  if (!cache_imgs || !pd[proj_idx].img)
  {
    to_ret = ReadSingleImgFromProjDataFromDiskHelper<Scalar>(pd_path_on_disk, proj_idx);
  }

  if (cache_imgs)
  {
    pd[proj_idx].img = to_ret;
  }

  return to_ret;
}

}  // un-namd

jhmr::DeferredProjReader::DeferredProjReader(const std::string& path, const bool cache_imgs)
  : orig_path_(path), cache_imgs_(cache_imgs)
{
  // only reads camera models and landmarks - no image pixels
  proj_data_F32_ = ReadProjDataH5F32FromDisk(path, false);

  proj_data_U16_ = CastProjData<unsigned short>(proj_data_F32_);
  proj_data_U8_  = CastProjData<unsigned char>(proj_data_F32_);
}
  
const jhmr::ProjDataF32List&
jhmr::DeferredProjReader::proj_data_F32()
{
  return proj_data_F32_;
}

const jhmr::ProjDataU16List&
jhmr::DeferredProjReader::proj_data_U16()
{
  return proj_data_U16_;
}

const jhmr::ProjDataU8List&
jhmr::DeferredProjReader::proj_data_U8()
{
  return proj_data_U8_;
}

jhmr::ProjDataF32::ProjPtr
jhmr::DeferredProjReader::read_proj_F32(const size_type proj_idx)
{
  return DeferredProjReaderHelpler<float>(&proj_data_F32_, proj_idx, orig_path_, cache_imgs_);
}

jhmr::ProjDataU16::ProjPtr
jhmr::DeferredProjReader::read_proj_U16(const size_type proj_idx)
{
  return DeferredProjReaderHelpler<unsigned short>(&proj_data_U16_, proj_idx, orig_path_, cache_imgs_);
}

jhmr::ProjDataU8::ProjPtr
jhmr::DeferredProjReader::read_proj_U8(const size_type proj_idx)
{
  return DeferredProjReaderHelpler<unsigned char>(&proj_data_U8_, proj_idx, orig_path_, cache_imgs_);
}
