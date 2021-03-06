// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_COMMON_EXTENSION_H_
#define XWALK_COMMON_EXTENSION_H_

// This is a C++ wrapper over Crosswalk Extension C API. It implements once the
// boilerplate for the common case of mapping XW_Extension and XW_Instance to
// objects of their own. The wrapper deals automatically with creating and
// destroying the objects.
//
// Extension object lives during the lifetime of the extension, and when the
// extension process is properly shutdown, it's destructor will be
// called. Instance objects (there can be many) live during the lifetime of a
// script context associated with a frame in the page. These objects serves as
// storage points for extension specific objects, use them for that.

#include <sys/types.h>

#include <string>

#include "xwalk/common/XW_Extension.h"
#include "xwalk/common/XW_Extension_EntryPoints.h"
#include "xwalk/common/XW_Extension_Message_2.h"
#include "xwalk/common/XW_Extension_Permissions.h"
#include "xwalk/common/XW_Extension_Runtime.h"
#include "xwalk/common/XW_Extension_SyncMessage.h"

namespace xwalk {
namespace common {

class Instance;
class Extension;

}  // namespace common
}  // namespace xwalk

// This function should be implemented by each extension and should return
// an appropriate Extension subclass.
xwalk::common::Extension* CreateExtension();

namespace xwalk {
namespace common {

#ifdef PostMessage
#undef PostMessage
#endif

class Extension {
 public:
  Extension();
  virtual ~Extension();

  // These should be called in the subclass constructor.
  void SetExtensionName(const char* name);
  void SetJavaScriptAPI(const char* api);
  void SetExtraJSEntryPoints(const char** entry_points);
  bool RegisterPermissions(const char* perm_table);

  // This API should be called in the message handler of extension
  bool CheckAPIAccessControl(const char* api_name);

  virtual Instance* CreateInstance();

  static std::string GetRuntimeVariable(const char* var_name, unsigned len);

  // XW_Extension callbacks.
  static void OnShutdown(XW_Extension xw_extension);
  static void OnInstanceCreated(XW_Instance xw_instance);
  static void OnInstanceDestroyed(XW_Instance xw_instance);
  static void HandleMessage(XW_Instance xw_instance, const char* msg);
  static void HandleBinaryMessage(XW_Instance xw_instance,
                                  const char* msg,
                                  const size_t size);
  static void HandleSyncMessage(XW_Instance xw_instance, const char* msg);
};

class Instance {
 public:
  Instance();
  virtual ~Instance();

  void PostMessage(const char* msg);
  void PostBinaryMessage(const char* msg, const size_t size);
  void SendSyncReply(const char* reply);

  virtual void Initialize() {}
  virtual void HandleMessage(const char* msg) = 0;
  virtual void HandleBinaryMessage(const char* msg, const size_t size) {}
  virtual void HandleSyncMessage(const char* msg) {}

  XW_Instance xw_instance() const { return xw_instance_; }

 private:
  friend class Extension;

  XW_Instance xw_instance_;
};

}  // namespace common
}  // namespace xwalk

#endif  // XWALK_COMMON_EXTENSION_H_
