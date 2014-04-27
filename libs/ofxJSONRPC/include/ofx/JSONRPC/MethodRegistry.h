// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include <map>
#include <string>
#include <json/json.h>
#include "Poco/Mutex.h"
#include "ofEvents.h"
#include "ofLog.h"
#include "ofTypes.h" // std::shared_ptr
#include "ofx/JSONRPC/Method.h"
#include "ofx/JSONRPC/MethodArgs.h"
#include "ofx/JSONRPC/Response.h"
#include "ofx/JSONRPC/Request.h"


namespace ofx {
namespace JSONRPC {


/// \brief A MethodRegistry is a thread-safe method callback manager.
///
/// Additionally, a MethodRegistry is in charge of invoking methods by
/// name using the method signature defined in Method.
class MethodRegistry
{
public:
    /// \brief A typedef for a shared pointer.
    typedef std::shared_ptr<MethodRegistry> SharedPtr;

    /// \brief A typedef mapping method names to method descriptions.
    typedef std::map<std::string, Json::Value> MethodDescriptionMap;

    /// \brief A typedef for a MethodDescriptionMap iterator.
    typedef std::map<std::string, Json::Value>::iterator MethodDescriptionMapIter;

    /// \brief Create a MethodRegistry.
    MethodRegistry();

    /// \brief Destroy the MethodRegistry.
    virtual ~MethodRegistry();

