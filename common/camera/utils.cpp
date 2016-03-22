/*
 * This file is part of PISAK project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2015, Alex Khrabrov <alex@mroja.net>
 *
 */

#include "utils.h"

#include <opencv2/opencv.hpp>

static void matDeleter(void * mat)
{
    if(mat)
        delete static_cast<cv::Mat *>(mat);
}

QImage convertMatToQImage(const cv::Mat & input_img, bool bgr2rgb)
{
    cv::Mat tmp;
    if(bgr2rgb)
        cv::cvtColor(input_img, tmp, CV_BGR2RGB);
    else
        tmp = input_img;
    return QImage(tmp.data, tmp.cols, tmp.rows, tmp.step,
                  QImage::Format_RGB888, &matDeleter, new cv::Mat(tmp));
}
