/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef _FLUID_PRESSURE_SENSOR_HH_
#define _FLUID_PRESSURE_SENSOR_HH_

#include <string>

#include <sdf/sdf.hh>

#include "gazebo/sensors/Sensor.hh"
#include "gazebo/common/CommonTypes.hh"
#include "gazebo/physics/PhysicsTypes.hh"
#include "gazebo/sensors/SensorTypes.hh"
#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace sensors
  {
    /// \addtogroup gazebo_sensors
    /// \{

    /// \class FluidPressureSensor FluidPressureSensor.hh sensors/sensors.hh
    /// \brief FluidPressureSensor to provide position measurement.
    class GAZEBO_VISIBLE FluidPressureSensor: public Sensor
    {
      /// \brief Constructor.
      public: FluidPressureSensor();

      /// \brief Destructor.
      public: virtual ~FluidPressureSensor();

      // Documentation inherited
      public: virtual void Load(const std::string & _worldName,
                                sdf::ElementPtr _sdf);

      // Documentation inherited
      public: virtual void Load(const std::string & _worldName);

      // Documentation inherited
      public: virtual void Init();

      // Documentation inherited
      protected: virtual bool UpdateImpl(bool _force);

      // Documentation inherited
      public: virtual void Fini();

      /// \brief Accessor for current fluid pressure (in Pascals)
      /// \return Noise perturbed fluid pressure
      public: double GetFluidPressure() const;

      /// \brief Fluid pressure data publisher.
      private: transport::PublisherPtr fpPub;

      /// \brief Topic name for fluid pressure data publisher.
      private: std::string topicName;

      /// \brief Parent link of this sensor.
      private: physics::LinkPtr parentLink;

      /// \brief Stores most recent fluid pressure sensor data.
      private: msgs::FluidPressure lastFpMsg;
    };
    /// \}
  }
}
#endif