    /// \brief Register a method callback.
    ///
    /// Each method needs a name, description, class and method.  This method
    /// registers remote methods with the following signature:
    ///
    /// ~~~{.cpp}
    ///     void ListenerClass::listenerMethod(const void* pSender, MethodArgs& args);
    /// ~~~
    ///
    /// \param name The name of the class to be called by the client.
    /// \param description A JSON description of any information to
    ///        advertise with this method.  This might include a
    ///        description of the functionality, the input / output
    ///        arguments, expected values, etc.
    /// \param listener A pointer to the listener class.
    /// \param listenerMethod A pointer to the method to invoke.
    /// \param priority The priority of the event.
    template <class ListenerClass>
    void registerMethod(const std::string& name,
                        const Json::Value& description,
                        ListenerClass* listener,
                        void (ListenerClass::*listenerMethod)(const void*, MethodArgs&),
                        int priority = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Register a method callback.
    ///
    /// Each method needs a name, description, class and method.  This method
    /// registers remote methods with the following signature:
    ///
    /// ~~~{.cpp}
    ///    void ListenerClass::listenerMethod(MethodArgs& args);
    /// ~~~
    ///
    /// \param name The name of the class to be called by the client.
    /// \param description A JSON description of any information to
    ///        advertise with this method.  This might include a
    ///        description of the functionality, the input / output
    ///        arguments, expected values, etc.
    /// \param listener A pointer to the listener class.
    /// \param listenerMethod A pointer to the method to invoke.
    /// \param priority The priority of the event.
    template <class ListenerClass>
    void registerMethod(const std::string& name,
                        const Json::Value& description,
                        ListenerClass* listener,
                        void (ListenerClass::*listenerMethod)(MethodArgs&),
                        int priority = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Register a no argument method callback.
    ///
    /// Each method needs a name, description, class and method. This method
    /// registers remote methods with the following signature:
    ///
    /// ~~~{.cpp}
    ///    void ListenerClass::listenerMethod(const void* pSender);
    /// ~~~
    ///
    /// \param name The name of the class to be called by the client.
    /// \param description A JSON description of any information to
    ///        advertise with this method.  This might include a
    ///        description of the functionality, the input / output
    ///        arguments, expected values, etc.
    /// \param listener A pointer to the listener class.
    /// \param listenerMethod A pointer to the method to invoke.
    /// \param priority The priority of the event.
    template <class ListenerClass>
    void registerMethod(const std::string& name,
                        const Json::Value& description,
                        ListenerClass* listener,
                        void (ListenerClass::*listenerMethod)(const void*),
                        int priority = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Register a no argument method callback.
    ///
    /// Each method needs a name, description, class and method. This method
    /// registers remote methods with the following signature:
    ///
    /// ~~~{.cpp}
    ///    void ListenerClass::listenerMethod();
    /// ~~~
    ///
    /// \param name The name of the class to be called by the client.
    /// \param description A JSON description of any information to
    ///        advertise with this method.  This might include a
    ///        description of the functionality, the input / output
    ///        arguments, expected values, etc.
    /// \param listener A pointer to the listener class.
    /// \param listenerMethod A pointer to the method to invoke.
    /// \param priority The priority of the event.
    template <class ListenerClass>
    void registerMethod(const std::string& name,
                        const Json::Value& description,
                        ListenerClass* listener,
                        void (ListenerClass::*listenerMethod)(void),
                        int priority = OF_EVENT_ORDER_AFTER_APP);

    /// \brief Unregister a method by name.
    /// \param method is the name of the method callback to be removed.
    /// \note If the given method does not exist, the unregister
    ///        request will be ignored.
    void unregisterMethod(const std::string& method);

    /// \brief Process a Request.
    /// \param pSender A pointer to the sender.  This might be a pointer
    ///        to a session cookie or WebSocket connection.  While not
    ///        required, this pointer is useful for returning
    ///        client-specific method results or responding updating
    ///        the calling client's session information.  The user is
    ///        responsible for using this sender information in
    ///        corresponding method callback.
    /// \param request The incoming Request from a client.
    /// \returns A success or error Response.
    Response processCall(const void* pSender, const Request& request);

    /// \brief Process a Request.
    /// \param pSender A pointer to the sender.  This might be a pointer
    ///        to a session cookie or WebSocket connection.  While not
    ///        required, this pointer is useful for returning
    ///        client-specific method results or responding updating
    ///        the calling client's session information.  The user is
    ///        responsible for using this sender information in
    ///        corresponding method callback.
    /// \param request The incoming Request from a client.
    void processNotification(const void* pSender, const Request& request);

    /// \brief Query the registry for the given method.
    /// \param method the name of the method to find.
    /// \returns true iff the given method is in the registry.
    bool hasMethod(const std::string& method) const;

    /// \brief Get a list of all method names and their descriptions.
    /// \returns a MethodDescriptionMap containting a map of the
    ///        method names and the method descriptions.
    MethodDescriptionMap getMethods() const;

    /// \brief Make a shared pointer.
    /// \returns a SharedPtr to a MethodRegistry.
    static SharedPtr makeShared()
    {
        return SharedPtr(new MethodRegistry());
    }

protected:
    typedef std::shared_ptr<Method> SharedMethodPtr;
        ///< \brief A shared pointer typedef for methods to
        ///<        simplify pointer memory management.

    typedef std::shared_ptr<NoArgMethod> SharedNoArgMethodPtr;
        ///< \brief A shared pointer typedef for no argument methods to
        ///<        simplify pointer memory management.

    typedef std::map<std::string, SharedMethodPtr> MethodMap;
        ///< \brief A method map.

    typedef std::map<std::string, SharedNoArgMethodPtr> NoArgMethodMap;
        ///< \brief A void no argument method map.

    typedef MethodMap::iterator MethodMapIter;
        ///< \brief A method map iterator.

    typedef NoArgMethodMap::iterator NoArgMethodMapIter;
        ///< \brief A no argument method map iterator.

    MethodMap _methodMap;
        ///< \brief Maps method names to their method pointers.

    NoArgMethodMap _noArgMethodMap;
        ///< \brief Maps no argument method names to their method pointers.

    mutable Poco::FastMutex _mutex;
        ///< \brief A mutext to ensure method map validity.
    
};


template <class ListenerClass>
void MethodRegistry::registerMethod(const std::string& name,
                                    const Json::Value& description,
                                    ListenerClass* listener,
                                    void (ListenerClass::*listenerMethod)(const void*, MethodArgs&),
                                    int priority)
{
    unregisterMethod(name);

    Poco::FastMutex::ScopedLock lock(_mutex);
    _methodMap[name] = SharedMethodPtr(new Method(name, description));
    _methodMap[name]->event += Poco::priorityDelegate(listener,
                                                      listenerMethod,
                                                      priority);
}

template <class ListenerClass>
void MethodRegistry::registerMethod(const std::string& name,
                                    const Json::Value& description,
                                    ListenerClass* listener,
                                    void (ListenerClass::*listenerMethod)(MethodArgs&),
                                    int priority)
{
    unregisterMethod(name);

    Poco::FastMutex::ScopedLock lock(_mutex);
    _methodMap[name] = SharedMethodPtr(new Method(name, description));
    _methodMap[name]->event += Poco::priorityDelegate(listener,
                                                listenerMethod,
                                                priority);
}

template <class ListenerClass>
void MethodRegistry::registerMethod(const std::string& name,
                                    const Json::Value& description,
                                    ListenerClass* listener,
                                    void (ListenerClass::*listenerMethod)(const void*),
                                    int priority)
{
    unregisterMethod(name);

    Poco::FastMutex::ScopedLock lock(_mutex);
    _noArgMethodMap[name] = SharedNoArgMethodPtr(new NoArgMethod(name,
                                                                 description));
    _noArgMethodMap[name]->event += Poco::priorityDelegate(listener,
                                                           listenerMethod,
                                                           priority);
}

template <class ListenerClass>
void MethodRegistry::registerMethod(const std::string& name,
                                    const Json::Value& description,
                                    ListenerClass* listener,
                                    void (ListenerClass::*listenerMethod)(void),
                                    int priority)
{
    unregisterMethod(name);

    Poco::FastMutex::ScopedLock lock(_mutex);
    _noArgMethodMap[name] = SharedNoArgMethodPtr(new NoArgMethod(name,
                                                                 description));
    _noArgMethodMap[name]->event += Poco::priorityDelegate(listener,
                                                           listenerMethod,
                                                           priority);
}


} } // namespace ofx::JSONRPC
