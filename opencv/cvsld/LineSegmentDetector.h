//
// Created by wangyaofu on 2019/2/14.
//

#ifndef UNTITLED_LINESEGMENTDETECTOR_H
#define UNTITLED_LINESEGMENTDETECTOR_H

#include "opencv2/core.hpp"

namespace cvstd {
//! Variants of Line Segment %Detector
//! @ingroup imgproc_feature
    enum LineSegmentDetectorModes {
        LSD_REFINE_NONE = 0, //!< No refinement applied
        LSD_REFINE_STD = 1, //!< Standard refinement is applied. E.g. breaking arches into smaller straighter line approximations.
        LSD_REFINE_ADV = 2  //!< Advanced refinement. Number of false alarms is calculated, lines are
        //!< refined through increase of precision, decrement in size, etc.
    };

/** @brief Line segment detector class

following the algorithm described at @cite Rafael12 .
*/
    class CV_EXPORTS_W LineSegmentDetector : public cv::Algorithm {
    public:

/** @brief Finds lines in the input image.

This is the output of the default parameters of the algorithm on the above shown image.

![image](pics/building_lsd.png)

@param _image A grayscale (CV_8UC1) input image. If only a roi needs to be selected, use:
`lsd_ptr-\>detect(image(roi), lines, ...); lines += Scalar(roi.x, roi.y, roi.x, roi.y);`
@param _lines A vector of Vec4i or Vec4f elements specifying the beginning and ending point of a line. Where
Vec4i/Vec4f is (x1, y1, x2, y2), point 1 is the start, point 2 - end. Returned lines are strictly
oriented depending on the gradient.
@param width Vector of widths of the regions, where the lines are found. E.g. Width of line.
@param prec Vector of precisions with which the lines are found.
@param nfa Vector containing number of false alarms in the line region, with precision of 10%. The
bigger the value, logarithmically better the detection.
- -1 corresponds to 10 mean false alarms
- 0 corresponds to 1 mean false alarm
- 1 corresponds to 0.1 mean false alarms
This vector will be calculated only when the objects type is #LSD_REFINE_ADV.
*/
    CV_WRAP virtual void detect(InputArray _image, OutputArray _lines,
                                OutputArray width = noArray(), OutputArray prec = noArray(),
                                OutputArray nfa = noArray()) = 0;

/** @brief Draws the line segments on a given image.
@param _image The image, where the lines will be drawn. Should be bigger or equal to the image,
where the lines were found.
@param lines A vector of the lines that needed to be drawn.
 */
    CV_WRAP virtual void drawSegments(InputOutputArray _image, InputArray lines) = 0;

/** @brief Draws two groups of lines in blue and red, counting the non overlapping (mismatching) pixels.

@param size The size of the image, where lines1 and lines2 were found.
@param lines1 The first group of lines that needs to be drawn. It is visualized in blue color.
@param lines2 The second group of lines. They visualized in red color.
@param _image Optional image, where the lines will be drawn. The image should be color(3-channel)
in order for lines1 and lines2 to be drawn in the above mentioned colors.
 */
    CV_WRAP virtual int
    compareSegments(const Size &size, InputArray lines1, InputArray lines2, InputOutputArray _image = noArray()) = 0;

    virtual ~LineSegmentDetector() {}
};

/** @brief Creates a smart pointer to a LineSegmentDetector object and initializes it.

The LineSegmentDetector algorithm is defined using the standard values. Only advanced users may want
to edit those, as to tailor it for their own application.

@param _refine The way found lines will be refined, see #LineSegmentDetectorModes
@param _scale The scale of the image that will be used to find the lines. Range (0..1].
@param _sigma_scale Sigma for Gaussian filter. It is computed as sigma = _sigma_scale/_scale.
@param _quant Bound to the quantization error on the gradient norm.
@param _ang_th Gradient angle tolerance in degrees.
@param _log_eps Detection threshold: -log10(NFA) \> log_eps. Used only when advance refinement
is chosen.
@param _density_th Minimal density of aligned region points in the enclosing rectangle.
@param _n_bins Number of bins in pseudo-ordering of gradient modulus.
 */
CV_EXPORTS_W Ptr<LineSegmentDetector>

createLineSegmentDetector(
        int _refine = LSD_REFINE_STD, double _scale = 0.8,
        double _sigma_scale = 0.6, double _quant = 2.0, double _ang_th = 22.5,
        double _log_eps = 0, double _density_th = 0.7, int _n_bins = 1024);

}
#endif //UNTITLED_LINESEGMENTDETECTOR_H
