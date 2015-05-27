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

#include "gazebo/common/MeshManager.hh"

#include "gazebo/rendering/DynamicLines.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/ApplyWrenchVisualPrivate.hh"
#include "gazebo/rendering/ApplyWrenchVisual.hh"

using namespace gazebo;
using namespace rendering;

/////////////////////////////////////////////////
ApplyWrenchVisual::ApplyWrenchVisual(const std::string &_name,
    VisualPtr _parentVis)
    : Visual(*new ApplyWrenchVisualPrivate, _name, _parentVis, false)
{
}

/////////////////////////////////////////////////
ApplyWrenchVisual::~ApplyWrenchVisual()
{
  this->Fini();
}

/////////////////////////////////////////////////
void ApplyWrenchVisual::Fini()
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (dPtr->torqueVisual && dPtr->torqueLine)
    dPtr->torqueVisual->DeleteDynamicLine(dPtr->torqueLine);

  if (!dPtr->scene)
    return;

  // Destroy objects and nodes
  Ogre::SceneManager *manager = dPtr->scene->GetManager();
  if (!manager)
    return;

  std::vector<std::string> suffixes = {
      "_FORCE_SHAFT_",
      "_FORCE_HEAD_",
      "_TORQUE_TUBE_",
      "_TORQUE_HEAD_"};

  for (auto suffix : suffixes)
  {
    std::string name = this->GetName() + suffix;
    if (manager->hasEntity(name))
      manager->destroyMovableObject(manager->getEntity(name));

    name += "NODE_";
    if (manager->hasSceneNode(name))
      manager->destroySceneNode(manager->getSceneNode(name));
  }

  // Remove visuals
  if (dPtr->forceVisual &&
      dPtr->scene->GetVisual(dPtr->forceVisual->GetName()))
  {
    dPtr->scene->RemoveVisual(dPtr->forceVisual);
  }

  if (dPtr->torqueVisual &&
      dPtr->scene->GetVisual(dPtr->torqueVisual->GetName()))
  {
    dPtr->scene->RemoveVisual(dPtr->torqueVisual);
  }

  dPtr->forceVisual.reset();
  dPtr->torqueVisual.reset();
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::Load()
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->scene)
  {
    gzerr << "Visual has no scene, not loading." << std::endl;
    return;
  }

  dPtr->unselectedMaterial = "Gazebo/DarkOrangeTransparentOverlay";

  // Force visual
  dPtr->forceVisual.reset(new rendering::Visual(
      this->GetName() + "_FORCE_VISUAL_", shared_from_this()));
  dPtr->forceVisual->Load();

  // Force shaft
  this->InsertMesh("axis_shaft");

  Ogre::MovableObject *shaftObj =
      (Ogre::MovableObject*)(dPtr->scene->GetManager()->createEntity(
      this->GetName()+"_FORCE_SHAFT_", "axis_shaft"));
  shaftObj->getUserObjectBindings().setUserAny(
      Ogre::Any(std::string(dPtr->forceVisual->GetName())));

  Ogre::SceneNode *shaftNode =
      dPtr->forceVisual->GetSceneNode()->createChildSceneNode(
      this->GetName() + "_FORCE_SHAFT_NODE_");
  shaftNode->attachObject(shaftObj);
  shaftNode->setPosition(0, 0, 0.1);

  // Force head
  this->InsertMesh("axis_head");

  Ogre::MovableObject *headObj =
      (Ogre::MovableObject*)(dPtr->scene->GetManager()->createEntity(
      this->GetName()+"_FORCE_HEAD_", "axis_head"));
  headObj->getUserObjectBindings().setUserAny(
      Ogre::Any(std::string(dPtr->forceVisual->GetName())));

  Ogre::SceneNode *headNode =
      dPtr->forceVisual->GetSceneNode()->createChildSceneNode(
      this->GetName() + "_FORCE_HEAD_NODE_");
  headNode->attachObject(headObj);
  headNode->setPosition(0, 0, 0.24);

  dPtr->forceVisual->SetMaterial(dPtr->unselectedMaterial);
  dPtr->forceVisual->GetSceneNode()->setInheritScale(false);

  // Torque visual
  dPtr->torqueVisual.reset(new rendering::Visual(
      this->GetName() + "_TORQUE_VISUAL_", shared_from_this()));
  dPtr->torqueVisual->Load();

  // Torque tube
  common::MeshManager::Instance()->CreateTube("torque_tube",
      0.1, 0.15, 0.05, 2, 32, 1.5*M_PI);
  this->InsertMesh("torque_tube");

  Ogre::MovableObject *tubeObj =
      (Ogre::MovableObject*)(dPtr->scene->GetManager()->createEntity(
      this->GetName()+"_TORQUE_TUBE_", "torque_tube"));
  tubeObj->getUserObjectBindings().setUserAny(
      Ogre::Any(std::string(dPtr->torqueVisual->GetName())));

  Ogre::SceneNode *tubeNode =
      dPtr->torqueVisual->GetSceneNode()->createChildSceneNode(
      this->GetName() + "_TORQUE_TUBE_NODE_");
  tubeNode->attachObject(tubeObj);

  // Torque arrow
  this->InsertMesh("axis_head");

  Ogre::MovableObject *torqueHeadObj =
      (Ogre::MovableObject*)(dPtr->scene->GetManager()->createEntity(
      this->GetName()+"_TORQUE_HEAD_", "axis_head"));
  torqueHeadObj->getUserObjectBindings().setUserAny(
      Ogre::Any(std::string(dPtr->torqueVisual->GetName())));

  Ogre::SceneNode *torqueHeadNode =
      dPtr->torqueVisual->GetSceneNode()->createChildSceneNode(
      this->GetName() + "_TORQUE_HEAD_NODE_");
  torqueHeadNode->attachObject(torqueHeadObj);
  torqueHeadNode->setScale(3, 3, 1);
  torqueHeadNode->setPosition(-0.04, 0.125, 0);
  math::Quaternion quat(0, -M_PI/2.0, 0);
  torqueHeadNode->setOrientation(
      Ogre::Quaternion(quat.w, quat.x, quat.y, quat.z));

  dPtr->torqueVisual->SetMaterial(dPtr->unselectedMaterial);
  dPtr->torqueVisual->GetSceneNode()->setInheritScale(false);

  // Torque line
  dPtr->torqueLine = dPtr->torqueVisual->
      CreateDynamicLine(rendering::RENDERING_LINE_LIST);
  dPtr->torqueLine->setMaterial(dPtr->unselectedMaterial);
  dPtr->torqueLine->AddPoint(0, 0, 0);
  dPtr->torqueLine->AddPoint(0, 0, 0.1);

  // Initialize
  dPtr->forceVector = math::Vector3::Zero;
  dPtr->torqueVector = math::Vector3::Zero;

  this->SetVisibilityFlags(GZ_VISIBILITY_GUI | GZ_VISIBILITY_SELECTABLE);
  this->Resize();
  this->UpdateForceVisual();
  this->UpdateTorqueVisual();
}

