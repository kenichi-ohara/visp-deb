/****************************************************************************
 *
 * $Id: servoViper650Point2DCamVelocity.cpp 3616 2012-03-09 14:31:52Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2013 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 *   tests the control law
 *   eye-in-hand control
 *   velocity computed in camera frame
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/


/*!
  \example servoViper650Point2DCamVelocity.cpp

  Example of eye-in-hand control law. We control here a real robot, the
  ADEPT Viper 650 robot (arm, with 6 degrees of freedom). The velocity is
  computed in the camera frame. The visual feature is the center of gravity of a
  point.

*/

#include <visp/vpConfig.h>
#include <visp/vpDebug.h> // Debug trace

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#if (defined (VISP_HAVE_VIPER650) && defined (VISP_HAVE_DC1394_2))

#include <visp/vp1394TwoGrabber.h>
#include <visp/vpImage.h>
#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpPoint.h>
#include <visp/vpServo.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpRobotViper650.h>
#include <visp/vpIoTools.h>
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>
#include <visp/vpServoDisplay.h>
#include <visp/vpImageIo.h>
#include <visp/vpDot2.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>

int
main()
{
  // Log file creation in /tmp/$USERNAME/log.dat
  // This file contains by line:
  // - the 6 computed joint velocities (m/s, rad/s) to achieve the task
  // - the 6 mesured joint velocities (m/s, rad/s)
  // - the 6 mesured joint positions (m, rad)
  // - the 2 values of s - s*
  std::string username;
  // Get the user login name
  vpIoTools::getUserName(username);

  // Create a log filename to save velocities...
  std::string logdirname;
  logdirname ="/tmp/" + username;

  // Test if the output path exist. If no try to create it
  if (vpIoTools::checkDirectory(logdirname) == false) {
    try {
      // Create the dirname
      vpIoTools::makeDirectory(logdirname);
    }
    catch (...) {
      std::cerr << std::endl
                << "ERROR:" << std::endl;
      std::cerr << "  Cannot create " << logdirname << std::endl;
      exit(-1);
    }
  }
  std::string logfilename;
  logfilename = logdirname + "/log.dat";

  // Open the log file name
  std::ofstream flog(logfilename.c_str());

  try {
    vpRobotViper650 robot ;

    vpServo task ;

    vpImage<unsigned char> I ;

    bool reset = false;
    vp1394TwoGrabber g(reset);

#if 1
    g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_640x480_MONO8);
    g.setFramerate(vp1394TwoGrabber::vpFRAMERATE_60);
#else
    g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_FORMAT7_0);
    g.setColorCoding(vp1394TwoGrabber::vpCOLOR_CODING_MONO8);
#endif
    g.open(I) ;


    vpDisplayX display(I, (int)(100+I.getWidth()+30), 200, "Current image") ;

    vpDisplay::display(I) ;
    vpDisplay::flush(I) ;

    vpDot2 dot ;
    vpImagePoint cog;

    dot.setGraphics(true);

    for (int i=0; i< 10; i++)
      g.acquire(I) ;

    std::cout << "Click on a dot..." << std::endl;
    dot.initTracking(I) ;

    cog = dot.getCog();
    vpDisplay::displayCross(I, cog, 10, vpColor::blue) ;
    vpDisplay::flush(I);

    vpCameraParameters cam ;
    // Update camera parameters
    robot.getCameraParameters (cam, I);

    // sets the current position of the visual feature
    vpFeaturePoint p ;
    // retrieve x,y and Z of the vpPoint structure
    vpFeatureBuilder::create(p,cam, dot);  

    // sets the desired position of the visual feature
    vpFeaturePoint pd ;
    pd.buildFrom(0,0,1) ;

    // define the task
    // - we want an eye-in-hand control law
    // - robot is controlled in the camera frame
    task.setServo(vpServo::EYEINHAND_CAMERA) ;

    // - we want to see a point on a point
    task.addFeature(p,pd) ;

    // - set the constant gain
    task.setLambda(0.8) ;

    // Display task information 
    task.print() ;

    // Now the robot will be controlled in velocity
    robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL) ;

    std::cout << "\nHit CTRL-C to stop the loop...\n" << std::flush;
    vpColVector v ;
    for ( ; ; ) {
      try {
        // Acquire a new image from the camera
        g.acquire(I) ;

        // Display this image
        vpDisplay::display(I) ;

        // Achieve the tracking of the dot in the image
        dot.track(I) ;

        // Get the dot cog
        cog = dot.getCog();

        // Display a green cross at the center of gravity position in the image
        vpDisplay::displayCross(I, cog, 10, vpColor::green) ;

        // Update the point feature from the dot location
        vpFeatureBuilder::create(p, cam, dot);

        // Compute the visual servoing skew vector
        v = task.computeControlLaw() ;

        // Display the current and desired feature points in the image display
        vpServoDisplay::display(task, cam, I) ;

        // Apply the computed camera velocities to the robot
        robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
      }
      catch(...) {
        std::cout << "Tracking failed... Stop the robot." << std::endl;
        v = 0;
        // Stop robot
        robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
        return 0;
      }

      // Save velocities applied to the robot in the log file
      // v[0], v[1], v[2] correspond to camera translation velocities in m/s
      // v[3], v[4], v[5] correspond to camera rotation velocities in rad/s
      flog << v[0] << " " << v[1] << " " << v[2] << " "
           << v[3] << " " << v[4] << " " << v[5] << " ";

      // Get the measured joint velocities of the robot
      vpColVector qvel;
      robot.getVelocity(vpRobot::ARTICULAR_FRAME, qvel);
      // Save measured joint velocities of the robot in the log file:
      // - qvel[0], qvel[1], qvel[2] correspond to measured joint translation
      //   velocities in m/s
      // - qvel[3], qvel[4], qvel[5] correspond to measured joint rotation
      //   velocities in rad/s
      flog << qvel[0] << " " << qvel[1] << " " << qvel[2] << " "
           << qvel[3] << " " << qvel[4] << " " << qvel[5] << " ";

      // Get the measured joint positions of the robot
      vpColVector q;
      robot.getPosition(vpRobot::ARTICULAR_FRAME, q);
      // Save measured joint positions of the robot in the log file
      // - q[0], q[1], q[2] correspond to measured joint translation
      //   positions in m
      // - q[3], q[4], q[5] correspond to measured joint rotation
      //   positions in rad
      flog << q[0] << " " << q[1] << " " << q[2] << " "
           << q[3] << " " << q[4] << " " << q[5] << " ";

      // Save feature error (s-s*) for the feature point. For this feature
      // point, we have 2 errors (along x and y axis).  This error is expressed
      // in meters in the camera frame
      flog << ( task.getError() ).t() << std::endl; // s-s* for point

      // Flush the display
      vpDisplay::flush(I) ;

    }

    flog.close() ; // Close the log file

    // Display task information
    task.print() ;

    // Kill the task
    task.kill();

    return 0;
  }
  catch (...)
  {
    flog.close() ; // Close the log file
    vpERROR_TRACE(" Test failed") ;
    return 0;
  }
}

#else
int
main()
{
  vpERROR_TRACE("You do not have a Viper robot or a firewire framegrabber connected to your computer...");
}
#endif
