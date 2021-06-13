/*************************************************************************/
/*  resource_importer_lunasvg.h	                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifndef RESOURCE_IMPORTER_LUNASVG
#define RESOURCE_IMPORTER_LUNASVG

#include "core/error/error_macros.h"
#endif // RESOURCE_IMPORTER_LUNASVG
#include "core/io/image_loader.h"
#include "core/string/ustring.h"

#include <iostream>
#include <memory>
#include <sstream>

#include "include/document.h"
#include <stdint.h>

#include "core/templates/local_vector.h"


class ImageLoaderLunaSVG : public ImageFormatLoader {

public:
  static void create_image_from_string(Ref<Image> p_image, String p_string, float p_scale, bool upsample, bool p_convert_color) {
      using namespace lunasvg;
      Vector<uint8_t> data = p_string.to_utf8_buffer();
      std::unique_ptr<Document> document =
          Document::loadFromData((const char *)data.ptr(), data.size());
      std::uint32_t width = document->width(), height = document->height();
      width *= p_scale;
      height *= p_scale;
      std::uint32_t bgColor = 0x00000000;
      Bitmap bitmap = document->renderToBitmap(width, height, bgColor);
      ERR_FAIL_COND(!bitmap.valid());
      size_t size = width * height * 4; // RGBA8
      Vector<uint8_t> result;
      result.resize(size);
      memcpy(result.ptrw(), bitmap.data(), size);
      p_image->create(width, height, false, Image::FORMAT_RGBA8, result);
  }
  virtual Error load_image(Ref<Image> p_image, FileAccess *p_fileaccess,
                           bool p_force_linear, float p_scale) override {
    String svg = p_fileaccess->get_as_utf8_string();
    create_image_from_string(p_image, svg, p_scale, false, false);
    ERR_FAIL_COND_V(p_image->is_empty(), FAILED);
    if (p_force_linear) {
      p_image->srgb_to_linear();
    }
    return OK;
  }

  virtual void
  get_recognized_extensions(List<String> *p_extensions) const override {
    p_extensions->push_back("svg");
    p_extensions->push_back("svgz");
  }

public:
  virtual ~ImageLoaderLunaSVG() {}
};