///////////////////////////////////////////////////
math::Quaternion ApplyWrenchVisual::GetQuaternionFromVector(
    const math::Vector3 &_vec)
{
  double roll = 0;
  double pitch = -atan2(_vec.z, sqrt(pow(_vec.x, 2) + pow(_vec.y, 2)));
  double yaw = atan2(_vec.y, _vec.x);

  return math::Quaternion(roll, pitch, yaw);
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::SetCoM(const math::Vector3 &_comVector)
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  dPtr->comVector = _comVector;
  this->UpdateTorqueVisual();
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::SetForcePos(const math::Vector3 &_forcePosVector)
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  dPtr->forcePosVector = _forcePosVector;
  this->UpdateForceVisual();
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::SetForce(const math::Vector3 &_forceVector)
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  dPtr->forceVector = _forceVector;
  this->UpdateForceVisual();
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::SetTorque(const math::Vector3 &_torqueVector)
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  dPtr->torqueVector = _torqueVector;
  this->UpdateTorqueVisual();
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::UpdateForceVisual()
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->forceVisual)
  {
    gzwarn << "No force visual" << std::endl;
    return;
  }

  // Protect forceVisual
  std::lock_guard<std::mutex> lock(dPtr->mutex);

  math::Vector3 normVec = dPtr->forceVector;
  normVec.Normalize();

  // Place it on X axis in case it is zero
  if (normVec == math::Vector3::Zero)
    normVec = math::Vector3::UnitX;

  // Set rotation in the vector direction
  math::Quaternion quat = this->GetQuaternionFromVector(normVec);
  dPtr->forceVisual->SetRotation(quat * math::Quaternion(
      math::Vector3(0, M_PI/2.0, 0)));

  // Set arrow tip to forcePosVector
  dPtr->forceVisual->SetPosition(-normVec * 0.28 *
      dPtr->forceVisual->GetScale().z + dPtr->forcePosVector);
}

