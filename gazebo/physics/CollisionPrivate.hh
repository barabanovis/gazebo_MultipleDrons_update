/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
#ifndef _GAZEBO_PHYSICS_COLLISIONPRIVATE_HH_
#define _GAZEBO_PHYSICS_COLLISIONPRIVATE_HH_

#include "gazebo/physics/CollisionState.hh"
#include "gazebo/physics/PhysicsTypes.hh"
#include "gazebo/physics/EntityPrivate.hh"

namespace gazebo
{
  namespace physics
  {
    /// \internal
    /// \brief Private Collision data
    class CollisionPrivate : public EntityPrivate
    {
      /// \brief The link this collision belongs to
      public: LinkPtr link;

      /// \brief Flag for placeable.
      public: bool placeable;

      /// \brief Pointer to physics::Shape.
      public: ShapePtr shape;

      /// \brief The surface parameters.
      public: SurfaceParamsPtr surface;

      /// \brief The laser retro value.
      public: float laserRetro;

      /// \brief Stores collision state information.
      public: CollisionState state;

      /// \brief Number of contact points allowed for this collision.
      public: unsigned int maxContacts;

      /// \brief Unique id for collision visual.
      public: uint32_t collisionVisualId;

      /// \brief True if the world pose should be recalculated.
      public: mutable bool worldPoseDirty;
    };
  }
}
#endif