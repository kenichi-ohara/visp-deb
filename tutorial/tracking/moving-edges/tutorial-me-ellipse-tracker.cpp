/*! \example tutorial-me-ellipse-tracker.cpp */
#include <visp/vp1394CMUGrabber.h>
#include <visp/vp1394TwoGrabber.h>
#include <visp/vpV4l2Grabber.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayX.h>
#include <visp/vpMeEllipse.h>

int main()
{
#if (defined(VISP_HAVE_DC1394_2) || defined(VISP_HAVE_CMU1394) || defined(VISP_HAVE_V4L2))

  vpImage<unsigned char> I; 

#if defined(VISP_HAVE_DC1394_2)
  vp1394TwoGrabber g(false);
#elif defined(VISP_HAVE_CMU1394)
  vp1394CMUGrabber g;
#elif defined(VISP_HAVE_V4L2)
  vpV4l2Grabber g;     
#endif
  g.open(I);
  g.acquire(I);

#if defined(VISP_HAVE_X11)
  vpDisplayX d(I, 0, 0, "Camera view");
#elif defined(VISP_HAVE_GDI)
  vpDisplayGDI d(I, 0, 0, "Camera view");
#else
  std::cout << "No image viewer is available..." << std::endl;
#endif

  vpDisplay::display(I);
  vpDisplay::flush(I);

  vpMe me;
  me.setRange(25);
  me.setThreshold(15000);
  me.setSampleStep(10);

  vpMeEllipse ellipse;
  ellipse.setMe(&me);
  ellipse.setDisplay(vpMeSite::RANGE_RESULT);
  ellipse.initTracking(I);
 
  while(1) {
    g.acquire(I); 
    vpDisplay::display(I);
    ellipse.track(I);
    ellipse.display(I, vpColor::red);
    vpDisplay::flush(I);
  }
#endif
}
