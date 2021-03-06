/*! \example tutorial-ibvs-4pts-wireframe-camera.cpp */
#include <visp/vpFeatureBuilder.h>
#include <visp/vpServo.h>
#include <visp/vpSimulatorCamera.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpProjectionDisplay.h>
#include <visp/vpServoDisplay.h>
#include <visp/vpWireFrameSimulator.h>

void display_trajectory(const vpImage<unsigned char> &I, std::vector<vpPoint> &point,
                        const vpHomogeneousMatrix &cMo, const vpCameraParameters &cam)
{
  static std::vector<vpImagePoint> traj[4];
  vpImagePoint cog;
  for (unsigned int i=0; i<4; i++) {
    // Project the point at the given camera position
    point[i].project(cMo);
    vpMeterPixelConversion::convertPoint(cam, point[i].get_x(), point[i].get_y(), cog);
    traj[i].push_back(cog);
  }
  for (unsigned int i=0; i<4; i++) {
    for (unsigned int j=1; j<traj[i].size(); j++) {
      vpDisplay::displayLine(I, traj[i][j-1], traj[i][j], vpColor::green);
    }
  }
}

int main()
{
  vpHomogeneousMatrix cdMo(0, 0, 0.75, 0, 0, 0);
  vpHomogeneousMatrix cMo(0.15, -0.1, 1., vpMath::rad(10), vpMath::rad(-10), vpMath::rad(50));

  std::vector<vpPoint> point(4) ;
  point[0].setWorldCoordinates(-0.1,-0.1, 0);
  point[1].setWorldCoordinates( 0.1,-0.1, 0);
  point[2].setWorldCoordinates( 0.1, 0.1, 0);
  point[3].setWorldCoordinates(-0.1, 0.1, 0);

  vpServo task ;
  task.setServo(vpServo::EYEINHAND_CAMERA);
  task.setInteractionMatrixType(vpServo::CURRENT);
  task.setLambda(0.5);

  vpFeaturePoint p[4], pd[4] ;
  for (int i = 0 ; i < 4 ; i++) {
    point[i].track(cdMo);
    vpFeatureBuilder::create(pd[i], point[i]);
    point[i].track(cMo);
    vpFeatureBuilder::create(p[i], point[i]);
    task.addFeature(p[i], pd[i]);
  }

  vpHomogeneousMatrix wMc, wMo;
  vpSimulatorCamera robot;
  robot.setSamplingTime(0.040);
  robot.getPosition(wMc);
  wMo = wMc * cMo;

  vpImage<unsigned char> Iint(480, 640, 0) ;
  vpImage<unsigned char> Iext(480, 640, 0) ;
#if defined VISP_HAVE_X11
  vpDisplayX displayInt(Iint, 0, 0, "Internal view");
  vpDisplayX displayExt(Iext, 670, 0, "External view");
#elif  defined VISP_HAVE_GDI
  vpDisplayGDI displayInt(Iint, 0, 0, "Internal view");
  vpDisplayGDI displayExt(Iext, 670, 0, "External view");
#else
  std::cout << "No image viewer is available..." << std::endl;
#endif

  vpCameraParameters cam(840, 840, Iint.getWidth()/2, Iint.getHeight()/2);
  vpHomogeneousMatrix cextMo(0,0,3, 0,0,0);

  vpWireFrameSimulator sim;
  sim.initScene(vpWireFrameSimulator::PLATE, vpWireFrameSimulator::D_STANDARD);
  sim.setCameraPositionRelObj(cMo);
  sim.setDesiredCameraPosition(cdMo);
  sim.setExternalCameraPosition(cextMo);
  sim.setInternalCameraParameters(cam);
  sim.setExternalCameraParameters(cam);

  while(1) {
    robot.getPosition(wMc);
    cMo = wMc.inverse() * wMo;
    for (int i = 0 ; i < 4 ; i++) {
      point[i].track(cMo);
      vpFeatureBuilder::create(p[i], point[i]);
    }
    vpColVector v = task.computeControlLaw();
    robot.setVelocity(vpRobot::CAMERA_FRAME, v);

    sim.setCameraPositionRelObj(cMo);

    vpDisplay::display(Iint) ;
    vpDisplay::display(Iext) ;

    sim.getInternalImage(Iint);
    sim.getExternalImage(Iext);

    display_trajectory(Iint, point, cMo, cam);
    vpDisplay::flush(Iint);
    vpDisplay::flush(Iext);

    // A click in the internal view to exit
    if (vpDisplay::getClick(Iint, false))
      break;
    vpTime::wait(1000*robot.getSamplingTime());
  }
  task.kill();
}