///////////////////////////////////////////////////
void ApplyWrenchVisual::UpdateTorqueVisual()
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->torqueVisual)
  {
    gzwarn << "No torque visual" << std::endl;
    return;
  }

  // Protect torqueVisual
  std::lock_guard<std::mutex> lock(dPtr->mutex);

  math::Vector3 normVec = dPtr->torqueVector;
  normVec.Normalize();

  // Place it on X axis in case it is zero
  if (normVec == math::Vector3::Zero)
    normVec = math::Vector3::UnitX;

  // Set rotation in the vector direction
  math::Quaternion quat = this->GetQuaternionFromVector(normVec);
  dPtr->torqueVisual->SetRotation(quat * math::Quaternion(
      math::Vector3(0, M_PI/2.0, 0)));

  // Position towards comVector
  double linkDiagonal = dPtr->parent->GetBoundingBox().GetSize().GetLength();
  dPtr->torqueVisual->SetPosition(normVec*linkDiagonal*0.75 + dPtr->comVector);
  dPtr->torqueLine->SetPoint(1,
      math::Vector3(0, 0, -linkDiagonal*0.75)/dPtr->torqueVisual->GetScale());
}

/////////////////////////////////////////////////
void ApplyWrenchVisual::Resize()
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->parent || !dPtr->forceVisual || !dPtr->torqueVisual)
  {
    gzwarn << "ApplyWrenchVisual is incomplete." << std::endl;
    return;
  }

  // Protect force/torque visuals
  std::lock_guard<std::mutex> lock(dPtr->mutex);

  double linkSize = std::max(0.1,
      dPtr->parent->GetBoundingBox().GetSize().GetLength());

  // Force visual
  dPtr->forceVisual->SetScale(math::Vector3(2*linkSize,
                                            2*linkSize,
                                            2*linkSize));

  // Torque visual
  dPtr->torqueVisual->SetScale(math::Vector3(linkSize,
                                             linkSize,
                                             linkSize));
}

///////////////////////////////////////////////////
rendering::VisualPtr ApplyWrenchVisual::GetForceVisual() const
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->forceVisual)
  {
    gzerr << "Force visual not found, but it should exist." << std::endl;
    return NULL;
  }

  std::lock_guard<std::mutex> lock(dPtr->mutex);

  return dPtr->forceVisual;
}

///////////////////////////////////////////////////
rendering::VisualPtr ApplyWrenchVisual::GetTorqueVisual() const
{
  ApplyWrenchVisualPrivate *dPtr =
      reinterpret_cast<ApplyWrenchVisualPrivate *>(this->dataPtr);

  if (!dPtr->torqueVisual)
  {
    gzerr << "Torque visual not found, but it should exist." << std::endl;
    return NULL;
  }

  std::lock_guard<std::mutex> lock(dPtr->mutex);

  return dPtr->torqueVisual;
}