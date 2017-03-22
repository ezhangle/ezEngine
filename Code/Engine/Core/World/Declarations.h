#pragma once

#include <Foundation/Memory/BlockStorage.h>
#include <Foundation/Memory/LargeBlockAllocator.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/Id.h>

#include <Core/Basics.h>

class ezWorld;
class ezSpatialSystem;
class ezCoordinateSystemProvider;

namespace ezInternal
{
  class WorldData;

  enum
  {
    DEFAULT_BLOCK_SIZE = 4096
  };

  typedef ezLargeBlockAllocator<DEFAULT_BLOCK_SIZE> WorldLargeBlockAllocator;
}

class ezGameObject;
struct ezGameObjectDesc;

class ezComponentManagerBase;
class ezComponent;

/// \brief Internal game object id used by ezGameObjectHandle.
struct ezGameObjectId
{
  typedef ezUInt32 StorageType;

  EZ_DECLARE_ID_TYPE(ezGameObjectId, 20, 6);

  EZ_FORCE_INLINE ezGameObjectId(StorageType instanceIndex, StorageType generation,
    StorageType worldIndex = 0)
  {
    m_Data = 0;
    m_InstanceIndex = instanceIndex;
    m_Generation = generation;
    m_WorldIndex = worldIndex;
  }

  union
  {
    StorageType m_Data;
    struct
    {
      StorageType m_InstanceIndex : 20;
      StorageType m_Generation : 6;
      StorageType m_WorldIndex : 6;
    };
  };
};

/// \brief A handle to a game object.
///
/// Never store a direct pointer to a game object. Always store a handle instead. A pointer to a game object can
/// be received by calling ezWorld::TryGetObject with the handle.
/// Note that the object might have been deleted so always check the return value of TryGetObject.
class ezGameObjectHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezGameObjectHandle, ezGameObjectId);

  friend class ezWorld;
  friend class ezGameObject;
};

/// \brief HashHelper implementation so game object handles can be used as key in a hashtable.
template <>
struct ezHashHelper<ezGameObjectHandle>
{
  EZ_FORCE_INLINE static ezUInt32 Hash(ezGameObjectHandle value)
  {
    return value.GetInternalID().m_Data * 2654435761U;
  }

  EZ_ALWAYS_INLINE static bool Equal(ezGameObjectHandle a, ezGameObjectHandle b)
  {
    return a == b;
  }
};

typedef ezGenericId<24, 8> ezGenericComponentId;

/// \brief Internal component id used by ezComponentHandle.
struct ezComponentId : public ezGenericComponentId
{
  EZ_FORCE_INLINE ezComponentId() : ezGenericComponentId()
  {
    m_TypeId = 0;
    m_WorldIndex = 0;
  }

  EZ_FORCE_INLINE ezComponentId(StorageType instanceIndex, StorageType generation, ezUInt16 typeId = 0, ezUInt16 worldIndex = 0) :
    ezGenericComponentId(instanceIndex, generation)
  {
    m_TypeId = typeId;
    m_WorldIndex = worldIndex;
  }

  EZ_FORCE_INLINE ezComponentId(ezGenericComponentId genericId, ezUInt16 typeId, ezUInt16 worldIndex) :
    ezGenericComponentId(genericId)
  {
    m_TypeId = typeId;
    m_WorldIndex = worldIndex;
  }

  ezUInt16 m_TypeId;
  ezUInt16 m_WorldIndex;
};

/// \brief A handle to a component.
///
/// Never store a direct pointer to a component. Always store a handle instead. A pointer to a component can
/// be received by calling ezWorld::TryGetComponent or TryGetComponent on the corresponding component manager.
/// Note that the component might have been deleted so always check the return value of TryGetComponent.
class ezComponentHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezComponentHandle, ezComponentId);

  friend class ezWorld;
  friend class ezComponentManagerBase;
  friend class ezComponent;
};

/// \brief Internal flags of game objects or components.
struct ezObjectFlags
{
  typedef ezUInt32 StorageType;

  enum Enum
  {
    None = 0,
    Dynamic = EZ_BIT(0),
    Active  = EZ_BIT(1),
    Initialized = EZ_BIT(2),
    Initializing = EZ_BIT(3),
    SimulationStarted = EZ_BIT(4),
    SimulationStarting = EZ_BIT(5),
    UnhandledMessageHandler = EZ_BIT(6), ///< For components, when a message is not handled, a virtual function is called

    Default = Dynamic | Active
  };

  struct Bits
  {
    StorageType Dynamic : 1;
    StorageType Active : 1;
    StorageType Initialized : 1;
    StorageType Initializing : 1;
    StorageType SimulationStarted : 1;
    StorageType SimulationStarting : 1;
  };
};

EZ_DECLARE_FLAGS_OPERATORS(ezObjectFlags);

/// \brief Different options for routing a message through the game object graph.
struct ezObjectMsgRouting
{
  enum Enum
  {
    ToObjectOnly, ///< Send the message only to the object itself.
    ToComponents, ///< Send the message to the object itself and its components.
    ToAllParents, ///< Send the message to the object, all parent objects and their components.
    ToChildren,   ///< Send the message to the object, all child objects (recursively) and their components.
    ToSubTree,    ///< Send the message to the whole subtree starting at the top-level parent object.
    Default = ToComponents
  };
};

/// \brief Specifies at which phase the queued message should be processed.
struct ezObjectMsgQueueType
{
  enum Enum
  {
    PostAsync,        ///< Process the message in the PostAsync phase.
    PostTransform,    ///< Process the message in the PostTransform phase.
    NextFrame,        ///< Process the message in the PreAsync phase of the next frame.
    AfterInitialized, ///< Deliver this message after all components have been initialized. This is basically the same as NextFrame.
    COUNT
  };
};

/// \brief Certain components may delete themselves or their owner when they are finished with their main purpose
struct EZ_CORE_DLL ezOnComponentFinishedAction
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    None,
    DeleteComponent,
    DeleteEntity,

    Default = None
  };
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_CORE_DLL, ezOnComponentFinishedAction);

/// \brief Used as return value of visitor functions to define whether calling function should stop or continue visiting
struct ezVisitorExecution
{
  enum Enum
  {
    Continue,
    Stop
  };
};

typedef ezGenericId<24, 8> ezSpatialDataId;
class ezSpatialDataHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezSpatialDataHandle, ezSpatialDataId);
};
