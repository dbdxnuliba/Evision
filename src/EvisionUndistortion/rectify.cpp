/* ----------------------------------------------------------------------------
 * Robotics Laboratory, Westphalian University of Applied Science
 * ----------------------------------------------------------------------------
 * Project			: 	Stereo Vision Project
 * Revision			: 	1.0
 * Recent changes	: 	18.06.2014
 * ----------------------------------------------------------------------------
 * LOG:
 * ----------------------------------------------------------------------------
 * Developer		: 	Dennis Luensch 		(dennis.luensch@gmail.com)
						Tom Marvin Liebelt	(fh@tom-liebelt.de)
						Christian Blesing	(christian.blesing@gmail.com)
 * License 			: 	BSD
 *
 * Copyright (c) 2014, Dennis L??nsch, Tom Marvin Liebelt, Christian Blesing
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * # Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * # Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * # Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ------------------------------------------------------------------------- */

#include "rectify.h"

bool intrinsicExtrinsic::undistortStereoImages(std::vector<cv::Mat> &inputImages, std::vector<cv::Mat> &outputImages,
                                               cv::Mat &cameraMatrix1, cv::Mat &cameraMatrix2,
                                               cv::Mat &distCoeffs1, cv::Mat &distCoeffs2)
{
	bool undistortSuccess = false;

	if (inputImages.size() % 2 == 0)    // Nur wenn fuer die linke und rechte Kamera gleich viele Bilder existieren.
	{
		for (int i = 0; i < (inputImages.size() / 2); ++i)
		{
			cv::Mat undistort1, undistort2;
			undistort(inputImages[i * 2], undistort1, cameraMatrix1, distCoeffs1);
			undistort(inputImages[i * 2 + 1], undistort2, cameraMatrix2, distCoeffs2);

			outputImages.push_back(undistort1);
			outputImages.push_back(undistort2);
		}

		undistortSuccess = true;
	}
	else
	{
		std::cout<<"undistortStereoImages"<<", Odd number of images! Even number of images required!"<<std::endl;
	}

	return undistortSuccess;

}
